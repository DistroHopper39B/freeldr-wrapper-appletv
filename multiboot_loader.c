/**
 * Multiboot support for atv-playground - C functions
 * Multiboot 1 does not officially support UEFI, but should be mostly usable on it. Boot loaders requiring VBE modes
 * will not work without modification.
 */

#include <atvlib.h>
#include <multiboot.h>

multiboot_info_t *multibootInfo;
u8 *MultibootExecutablePtr = NULL;
u32 MultibootLen = 0;

#define MULTIBOOT_FLAGS ( MULTIBOOT_INFO_BOOTDEV | MULTIBOOT_INFO_CMDLINE | \
MULTIBOOT_INFO_MEM_MAP | MULTIBOOT_INFO_BOOT_LOADER_NAME | MULTIBOOT_INFO_FRAMEBUFFER_INFO)

#define MULTIBOOT_SEARCH 8192
/**
 * Convert EFI memory map to Multiboot memory map to be passed to freeldr.
 * This code is based on a Linux kernel patch submitted by Edgar Hucek and modified for use with a Multiboot
 * memory map.
 */
void quirk_fixup_efi_memmap(multiboot_info_t *mb)
{
    /* November 26, 2007 -- Scott Davilla (davilla@4pi.com)
      The appletv efi firmware has a bug that effects linux kernel when
    booting from efi. Three EFI RunTime Services Code/Data segments overlap
    a declared free ememory segment. This can cause code/data overwrites
    and result in unknown crashes/hangs when running linux.
    */
    u32                     num_maps, i;
    UINT64                  bgn, end, bgn_match, end_match;
    efi_memory_desc_t       *md, *p;

    bgn_match = end_match = -1;

    num_maps = mb->efi_memory_map_size/mb->efi_memory_descriptor_size;

    ChangeColors(0xFFFF00FF, 0x00000000);
    printk("[QUIRK] Fixing EFI memory map...\n");

    // gather up the offending memory ranges
    // these are the two EFI_RUNTIME_SERVICES_CODE and one EFI_RUNTIME_SERVICES_DATA
    // memmap sections. This routine assumes that the sections will appear in order
    // which they seem to always do for the appleTV
    for (i = 0, p = (efi_memory_desc_t*)mb->efi_memory_map_addr; i < num_maps; i++) {
        md = p;

        bgn = md->phys_addr;
        end = md->phys_addr + (md->num_pages << EFI_PAGE_SHIFT);
        //
        switch (md->type) {
            case EFI_RUNTIME_SERVICES_CODE:
            case EFI_RUNTIME_SERVICES_DATA:
                if (bgn_match == -1) {
                    bgn_match = md->phys_addr;
                    end_match = md->phys_addr + (md->num_pages << EFI_PAGE_SHIFT);
                } else {
                    if (end_match == md->phys_addr) {
                        end_match = md->phys_addr + (md->num_pages << EFI_PAGE_SHIFT);
                    }
                }

                break;
        }
        p = NextEFIMemoryDescriptor(p, mb->efi_memory_descriptor_size);
    }

    for (i = 0, p = (efi_memory_desc_t*)mb->efi_memory_map_addr; i < num_maps; i++) {

        md = p;

        bgn = md->phys_addr;
        end = md->phys_addr + (md->num_pages << EFI_PAGE_SHIFT);

        /*
        printk("mem%02d: type=%d, ", i, md->Type );
        printk("attr=0x%08X%08X\n", hi32(md->Attribute), lo32(md->Attribute) );
        printk("   range=[0x%08X%08X-", hi32(bgn), lo32(bgn) );
        printk("0x%08X%08X], ", hi32(end), lo32(end) );
        printk("%dMB\n", lo32(md->NumberOfPages >> (20 - EFI_PAGE_SHIFT)) );
        */

        // find problem free memory segment */
        if ( (bgn == bgn_match) & (end >= end_match) ) {
            UINT64		new_bgn, new_end, new_pages;

            printk("[QUIRK] Found memory overlap\n");
            printk("[QUIRK] Memory range=[0x%08X%08X-", hi32(bgn), lo32(bgn) );
            printk("0x%08X%08X]\n", hi32(end), lo32(end) );

            new_bgn = end_match;
            new_pages = (end - new_bgn) / (1 << EFI_PAGE_SHIFT);

            new_end = new_bgn + (new_pages << EFI_PAGE_SHIFT);
            printk("[QUIRK] Fixing memory overlap\n");
            printk("[QUIRK] Memory range=[0x%08X%08X-", hi32(new_bgn), lo32(new_bgn) );
            printk("0x%08X%08X]\n", hi32(new_end), lo32(new_end) );

            md->phys_addr = new_bgn;
            md->num_pages = new_pages;
        }

        p = NextEFIMemoryDescriptor(p, mb->efi_memory_descriptor_size);
    }

    for (i = 0, p = (efi_memory_desc_t*)mb->efi_memory_map_addr; i < num_maps; i++) {
        UINT64   target;

        target = 0x025AE000;
        md = p;

        bgn = md->phys_addr;
        end = md->phys_addr + (md->num_pages << EFI_PAGE_SHIFT);

        if ( (bgn < target) & (end > target) ) {
            UINT64          new_bgn, new_end, new_pages;


            new_bgn = bgn;
            new_pages = (target - new_bgn) / (1 << EFI_PAGE_SHIFT);

            new_end = new_bgn + (new_pages << EFI_PAGE_SHIFT);
            printk("[QUIRK] Fixing memory target\n");

            md->phys_addr = new_bgn;
            md->num_pages = new_pages;

            md->num_pages = new_pages;
        }

        p = NextEFIMemoryDescriptor(p, mb->efi_memory_descriptor_size);
    }
    printk("[QUIRK] Fixup complete.\n");
    ChangeColors(0xFFFFFFFF, 0x00000000);
}

