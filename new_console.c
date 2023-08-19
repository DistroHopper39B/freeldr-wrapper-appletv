//
// Created by distrohopper39b on 7/4/23.
// This is an attempt to rewrite the screen printing logic for atv-playground. The original code was messy and originally designed for the original Xbox, which is not targeted here.
//

#include <atvlib.h>
#include <font.h>

// Clear screen function
void ClearScreen(int alpha) {
    memset((void *) mach_bp->video.addr, alpha, mach_bp->video.width * mach_bp->video.height * 4);
    VIDEO_CURSOR_POSX = 0;
    VIDEO_CURSOR_POSY = 0;
    VIDEO_ATTR = 0x0000FFFF; // VIDEO_* is deprecated (for old console.c, to be removed)
}

// Pixel placement code. The top left corner is located at (1, 1)
void PlacePixel(u32 PixelLocationX, u32 PixelLocationY, u32 RgbaValue) {
    // convert from 32-bit RGBA number to 4 8-bit numbers
    u8 Red = (RgbaValue >> 24) & 0xFF;
    u8 Green = (RgbaValue >> 16) & 0xFF;
    u8 Blue = (RgbaValue >> 8) & 0xFF;
    u8 Alpha = RgbaValue & 0xFF;
    // find pixel address and correct top left pixel from (0, 0) to (1, 1)
    u32 PixelStartingAddr = mach_bp->video.addr + ((PixelLocationX - 1) * 4) + ((PixelLocationY - 1) * mach_bp->video.width * 4);
    /* Apple TV linear frame buffer printing logic. Works the same as every other RGBA linear frame buffer. */
    memset((void *) PixelStartingAddr, Blue, 1); // blue
    memset((void *) PixelStartingAddr + 1, Green, 1); // green
    memset((void *) PixelStartingAddr + 2, Red, 1); // red
    memset((void *) PixelStartingAddr + 3, Alpha, 1); // alpha
}

void PlaceCharacter(char Character, u32 StartingPositionX, u32 StartingPositionY, u32 BackgroundColor, u32 ForegroundColor) {
    int CharPosition = Character * 16;
    for(int i = 0; i < ISO_CHAR_HEIGHT; i++) { // print all lines (runs 16 times = 128 total)
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