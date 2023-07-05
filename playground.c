/*
atv-playground. Based on atv-bootloader.
*/
#include <atvlib.h>

mach_boot_parms *mach_bp;
__attribute__((unused)) void playground_start(unsigned int args) { // called by start symbol in the executable

	mach_bp = (mach_boot_parms *) args;

    vmode.width  = mach_bp->video.width;
	vmode.height = mach_bp->video.height;
	vmode.xmargin = 0;

    ClearScreen(0x00);
    VIDEO_CURSOR_POSX = 0;
	VIDEO_CURSOR_POSY = 0;
	VIDEO_ATTR = 0x0000FFFF; // controls colors too!!

    int i = 0;
    while(i <= 24) {
        printk(" !\"#$%&\\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ !\"#$%&\\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUV\n");
        i++;
    }
    // do not delete this line!
    printk("\n");
    while (1);
}