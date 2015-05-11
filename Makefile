# Project: 68k-emu
include config.mk
MAKEFLAGS	+=	--no-print-directory

TOPDIR		=	$(shell pwd)
export TOPDIR

.PHONY: all clean bios kernel init

all: init bios kernel
	@cat "$(OSFS)" >> "$(BOOTIMG)"
	
	@echo "Build complete."
	@echo 

bin:
	@echo " [INIT] bin/"
	@$(MKDIR) bin/

init: | bin
	@echo " [ CD ] userspace/init/"
	+@make -C userspace/init/

kernel: init | bin
	@echo " [ CD ] kernel/"
	+@make -C kernel/

bios: | bin
	@echo " [ CD ] bios/"
	+@make -C bios/

clean:
	@echo " [ RM ] bin/"
	@$(RM) $(BIOS) $(BOOTIMG) $(KERNEL) $(OSFS)
	@$(RMDIR) bin/
	@echo " [ CD ] bios/"
	+@make -C bios/ clean
	@echo " [ CD ] kernel/"
	+@make -C kernel/ clean
	@echo " [ CD ] userspace/init"
	+@make -C userspace/init/ clean
	@echo
	@echo "Source tree cleaned."
	@echo
