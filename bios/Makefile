# Project: trollos-bios
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
CFLAGS		+=	-fno-builtin
LDFLAGS		+=	-Wl,--oformat=binary,-Tlink.ld
ASFLAGS		+=	

# Sub directories to build
SUBDIRS	=	

.PHONY: all clean
.PHONY: $(SUBDIRS)

all: $(OBJFILES) $(DEPENDS) $(SUBDIRS)
	@echo $(BUILDDIRS)
	@echo " [ LD ] bin/bios.img"
	@$(TARGETCC) $(CFLAGS) $(OBJFILES) -o "$(BIOS)" $(LDFLAGS)
	@echo " [MIMG] $(BOOTIMG)"
	@dd if=/dev/zero of="$(BOOTIMG)" bs=1024 count=64 2>/dev/null
	@dd if=$(BIOS) of="$(BOOTIMG)" bs=1024 seek=0 conv=notrunc 2>/dev/null
	@echo "Done."
	@echo
	
clean: $(SUBDIRS)
	@echo
	@echo " [ RM ] $(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo " [ RM ] $(BIOS)"
	@$(RM) $(BIOS)
	@echo "Done."
	@echo 

$(SUBDIRS):
	@echo " [ CD ] $(CURRENTPATH)$@/"
	@+make -C "$@" "CURRENTPATH=$(CURRENTPATH)$@/" $(MAKECMDGOALS)

%.c.o: %.c
	@echo " [ CC ] $(CURRENTPATH)$<"
	@$(TARGETCC) $(CFLAGS) -c -o $@ $<

%.S.o: %.S
	@echo " [ AS ] $(CURRENTPATH)$<"
	@$(TARGETAS) $(ASFLAGS) -o $@ $<
