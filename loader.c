/**
 * Multiboot support for atv-playground - C functions
 * Multiboot 1 does not officially support UEFI, but should be mostly usable on it. Boot loaders requiring VBE modes
 * will not work without modification.
 */

#include <atvlib.h>

handoff_boot_info *handoffBootInfo;
u8 *FreeldrPtr = NULL;
u32 FreeldrLen = 0;

#define MULTIBOOT_SEARCH 8192

const struct section *getsectbynamefromheader(struct mach_header *mhp, const char *segname, const char *sectname)
{
    struct segment_command *sgp;
    struct section         *sp;
    long                   i, j;

    //printk("getsectbynamefromheader\n");
    sgp = (struct segment_command *) ((char *) mhp + sizeof(struct mach_header));
    for (i = 0; i < mhp->ncmds; i++) {
        if (sgp->cmd == LC_SEGMENT) {
            //printk("sgp->segname = %s, matching to %s.\n", sgp->segname, segname);

            //if (strncmp(sgp->segname, segname, sizeof(sgp->segname)) == 0 || mhp->filetype == MH_OBJECT) {
            if (strncmp(sgp->segname, segname, strlen(sgp->segname)) == 0 || mhp->filetype == MH_OBJECT) {
                sp = (struct section *) ((char *) sgp + sizeof(struct segment_command));
                for (j = 0; j < sgp->nsects; j++) {
                    // if (strncmp(sp->sectname, sectname, sizeof(sp->sectname)) == 0 &&
                    //  strncmp(sp->segname,  segname,  sizeof(sp->segname)) == 0) {
                    if (strncmp(sp->sectname, sectname, strlen(sp->sectname)) == 0 &&
                        strncmp(sp->segname, segname, strlen(sp->segname)) == 0) {
                        printk("Found %s,%s ", segname, sectname);
                        return (sp);
                    }
                    //
                    sp = (struct section *)((char *) sp + sizeof(struct section));
                }
            }
        }
        sgp = (struct segment_command *)((char *)sgp + sgp->cmdsize);
    }
    ChangeColors(0xFFFF0000, 0x00000000);
    printk_always("FATAL: Could not find FreeLoader in executable!\n", segname, sectname);
    fail();
    //
    return ((struct section *) 0);
}
/**********************************************************************/
// This routine returns the a pointer to the data for the named section in the
// named segment if it exist in the mach header passed to it.  Also it returns
// the size of the section data indirectly through the pointer size.  Otherwise
//  it returns zero for the pointer and the size.
char *getsectdatafromheader(struct mach_header *mhp, const char *segname, const char *sectname, unsigned long *size)
{
    const struct section	*sp;

    // printk("getsectdatafromheader\n");
    sp = getsectbynamefromheader(mhp, segname, sectname);
    if (sp == (struct section *) 0) {
        *size = 0;
        return ((char*) 0);
    }
    *size = sp->size;
    //
    printk("@ 0x%08X size %i\n", sp->addr, sp->size);
    return ((char *) (sp->addr));
}

/**
 * Convert EFI memory map to Multiboot memory map to be passed to freeldr.
 * This code is based on a Linux kernel patch submitted by Edgar Hucek and modified for use with a Multiboot
 * memory map.
 */
void quirk_fixup_efi_memmap(handoff_boot_info *handoff)
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

    num_maps = handoff->efi_map.size / handoff->efi_map.descriptor_size;

    ChangeColors(0xFFFF00FF, 0x00000000);
    printk("[QUIRK] Fixing EFI memory map...\n");

    // gather up the offending memory ranges
    // these are the two EFI_RUNTIME_SERVICES_CODE and one EFI_RUNTIME_SERVICES_DATA
    // memmap sections. This routine assumes that the sections will appear in order
    // which they seem to always do for the appleTV
    for (i = 0, p = (efi_memory_desc_t*)handoff->efi_map.addr; i < num_maps; i++) {
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
        p = NextEFIMemoryDescriptor(p, handoff->efi_map.descriptor_size);
    }

    for (i = 0, p = (efi_memory_desc_t*)handoff->efi_map.addr; i < num_maps; i++) {

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

            // printk("[QUIRK] Found memory overlap\n");
            // printk("[QUIRK] Memory range=[0x%08X%08X-", hi32(bgn), lo32(bgn) );
            // printk("0x%08X%08X]\n", hi32(end), lo32(end) );

            new_bgn = end_match;
            new_pages = (end - new_bgn) / (1 << EFI_PAGE_SHIFT);

            new_end = new_bgn + (new_pages << EFI_PAGE_SHIFT);
            printk("[QUIRK] Fixing memory overlap...\n");
            printk("[QUIRK] Memory range=[0x%08X%08X-", hi32(new_bgn), lo32(new_bgn) );
            printk("0x%08X%08X]\n", hi32(new_end), lo32(new_end) );

            md->phys_addr = new_bgn;
            md->num_pages = new_pages;
        }

        p = NextEFIMemoryDescriptor(p, handoff->efi_map.descriptor_size);
    }

    for (i = 0, p = (efi_memory_desc_t*)handoff->efi_map.descriptor_size; i < num_maps; i++) {
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
            printk("[QUIRK] Fixing memory target...\n");

            md->phys_addr = new_bgn;
            md->num_pages = new_pages;

            md->num_pages = new_pages;
        }

        p = NextEFIMemoryDescriptor(p, handoff->efi_map.descriptor_size);
    }
    printk("[QUIRK] Fixup complete.\n");
    ChangeColors(0xFFFFFFFF, 0x00000000);
}

