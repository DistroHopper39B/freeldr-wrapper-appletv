/*
atv-playground. Based on atv-bootloader.
*/
#include <atvlib.h>

mach_boot_parms *mach_bp;

void playground_start(unsigned int args) { // called by start symbol in the executable
    mach_bp = (mach_boot_parms *) args;

    // ClearScreen(0x00);

    PlaceCharacter('H', 1, 1, 0x00000000, 0xFFFFFFFF);
    PlaceCharacter('e', 9, 1, 0x00000000, 0xFFFFFFFF);
    PlaceCharacter('l', 17, 1, 0x00000000, 0xFFFFFFFF);
    PlaceCharacter('l', 25, 1, 0x00000000, 0xFFFFFFFF);
    PlaceCharacter('o', 33, 1, 0x00000000, 0xFFFFFFFF);
    PlaceCharacter(',', 41, 1, 0x00000000, 0xFFFFFFFF);
    PlaceCharacter(' ', 49, 1, 0x00000000, 0xFFFFFFFF);
    PlaceCharacter('W', 57, 1, 0x00000000, 0xFFFFFFFF);
    PlaceCharacter('o', 65, 1, 0x00000000, 0xFFFFFFFF);
    PlaceCharacter('r', 73, 1, 0x00000000, 0xFFFFFFFF);
    PlaceCharacter('l', 81, 1, 0x00000000, 0xFFFFFFFF);
    PlaceCharacter('d', 89, 1, 0x00000000, 0xFFFFFFFF);
    PlaceCharacter('!', 97, 1, 0x00000000, 0xFFFFFFFF);

    while (1);
}