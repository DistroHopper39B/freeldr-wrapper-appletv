//
// Created by distrohopper39b on 8/18/23.
//

#include <demos.h>
#include <atvlib.h>

void DrawColorBars() {
    u32 ColorWhite = 0xFFFFFFFF;
    u32 ColorYellow = 0xFFFF00FF;
    u32 ColorCyan = 0x00FFFFFF;
    u32 ColorGreen = 0x00FF00FF;
    u32 ColorMagenta = 0xFF00FFFF;
    u32 ColorRed = 0xFF0000FF;
    u32 ColorBlue = 0x0000FFFF;
    u32 ColorBlack = 0x000000FF;

    // white
    for(u32 y = 1; y <= mach_bp->video.height; y++) {
        for(u32 x = 1; x <= mach_bp->video.width / 8; x++)
            PlacePixel(x, y, ColorWhite);
    }
    // yellow
    for(u32 y = 1; y <= mach_bp->video.height; y++) {
        for(u32 x = mach_bp->video.width / 8; x <= (mach_bp->video.width / 8) * 2; x++)
            PlacePixel(x, y, ColorYellow);
    }
    // cyan
    for(u32 y = 1; y <= mach_bp->video.height; y++) {
        for(u32 x = (mach_bp->video.width / 8) * 2; x <= (mach_bp->video.width / 8) * 3; x++)
            PlacePixel(x, y, ColorCyan);
    }
    // green
    for(u32 y = 1; y <= mach_bp->video.height; y++) {
        for(u32 x = (mach_bp->video.width / 8) * 3; x <= (mach_bp->video.width / 8) * 4; x++)
            PlacePixel(x, y, ColorGreen);
    }
    // magenta
    for(u32 y = 1; y <= mach_bp->video.height; y++) {
        for(u32 x = (mach_bp->video.width / 8) * 4; x <= (mach_bp->video.width / 8) * 5; x++)
            PlacePixel(x, y, ColorMagenta);
    }
    // red
    for(u32 y = 1; y <= mach_bp->video.height; y++) {
        for(u32 x = (mach_bp->video.width / 8) * 5; x <= (mach_bp->video.width / 8) * 6; x++)
            PlacePixel(x, y, ColorRed);
    }
    // blue
    for(u32 y = 1; y <= mach_bp->video.height; y++) {
        for(u32 x = (mach_bp->video.width / 8) * 6; x <= (mach_bp->video.width / 8) * 7; x++)
            PlacePixel(x, y, ColorBlue);
    }
    // black
    for(u32 y = 1; y <= mach_bp->video.height; y++) {
        for(u32 x = (mach_bp->video.width / 8) * 7; x <= (mach_bp->video.width / 8) * 8; x++)
            PlacePixel(x, y, ColorBlack);
    }
}
void DrawPrideFlag() {
    u32 PrideColorRed = 0xE40303FF;
    u32 PrideColorOrange = 0xFF8C00FF;
    u32 PrideColorYellow = 0xFFED00FF;
    u32 PrideColorGreen = 0x008026FF;
    u32 PrideColorIndigo = 0x24408EFF;
    u32 PrideColorViolet = 0x732982FF;

    // red
    for(u32 y = 1; y <= mach_bp->video.height / 6; y++) {
        for (u32 x = 1; x <= mach_bp->video.width; x++)
            PlacePixel(x, y, PrideColorRed);
    }
    // orange
    for(u32 y = (mach_bp->video.height / 6) * 1; y <= (mach_bp->video.height / 6) * 2; y++) {
        for (u32 x = 1; x <= mach_bp->video.width; x++)
            PlacePixel(x, y, PrideColorOrange);
    }
    // yellow
    for(u32 y = (mach_bp->video.height / 6) * 2; y <= (mach_bp->video.height / 6) * 3; y++) {
        for (u32 x = 1; x <= mach_bp->video.width; x++)
            PlacePixel(x, y, PrideColorYellow);
    }
    // green
    for(u32 y = (mach_bp->video.height / 6) * 3; y <= (mach_bp->video.height / 6) * 4; y++) {
        for (u32 x = 1; x <= mach_bp->video.width; x++)
            PlacePixel(x, y, PrideColorGreen);
    }
    // indigo
    for(u32 y = (mach_bp->video.height / 6) * 4; y <= (mach_bp->video.height / 6) * 5; y++) {
        for (u32 x = 1; x <= mach_bp->video.width; x++)
            PlacePixel(x, y, PrideColorIndigo);
    }
    // violet
    for(u32 y = (mach_bp->video.height / 6) * 5; y <= (mach_bp->video.height / 6) * 6; y++) {
        for (u32 x = 1; x <= mach_bp->video.width; x++)
            PlacePixel(x, y, PrideColorViolet);
    }
}

void DrawTransFlag() {
    u32 TransColorBlue = 0x5BCEFAFF;
    u32 TransColorPink = 0xF5A9B8FF;
    u32 TransColorWhite = 0xFFFFFFFF;

    // blue (1)
    for(u32 y = 1; y <= mach_bp->video.height / 5; y++) {
        for (u32 x = 1; x <= mach_bp->video.width; x++)
            PlacePixel(x, y, TransColorBlue);
    }
    // pink (1)
    for(u32 y = (mach_bp->video.height / 5) * 1; y <= (mach_bp->video.height / 5) * 2; y++) {
        for (u32 x = 1; x <= mach_bp->video.width; x++)
            PlacePixel(x, y, TransColorPink);
    }
    // white
    for(u32 y = (mach_bp->video.height / 5) * 2; y <= (mach_bp->video.height / 5) * 3; y++) {
        for (u32 x = 1; x <= mach_bp->video.width; x++)
            PlacePixel(x, y, TransColorWhite);
    }
    // pink (2)
    for(u32 y = (mach_bp->video.height / 5) * 3; y <= (mach_bp->video.height / 5) * 4; y++) {
        for (u32 x = 1; x <= mach_bp->video.width; x++)
            PlacePixel(x, y, TransColorPink);
    }
    // blue (2)
    for(u32 y = (mach_bp->video.height / 5) * 4; y <= (mach_bp->video.height / 5) * 5; y++) {
        for (u32 x = 1; x <= mach_bp->video.width; x++)
            PlacePixel(x, y, TransColorBlue);
    }
}
