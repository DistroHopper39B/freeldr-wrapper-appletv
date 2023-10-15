/*
atv-playground. Based on atv-bootloader.
*/
#include <atvlib.h>

mach_boot_parms *mach_bp;

void playground_start(unsigned int args) { // called by start symbol in the executable
    mach_bp = (mach_boot_parms *) args;
    efi_time_t currentTime;

    ClearScreen(0x00);
    DrawColorBars();
    CreateNewBootStruct();
    printk("Apple TV Windows loader, version dev0\n");
    printk("Magic number: 0x%08X\n", BootStruct->AppleTVMagic);
    printk("End of bootstruct: 0x%08X\n", &mach_bp->__reserved2[7]);
    uefi_call_wrapper(BootStruct->EfiRuntimeServices->get_time, 2, &currentTime, NULL);
    printk("The current time is %04d-%02d-%02d %02d:%02d:%02d\n", currentTime.year, currentTime.month, currentTime.day,
           currentTime.hour, currentTime.minute, currentTime.second);
    //quirk_fixup_efi_memmap();
    printk("Framebuffer info: Start 0x%08X, dimensions %ux%ux%u\n", BootStruct->Video.BaseAddress, BootStruct->Video.Width,
           BootStruct->Video.Height, BootStruct->Video.Depth);
    printk("Command line args: %s\n", BootStruct->CommandLine);

    printk("Kernel address: 0x%08X\n", mach_bp->kaddr);
    printk("Kernel size: 0x%08X\n", mach_bp->ksize);
    printk("Kernel end: 0x%08X\n", mach_bp->kaddr + mach_bp->ksize);
}