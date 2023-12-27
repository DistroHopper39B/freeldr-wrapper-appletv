#ifndef _FLDRWRAPPER_H
#define _FLDRWRAPPER_H

#include "types.h"
#include "mach.h"
#include "utils.h"
#include "console.h"
#include "bootinfo.h"
#include <firmware.h>
#include "pci.h"
#include "ioports.h"

// from loader.c
extern void CreateBootInfo();
extern u32 LoadFreeldr();
extern void SetupCmdline(const char MachCmdLine[]);
extern bool WrapperVerbose;

// from assembly
extern void JumpToFreeldr(u32 start);
extern void fail();

#endif