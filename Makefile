# Project: m68k-emu
include $(TOPDIR)/config.mk

CFLAGS		+=	-nostdlib -O3 -ffreestanding -fno-builtin -nodefaultlibs
LDFLAGS		=	-Wl,--oformat=binary,-e0x400,-Tbss=0x87000,-Ttext=0x400
#,-Tdata=0x400
SRCFILES	=	start.c test.c boot_term.c util.c
OBJFILES	=	$(SRCFILES:.c=.o)
BOOTBIN		=	boot.bin
BOOTVEC		=	boot.vec
.PHONY: all clean

all: $(OBJFILES) $(DEPENDS)
	@echo " [ LD ] $(BOOTBIN)"
	@$(TARGETCC) $(CFLAGS) $(OBJFILES) -o $(BOOTBIN) $(LDFLAGS)
	@echo " [MIMG] $(BOOTIMG)"
	@cat $(BOOTVEC) $(BOOTBIN) > $(BOOTIMG)
	@echo "Done."
	@echo
	
clean:
	@echo
	@echo " [ RM ] $(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo " [ RM ] $(BOOTBIN)"
	@echo "Done."
	@echo 

%.o: %.c %.h
	@echo " [ CC ] src/$<"
	@$(TARGETCC) $(CFLAGS) -c -o $@ $<
	
