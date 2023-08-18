/*
atv-playground. Based on atv-bootloader.
*/
#include <atvlib.h>

void playground_start(unsigned int idkwtfthisworks) { // called by start symbol in the executable
    ClearScreen(idkwtfthisworks, 0x00);
    if(strstr(mach_bp->cmdline, "gay")) {
        DrawPrideFlag();
    } else if(strstr(mach_bp->cmdline, "trans")) {
        DrawTransFlag();
    } else {
        DrawColorBars();
    }

    /*
    int i = 0;
    while(i <= 24) {
        printk(" !\"#$%&\\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ !\"#$%&\\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUV\n");
        i++;
    }
    */

    // do not delete this line!
    printk("\n");
    while (1);
}