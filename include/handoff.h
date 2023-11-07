//
// Created by distrohopper39b on 10/8/23.
// The struct passed to freeldr
//

#ifndef PLAYGROUND_HANDOFF_STRUCT_H
#define PLAYGROUND_HANDOFF_STRUCT_H

#include "atvint.h"

// Apple TV Magic number: verifies that we loaded the struct correctly.
#define ATV_LOADER_MAGIC_NUMBER 0xBAADC0DE
// Freeldr magic number: symbolizes the start of the executable info structure. Must be within the first 8192 bytes, just like multiboot.
#define FREELDR_MAGIC_NUMBER 0xF00DBEEF

// EFI memory map information.
typedef struct {
    u32 addr;
    u32 size;
    u32 descriptor_size;
    u32 descriptor_version;
} efi_memory_map;

// Video mode information.
typedef struct {
    u32 base; // Address to write to for VRAM
    u32 pitch; // Bytes per row
    u32 width; // Width
    u32 height; // Height
    u32 depth; // Color depth
} video_attr;

// Kernel geometry information. Useful for the memory allocator later on.
typedef struct {
    u32 base_addr; // Base address of kernel as specified by linker argument "-segaddr __TEXT".
    u32 size; // Size of kernel + other memory we can't write to in userspace (such as UEFI BIOS information, memory maps, etc).
    u32 end; // End of kernel. Everything past this address should always be safe to write to by userspace.
} mach_environment_params;

// Multiboot/E820 memory map information.
typedef struct {
    u32 addr; // Pointer to memory map info
    u32 size; // Size of memory map
    u32 entries; // Number of entries in memory map
} memory_map;

// Multiboot/E820 memory map entry.
struct mmap_entry {
    u64 addr;
    u64 len;
#define MULTIBOOT_MEMORY_AVAILABLE		1
#define MULTIBOOT_MEMORY_RESERVED		2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
    u32 type;
} __attribute__((packed));
typedef struct mmap_entry memory_map_entry;

typedef struct {
    u32 magic; // Apple TV magic number
    efi_memory_map efi_map; // EFI memory map information
    u32 efi_system_table_ptr; // Pointer to EFI system table.
    memory_map multiboot_map; // E820/Multiboot memory map information
    video_attr video; // Frame buffer information
    u32 cmdline_ptr; // Pointer to command line
    mach_environment_params kernel; // Kernel geometry.
} handoff_boot_info;

// This struct goes at the beginning of freeldr and is used as a header for information, much like multiboot.
typedef struct {
    u32 magic; // Freeldr magic number.
    u32 load_addr; // Where to load freeldr into memory.
    u32 entry_point; // Where to start executing code.
} freeldr_hdr;


#endif //PLAYGROUND_HANDOFF_STRUCT_H
