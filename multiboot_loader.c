/**
 * Multiboot support for atv-playground - C functions
 * Multiboot 1 does not officially support UEFI, but should be mostly usable on it. Boot loaders requiring VBE modes
 * will not work without modification.
 */

#include <atvlib.h>
#include <multiboot.h>

// Convert EFI memory map to Multiboot memory map.
void CreateMultibootMemoryMap() {

}

// Create Multiboot information structure to pass to boot loader.
void CreateMultibootInfoStructure() {
    multiboot_info_t *multiboot_info;

}

// Validate executable header to be Multiboot-compatible and have correct flags.
void ValidateHeader(u32 *Stage2Ptr) {

}

// Jump to multiboot code. This is defined in multiboot.asm
extern void JumpToMultiboot(int start, int mb_info);