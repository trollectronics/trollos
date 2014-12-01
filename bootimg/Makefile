# Project: m68k-emu
include $(TOPDIR)/config.mk

CFLAGS		+=	-m68030 -nostdlib -O3 -ffreestanding -fno-builtin -nostdlib -I../include
LDFLAGS		=	-Wl,--oformat=binary,-Tlink.ld
ASFLAGS		+=	-m68030
#,-Tdata=0x400
ASMFILES	=	$(wildcard *.S)
SRCFILES	=	$(wildcard *.c)
AOBJFILES	=	$(ASMFILES:.S=.ao)
OBJFILES	=	$(SRCFILES:.c=.o)

.PHONY: all clean

all: $(OBJFILES) $(AOBJFILES) $(DEPENDS)
	@echo " [ LD ] $(BIOS)"
	@$(TARGETCC) $(CFLAGS) $(OBJFILES) $(AOBJFILES) -o "$(BIOS)" $(LDFLAGS)
	@echo " [MIMG] $(BOOTIMG)"
	@dd if=/dev/zero of="$(BOOTIMG)" bs=1024 count=64 2>/dev/null
	@dd if=$(BIOS) of="$(BOOTIMG)" bs=1024 seek=0 conv=notrunc 2>/dev/null
	@echo "Done."
	@echo
	
clean:
	@echo
	@echo " [ RM ] $(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo " [ RM ] $(BIOS)"
	@$(RM) $(BIOS)
	@echo "Done."
	@echo 

%.o: %.c %.h
	@echo " [ CC ] bootimg/$<"
	@$(TARGETCC) $(CFLAGS) -c -o $@ $<

%.ao: %.S
	@echo " [ AS ] bootimg/$<"
	@$(TARGETAS) $(ASFLAGS) -o $@ $<
