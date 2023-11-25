SECTION .text
global _JumpToFreeldr

; Jump to multiboot code.
; void JumpToFreeldr(u32 start, u32 info);
_JumpToFreeldr:
    ; Disable interrupts.
    cli
    ; Move info struct pointer to ebx register
    mov ebx, [esp + 8]
    ; Execute freeldr finally!
    call [esp + 4]
    ; Halt CPU if anything goes wrong
    hlt
