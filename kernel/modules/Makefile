# Project: trollos-kernel
TOPDIR	=	$(shell DIR=.; while [ ! "`readlink -f \"$${DIR}\"`" = "/" -a ! -f "$${DIR}/config.mk" ]; do DIR="../$${DIR}"; done; echo "$${DIR}")
ifeq ($(shell readlink -f "$(TOPDIR)"),/)
	$(error Could not find the project top directory with config.mk)
endif
include $(TOPDIR)/config.mk

ASMFILES	=	$(wildcard *.S)
SRCFILES	=	$(wildcard *.c)
OBJFILES	=	$(SRCFILES:.c=.c.o)
OBJFILES	+=	$(ASMFILES:.S=.S.o)

OUTFILE		=	out.a

# Sub directories to build
SUBDIRS		=	fs blockdev chardev binformat

LIBS		=	$(addsuffix /$(OUTFILE),$(SUBDIRS))

.PHONY: all clean
.PHONY: $(SUBDIRS)

all: $(OBJFILES) $(DEPENDS) $(SUBDIRS)
	@echo " [ AR ] $(CURRENTPATH)$(OUTFILE)"
	@$(RM) $(OUTFILE)
	@$(TARGETAR) -cqT $(OUTFILE) $(OBJFILES) $(LIBS)
	@echo
	@echo "Done."
	@echo
	
clean: $(SUBDIRS)
	@echo
	@echo " [ RM ] $(OBJFILES) $(OUTFILE)"
	@$(RM) $(OBJFILES) $(OUTFILE)
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
