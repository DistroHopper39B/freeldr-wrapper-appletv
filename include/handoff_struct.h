//
// Created by distrohopper39b on 10/8/23.
// The struct passed to freeldr
//

#ifndef PLAYGROUND_HANDOFF_STRUCT_H
#define PLAYGROUND_HANDOFF_STRUCT_H

#include "atvint.h"

#define ATV_MAGIC_NUMBER 0xBAADC0DE
typedef struct {
    u32 EfiMemoryMap;
    u32 EfiMemoryMapSize;
    u32 EfiMemDescSize;
    u32 EfiMemDescVer;
} EfiMemoryMap;

typedef struct {
    u8 E820MemoryMap[2560];
    u8 E820NrMap;
} E820MemoryMap;

typedef struct {
    u32 BaseAddress; // Address to write to for VRAM
    u32 Pitch; // Bytes per row
    u32 Width; // Width
    u32 Height; // Height
    u32 Depth; // Color depth
} FrameBuffer;

typedef struct {
    u32 AppleTVMagic; // Apple TV magic number
    EfiMemoryMap EfiMap; // EFI memory map information
    efi_runtime_services_t *EfiRuntimeServices; // EFI Runtime Services
    E820MemoryMap E820Map; // E820 memory map information
    FrameBuffer Video; // Frame buffer information
    char CommandLine[CMDLINE]; // Mach command line (not sure how we'll use this yet)
} HandoffBootStruct;


#endif //PLAYGROUND_HANDOFF_STRUCT_H
