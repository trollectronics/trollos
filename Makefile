# Project: 68k-emu
include config.mk
MAKEFLAGS	+=	--no-print-directory

TOPDIR		=	$(shell pwd)
export TOPDIR

.PHONY: all clean

all:
	@echo " [INIT] bin/"
	@$(MKDIR) bin/
	@echo " [ CD ] bios/"
	+@make -C bios/
	@echo " [ CD ] kernel/"
	+@make -C kernel/
	@cat "$(OSFS)" >> "$(BOOTIMG)"
	
	@echo "Build complete."
	@echo 

clean:
	@echo " [ RM ] bin/"
	+@$(RM) bin/
	@echo " [ CD ] bios/"
	+@make -C bios/ clean
	@echo " [ CD ] kernel/"
	+@make -C kernel/ clean
	@echo
	@echo "Source tree cleaned."
	@echo
