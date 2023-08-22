/*
atv-playground. Based on atv-bootloader.
*/
#include <atvlib.h>

mach_boot_parms *mach_bp;

void playground_start(unsigned int args) { // called by start symbol in the executable
    efi_time_t currentTime;
    mach_bp = (mach_boot_parms *) args;
    DrawColorBars();
    //ClearScreen(0x00);
    printk("Hello, world!\n");
    PlacePixel(100, 100, 0x33333333);
    quirk_fixup_efi_memmap();
    uefi_call_wrapper(mach_bp->efi_sys_tbl->runtime->get_time, 2, &currentTime, NULL);
    printk("The current time is %04d-%02d-%02d %02d:%02d:%02d.\n", currentTime.year, currentTime.month, currentTime.day, currentTime.hour, currentTime.minute, currentTime.second);
    
    while (1);

}