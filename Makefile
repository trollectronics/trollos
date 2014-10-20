# Project: m68k-emu
include $(TOPDIR)/config.mk

LOADERBIN	=	osloader.elf

SRCFILES	=	$(wildcard *.c)
OBJFILES	=	$(SRCFILES:.c=.o)
.PHONY: all clean

LDFLAGS		=	-nostdlib -Wl,-Ttext=0x10000000 -static -lgcc
CFLAGS		=	-Wall -O2 -I../

all: $(OBJFILES) $(DEPENDS)
	@echo " [ LD ] $(LOADERBIN)"
	@$(TARGETCC) $(OSCFLAGS) $(CFLAGS) $(OBJFILES) -o $(LOADERBIN) $(LDFLAGS) $(OSLDFLAGS)
	@echo " [MKFS] $(OSFS)"
	@mkdir -p root
	@mkdir -p root/boot
	@cp $(LOADERBIN) root/boot/$(LOADERBIN)
	@genromfs -f $(OSFS) -d root/ -V TrollBookOS
	@rm -Rf root/
	@echo "Done."
	@echo
	
clean:
	@echo
	@echo " [ RM ] $(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo "Done."
	@echo 

%.o: %.c %.h
	@echo " [ CC ] osloader/$<"
	@$(TARGETCC) $(CFLAGS) $(OSCFLAGS) -c -o $@ $<
	
