//
// Created by distrohopper39b on 11/28/23.
//

#include <fldrwrapper.h>

/**
 * Convert EFI memory map to Multiboot memory map to be passed to freeldr.
 * This code is based on a Linux kernel patch submitted by Edgar Hucek and modified for use with a Multiboot
 * memory map.
 */
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
    //debug_printf("Number of EFI memory map entries: %i\n", EfiNumberOfEntries);
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
                debug_printf("ERROR: default add_memory_region, should not see this\n");
                AddMemoryRegion(MultibootMapEntry, &MultibootNumberOfEntries,
                                md->phys_addr,
                                md->num_pages << EFI_PAGE_SHIFT,
                                MULTIBOOT_MEMORY_RESERVED);
                break;
        }
        p = (efi_memory_desc_t *) NextEFIMemoryDescriptor(p, handoff->efi_map.descriptor_size);
    }
    // debug_printf("Number of multiboot entries: %i\n", MultibootNumberOfEntries);
    handoff->multiboot_map.size = sizeof(struct mmap_entry) * MultibootNumberOfEntries;
    handoff->multiboot_map.entries = MultibootNumberOfEntries;
}

void PrintMultibootMemoryMap(handoff_boot_info *handoff) {
    int	i;
    struct mmap_entry *MultibootMapEntry;
    MultibootMapEntry = (struct mmap_entry *) handoff->multiboot_map.addr;


    for (i = 0; i < handoff->multiboot_map.entries; i++) {
        debug_printf("%s: 0x%08X%08X - 0x%08X%08X ", "Multiboot Memory Map",
                     hi32(MultibootMapEntry[i].addr),
                     lo32(MultibootMapEntry[i].addr),
                     hi32(MultibootMapEntry[i].addr + MultibootMapEntry[i].len),
                     lo32(MultibootMapEntry[i].addr + MultibootMapEntry[i].len));
        switch (MultibootMapEntry[i].type) {
            case MULTIBOOT_MEMORY_AVAILABLE:
                debug_printf("(usable)\n");
                break;
            case MULTIBOOT_MEMORY_RESERVED:
                debug_printf("(reserved)\n");
                break;
            case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE:
                debug_printf("(ACPI data)\n");
                break;
            case MULTIBOOT_MEMORY_NVS:
                debug_printf("(ACPI NVS)\n");
                break;
            default:
                debug_printf("type %u\n", MultibootMapEntry[i].type);
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
    efi_tables_t efitab;
    u32					i, num_config_tables;

    system_table		= (efi_system_table_t*)mach_bp->efi_sys_tbl;
    num_config_tables	= system_table->nr_tables;
    config_tables		= (efi_config_table_t*)system_table->tables;

    // Let's see what config tables the efi firmware passed to us.
    for (i = 0; i < num_config_tables; i++) {
        if (efi_guidcmp(config_tables[i].guid, MPS_TABLE_GUID) == 0) {
            efitab.mps = (void*)config_tables[i].table;
            //debug_printf(" MPS=0x%lx ", config_tables[i].table);
            //
        } else if (efi_guidcmp(config_tables[i].guid, ACPI_20_TABLE_GUID) == 0) {
            efitab.acpi20 = (void*)config_tables[i].table;
            //debug_printf(" ACPI 2.0=0x%lx ", config_tables[i].table);
            //
        } else if (efi_guidcmp(config_tables[i].guid, ACPI_TABLE_GUID) == 0) {
            efitab.acpi = (void*)config_tables[i].table;
            //debug_printf(" ACPI=0x%lx ", config_tables[i].table);
            //
        } else if (efi_guidcmp(config_tables[i].guid, SMBIOS_TABLE_GUID) == 0) {
            efitab.smbios = (void*) config_tables[i].table;
            //debug_printf(" SMBIOS=0x%lx ", config_tables[i].table);
            //
        } else if (efi_guidcmp(config_tables[i].guid, HCDP_TABLE_GUID) == 0) {
            //efi.hcdp = (void*)config_tables[i].table;
            //debug_printf(" HCDP=0x%lx ", config_tables[i].table);
            //
        } else if (efi_guidcmp(config_tables[i].guid, UGA_IO_PROTOCOL_GUID) == 0) {
            //efi.uga = (void*)config_tables[i].table;
            //debug_printf(" UGA=0x%lx ", config_tables[i].table);
        }
    }
    //debug_printf("\n");

    // rsdp_low_mem is unsigned long so alignment below works
    u32	*rsdp_low_mem   = (u32 *) ACPI_TABLE_LOW;
    u32	*smbios_low_mem = (u32 *) SMBIOS_TABLE_LOW;
    //
    debug_printf("Cloning ACPI entry from 0x%08X to 0x%lX...", efitab.acpi20, rsdp_low_mem);
    // We need at copy the RSDP down low so linux can find it
    // copy RSDP table entry from efi location to low mem location
    memcpy((void *) rsdp_low_mem, efitab.acpi20, sizeof(acpi_rsdp_t) );
    debug_printf("acpi location = 0x%08X\n", *rsdp_low_mem);
    debug_printf("done.\n");

    debug_printf("Cloning SMBIOS entry from 0x%08X to 0x%lX...", efitab.smbios, smbios_low_mem);
    // We need at copy the SMBIOS Table Entry Point down low so linux can find it
    // copy SMBIOS Table Entry Point from efi location to low mem location
    memcpy((void *) smbios_low_mem, efitab.smbios, sizeof(smbios_entry_t) );
    debug_printf("smbios location = 0x%08X\n", *smbios_low_mem);
    debug_printf("done.\n");
}
