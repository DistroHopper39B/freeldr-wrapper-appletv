/*
atv-playground. Based on atv-bootloader.
*/

#include <stdint.h>
#include <atvutils.h>
#include <atvmach.h>
#include <atvlib.h>

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

	printk("DATA OK, first stage initialization complete!\n");
	printk("Hello, World!\n");

	// print information
	printk("Firmware version: %d.%d\n", mach_bp->ver, mach_bp->rev);
	printk("Framebuffer Address: 0x%08X\n", mach_bp->video.addr);
	printk("Resolution: %dx%d\n", mach_bp->video.width, mach_bp->video.height);
	printk("Row bytes: %d\n", mach_bp->video.rowb);
	printk("Color depth: %d\n", mach_bp->video.depth);
	printk("boot-args: %s\n", mach_bp->cmdline);
	printk("devtree size: 0x%d\n", mach_bp->devtree_len);
	printk("devtree ptr: 0x%d\n", mach_bp->devtree_ptr);

	while (1);
}