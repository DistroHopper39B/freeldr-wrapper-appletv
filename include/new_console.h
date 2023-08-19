//
// Created by distrohopper39b on 7/4/23.
//

#ifndef PLAYGROUND_NEW_CONSOLE_H
#define PLAYGROUND_NEW_CONSOLE_H

extern void ClearScreen(int alpha);
extern void PlacePixel(u32 PixelLocationX, u32 PixelLocationY, u32 RgbaValue);
void PlaceCharacter(char Character, u32 StartingPositionX, u32 StartingPositionY, u32 BackgroundColor, u32 ForegroundColor);
#endif //PLAYGROUND_NEW_CONSOLE_H
