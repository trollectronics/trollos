# Project: trollos-kernel
TOPDIR	=	$(shell DIR=.; while [ ! "`readlink -f \"$${DIR}\"`" = "/" -a ! -f "$${DIR}/config.mk" ]; do DIR="../$${DIR}"; done; echo "$${DIR}")
ifeq ($(shell readlink -f "$(TOPDIR)"),/)
	$(error Could not find the project top directory with config.mk)
endif
include $(TOPDIR)/config.mk

# Files to build
ASMFILES	=	$(wildcard *.S)
SRCFILES	=	$(wildcard *.c)
OBJFILES	=	$(SRCFILES:.c=.c.o)
OBJFILES	+=	$(ASMFILES:.S=.S.o)

# Flags
CFLAGS		+=	
ASFLAGS		+=	
LDFLAGS		+=	-Tlink.ld

# Sub directories to build
SUBDIRS	=	syscall mmu modules util

LIBS		=	$(addsuffix /out.a,$(SUBDIRS))

.PHONY: all clean
.PHONY: $(SUBDIRS)

all: $(OBJFILES) $(DEPENDS) $(SUBDIRS)
	@echo " [ LD ] bin/kernel.elf"
	@$(TARGETCC) $(OSCFLAGS) $(CFLAGS) $(OBJFILES) $(LIBS) -o $(KERNEL) $(LDFLAGS)
	@mkdir -p $(TOPDIR)/bin/root/boot
	@cp $(KERNEL) $(TOPDIR)/bin/root/boot/`basename "$(KERNEL)"`
	@echo "Done."
	@echo
	
clean: $(SUBDIRS)
	@echo
	@echo " [ RM ] $(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo "Done."
	@echo 

$(SUBDIRS):
	@echo " [ CD ] $(CURRENTPATH)$@/"
	@+make -C "$@" "CURRENTPATH=$(CURRENTPATH)$@/" $(MAKECMDGOALS)

%.c.o: %.c
	@echo " [ CC ] $(CURRENTPATH)$<"
	@$(TARGETCC) $(CFLAGS) $(OSCFLAGS) -c -o $@ $<
	
%.S.o: %.S
	@echo " [ AS ] $(CURRENTPATH)$<"
	@$(TARGETCC) -c $(ASFLAGS) -o $@ $<
