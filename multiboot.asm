; Multiboot support for atv-playground - C functions
; Multiboot 1 does not officially support UEFI, but should be mostly usable on it. Boot loaders requiring VBE modes
; will not work without modification.

extern _printk

SECTION .text
global _JumpToMultiboot

; Jump to multiboot code.
; void JumpToMultiboot(u32 start, u32 mb_info);
_JumpToMultiboot:
    ; Disable interrupts.
    cli
    ; Move magic number to eax register.
    mov eax, 0x2BADB002
    ; Move multiboot info struct pointer to ebx register
    mov ebx, [esp + 8]
    ; Print message
    ; push status_msg
    ; call _printk
    ; Execute multiboot finally!
    call [esp + 4]
    ; Print post call
    ; push post_call
    call _printk
    hlt

SECTION .data
    status_msg db 'ASM: Jumping to multiboot code now...', 0x0A, 0
    post_call db 'ASM: Past the point of no return. We should not see this.', 0x0A, 0
