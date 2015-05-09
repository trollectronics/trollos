# Project: m68k-emu
include $(TOPDIR)/config.mk

ASMFILES	=	$(wildcard *.S)
SRCFILES	=	$(wildcard *.c)
AOBJFILES	=	$(ASMFILES:.S=.ao)
OBJFILES	=	$(SRCFILES:.c=.o)
.PHONY: all clean

LDFLAGS		=	-nostdlib -Wl,-Ttext=0xC0000000 -static -lgcc
CFLAGS		=	-m68030 -Wall -O2 -ffreestanding -I../include
ASFLAGS		=	-m68030

all: $(OBJFILES) $(DEPENDS) $(AOBJFILES)
	@echo " [ LD ] $(KERNEL)"
	@$(TARGETCC) $(OSCFLAGS) $(CFLAGS) $(OBJFILES) $(AOBJFILES) -o $(KERNEL) $(LDFLAGS) $(OSLDFLAGS)
	@echo " [MKFS] $(OSFS)"
	@mkdir -p root
	@mkdir -p root/boot
	@cp $(KERNEL) root/boot/`basename "$(KERNEL)"`
	@genromfs -f $(OSFS) -d root/ -V TrollOS
	@rm -Rf root/
	@echo "Done."
	@echo
	
clean:
	@echo
	@echo " [ RM ] $(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo "Done."
	@echo 

%.o: %.c
	@echo " [ CC ] kernel/$<"
	@$(TARGETCC) $(CFLAGS) $(OSCFLAGS) -c -o $@ $<
	
%.ao: %.S
	@echo " [ AS ] kernel/$<"
	@$(TARGETAS) $(ASFLAGS) -o $@ $<
