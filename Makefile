#
# PROJECT:		FreeLoader wrapper for Apple TV
# LICENSE:		MIT (https://spdx.org/licenses/MIT)
# PURPOSE:		Makefile for FreeLoader wrapper for original Apple TV
# COPYRIGHT:	Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
#

USER = $(shell whoami)
HOST = $(shell hostname)

# What freeldr.sys we are using
FREELDR_SYS :=

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
LDFLAGS := -static \
           -segalign 0x1000 \
           -segaddr __TEXT 0x00400000 \
           -sectalign __TEXT __text 0x1000 \
           -sectalign __DATA __common 0x1000 \
           -sectalign __DATA __bss 0x1000 \
           -sectcreate __TEXT __freeldr $(FREELDR_SYS) \


DEFINES := -D__BUILD_USER__=\"$(USER)\" -D__BUILD_HOST__=\"$(HOST)\"

CFLAGS := -Wall -nostdlib -fno-stack-protector -fno-builtin -O0 --target=$(TARGET) -Iinclude $(DEFINES)

OBJS = asm.o console.o utils.o vsprintf.o loader.o ioports.o pci.o firmware.o macho.o

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
mach_kernel: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@
all: mach_kernel

clean:
	rm -f *.o mach_kernel