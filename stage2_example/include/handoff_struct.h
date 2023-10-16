//
// Created by distrohopper39b on 10/8/23.
// The struct passed to freeldr
//

#ifndef PLAYGROUND_HANDOFF_STRUCT_H
#define PLAYGROUND_HANDOFF_STRUCT_H

#include "atvint.h"

// Apple TV Magic number: verifies that we loaded the struct correctly.
#define ATV_MAGIC_NUMBER 0xBAADC0DE

// EFI memory map information.
typedef struct {
    u32 EfiMemoryMap;
    u32 EfiMemoryMapSize;
    u32 EfiMemDescSize;
    u32 EfiMemDescVer;
} EfiMemoryMap;

// E820 memory map information (not sure how or if this will be used).
typedef struct {
    u8 E820MemoryMap[2560];
    u8 E820NrMap;
} E820MemoryMap;

// Video mode information.
typedef struct {
    u32 BaseAddress; // Address to write to for VRAM
    u32 Pitch; // Bytes per row
    u32 Width; // Width
    u32 Height; // Height
    u32 Depth; // Color depth
} FrameBuffer;

// Kernel geometry information. Useful for the memory allocator later on.
typedef struct {
    u32 BaseAddress; // Base address of kernel as specified by linker argument "-segaddr __TEXT".
    u32 Size; // Size of kernel + other memory we can't write to in userspace (such as UEFI BIOS information, memory maps, etc).
    u32 End; // End of kernel. Everything past this address should always be safe to write to by userspace.
} Kernel;

typedef struct {
    u32 AppleTVMagic; // Apple TV magic number
    EfiMemoryMap EfiMap; // EFI memory map information
    efi_runtime_services_t *EfiRuntimeServices; // EFI Runtime Services
    E820MemoryMap E820Map; // E820 memory map information
    FrameBuffer Video; // Frame buffer information
    char CommandLine[CMDLINE]; // Mach command line (not sure how we'll use this yet)
    Kernel Kernel; // Kernel geometry.
} HandoffBootStruct;


#endif //PLAYGROUND_HANDOFF_STRUCT_H
