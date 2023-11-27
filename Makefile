# New makefile to use because CMake was being annoying.

# Check what OS we're running. Should work on Linux and macOS.
OSTYPE = $(shell uname)

# Target defs for Linux cross compiler.
TARGET = i386-apple-darwin8

# Definitions for compilers
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

CFLAGS := -Wall -static -nostdlib -arch i386 -fno-stack-protector -fno-builtin -O3 --target=$(TARGET) -Iinclude

ASM_FLAGS := -DASSEMBLER $(CFLAGS)

OBJS = start.o console.o utils.o vsprintf.o playground.o loader.o jump.o io.o pci.o

%.o: %.S
	$(CC) $(ASM_FLAGS) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
mach_kernel: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@
all: mach_kernel

clean:
	rm -f *.o mach_kernel