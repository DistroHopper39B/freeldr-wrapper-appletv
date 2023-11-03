# New makefile to use because CMake was being annoying.

# Check what OS we're running. Should work on Linux and macOS.
OSTYPE = $(shell uname)

# Target defs for Linux cross compiler.
TARGET = i386-apple-darwin8
SYSROOT = /opt/cross/SDK/MacOSX10.4u.sdk

# Definitions for compilers
CC := clang
ASM := nasm

# Definitions for linker
ifeq ($(OSTYPE),Linux)
	LD := /opt/cross/bin/$(TARGET)-ld
else
	LD := ld
endif

# Embedded stage2 executable path
STAGE2_PATH=stage2_example/stage2.bin

# Flags for mach-o linker
LDFLAGS := -static -force_cpusubtype_ALL \
               -segalign 0x4000 -segaddr __TEXT 0x00400000 \
               -sectalign __TEXT __text 0x4000 \
               -sectalign __DATA __common 0x4000 \
               -sectalign __DATA __bss 0x4000 \
               -sectcreate __PRELINK __text /dev/null \
               -sectcreate __PRELINK __symtab /dev/null \
               -sectcreate __PRELINK __info /dev/null \
               -sectcreate __TEXT __stage2 $(STAGE2_PATH) \

# Include directories for headers
INCLUDE_DIR = include

CFLAGS := -Wall -static -nostdlib -arch i386 -fno-stack-protector -O3 --target=$(TARGET) -isysroot $(SYSROOT) -Iinclude
ASM_FLAGS := -fmacho32

OBJS = start.o console.o tvfixes.o utils.o vsprintf.o playground.o multiboot_loader.o multiboot.o

%.o: %.asm
	$(ASM) $(ASM_FLAGS) $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
mach_kernel: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@
all: mach_kernel

clean:
	rm -f *.o mach_kernel