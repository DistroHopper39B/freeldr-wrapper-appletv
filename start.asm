; Apple TV asm entry point.

extern _printk
extern _playground_start

SECTION .text
global start

start:
    ; Push bootArgs pointer from boot.efi.
    push eax
    ; Call C entry point.
    call _playground_start
    ; Halt the CPU
    hlt