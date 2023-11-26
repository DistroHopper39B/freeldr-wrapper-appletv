# Apple TV asm entry point.

.extern _printk_always
.extern _playground_start

.text
.globl start
.globl _fail

start:
    # Push bootArgs pointer from boot.efi to the stack.
    push %eax
    # Call C entry point.
    call _playground_start
    call _fail

_fail:
    # Print error to the screen. We should never hit this case.
    push $msg_halted
    call _printk_always
    # Halt the CPU
    hlt

.data
    msg_halted: .ascii "FATAL: Could not load FreeLoader! System halted.\n\0"