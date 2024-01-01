/*
 * PROJECT:     FreeLoader wrapper for Apple TV
 * LICENSE:     GPL-2.0-only (https://spdx.org/licenses/GPL-2.0-only)
 * PURPOSE:     Header file for screen printing functions for the original Apple TV
 * COPYRIGHT:   Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
 */

#include <stdarg.h>
#ifndef _CONSOLE_H
#define _CONSOLE_H

extern void ClearScreen(bool VerboseEnable);
extern void SetupScreen();
extern void debug_printf(const char *szFormat, ...);
extern void printf(const char *szFormat, ...);
extern void ChangeColors(u32 Foreground, u32 Background);
extern int vsprintf(char *buf, const char *fmt, va_list args);
extern bool WrapperVerbose;

typedef enum {
    Blue = 0,
    Green,
    Red,
    Reserved,
} FrameBufferColors;

#define COM1 0x3F8

#endif //_CONSOLE_H
