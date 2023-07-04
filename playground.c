/*
atv-playground. Based on atv-bootloader.
*/

#include <atvlib.h>

mach_boot_parms *mach_bp;

__attribute__((unused)) void playground_start(unsigned int args) { // called by start symbol in the executable

	mach_bp = (mach_boot_parms *) args;

    vmode.width  = mach_bp->video.rowb / 4;
	vmode.height = mach_bp->video.height;
	vmode.xmargin = 0;

    memset((char *) mach_bp->video.addr, 0x00, vmode.width * vmode.height * 4); // screen clear function

    VIDEO_CURSOR_POSX = 0;
	VIDEO_CURSOR_POSY = 0;
	VIDEO_ATTR = 0xFFFFFFFF;

    printk(" !\"#$%&\\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");

    // do not delete this line!
    printk("\n");
    while (1);
}