#include <atvlib.h>

mach_boot_parms *mach_bp;
handoff_boot_info *handoffBootInfo;
u8 *FreeldrPtr = NULL;
u32 FreeldrLen = 0;

#define MULTIBOOT_SEARCH 8192

// C entry point.
void c_entry(u32 args) {
    // set up bootArgs
    mach_bp = (mach_boot_parms *) args;
    // set up screen
    SetupScreen();
    // zero out low memory/freeldr stack area
    memset((void *) 0x0000, 0x0, 0xF000);
    // Fixup Apple TV IDE controller
    AppleTVFixupIdeController();
    // Create the info structure.
    CreateBootInfo(handoffBootInfo);
    // Clone the ACPI and SMBIOS entries to low memory so that Windows detects them.
    LegacyAcpiSmbiosFix();
    // Load freeldr into memory
    printf("Loading FreeLoader...\n");
    u32 EntryPoint = LoadFreeldr();
    // Jump to assembly loader
    JumpToFreeldr(EntryPoint, (u32) &handoffBootInfo);
}

// Create boot struct for use with freeldr.
u32 *CreateBootInfo(handoff_boot_info *h) {
    h->magic = ATV_LOADER_MAGIC_NUMBER;

    h->cmdline_ptr = (u32) mach_bp->cmdline;

    h->efi_map.addr = mach_bp->efi_mem_map;
    h->efi_map.size = mach_bp->efi_mem_map_size;
    h->efi_map.descriptor_size = mach_bp->efi_mem_desc_size;
    h->efi_map.descriptor_version = mach_bp->efi_mem_desc_ver;

    quirk_fixup_efi_memmap(h);
    h->multiboot_map.addr = 0x003F0000;
    FillMultibootMemoryMap(h);
    PrintMultibootMemoryMap(h);

    h->video.base = mach_bp->video.addr;
    h->video.pitch = mach_bp->video.rowb;
    h->video.width = mach_bp->video.width;
    h->video.height = mach_bp->video.height;
    h->video.depth = mach_bp->video.depth;

    h->efi_system_table_ptr = (u32) mach_bp->efi_sys_tbl;

    h->kernel.base_addr = mach_bp->kaddr;
    h->kernel.size = mach_bp->ksize;
    h->kernel.end = (mach_bp->kaddr + mach_bp->ksize);

    return (u32 *) h;
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
    freeldr_hdr *hdr;
    hdr = (freeldr_hdr *) (FreeldrPtr + FreeldrOffset);
    // Copy FreeLDR into the specified location.
    debug_printf("Copying FreeLoader to 0x%08X...", hdr->load_addr);
    memcpy((void *) hdr->load_addr, FreeldrPtr, FreeldrLen);
    debug_printf("done.\n");
    return hdr->entry_point;
}
