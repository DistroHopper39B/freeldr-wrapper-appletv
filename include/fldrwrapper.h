/*
 * PROJECT:     FreeLoader wrapper for Apple TV
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Main header file for FreeLoader wrapper
 * COPYRIGHT:   Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
 */

#ifndef _FLDRWRAPPER_H
#define _FLDRWRAPPER_H

#include "types.h"
#include "utils.h"
#include "console.h"
#include "bootinfo.h"
#include "firmware.h"
#include "pci.h"
#include "ioports.h"
#include "mach.h"

// from assembly
extern void JumpToFreeldr(u32 start);
extern void fail();

#endif