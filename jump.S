.text
.globl _JumpToFreeldr

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