// Add memory region to the multiboot memory map.
void AddMemoryRegion(struct mmap_entry *map, u32 *NumberOfEntries, u64 addr, u64 len, u32 type) {

    u32 x = *NumberOfEntries;

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
void FillMultibootMemoryMap(handoff_boot_info *handoff) {
    u32 EfiNumberOfEntries, MultibootNumberOfEntries = 0, i;
    u64 start, end, size;
    efi_memory_desc_t *md, *p;
    struct mmap_entry *MultibootMapEntry;

    EfiNumberOfEntries = handoff->efi_map.size / handoff->efi_map.descriptor_size;
    //printk("Number of EFI memory map entries: %i\n", EfiNumberOfEntries);
    MultibootMapEntry = (struct mmap_entry *) handoff->multiboot_map.addr;

    for(i = 0, p = (efi_memory_desc_t *) handoff->efi_map.addr; i < EfiNumberOfEntries; i++) {
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
                printk("ERROR: default add_memory_region, should not see this\n");
                AddMemoryRegion(MultibootMapEntry, &MultibootNumberOfEntries,
                                md->phys_addr,
                                md->num_pages << EFI_PAGE_SHIFT,
                                MULTIBOOT_MEMORY_RESERVED);
                break;
        }
        p = (efi_memory_desc_t *) NextEFIMemoryDescriptor(p, handoff->efi_map.descriptor_size);
    }
    // printk("Number of multiboot entries: %i\n", MultibootNumberOfEntries);
    handoff->multiboot_map.size = sizeof(struct mmap_entry) * MultibootNumberOfEntries;
    handoff->multiboot_map.entries = MultibootNumberOfEntries;
}

void PrintMultibootMemoryMap(handoff_boot_info *handoff) {
    int	i;
    struct mmap_entry *MultibootMapEntry;
    MultibootMapEntry = (struct mmap_entry *) handoff->multiboot_map.addr;


    for (i = 0; i < handoff->multiboot_map.entries; i++) {
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
    printk("Looking for FreeLoader magic number...");
    for(size_t i = 0; i < MULTIBOOT_SEARCH; i += 4) {
        u32 *current_addr = (u32 *) (FreeldrPtr + i);

        if (*current_addr == FREELDR_MAGIC_NUMBER) {
            printk("Found magic number 0x%08X at offset 0x%lX\n", FREELDR_MAGIC_NUMBER, i);
            FreeldrOffset = i;
        }
    }
    if(FreeldrOffset == 0) {
        ChangeColors(0xFFFF0000, 0x00000000);
        printk_always("FATAL: Version of FreeLoader corrupted or not for Apple TV!\n");
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
    printk("Copying FreeLoader to 0x%08X...", hdr->load_addr);
    memcpy((void *) hdr->load_addr, FreeldrPtr, FreeldrLen);
    printk("done.\n");
    return hdr->entry_point;
}

// Jump to freeldr code. This is defined in jump.S
extern void JumpToFreeldr(u32 start, u32 info);

// Put everything together
void load_freeldr() {
    // zero out low memory
    memset((void *) 0x0000, 0x0, 0xF000);
    // Fixup Apple TV IDE controller.
    AppleTVFixupIdeController();
    // Create the info structure.
    CreateBootInfo(handoffBootInfo);
    // Clone the ACPI and SMBIOS entries to low memory so that Windows detects them.
    LegacyAcpiSmbiosFix();
    // Load freeldr into memory
    printk_always("Loading FreeLoader...\n");
    u32 EntryPoint = LoadFreeldr();
    // Jump to assembly loader
    JumpToFreeldr(EntryPoint, (u32) &handoffBootInfo);
}