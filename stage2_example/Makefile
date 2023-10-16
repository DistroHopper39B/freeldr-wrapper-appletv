osname := $(shell uname -s)
COMPILER_FLAGS := -nostdlib
CROSS_PREFIX=i386-elf-
OBJS := start.o console.o playground.o utils.o vsprintf.o

all: stage2.bin

stage2.bin: stage2.elf
	$(CROSS_PREFIX)objcopy -O binary $< $@

stage2.elf: $(OBJS)
	$(CROSS_PREFIX)ld -Tstub.ld $^ -o $@

%.o: %.S
	$(CROSS_PREFIX)gcc $(COMPILER_FLAGS) -DASSEMBLER -c $< -o $@

%.o: %.c
	$(CROSS_PREFIX)gcc $(COMPILER_FLAGS) -Iinclude -c $< -o $@

clean:
	rm -f *.o *.bin *.elf
