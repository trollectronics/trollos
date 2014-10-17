# Project: m68k-emu
include $(TOPDIR)/config.mk

CFLAGS		+=	-fPIC -nostdlib -O3
LDFLAGS		=	-Wl,--oformat=binary
SRCFILES	=	start.c test.c
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
	
