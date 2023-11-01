//
// Created by distrohopper39b on 7/4/23.
//
#include <stdarg.h>
#ifndef PLAYGROUND_NEW_CONSOLE_H
#define PLAYGROUND_NEW_CONSOLE_H

extern void ClearScreen(int alpha);
extern void SetupScreen();
extern void PlacePixel(u32 PixelLocationX, u32 PixelLocationY, u32 RgbaValue);
extern void printk(const char *szFormat, ...);
extern void ChangeColors(u32 Foreground, u32 Background);
extern volatile u32 VideoCursorX;
extern volatile u32 VideoCursorY;
extern volatile u32 TextBackgroundColor;
extern volatile u32 TextForegroundColor;

#endif //PLAYGROUND_NEW_CONSOLE_H
