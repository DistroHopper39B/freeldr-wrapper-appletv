# Apple TV asm entry point and functions.

.extern _printf
.extern _c_entry

.text
.globl start
.globl _fail
.globl _JumpToFreeldr

        .text
        .align  4,0x90
        .globl  _start
        .globl  _start1
_start:
_start1:
	mov     %ds, %bx
	mov     %bx, %es
	mov     %eax, %ebp

        .text
        .globl __start
        .set __start, (_start1)

	.align  4,0x90
	.globl  start1
start1:
	mov     %ebp, %ebx
	mov     $0,%ax
	mov     %ax,%fs
	mov     %ax,%gs

	pushl   %ebp
	xorl    %ebp,%ebp

	call    _c_entry
	hlt
_fail:
    # Print error to the screen.
    push $msg_halted
    call _printf
    # Halt the CPU
    hlt

# Jump to multiboot code.
# void JumpToFreeldr(u32 start);
_JumpToFreeldr:
    # Disable interrupts.
    cli
    # Execute freeldr finally!
    call *4(%esp)
    # Halt CPU if anything goes wrong
    hlt

.data
    msg_halted: .ascii "FATAL: Could not load FreeLoader! System halted.\n\0"