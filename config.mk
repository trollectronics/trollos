# Project: 68kemu
# Makefile configurations
MAKEFLAGS	+=	--no-print-directory

TARGETCC	?=	m68k-elf-gcc
TARGETAS	?=	m68k-elf-as
TARGETAR	?=	m68k-elf-ar
TARGETLD	?=	m68k-elf-ld

KERNEL		=	$(TOPDIR)/bin/kernel.elf
BOOTIMG		=	$(TOPDIR)/bin/bootimg.img
BIOS		=	$(TOPDIR)/bin/bios.bin
OSFS		=	$(TOPDIR)/bin/os.romfs

DBGFLAGS	=	-O2 -g

#General flags
LDFLAGS		=	-nostdlib -static -lgcc
CFLAGS		=	-m68030 -Wall -std=c99 -ffreestanding -I"$(TOPDIR)/include" $(DBGFLAGS)
ASFLAGS		=	-m68030

#Extra install targets
INSTARG		=	

#Makefile tools
MKDIR		=	mkdir -p
RMDIR		=	rmdir --ignore-fail-on-non-empty
