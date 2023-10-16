/*
atv-playground. Based on atv-bootloader.
*/
#include <atvlib.h>

HandoffBootStruct *BootStruct;

void stage2_start() { // called by start symbol in the executable
    BootStruct = (HandoffBootStruct *) 0x001F0000; // setup boot struct
    efi_time_t currentTime;

    ClearScreen(0xFF);
    //ChangeColors(0xFFFFFFFF, 0x00000000);
    printk("Hello stage2\n");
    uefi_call_wrapper(BootStruct->EfiRuntimeServices->get_time, 2, &currentTime, NULL);
    printk("The current time is %04d-%02d-%02d %02d:%02d:%02d\n", currentTime.year, currentTime.month, currentTime.day,
           currentTime.hour, currentTime.minute, currentTime.second);
    printk("Framebuffer info: Start 0x%08X, dimensions %ux%ux%u\n", BootStruct->Video.BaseAddress, BootStruct->Video.Width,
           BootStruct->Video.Height, BootStruct->Video.Depth);
    printk("Command line args: %s\n", BootStruct->CommandLine);
}
