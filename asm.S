#
# PROJECT:		FreeLoader wrapper for Apple TV
# LICENSE:		GPL-2.0-only (https://spdx.org/licenses/GPL-2.0-only)
# PURPOSE:		Makefile for FreeLoader wrapper for original Apple TV
# COPYRIGHT:	Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
#

.extern _printf
.extern _c_entry

.text
.globl start
.globl _fail
.globl _JumpToFreeldr

start:
    # Push BootArgs pointer to C loader
    pushl %eax
    # Jump to C
	call _c_entry
	# Halt the system
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
    # Execute freeldr finally!
    call *4(%esp)
    # Halt CPU if anything goes wrong
    hlt

.data
    msg_halted: .ascii "FATAL: Could not load FreeLoader! System halted.\n\0"