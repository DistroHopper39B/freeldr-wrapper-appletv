/*
atv-playground. Based on atv-bootloader.
*/

#include "types.h"
#include "utils.h"
#include "mach-o.h"
#include "playground.h"

extern int sprintf(char *buf, const char *fmt, ...);
extern int printk(const char *szFormat, ...);

extern volatile uint32_t VIDEO_CURSOR_POSY;
extern volatile uint32_t VIDEO_CURSOR_POSX;
extern volatile uint32_t VIDEO_ATTR;
extern volatile CURRENT_VIDEO_MODE_DETAILS vmode;

mach_boot_parms *mach_bp;

void playground_start(unsigned int args) {
	mach_bp = (mach_boot_parms *) args;

    vmode.width  = mach_bp->video.rowb / 4;
	vmode.height = mach_bp->video.height;
	vmode.xmargin = 0;

    memset((char *) mach_bp->video.addr, 0x00, vmode.width * vmode.height * 4); // screen clear function

    VIDEO_CURSOR_POSX = 0;
	VIDEO_CURSOR_POSY = 0;
	VIDEO_ATTR = 0xffc8c8c8;

	printk("DATA OK\n");
	printk("Hello, World!\n");

	while (1);
}