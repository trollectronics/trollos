# Project: 68kemu
# Makefile configurations

TARGETCC	?=	m68k-elf-gcc
TARGETAS	?=	m68k-elf-as
KERNEL		=	$(TOPDIR)/bin/kernel.elf
BOOTIMG		=	$(TOPDIR)/bin/bootimg.img
BIOS		=	$(TOPDIR)/bin/bios.bin
OSFS		=	$(TOPDIR)/bin/os.romfs

DBGFLAGS	=	-O0 -g -D__DEBUG__
#DBGFLAGS	=	-O3 -g
#General flags


#Extra install targets
INSTARG		=	

#Makefile tools
RM		=	rm -Rf
MKDIR		=	mkdir -p

