# Project: 68k-emu
TOPDIR	=	.
include config.mk

# Sub directories to build
SUBDIRS	=	 userspace kernel

.PHONY: all clean rootfs
.PHONY: $(SUBDIRS)

all: $(SUBDIRS)
	@dd if=/dev/zero of=bin/sd.img bs=1M count=32
	@/sbin/mkfs.msdos bin/sd.img
	@cp bin/kernel.elf bin/kernel-debug.elf
	@m68k-elf-strip bin/kernel.elf
	@mcopy -i bin/sd.img -D o bin/kernel.elf ::/
	
	@echo "Build complete."
	@echo 

rootfs:
	@xxd -i $(OSFS) kernel/root.c

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

kernel: userspace | bin
	@echo " [ CD ] $(CURRENTPATH)$@/"
	@+make -C "$@" "CURRENTPATH=$(CURRENTPATH)$@/" $(MAKECMDGOALS)

userspace: | bin
	@echo " [ CD ] $(CURRENTPATH)$@/"
	@+make -C "$@" "CURRENTPATH=$(CURRENTPATH)$@/" $(MAKECMDGOALS)
	@echo " [MKFS] $(OSFS)"
	@genromfs -f $(OSFS) -d bin/root/ -V TrollOS
	@echo " [OCPY] kernel/rootfs.o"
	@m68k-elf-objcopy -I binary -O elf32-m68k "$(OSFS)" kernel/rootfs.o
