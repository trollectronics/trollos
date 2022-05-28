# Project: trollos-init
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

BINFILE		=	 $(TOPDIR)/bin/testbin

# Flags
CFLAGS		+=	
ASFLAGS		+=
LSFLAGS		+=

# Sub directories to build
SUBDIRS	=	

.PHONY: all clean
.PHONY: $(SUBDIRS)

all: $(OBJFILES) $(DEPENDS) $(SUBDIRS)
	@echo " [ LD ] bin/testbin"
	@$(TARGETCC) $(CFLAGS) $(OBJFILES) -o $(BINFILE) $(LDFLAGS)
	@echo "Done."
	@echo
	
clean: $(SUBDIRS)
	@echo
	@echo " [ RM ] $(OBJFILES) $(BINFILE)"
	@$(RM) $(OBJFILES) $(BINFILE)
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
