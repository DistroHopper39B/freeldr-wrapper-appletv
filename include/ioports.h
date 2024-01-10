/*
 * PROJECT:     FreeLoader wrapper for Apple TV
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Header file for I/O access functions for the original Apple TV
 * COPYRIGHT:   Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
 */

#ifndef _IOPORTS_H
#define _IOPORTS_H

extern void outb(uint16_t port, uint8_t val);
extern uint8_t inb(uint16_t port);
extern void outl(uint16_t port, uint32_t val);
extern uint32_t inl(uint16_t port);

#endif //_IOPORTS_H
