/*
atv-playground. Based on atv-bootloader.
*/
#include <atvlib.h>

mach_boot_parms *mach_bp;

void playground_start(unsigned int args) { // called by start symbol in the executable
    mach_bp = (mach_boot_parms *) args;
    efi_time_t currentTime;

    SetupScreen();
    //ClearScreen(0xFF);
    printk("Apple TV Windows loader, version dev0\n");
    uefi_call_wrapper(mach_bp->efi_sys_tbl->runtime->get_time, 2, &currentTime, NULL);
    printk("The current time is %04d-%02d-%02d %02d:%02d:%02d\n", currentTime.year, currentTime.month, currentTime.day,
           currentTime.hour, currentTime.minute, currentTime.second);
    printk("Framebuffer info: Start 0x%08X, dimensions %ux%ux%u\n", mach_bp->video.addr, mach_bp->video.width,
           mach_bp->video.height, mach_bp->video.depth);
    printk("Command line args: %s\n", mach_bp->cmdline);
    printk_always("Loading FreeLoader...\n");
    load_freeldr();
    fail();
}