// Add memory region to the multiboot memory map.
void AddMemoryRegion(struct multiboot_mmap_entry *map, u32 *NumberOfEntries, u64 addr, u64 len, u32 type) {

    u32 x = *NumberOfEntries;

    if(x > 128) {
        printk("I don't know what you did\n");
        fail();
    }

    if ((x > 0) && map[x-1].addr + map[x-1].len == addr
        && map[x-1].type == type)
        map[x-1].len += len;
    else {
        map[x].addr = addr;
        map[x].len = len;
        map[x].type = type;
        (*NumberOfEntries)++;
    }

}

// Convert EFI memory map to Multiboot memory map.
void FillMultibootMemoryMap(multiboot_info_t *mb) {
    u32 EfiNumberOfEntries, MultibootNumberOfEntries = 0, i;
    u64 start, end, size;
    efi_memory_desc_t *md, *p;
    struct multiboot_mmap_entry *MultibootMapEntry;

    EfiNumberOfEntries = mb->efi_memory_map_size / mb->efi_memory_descriptor_size;
    //printk("Number of EFI memory map entries: %i\n", EfiNumberOfEntries);
    MultibootMapEntry = (struct multiboot_mmap_entry *) mb->mmap_addr;

    for(i = 0, p = (efi_memory_desc_t *) mb->efi_memory_map_addr; i < EfiNumberOfEntries; i++) {
        md = p;
        switch (md->type) {
            // ACPI tables -- to be preserved by loader/OS until ACPI is enable
            // once enabled, can be treated as conventional memory
            case EFI_ACPI_RECLAIM_MEMORY:
                AddMemoryRegion(MultibootMapEntry, &MultibootNumberOfEntries,
                                md->phys_addr,
                                md->num_pages << EFI_PAGE_SHIFT,
                                MULTIBOOT_MEMORY_ACPI_RECLAIMABLE);
                break;
                // must be preserved by loader/OS in working an ACPI S1-S3 states
            case EFI_RUNTIME_SERVICES_CODE:
            case EFI_RUNTIME_SERVICES_DATA:
            case EFI_RESERVED_TYPE:
            case EFI_MEMORY_MAPPED_IO:
            case EFI_MEMORY_MAPPED_IO_PORT_SPACE:
            case EFI_UNUSABLE_MEMORY:
            case EFI_PAL_CODE:
                AddMemoryRegion(MultibootMapEntry, &MultibootNumberOfEntries,
                                md->phys_addr,
                                md->num_pages << EFI_PAGE_SHIFT,
                                MULTIBOOT_MEMORY_RESERVED);
                break;
                // can be treaded as conventional memory by loader/OS
            case EFI_LOADER_CODE:
            case EFI_LOADER_DATA:
            case EFI_BOOT_SERVICES_CODE:
            case EFI_BOOT_SERVICES_DATA:
            case EFI_CONVENTIONAL_MEMORY:
                start = md->phys_addr;
                size = md->num_pages << EFI_PAGE_SHIFT;
                end = start + size;
                /* Fix up for BIOS that claims RAM in 640K-1MB region */
                if (start < 0x100000ULL && end > 0xA0000ULL) {
                    if (start < 0xA0000ULL) {
                        /* start < 640K
                         * set memory map from start to 640K
                         */
                        AddMemoryRegion(MultibootMapEntry,
                                        &MultibootNumberOfEntries,
                                        start,
                                        0xA0000ULL - start,
                                        MULTIBOOT_MEMORY_AVAILABLE);
                    }
                    if (end <= 0x100000ULL) {
                        continue;
                    }
                    // end > 1MB, set memory map avoiding 640K to 1MB hole
                    start = 0x100000ULL;
                    size = end - start;
                }
                AddMemoryRegion(MultibootMapEntry, &MultibootNumberOfEntries,
                                start, size, MULTIBOOT_MEMORY_AVAILABLE);
                break;
                // ACPI working memory --- should be preserved by loader/OS in the working
                //  and ACPI S1-S3 states
            case EFI_ACPI_MEMORY_NVS:
                AddMemoryRegion(MultibootMapEntry, &MultibootNumberOfEntries,
                                md->phys_addr,
                                md->num_pages << EFI_PAGE_SHIFT,
                                MULTIBOOT_MEMORY_NVS);
                break;
            default:
                /* We should not hit this case */
                printk("ATV: default add_memory_region, should not see this\n");
                AddMemoryRegion(MultibootMapEntry, &MultibootNumberOfEntries,
                                md->phys_addr,
                                md->num_pages << EFI_PAGE_SHIFT,
                                MULTIBOOT_MEMORY_RESERVED);
                break;
        }
        p = (efi_memory_desc_t *) NextEFIMemoryDescriptor(p, mb->efi_memory_descriptor_size);
    }
    //printk("Number of multiboot entries: %i\n", MultibootNumberOfEntries);
    mb->mmap_length = sizeof(struct multiboot_mmap_entry) * MultibootNumberOfEntries;
}

