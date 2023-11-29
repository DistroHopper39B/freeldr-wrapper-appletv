//
// Created by distrohopper39b on 11/28/23.
//

#ifndef PLAYGROUND_IOPORTS_H
#define PLAYGROUND_IOPORTS_H

extern void outb(uint16_t port, uint8_t val);
extern uint8_t inb(uint16_t port);
extern void outl(uint16_t port, uint32_t val);
extern uint32_t inl(uint16_t port);

#endif //PLAYGROUND_IOPORTS_H
