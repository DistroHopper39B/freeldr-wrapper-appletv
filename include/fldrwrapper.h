/*
 * PROJECT:     FreeLoader wrapper for Apple TV
 * LICENSE:     GPL-2.0-only (https://spdx.org/licenses/GPL-2.0-only)
 * PURPOSE:     Main header file for FreeLoader wrapper
 * COPYRIGHT:   Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
 */

#ifndef _FLDRWRAPPER_H
#define _FLDRWRAPPER_H

#include "types.h"
#include "mach.h"
#include "utils.h"
#include "console.h"
#include "bootinfo.h"
#include "firmware.h"
#include "pci.h"
#include "ioports.h"

// from assembly
extern void JumpToFreeldr(u32 start);
extern void fail();

#endif