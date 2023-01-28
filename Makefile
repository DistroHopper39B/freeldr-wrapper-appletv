# The worst Makefile you'll ever see...
.PHONY: inject_kernel

CC := i386-apple-darwin8-clang
LD := i386-apple-darwin8-ld

# Linking order
OUT = start.o vsprintf.o console.o utils.o mach-o.o playground.o

mach_kernel: $(OUT)
	$(LD) -o mach_kernel $(OUT) -static -e __start
%.o: %.c
	$(CC) -c -static -nostdlib -fno-stack-protector -o  $@ -c $<
%.o: %.S
	$(CC) -c -static -nostdlib -DASSEMBLER -o $@ -c $<

clean:
	rm -f *.o mach_kernel