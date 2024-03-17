/*
 * PROJECT:     FreeLoader wrapper for Apple TV
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Main loader functions for FreeLoader on the original Apple TV
 * COPYRIGHT:   Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
 */

/* INCLUDES *******************************************************************/

#include <fldrwrapper.h>

/* GLOBALS ********************************************************************/

#define MULTIBOOT_SEARCH 8192

PMACH_BOOTARGS BootArgs;
PBOOTINFO BootInfo;
u8 *FreeldrPtr = NULL;
u32 FreeldrLen = 0;

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 2

char ScreenString[] = "debug=debugport=screen ";

/* FUNCTIONS ******************************************************************/

/* Create boot info struct */
static
void CreateBootInfo() {
    BootInfo->MagicNumber = ATV_LOADER_MAGIC_NUMBER;

    BootInfo->MemoryMapAddr = 0x003F0000;
    FillMultibootMemoryMap(BootInfo);
    PrintMultibootMemoryMap(BootInfo);

    BootInfo->VideoBaseAddr = BootArgs->Video.BaseAddress;
    BootInfo->VideoPitch = BootArgs->Video.Pitch;
    BootInfo->VideoWidth = (BootArgs->Video.Pitch / 4);
    BootInfo->VideoHeight = BootArgs->Video.Height;
    BootInfo->VideoDepth = BootArgs->Video.Depth;
    BootInfo->GlobalSystemTable = BootArgs->EfiSystemTable;

    /* Copy bootinfo to proper location */
    memcpy((void *) BOOTINFO_LOC, BootInfo, sizeof(BOOTINFO));
}

/* Validate FreeLoader segment */
static
u32 ValidateFreeldr() {
    u32 FreeldrOffset = 0;
    /* Find FreeLoader segment in the mach_kernel. */
    FreeldrPtr = GetSectionDataFromHeader(&_mh_execute_header, "__TEXT", "__freeldr",
                                          &FreeldrLen);
    /* Look for FreeLoader magic number */
    debug_printf("Looking for FreeLoader magic number...");
    for (size_t i = 0; i < MULTIBOOT_SEARCH; i += 4) {
        u32 *current_addr = (u32 *) (FreeldrPtr + i);
        if (*current_addr == FREELDR_MAGIC_NUMBER) {
            debug_printf("Found magic number 0x%08X at offset 0x%lX\n", FREELDR_MAGIC_NUMBER, i);
            FreeldrOffset = i;
            break;
        }
    }
    if (FreeldrOffset == 0) {
        printf("FATAL: Version of FreeLoader corrupted or not for Apple TV!\n");
        fail();
    }
    return FreeldrOffset;
}

/* Load FreeLoader to the correct location in memory and calculate the entry point. */
static
u32 LoadFreeldr() {
    u32 FreeldrOffset = ValidateFreeldr();
    PFLDRHEADER Header;
    Header = (PFLDRHEADER) (FreeldrPtr + FreeldrOffset);
    /* Copy FreeLoader into the specified location. */
    debug_printf("Copying FreeLoader to 0x%08X...", Header->LoadAddress);
    memcpy((void *) Header->LoadAddress, FreeldrPtr, FreeldrLen);
    debug_printf("done.\n");
    return Header->EntryPoint;
}

/* Set up command line and enable verbose mode */
static
void SetupCmdline() {
    char *CmdLine = (char *) CMDLINE_LOC;
    /* Check if we should enable verbose mode */
    if (strstr(BootArgs->CmdLine, "-v") || strstr(BootArgs->CmdLine, "-s")) {
        /* Enable verbose printing in freeldr-wrapper-appletv */
        ClearScreen(TRUE);
        debug_printf("Booting in Verbose Mode. ");
        /* Add screen debug to command line */
        memcpy(CmdLine, ScreenString, sizeof(ScreenString));
        CmdLine += sizeof(ScreenString) - 1;
    }
    /* Copy the unparsed Mach command line to FreeLoader */
    memcpy(CmdLine, BootArgs->CmdLine, MACH_CMDLINE);
    debug_printf("Command line arguments: %s\n", CMDLINE_LOC);
}

/* C entry point. */
void WrapperInit(u32 BootArgPtr) {
    /* set up bootArgs */
    BootArgs = (PMACH_BOOTARGS) BootArgPtr;
    /* set up screen */
    SetupScreen();
    /* Parse command line */
    SetupCmdline();

    debug_printf("FreeLoader wrapper for Apple TV version %d.%d.%d (built with %s on %s %s) [%s@%s]\n",
                 VERSION_MAJOR,
                 VERSION_MINOR,
                 VERSION_PATCH,
                 __VERSION__,
                 __DATE__,
                 __TIME__,
                 __BUILD_USER__,
                 __BUILD_HOST__
                 );
    /* Fixup Apple TV IDE controller */
    AppleTVFixupIdeController();
    /* Create the info structure and copy it to the correct location */
    CreateBootInfo();
    /* Load freeldr into memory */
    printf("Loading FreeLoader...\n");
    u32 EntryPoint = LoadFreeldr();
    /* Clone the ACPI and SMBIOS entries to low memory so that Windows detects them. */
    LegacyAcpiSmbiosFix();
    /* For debugging purposes */
    if (WrapperVerbose) {
        debug_printf("DEBUG: Pausing here for 5 seconds.\n");
        sleep(5);
        ClearScreen(FALSE);
    }
    /* Jump to freeldr */
    JumpToFreeldr(EntryPoint);
}