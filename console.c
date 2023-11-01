//
// Created by distrohopper39b on 7/4/23.
// This is an attempt to rewrite the screen printing logic for atv-playground. The original code was messy and originally designed for the original Xbox, which is not targeted here.
//

#include <atvlib.h>
#include <stdarg.h>
#include <font.h>

const u32 VideoCursorOrigX = 1;
const u32 VideoCursorOrigY = 1;
volatile u32 TextBackgroundColor = 0x00000000;
volatile u32 TextForegroundColor = 0xFFFFFFFF;
volatile u32 VideoCursorX;
volatile u32 VideoCursorY;


// Clear screen function
void ClearScreen(int alpha) {
    memset((void *) mach_bp->video.addr, alpha, mach_bp->video.rowb * mach_bp->video.height);
    VideoCursorX = VideoCursorOrigX;
    VideoCursorY = VideoCursorOrigY;
}

// Setup screen without clearing it
void SetupScreen() {
    VideoCursorX = VideoCursorOrigX;
    VideoCursorY = VideoCursorOrigY;
}

// Pixel placement code. The top left corner is located at (1, 1)
void PlacePixel(u32 PixelLocationX, u32 PixelLocationY, u32 RgbaValue) {
    // convert from 32-bit RGBA number to 4 8-bit numbers
    u8 Red = (RgbaValue >> 24) & 0xFF;
    u8 Green = (RgbaValue >> 16) & 0xFF;
    u8 Blue = (RgbaValue >> 8) & 0xFF;
    u8 Alpha = RgbaValue & 0xFF;
    // find pixel address and correct top left pixel from (0, 0) to (1, 1)
    u32 PixelStartingAddr = mach_bp->video.addr + ((PixelLocationX - 1) * 4) + ((PixelLocationY - 1) * mach_bp->video.rowb);
    /* Apple TV linear frame buffer printing logic. Works the same as every other RGBA linear frame buffer. */
    memset((void *) PixelStartingAddr, Blue, 1); // blue
    memset((void *) PixelStartingAddr + 1, Green, 1); // green
    memset((void *) PixelStartingAddr + 2, Red, 1); // red
    memset((void *) PixelStartingAddr + 3, Alpha, 1); // alpha
}
// Character placement code
void PlaceCharacter(char Character, u32 StartingPositionX, u32 StartingPositionY, u32 BackgroundColor, u32 ForegroundColor) {
    // find position in font
    int CharPosition = Character * ISO_CHAR_HEIGHT;
    // actual printing stuff
    for(int i = 0; i < ISO_CHAR_HEIGHT; i++) { // print all rows (runs 16 times = 128 total)
        u8 CharLine = iso_font[CharPosition];
        for(int j = 0; j < ISO_CHAR_WIDTH; j++) { // print 1 row of a character (runs 8 times)
            int f = CharLine >> j;
            if((f & 1) == 1) {
                // foreground color
                PlacePixel(StartingPositionX + j, StartingPositionY + i, ForegroundColor);
            } else {
                // background color
                PlacePixel(StartingPositionX + j, StartingPositionY + i, BackgroundColor);
            }
        }
        CharPosition++;
    }
}
// Actually print to the screen
void PrintToScreen(const char *szBuffer) {
    for(int i = 0; szBuffer[i] != '\0'; i++) {
        if(szBuffer[i] == '\n') {
            VideoCursorX = VideoCursorOrigX;
            VideoCursorY += ISO_CHAR_HEIGHT;
        } else {
            PlaceCharacter(szBuffer[i], VideoCursorX, VideoCursorY, TextBackgroundColor, TextForegroundColor);
            VideoCursorX += ISO_CHAR_WIDTH;
        }
    }
}
// Change colors of text on the fly.
void ChangeColors(u32 Foreground, u32 Background) {
    TextForegroundColor = Foreground;
    TextBackgroundColor = Background;
}
// Serial port code
void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__ ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

// inb asm->c (not used yet)
uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__ ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}
// Print buffer to COM1
void PrintToSerial(const char *szBuffer) {
    for(int i = 0; szBuffer[i] != '\0'; i++) {
        outb(COM1, szBuffer[i]);
    }
}
void printk(const char *szFormat, ...) {
    char szBuffer[512 * 2];
    u16 wLength = 0;
    va_list argList;

    va_start(argList, szFormat);
    wLength = (u16) vsprintf(szBuffer, szFormat, argList);
    va_end(argList);

    szBuffer[sizeof(szBuffer) - 1] = 0;
    if (wLength > (sizeof(szBuffer) - 1))
        wLength = sizeof(szBuffer) - 1;
    szBuffer[wLength] = '\0';
    PrintToScreen(szBuffer);
    if(SERIAL_ENABLE == 1) {
        PrintToSerial(szBuffer);
    }
}