void PrintMultibootMemoryMap(multiboot_info_t *mb) {
    int	i;
    struct multiboot_mmap_entry *MultibootMapEntry;
    MultibootMapEntry = (struct multiboot_mmap_entry *) mb->mmap_addr;


    for (i = 0; i < mb->mmap_length / sizeof(struct multiboot_mmap_entry); i++) {
        printk("%s: 0x%08X%08X - 0x%08X%08X ", "Multiboot Memory Map",
               hi32( MultibootMapEntry[i].addr ),
               lo32( MultibootMapEntry[i].addr ),
               hi32( MultibootMapEntry[i].addr + MultibootMapEntry[i].len),
               lo32( MultibootMapEntry[i].addr + MultibootMapEntry[i].len) );
        switch (MultibootMapEntry[i].type) {
            case MULTIBOOT_MEMORY_AVAILABLE:
                printk("(usable)\n");
                break;
            case MULTIBOOT_MEMORY_RESERVED:
                printk("(reserved)\n");
                break;
            case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
                printk("(ACPI data)\n");
                break;
            case MULTIBOOT_MEMORY_NVS:
                printk("(ACPI NVS)\n");
                break;
            default:
                printk("type %u\n", MultibootMapEntry[i].type);
                break;
        }
    }

}

/**
 * On EFI systems, such as the Apple TV, the ACPI and SMBIOS tables are in a different location than where Windows and
 * other legacy OSes expect them to be. We need to copy them to the correct memory regions here.
 * This code is based on atv-bootloader.
 */
