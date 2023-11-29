# Apple TV asm entry point and functions.

.extern _printf
.extern _c_entry

.text
.globl start
.globl _fail
.globl _JumpToFreeldr

start:
    # Push bootArgs pointer from boot.efi to the stack.
    push %eax
    # Call C entry point.
    call _c_entry
    call _fail

_fail:
    # Print error to the screen.
    push $msg_halted
    call _printf
    # Halt the CPU
    hlt

# Jump to multiboot code.
# void JumpToFreeldr(u32 start, u32 info);
_JumpToFreeldr:
    # Disable interrupts.
    cli
    # Move info struct pointer to ebx register
    movl 8(%esp), %ebx
    # Execute freeldr finally!
    call *4(%esp)
    # Halt CPU if anything goes wrong
    hlt

.data
    msg_halted: .ascii "FATAL: Could not load FreeLoader! System halted.\n\0"