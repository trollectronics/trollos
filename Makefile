# Project: 68k-emu
TOPDIR	=	.
include config.mk

# Sub directories to build
SUBDIRS	=	bios kernel userspace

.PHONY: all clean
.PHONY: $(SUBDIRS)

all: $(SUBDIRS)
	@echo " [MKFS] $(OSFS)"
	@genromfs -f $(OSFS) -d bin/root/ -V TrollOS
	@cat "$(OSFS)" >> "$(BOOTIMG)"
	
	@echo "Build complete."
	@echo 

bin:
	@echo " [INIT] bin/"
	@$(MKDIR) bin/

clean: $(SUBDIRS)
	@echo " [ RM ] bin/"
	@$(RM) $(BIOS) $(BOOTIMG) $(KERNEL) $(OSFS)
	@$(RMDIR) bin/
	@echo
	@echo "Source tree cleaned."
	@echo

$(SUBDIRS): | bin
	@echo " [ CD ] $(CURRENTPATH)$@/"
	@+make -C "$@" "CURRENTPATH=$(CURRENTPATH)$@/" $(MAKECMDGOALS)
