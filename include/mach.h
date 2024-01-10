/*
 * PROJECT:     FreeLoader wrapper for Apple TV
 * LICENSE:     GPL-2.0-only (https://spdx.org/licenses/GPL-2.0-only)
 * PURPOSE:     Header file for the Mach-O environment for the original Apple TV
 * COPYRIGHT:   Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
 */

#pragma once

#define MACH_CMDLINE 1024

/* Video parameters passed to kernel. */
typedef struct {
    u32 BaseAddress; /* Base video address */
    u32 DisplayMode; /* Display mode specifier */
    u32 Pitch; /* Bytes per row */
    u32 Width; /* Display width in pixels */
    u32 Height; /* Display height in pixels */
    u32 Depth; /* Display depth in bits */
} __attribute__((aligned(4))) MACH_VIDEO, PMACH_VIDEO;

/* Boot arguments struct passed into loader. A pointer to this struct is located in the EAX register upon kernel load.
 * See xnu-1228 pexpert/pexpert/i386/boot.h.
 */

typedef struct {
    u16 Revision; /* Revision of this structure */
    u16 Version; /* Version of this structure */

    char CmdLine[MACH_CMDLINE]; /* Command line data */

    u32 EfiMemoryMap; /* Location of EFI memory map */
    u32 EfiMemoryMapSize; /* Size of EFI memory map */
    u32 EfiMemoryDescriptorSize; /* Size of EFI descriptor */
    u32 EfiMemoryDescriptorVersion; /* Version of EFI memory descriptors */

    MACH_VIDEO Video; /* Video parameters */

    u32 DeviceTree; /* Pointer to base of Apple IODeviceTree */
    u32 DeviceTreeLength; /* Length of device tree */

    u32 KernelBaseAddress; /* Beginning of kernel as specified by `-segaddr __TEXT` */
    u32 KernelSize; /* Size of kernel and firmware */

    u32 EfiRuntimeServicesPageStart; /* Address of defragmented runtime pages */
    u32 EfiRuntimeServicesPageCount; /* Number of EFI pages */
    u32 EfiSystemTable; /* EFI System Table */

    u8 EfiMode; /* EFI mode: 32 = 32 bit EFI, 64 = 64 bit EFI */

    u8 __reserved1[3];
    u8 __reserved2[7];
} __attribute__((aligned(4))) MACH_BOOTARGS, *PMACH_BOOTARGS;

extern PMACH_BOOTARGS BootArgs;

#define MACHO_MAGIC 0xFEEDFACE /* Mach-O Magic number */
#define MACHO_CIGAM 0xCEFAEDFE /* Big-endian representation */
#define MACHO_OBJECT 0x1 /* Relocatable object file */

#define MACHO_LC_SEGMENT 0x1 /* Segment to be mapped */

/* Mach-O header */
typedef struct {
    u32 MagicNumber; /* Mach-O magic number. */

    s32 CpuType; /* CPU type */
    s32 CpuSubtype; /* CPU subtype */

    u32 FileType; /* Type of Mach-O file */

    u32 NumberOfCmds; /* Number of load commands */
    u32 SizeOfCmds; /* Size of all load commands */

    u32 Flags; /* Executable flags */
} MACHO_HEADER, *PMACHO_HEADER;

typedef struct {
    u32 Command; /* LC_SEGMENT */
    u32 CommandSize; /* Size of segment command */

    char SegmentName[16]; /* Name of segment */

    u32 VMAddress; /* Virtual memory address of this segment */
    u32 VMSize; /* Virtual memory size of this segment */

    u32 FileOffset; /* File offset of this segment */
    u32 FileSize; /* Amount to map from the file */

    s32 MaximumProtection; /* Maximum virtual memory protection */
    s32 InitialProtection; /* Initial virtual memory protection */

    u32 NumberOfSections; /* Number of sections in this segment */

    u32 Flags; /* Segment flags */
} MACHO_SEGMENT_COMMAND, *PMACHO_SEGMENT_COMMAND;

typedef struct {
    char SectionName[16]; /* Name of this section */
    char SegmentName[16]; /* Segment this section goes in */

    u32 Address; /* Memory address of this section */
    u32 Size; /* Size of this section in bytes */
    u32 Offset; /* File offset for this section */

    u32 Alignment; /* Alignment of section */

    u32 RelocationOffset; /* File offset of relocation entries */
    u32 NumberOfRelocation; /* Number of relocation entries */

    u32 Flags; /* Section flags */

    u32 Reserved1;
    u32 Reserved2;
} MACHO_SECTION, *PMACHO_SECTION;

extern MACHO_HEADER _mh_execute_header; /* Defined by Mach-O linker */

extern u8 *GetSectionDataFromHeader(PMACHO_HEADER Header, const char *SegmentName, const char *SectionName,
                                    u32 *Size);