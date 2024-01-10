/*
 * PROJECT:     FreeLoader wrapper for Apple TV
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Screen printing functions for the original Apple TV
 * COPYRIGHT:   Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
 */

/* INCLUDES *******************************************************************/

#include <fldrwrapper.h>
#include <font.h>

/* GLOBALS ********************************************************************/

#define VIDEO_STARTING_POSITION_X 1
#define VIDEO_STARTING_POSITION_Y 1

volatile u32 TextBackgroundColor = 0x00000000;
volatile u32 TextForegroundColor = 0xFFFFFFFF;
volatile u32 VideoCursorX;
volatile u32 VideoCursorY;
bool WrapperVerbose;
u32 NeedsWrapAround;

/* FUNCTIONS ******************************************************************/

/* Place pixel on screen */
static
void PlacePixel(u32 PixelLocationX, u32 PixelLocationY, u32 RgbaValue) {
    /* convert from 32-bit RGBA number to 4 8-bit numbers */
    u8 RedValue = (RgbaValue >> 24) & 0xFF;
    u8 GreenValue = (RgbaValue >> 16) & 0xFF;
    u8 BlueValue = (RgbaValue >> 8) & 0xFF;
    u8 ReservedValue = RgbaValue & 0xFF;
    /* find pixel address and correct top left pixel from (0, 0) to (1, 1) */
    FRAMEBUFFER PixelStartingAddr = ((FRAMEBUFFER) BootArgs->Video.BaseAddress) +
            ((PixelLocationX - 1) * 4) + ((PixelLocationY - 1) * BootArgs->Video.Pitch);
    /* Apple TV linear frame buffer printing logic. */
    PixelStartingAddr[Blue] = BlueValue;
    PixelStartingAddr[Green] = GreenValue;
    PixelStartingAddr[Red] = RedValue;
    PixelStartingAddr[Reserved] = ReservedValue;
}

/* Place character on screen */
static
void PlaceCharacter(char Character, u32 StartingPositionX, u32 StartingPositionY,
                    u32 BackgroundColor, u32 ForegroundColor) {
    /* find position in font */
    int CharPosition = Character * ISO_CHAR_HEIGHT;
    /* actual printing stuff */
    for (int i = 0; i < ISO_CHAR_HEIGHT; i++) {
        u8 CharLine = iso_font[CharPosition];
        for (int j = 0; j < ISO_CHAR_WIDTH; j++) {
            int f = CharLine >> j;
            if ((f & 1) == 1) {
                /* foreground color */
                PlacePixel(StartingPositionX + j, StartingPositionY + i,
                           ForegroundColor);
            } else {
                /* background color */
                PlacePixel(StartingPositionX + j, StartingPositionY + i,
                           BackgroundColor);
            }
        }
        CharPosition++;
    }
}

/* Print to screen */
static
void PrintToScreen(const char *szBuffer) {
    for (int i = 0; szBuffer[i] != '\0'; i++) {
        if (szBuffer[i] == '\n') {
            VideoCursorX = VIDEO_STARTING_POSITION_X;
            VideoCursorY += ISO_CHAR_HEIGHT;
        } else {
            PlaceCharacter(szBuffer[i], VideoCursorX,
                           VideoCursorY, TextBackgroundColor,
                           TextForegroundColor);
            if (VideoCursorX >= NeedsWrapAround) {
                VideoCursorX = VIDEO_STARTING_POSITION_X;
                VideoCursorY += ISO_CHAR_HEIGHT;
            } else {
                VideoCursorX += ISO_CHAR_WIDTH;
            }
        }
    }
}

/* Print to serial port */
static
void PrintToSerial(const char *szBuffer) {
    for (int i = 0; szBuffer[i] != '\0'; i++) {
        outb(COM1, szBuffer[i]);
    }
}

/* Change screen colors */
void ChangeColors(u32 Foreground, u32 Background) {
    TextForegroundColor = Foreground;
    TextBackgroundColor = Background;
}

/* Clear the screen */
void ClearScreen(bool VerboseEnable) {
    /* Set all pixels to black */
    FRAMEBUFFER FrameBuffer = (FRAMEBUFFER) BootArgs->Video.BaseAddress;
    for (int i = 0; i < (BootArgs->Video.Pitch * BootArgs->Video.Height); i++) {
        FrameBuffer[i] = 0;
    }
    /* (Re)set screen */
    SetupScreen();
    /* Enable verbose mode */
    if (VerboseEnable) {
        WrapperVerbose = TRUE;
    } else {
        WrapperVerbose = FALSE;
    }
}

/* Setup screen without clearing it */
void SetupScreen() {
    VideoCursorX = VIDEO_STARTING_POSITION_X;
    VideoCursorY = VIDEO_STARTING_POSITION_Y;
    NeedsWrapAround = ((BootArgs->Video.Pitch / 4) - VIDEO_STARTING_POSITION_X) - ISO_CHAR_WIDTH;
    /* Make serial look better */
    PrintToSerial("\n");
}

/* print only if Verbose mode is specified. */
void debug_printf(const char *szFormat, ...) {
    if (WrapperVerbose) {
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
        PrintToSerial(szBuffer);
    }
}

/* print always */
void printf(const char *szFormat, ...) {
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
    PrintToSerial(szBuffer);
}