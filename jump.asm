; Multiboot support for atv-playground - C functions
; Multiboot 1 does not officially support UEFI, but should be mostly usable on it. Boot loaders requiring VBE modes
; will not work without modification.

extern _printk

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
