//
// Created by distrohopper39b on 7/4/23.
//
#include <stdarg.h>
#ifndef _CONSOLE_H
#define _CONSOLE_H

extern void ClearScreen(int alpha);
extern void SetupScreen();
extern void PlacePixel(u32 PixelLocationX, u32 PixelLocationY, u32 RgbaValue);
extern void debug_printf(const char *szFormat, ...);
extern void printf(const char *szFormat, ...);
extern void PrintToSerial(const char *szBuffer);
extern void ChangeColors(u32 Foreground, u32 Background);
extern int vsprintf(char *buf, const char *fmt, va_list args);
extern volatile u32 VideoCursorX;
extern volatile u32 VideoCursorY;
extern volatile u32 TextBackgroundColor;
extern volatile u32 TextForegroundColor;

#define COM1 0x3F8

#endif //_CONSOLE_H
