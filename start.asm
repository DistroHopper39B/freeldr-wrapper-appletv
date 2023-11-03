; Apple TV asm entry point.

extern _printk
extern _playground_start

SECTION .text
global start

start:
    ; Push bootArgs pointer from boot.efi to the stack.
    push eax
    ; Call C entry point.
    call _playground_start
    ; Print error to the screen. We should never hit this case.
    push msg_halted
    call _printk
    ; Halt the CPU
    hlt

SECTION .data
    msg_halted db 0x0A, "FATAL: Could not load stage2! System halted.", 0x0A, 0