/*
atv-playground. Based on atv-bootloader.
*/
#include <atvlib.h>

mach_boot_parms *mach_bp;

u32 Stage2Location = 0x00100000;
u32 BootStructLocation = 0x001F0000;
unsigned char *Stage2Ptr = NULL;
unsigned long Stage2Len = 0;


void playground_start(unsigned int args) { // called by start symbol in the executable
    mach_bp = (mach_boot_parms *) args;
    efi_time_t currentTime;

    SetupScreen();
    ClearScreen(0xFF);
    CreateNewBootStruct();
    printk("Apple TV Windows loader, version dev0\n");
    uefi_call_wrapper(BootStruct->EfiRuntimeServices->get_time, 2, &currentTime, NULL);
    printk("The current time is %04d-%02d-%02d %02d:%02d:%02d\n", currentTime.year, currentTime.month, currentTime.day,
           currentTime.hour, currentTime.minute, currentTime.second);
    quirk_fixup_efi_memmap();
    printk("Framebuffer info: Start 0x%08X, dimensions %ux%ux%u\n", BootStruct->Video.BaseAddress, BootStruct->Video.Width,
           BootStruct->Video.Height, BootStruct->Video.Depth);
    printk("Command line args: %s\n", BootStruct->CommandLine);
    // Find the location of the embedded stage2 loader in the mach-o header.
    Stage2Ptr = (u8 *) getsectdatafromheader(&_mh_execute_header, "__TEXT", "__stage2", &Stage2Len);
    printk("@ 0x%08X size %i\n", Stage2Ptr, Stage2Len);

    // Copy the stage2 file to the stage2 location (1M).
    printk("Copying stage2 to 0x%08X...", Stage2Location);
    memcpy((void *) Stage2Location, Stage2Ptr, Stage2Len);
    printk("done.\n");

    // Copy the boot struct to its location.
    printk("Copying boot struct to 0x%08X...", BootStructLocation);
    memcpy((void *) BootStructLocation, BootStruct, sizeof(HandoffBootStruct));
    printk("done.\n");
    // Jump to stage2 code
    void (*entry_point)() = (void (*)())Stage2Location;
    printk("Jumping to stage2 in 3 seconds...\n");
    sleep(3);
    entry_point();
}
