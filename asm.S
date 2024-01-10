#
# PROJECT:		FreeLoader wrapper for Apple TV
# LICENSE:      MIT (https://spdx.org/licenses/MIT)
# PURPOSE:		Assembly functions for FreeLoader wrapper for original Apple TV
# COPYRIGHT:	Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
#

.extern _printf
.extern _WrapperInit

.text
.globl start
.globl _fail
.globl _JumpToFreeldr

start:
    # Push BootArgs pointer to C loader
    pushl %eax
    # Jump to C
	call _WrapperInit
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