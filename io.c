//
// Created by distrohopper39b on 11/12/23.
// Hardware functions for writing to devices.
//

#include <atvlib.h>
void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__ ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__ ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

void outl(uint16_t port, uint32_t val) {
    __asm__ __volatile__ ("out" "l" " %" "0,%" "w" "1"::"a" (val), "Nd" (port));
}

uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ __volatile__ ("in" "l" " %" "w" "1,%" "" "0":"=a" (ret):"Nd" (port));
    return ret;
}

