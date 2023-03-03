/*
atv-playground. Based on atv-bootloader.
*/

#include "types.h"
#include "utils.h"
#include "mach-o.h"
#include "playground.h"

mach_boot_parms *mach_bp;

#define BOOT_PARAM_MEMSIZE  0x00004000

void print_devtree(mach_boot_parms *mach_bp) {
	int i;
	struct devtreeEntry *devtree;
	devtree = (struct devtreeEntry *) mach_bp->devtree_ptr;

	for (i = 0; i < mach_bp->devtree_ptr, i++;) {
		printk("devtree size: 0x%d\n", mach_bp->devtree_len);
		printk("devtree ptr: 0x%d\n", mach_bp->devtree_ptr);
		}

}


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

	// print video information
	printk("ver: %d\n", &mach_bp->ver);
	printk("rev: %d\n", &mach_bp->rev);
	printk("Display information:\n");
	printk("Framebuffer Address: 0x%08X\n", mach_bp->video.addr);
	printk("Resolution: %dx%d\n", mach_bp->video.width, mach_bp->video.height);
	printk("Row bytes: %d\n", mach_bp->video.rowb);
	printk("Color depth: %d\n", mach_bp->video.depth);

	printk("devtree size: 0x%d\n", mach_bp->devtree_len);
	printk("devtree ptr: 0x%d\n", mach_bp->devtree_ptr);

	print_devtree(mach_bp);
	while (1);
}