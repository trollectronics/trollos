# Project: m68k-emu
include ../../config.mk

ASMFILES	=	$(wildcard *.S)
SRCFILES	=	$(wildcard *.c)
OBJFILES	=	$(SRCFILES:.c=.c.o)
OBJFILES	+=	$(ASMFILES:.S=.S.o)

BINFILE		=	 ../../bin/init

.PHONY: all clean

LDFLAGS		=	-nostdlib -static -lgcc
CFLAGS		=	-m68030 -Wall -O2 -ffreestanding -I../../include
ASFLAGS		=	-m68030

all: $(OBJFILES) $(DEPENDS)
	@echo " [ LD ] userspace/init/init"
	@$(TARGETCC) $(CFLAGS) $(OBJFILES) -o $(BINFILE) $(LDFLAGS)
	@echo "Done."
	@echo
	
clean:
	@echo
	@echo " [ RM ] $(OBJFILES)"
	@$(RM) $(OBJFILES) $(BINFILE)
	@echo "Done."
	@echo 

%.c.o: %.c
	@echo " [ CC ] userspace/init/$<"
	@$(TARGETCC) $(CFLAGS) -c -o $@ $<
	
%.S.o: %.S
	@echo " [ AS ] userspace/init/$<"
	@$(TARGETAS) $(ASFLAGS) -o $@ $<
