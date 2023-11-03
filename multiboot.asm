; Multiboot support for atv-playground - C functions
; Multiboot 1 does not officially support UEFI, but should be mostly usable on it. Boot loaders requiring VBE modes
; will not work without modification.

extern _printk

SECTION .text
global JumpToMultiboot

; Jump to multiboot code.
; void JumpToMultiboot(int start, int mb_info);
JumpToMultiboot:
