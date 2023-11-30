#ifndef _FLDRWRAPPER_H
#define _FLDRWRAPPER_H

#include "types.h"
#include "mach.h"
#include "utils.h"
#include "console.h"
#include "debug_console.h"
#include "handoff.h"
#include <firmware.h>
#include "pci.h"
#include "ioports.h"

// from loader.c
extern u32 *CreateBootInfo(handoff_boot_info *h);
extern u32 LoadFreeldr();

// from assembly
extern void JumpToFreeldr(u32 start, u32 info);
extern void fail();

#endif