void LegacyAcpiSmbiosFix() {
    efi_system_table_t	*system_table;
    efi_config_table_t	*config_tables;
    struct efi_tables efitab;
    int					i, num_config_tables;

    system_table		= (efi_system_table_t*)mach_bp->efi_sys_tbl;
    num_config_tables	= system_table->nr_tables;
    config_tables		= (efi_config_table_t*)system_table->tables;

    // Let's see what config tables the efi firmware passed to us.
    for (i = 0; i < num_config_tables; i++) {
        if (efi_guidcmp(config_tables[i].guid, MPS_TABLE_GUID) == 0) {
            efitab.mps = (void*)config_tables[i].table;
            //printk(" MPS=0x%lx ", config_tables[i].table);
            //
        } else if (efi_guidcmp(config_tables[i].guid, ACPI_20_TABLE_GUID) == 0) {
            efitab.acpi20 = (void*)config_tables[i].table;
            //printk(" ACPI 2.0=0x%lx ", config_tables[i].table);
            //
        } else if (efi_guidcmp(config_tables[i].guid, ACPI_TABLE_GUID) == 0) {
            efitab.acpi = (void*)config_tables[i].table;
            //printk(" ACPI=0x%lx ", config_tables[i].table);
            //
        } else if (efi_guidcmp(config_tables[i].guid, SMBIOS_TABLE_GUID) == 0) {
            efitab.smbios = (void*) config_tables[i].table;
            //printk(" SMBIOS=0x%lx ", config_tables[i].table);
            //
        } else if (efi_guidcmp(config_tables[i].guid, HCDP_TABLE_GUID) == 0) {
            //efi.hcdp = (void*)config_tables[i].table;
            //printk(" HCDP=0x%lx ", config_tables[i].table);
            //
        } else if (efi_guidcmp(config_tables[i].guid, UGA_IO_PROTOCOL_GUID) == 0) {
            //efi.uga = (void*)config_tables[i].table;
            //printk(" UGA=0x%lx ", config_tables[i].table);
        }
    }
    //printk("\n");

    // rsdp_low_mem is unsigned long so alignment below works
    unsigned long		rsdp_low_mem   = 0xF8000;
    unsigned long		smbios_low_mem = 0xF8100;
    //
    printk("Cloning ACPI entry from 0x%08X to 0x%lX...", efitab.acpi20, rsdp_low_mem);
    // We need at copy the RSDP down low so linux can find it
    // copy RSDP table entry from efi location to low mem location
    memcpy((void*)rsdp_low_mem, efitab.acpi20, sizeof(acpi_rsdp_t) );
    printk("done.\n");

    printk("Cloning SMBIOS entry from 0x%08X to 0x%lX...", efitab.smbios, smbios_low_mem);
    // We need at copy the SMBIOS Table Entry Point down low so linux can find it
    // copy SMBIOS Table Entry Point from efi location to low mem location
    memcpy((void*)smbios_low_mem, efitab.smbios, sizeof(smbios_entry_t) );
    printk("done.\n");
}

const char BootloaderName[] = "atv-playground";

