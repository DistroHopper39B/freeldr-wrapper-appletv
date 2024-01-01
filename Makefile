#
# PROJECT:		FreeLoader wrapper for Apple TV
# LICENSE:		GPL-2.0-only (https://spdx.org/licenses/GPL-2.0-only)
# PURPOSE:		Makefile for FreeLoader wrapper for original Apple TV
# COPYRIGHT:	Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
#

USER=$(shell whoami)
HOST=$(shell hostname)

# Check what OS we're running. Should work on Linux and macOS.
OSTYPE = $(shell uname)

# Target defs for Linux cross compiler.
TARGET = i386-apple-darwin8

# Definitions for compiler
CC := clang

# Definitions for linker
ifeq ($(OSTYPE),Linux)
	LD := /opt/cross/bin/$(TARGET)-ld
else
	LD := ld
endif

# Flags for mach-o linker
LDFLAGS := -static -force_cpusubtype_ALL \
               -segalign 0x1000 -segaddr __TEXT 0x00400000 \
               -sectalign __TEXT __text 0x1000 \
               -sectalign __DATA __common 0x1000 \
               -sectalign __DATA __bss 0x1000 \
               -sectcreate __PRELINK __text /dev/null \
               -sectcreate __PRELINK __symtab /dev/null \
               -sectcreate __PRELINK __info /dev/null \
               -sectcreate __TEXT __freeldr freeldr.sys \

# Include directories for headers
INCLUDE_DIR = include

CFLAGS := -Wall -nostdlib -fno-stack-protector -fno-builtin -O0 --target=$(TARGET) -Iinclude -D__BUILD_USER=\"$(USER)\" -D__BUILD_HOST=\"$(HOST)\"

ASM_FLAGS := -DASSEMBLER $(CFLAGS)

OBJS = asm.o console.o utils.o vsprintf.o loader.o ioports.o pci.o firmware.o

%.o: %.S
	$(CC) $(ASM_FLAGS) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
mach_kernel: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@
all: mach_kernel

clean:
	rm -f *.o mach_kernel