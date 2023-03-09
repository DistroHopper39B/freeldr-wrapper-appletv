# The worst Makefile you'll ever see...
.PHONY: inject_kernel

CC := i386-apple-darwin8-clang
LD := i386-apple-darwin8-ld
OBJCOPY := i686-linux-gnu-objcopy
MACHO := mach_kernel
LDFLAGS := -undefined dynamic_lookup

# Linking order
OUT = start.o vsprintf.o console.o utils.o mach-o.o playground.o

all: mach_kernel

mach_kernel: $(OUT)
	$(LD) $(LDFLAGS) -o $(MACHO) $(OUT) -static
%.o: %.c
	$(CC) -c -static -nostdlib -Iinclude -fno-stack-protector -o $@ -c $<
%.o: %.S
	$(CC) -c -static -nostdlib -DASSEMBLER -o $@ -c $<
pci/%.o: pci/%.c
	$(CC) -c -static -nostdlib -fno-stack-protector -o $@ -c $<
clean:
	rm -f *.o mach_kernel*