// Create Multiboot information structure to pass to bootloader.
u32 *CreateMultibootInfoStructure(multiboot_info_t *mb) {
    //printk("Multiboot flags: 0x%08X\n", MULTIBOOT_FLAGS);
    mb->flags = MULTIBOOT_FLAGS;


    mb->boot_device = 0; // The root partition should always be 0, but I don't know if I will handle this

    mb->cmdline = (multiboot_uint32_t) mach_bp->cmdline;

    // Setup memory map (including EFI memory map)
    mb->efi_memory_map_addr = mach_bp->efi_mem_map;
    mb->efi_memory_map_size = mach_bp->efi_mem_map_size;
    mb->efi_memory_descriptor_size = mach_bp->efi_mem_desc_size;
    quirk_fixup_efi_memmap(mb);
    mb->mmap_addr = 0x003F0000;
    FillMultibootMemoryMap(mb); // mb->mmap_length

    mb->drives_length = 0;
    mb->drives_addr = 0;

    mb->config_table = 0;

    mb->boot_loader_name = (multiboot_uint32_t) BootloaderName; // needs to be a pointer

    mb->apm_table = 0; // No APM BIOS present

    mb->framebuffer_addr = mach_bp->video.addr;
    mb->framebuffer_pitch = mach_bp->video.rowb;
    mb->framebuffer_width = mach_bp->video.width;
    mb->framebuffer_height = mach_bp->video.height;
    mb->framebuffer_bpp = mach_bp->video.depth;
    mb->framebuffer_type = MULTIBOOT_FRAMEBUFFER_TYPE_EFI;

    mb->efi_system_table = (multiboot_uint32_t) mach_bp->efi_sys_tbl;

    mb->appletv_kernel_base = mach_bp->kaddr;
    mb->appletv_kernel_size = mach_bp->ksize;
    mb->appletv_kernel_end = (mach_bp->kaddr + mach_bp->ksize);

    //printk("Kernel base: 0x%08X\n", mb->appletv_kernel_base);
    //printk("Kernel size: %i\n", mb->appletv_kernel_size);
    //printk("Kernel end: 0x%08X\n", mb->appletv_kernel_end);
    return (u32 *) mb;
}

// Validate executable header to be Multiboot-compatible and have correct flags.
u32 ValidateMultibootHeader() {
    u32 MultibootHeaderOffset = 0;
    // Find multiboot segment in the mach_kernel.
    MultibootExecutablePtr = (u8 *) getsectdatafromheader(&_mh_execute_header, "__TEXT", "__multiboot", &MultibootLen);
    // Look for multiboot magic number
    printk("Looking for multiboot magic number...");
    for(size_t i = 0; i < MULTIBOOT_SEARCH; i += 4) {
        u32 *current_addr = (u32 *) (MultibootExecutablePtr + i);

        if (*current_addr == MULTIBOOT_HEADER_MAGIC) {
            printk("Found magic number 0x%08X at offset 0x%08X\n", MULTIBOOT_HEADER_MAGIC, i);
            MultibootHeaderOffset = i;
        }
    }
    if(MultibootHeaderOffset == 0) {
        printk("FATAL: Could not find magic number 0x%08X!\n", MULTIBOOT_HEADER_MAGIC);
        fail();
    }
    return MultibootHeaderOffset;
}

// Load the multiboot executable to the correct location in memory and calculate the entry point.

u32 LoadMultibootExecutable() {
    u32 MultibootHeaderOffset = ValidateMultibootHeader();
    struct multiboot_header *MultibootHeader;
    MultibootHeader = (struct multiboot_header *) (MultibootExecutablePtr + MultibootHeaderOffset);
    // Copy the multiboot executable into the specified location.
    printk("Copying multiboot executable to 0x%08X...", MultibootHeader->load_addr);
    memcpy((void *) MultibootHeader->load_addr, MultibootExecutablePtr, MultibootLen);
    printk("done.\n");
    // Figure out our entry point
    //printk("Entry point: 0x%08X\n", MultibootHeader->entry_addr);
    return MultibootHeader->entry_addr;
}


// Jump to multiboot code. This is defined in multiboot.asm
extern void JumpToMultiboot(u32 start, u32 mb_info);

// Put everything together
void load_multiboot() {
    // Create the multiboot info structure.
    CreateMultibootInfoStructure(multibootInfo);
    // Clone the ACPI and SMBIOS entries to low memory so that Windows detects them.
    LegacyAcpiSmbiosFix();
    // Load the multiboot executable into memory
    u32 EntryPoint = LoadMultibootExecutable();
    // Jump to assembly loader
    JumpToMultiboot(EntryPoint, (u32) &multibootInfo);
}