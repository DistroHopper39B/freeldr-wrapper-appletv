//
// Created by distrohopper39b on 8/20/23.
// This file contains functions used to "fixup" quirks of the Apple TV and prepare to boot Windows.
//

#include <atvlib.h>
#include <handoff_struct.h>
void quirk_fixup_efi_memmap()
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

    num_maps = mach_bp->efi_mem_map_size/mach_bp->efi_mem_desc_size;

    ChangeColors(0xFFFF00FF, 0x00000000);
    printk("[QUIRK] Fixing EFI memory map...\n");

    // gather up the offending memory ranges
    // these are the two EFI_RUNTIME_SERVICES_CODE and one EFI_RUNTIME_SERVICES_DATA
    // memmap sections. This routine assumes that the sections will appear in order
    // which they seem to always do for the appleTV
    for (i = 0, p = (efi_memory_desc_t*)mach_bp->efi_mem_map; i < num_maps; i++) {
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
        p = NextEFIMemoryDescriptor(p, mach_bp->efi_mem_desc_size);
    }

    for (i = 0, p = (efi_memory_desc_t*)mach_bp->efi_mem_map; i < num_maps; i++) {

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

        p = NextEFIMemoryDescriptor(p, mach_bp->efi_mem_desc_size);
    }

    for (i = 0, p = (efi_memory_desc_t*)mach_bp->efi_mem_map; i < num_maps; i++) {
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

        p = NextEFIMemoryDescriptor(p, mach_bp->efi_mem_desc_size);
    }
    printk("[QUIRK] Fixup complete.\n");
    ChangeColors(0xFFFFFFFF, 0x00000000);
}

HandoffBootStruct *BootStruct;

// Create new boot struct for use with freeldr.
void CreateNewBootStruct() {
    BootStruct->AppleTVMagic = ATV_MAGIC_NUMBER;

    BootStruct->EfiMap.EfiMemoryMap = mach_bp->efi_mem_map;
    BootStruct->EfiMap.EfiMemoryMapSize = mach_bp->efi_mem_map_size;
    BootStruct->EfiMap.EfiMemDescSize = mach_bp->efi_mem_desc_size;
    BootStruct->EfiMap.EfiMemDescVer = mach_bp->efi_mem_desc_ver;
    BootStruct->EfiRuntimeServices = mach_bp->efi_sys_tbl->runtime;

    BootStruct->Video.BaseAddress = mach_bp->video.addr;
    BootStruct->Video.Pitch = mach_bp->video.rowb;
    BootStruct->Video.Width = mach_bp->video.width;
    BootStruct->Video.Height = mach_bp->video.height;
    BootStruct->Video.Depth = mach_bp->video.depth;

    memcpy(BootStruct->CommandLine, mach_bp->cmdline, CMDLINE);
}