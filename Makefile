# Project: 68k-emu
TOPDIR	=	.
include config.mk

# Sub directories to build
SUBDIRS	=	 kernel userspace

.PHONY: all clean
.PHONY: $(SUBDIRS)

all: $(SUBDIRS)
	@echo " [MKFS] $(OSFS)"
	@genromfs -f $(OSFS) -d bin/root/ -V TrollOS
	@cat "$(OSFS)" >> "$(BOOTIMG)"
	@dd if=/dev/zero of=bin/sd.img bs=1M count=32
	@mkfs.msdos bin/sd.img
	@cp bin/kernel.elf bin/kernel-debug.elf
	@m68k-elf-strip bin/kernel.elf
	@mcopy -i bin/sd.img -D o bin/kernel.elf ::/
	
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
