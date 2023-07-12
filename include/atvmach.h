#ifndef _ATVMACH_H_
#define _ATVMACH_H_

#include <atvint.h>

#define DARWIN_IMAGE_BASE	0x002000000
// xnu pexpert/pexpert/i386/boot.h
/* Values for v_display */
#define VGA_TEXT_MODE 0
#define GRAPHICS_MODE 1
#define FB_TEXT_MODE  2
#define CMDLINE	1024
//
typedef struct _Boot_Video {
	uint32_t v_baseAddr;	// Base address of video memory
	uint32_t v_display;	// Display Code (0, 1, 2)
	uint32_t v_rowBytes;	// Number of bytes per pixel row
	uint32_t v_width;	// Width
	uint32_t v_height;	// Height
	uint32_t v_depth;	// Pixel Depth
} __attribute__((aligned(4))) Boot_Video;
//
typedef struct _mach_video_parms {
	uint32_t addr;
	uint32_t disp;
	uint32_t rowb;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
} __attribute__((aligned(4))) mach_video_parms;
//
typedef struct _mach_boot_parms {
    uint16_t         rev;
    uint16_t         ver;
    char             cmdline[CMDLINE];
    uint32_t         efi_mem_map;
    uint32_t         efi_mem_map_size;
    uint32_t         efi_mem_desc_size;
    uint32_t         efi_mem_desc_ver;
    mach_video_parms video;
    uint32_t         devtree_ptr;			// Base of flattened device tree
    uint32_t         devtree_len;			// Length of flattened tree
    uint32_t         kaddr;
    uint32_t         ksize;
    uint32_t         efi_runtime_page;
    uint32_t         efi_runtime_page_count;
    uint32_t         efi_sys_tbl;
    uint8_t          efi_mode;
    uint8_t          __reserved1[3];
    uint32_t         __reserved2[7];
} __attribute__((aligned(4))) mach_boot_parms;
//
extern mach_boot_parms *mach_bp;

typedef unsigned long BGRA;


//

#endif
