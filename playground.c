/*
atv-playground. Based on atv-bootloader.
*/
#include <atvlib.h>
mach_boot_parms *mach_bp;

void playground_start(unsigned int args) { // called by start symbol in the executable
    mach_bp = (mach_boot_parms *) args;
    ClearScreen(0x33);
    printk("ASCII table: !\"#$%&\\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n");
    ChangeColors(0xFF0000FF, 0x00FF00FF);
    printk("ASCII table: !\"#$%&\\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n");
    while (1);
}