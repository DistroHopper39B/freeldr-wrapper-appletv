//
// Created by distrohopper39b on 8/20/23.
// This file contains functions used to "fixup" quirks of the Apple TV and prepare to boot Windows.
//

#include <atvlib.h>
#include <handoff_struct.h>
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

    BootStruct->Kernel.BaseAddress = mach_bp->kaddr;
    BootStruct->Kernel.Size = mach_bp->ksize;
    BootStruct->Kernel.End = (mach_bp->kaddr + mach_bp->ksize);
}

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
    printk("FATAL: Could not find %s,%s", segname, sectname);
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
    return ((char *) (sp->addr));
}