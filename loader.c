#include <fldrwrapper.h>

mach_boot_parms *mach_bp;
PBOOTINFO BootInfo;
u8 *FreeldrPtr = NULL;
u32 FreeldrLen = 0;
char *FrLdrCmdLine;

#define MULTIBOOT_SEARCH 8192

// C entry point.
void c_entry(u32 args) {
    // set up bootArgs
    mach_bp = (mach_boot_parms *) args;
    // set up screen
    SetupScreen();
    // Parse command line
    SetupCmdline(mach_bp->cmdline);
    // Fixup Apple TV IDE controller
    AppleTVFixupIdeController();
    // Create the info structure and copy it to the correct location
    CreateBootInfo();
    // Load freeldr into memory
    printf("Loading FreeLoader...\n");
    u32 EntryPoint = LoadFreeldr();
    // Clone the ACPI and SMBIOS entries to low memory so that Windows detects them.
    LegacyAcpiSmbiosFix();
    // For debugging purposes
    if(WrapperVerbose) {
        sleep(2);
        ClearScreen(FALSE);
    }
    // Jump to freeldr
    JumpToFreeldr(EntryPoint);
}

// Create boot struct for use with freeldr.
void CreateBootInfo() {
    BootInfo->MagicNumber = ATV_LOADER_MAGIC_NUMBER;

    BootInfo->MemoryMapAddr = 0x003F0000;
    FillMultibootMemoryMap(BootInfo);
    PrintMultibootMemoryMap(BootInfo);

    BootInfo->VideoBaseAddr = mach_bp->video.addr;
    BootInfo->VideoPitch = mach_bp->video.rowb;
    BootInfo->VideoWidth = (mach_bp->video.rowb / 4);
    BootInfo->VideoHeight = mach_bp->video.height;
    BootInfo->VideoDepth = mach_bp->video.depth;

    BootInfo->GlobalSystemTable = (u32) mach_bp->efi_sys_tbl;

    /* Copy bootinfo to proper location */
    memcpy((void *) BOOTINFO_LOC, BootInfo, sizeof(BOOTINFO));
}

// Validate executable header to be FreeLoader
u32 ValidateFreeldr() {
    u32 FreeldrOffset = 0;
    // Find FreeLoader segment in the mach_kernel.
    FreeldrPtr = (u8 *) getsectdatafromheader(&_mh_execute_header, "__TEXT", "__freeldr", &FreeldrLen);
    // Look for FreeLoader magic number
    debug_printf("Looking for FreeLoader magic number...");
    for(size_t i = 0; i < MULTIBOOT_SEARCH; i += 4) {
        u32 *current_addr = (u32 *) (FreeldrPtr + i);

        if (*current_addr == FREELDR_MAGIC_NUMBER) {
            debug_printf("Found magic number 0x%08X at offset 0x%lX\n", FREELDR_MAGIC_NUMBER, i);
            FreeldrOffset = i;
            break;
        }
    }
    if(FreeldrOffset == 0) {
        ChangeColors(0xFFFF0000, 0x00000000);
        printf("FATAL: Version of FreeLoader corrupted or not for Apple TV!\n");
        fail();
    }
    return FreeldrOffset;
}

// Load FreeLoader to the correct location in memory and calculate the entry point.
u32 LoadFreeldr() {
    u32 FreeldrOffset = ValidateFreeldr();
    PFLDRHEADER hdr;
    hdr = (PFLDRHEADER) (FreeldrPtr + FreeldrOffset);
    // Copy FreeLoader into the specified location.
    debug_printf("Copying FreeLoader to 0x%08X...", hdr->LoadAddress);
    memcpy((void *) hdr->LoadAddress, FreeldrPtr, FreeldrLen);
    debug_printf("done.\n");
    return hdr->EntryPoint;
}

void SetupCmdline(const char MachCmdLine[]) {
    FrLdrCmdLine = (char *) CMDLINE_LOC;
    /* Check if we should boot to verbose mode */
    if(strstr(MachCmdLine, "-v")) {
        /* Enable verbose printing in freeldr-wrapper-appletv */
        ClearScreen(TRUE);
    }
    /* Copy the unparsed Mach command line to FreeLoader */
    memcpy(FrLdrCmdLine, MachCmdLine, MACH_CMDLINE);
    debug_printf("Command line arguments: %s\n", CMDLINE_LOC);
}