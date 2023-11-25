; Apple TV asm entry point.

extern _printk_always
extern _playground_start

SECTION .text
global start
global _fail

start:
    ; Push bootArgs pointer from boot.efi to the stack.
    push eax
    ; Call C entry point.
    call _playground_start
    call _fail

_fail:
    ; Print error to the screen. We should never hit this case.
    push msg_halted
    call _printk_always
    ; Halt the CPU
    hlt


SECTION .data
    msg_halted db "FATAL: Could not load FreeLoader! System halted.", 0x0A, 0