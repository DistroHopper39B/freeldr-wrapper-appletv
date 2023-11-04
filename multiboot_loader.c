/**
 * Multiboot support for atv-playground - C functions
 * Multiboot 1 does not officially support UEFI, but should be mostly usable on it. Boot loaders requiring VBE modes
 * will not work without modification.
 */

#include <atvlib.h>
#include <multiboot.h>

//multiboot_info_t *mb;
u8 *MultibootPtrOrig = NULL;
u32 MultibootLen = 0;

#define MULTIBOOT_FLAGS (MULTIBOOT_INFO_MEMORY | MULTIBOOT_INFO_BOOTDEV | MULTIBOOT_INFO_CMDLINE | \
MULTIBOOT_INFO_MEM_MAP | MULTIBOOT_INFO_BOOT_LOADER_NAME | MULTIBOOT_INFO_FRAMEBUFFER_INFO)
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
    printk("Number of EFI memory map entries: %i\n", EfiNumberOfEntries);
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
    printk("Number of multiboot entries: %i\n", MultibootNumberOfEntries);
    mb->mmap_length = sizeof(struct multiboot_mmap_entry) * MultibootNumberOfEntries;
}


// Create Multiboot information structure to pass to bootloader.
void CreateMultibootInfoStructure(multiboot_info_t *mb) {
    printk("Multiboot flags: 0x%08X\n", MULTIBOOT_FLAGS);
    mb->flags = MULTIBOOT_FLAGS;

    mb->mem_lower = 0xDEAD; // we need more stuff
    mb->mem_upper = 0xDEAD; // we need more stuff

    mb->boot_device = 0; // The root partition should always be 0, but I don't know if I will handle this

    mb->cmdline = 0xBEEF; // I think this has to be a pointer?

    mb->mods_count = 0;
    mb->mods_addr = 0;

    //Setup memory map (including EFI memory map)
    mb->efi_memory_map_addr = mach_bp->efi_mem_map;
    mb->efi_memory_map_size = mach_bp->efi_mem_map_size;
    mb->efi_memory_descriptor_size = mach_bp->efi_mem_desc_size;
    quirk_fixup_efi_memmap(mb);
    mb->mmap_addr = 0x001F0000;
    FillMultibootMemoryMap(mb); // mb->mmap_length

    mb->drives_length = 0;
    mb->drives_addr = 0;

    mb->config_table = 0;

    mb->boot_loader_name = 0xBEEF; // needs to be a pointer

    mb->apm_table = 0; // No APM BIOS present

    mb->framebuffer_addr = mach_bp->video.addr;
    mb->framebuffer_pitch = mach_bp->video.rowb;
    mb->framebuffer_width = mach_bp->video.width;
    mb->framebuffer_height = mach_bp->video.height;
    mb->framebuffer_bpp = mach_bp->video.depth;
    mb->framebuffer_type = MULTIBOOT_FRAMEBUFFER_TYPE_EFI;

    mb->efi_runtime_services = (multiboot_uint32_t) mach_bp->efi_sys_tbl->runtime;
}

// Validate executable header to be Multiboot-compatible and have correct flags.
void ValidateMultibootHeader() {
    // Find multiboot in the executable.
    MultibootPtrOrig = (u8 *) getsectdatafromheader(&_mh_execute_header, "__TEXT", "__stage2", &MultibootLen);
    printk("@ 0x%08X size %i\n", MultibootPtrOrig, MultibootLen);
}

void PrintMultibootMemoryMap(multiboot_info_t *mb) {
    int					i;
    struct multiboot_mmap_entry *MultibootMapEntry;
    MultibootMapEntry = (struct multiboot_mmap_entry *) mb->mmap_addr;
    

    for (i = 0; i < mb->mmap_length / sizeof(struct multiboot_mmap_entry); i++) {
        printk("ATV: %s: 0x%08X%08X - 0x%08X%08X ", "Multiboot Memory Map",
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

// Jump to multiboot code. This is defined in multiboot.asm
extern void JumpToMultiboot(int start, int mb_info);