# Project: m68k-emu
include $(TOPDIR)/config.mk

TEXT		=	0x00400
ENTRY		=	0x00400
BSS			=	0xEA000


CFLAGS		+=	-m68030 -nostdlib -O3 -ffreestanding -fno-builtin -nostdlib -DENTRY=$(ENTRY) -DTEXT=$(TEXT) -DBSS=$(BSS)
LDFLAGS		=	-Wl,--oformat=binary,-e$(ENTRY),-Tbss=$(BSS),-Ttext=$(TEXT)
ASFLAGS		+=	-m68030
#,-Tdata=0x400
ASMFILES	=	dummy.s mmu-helpers.s
SRCFILES	=	start.c test.c boot_term.c util.c romfs.c elf.c mmu.c printf.c spi.c sd.c
AOBJFILES	=	$(ASMFILES:.s=.ao)
OBJFILES	=	$(SRCFILES:.c=.o)
BOOTBIN		=	boot.bin
BOOTVEC		=	boot.vec
.PHONY: all clean

all: $(OBJFILES) $(AOBJFILES) $(DEPENDS)
	@echo " [ LD ] $(BOOTBIN)"
	@$(TARGETCC) $(CFLAGS) $(OBJFILES) $(AOBJFILES) -o $(BOOTBIN) $(LDFLAGS)
	@echo " [MIMG] $(BOOTIMG)"
	@dd if=/dev/zero of=boot.img bs=1024 count=64 2>/dev/null
	@dd if=$(BOOTVEC) of=boot.img conv=notrunc 2>/dev/null
	@dd if=$(BOOTBIN) of=boot.img bs=1024 seek=1 conv=notrunc 2>/dev/null
	@mv boot.img $(BOOTIMG)
	@echo "Done."
	@echo
	
clean:
	@echo
	@echo " [ RM ] $(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo " [ RM ] $(BOOTBIN)"
	@$(RM) $(BOOTBIN)
	@echo "Done."
	@echo 

%.o: %.c %.h
	@echo " [ CC ] bootimg/$<"
	@$(TARGETCC) $(CFLAGS) -c -o $@ $<

%.ao: %.s
	@echo " [ AS ] bootimg/$<"
	@$(TARGETAS) $(ASFLAGS) -o $@ $<
