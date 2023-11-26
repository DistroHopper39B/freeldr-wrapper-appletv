#ifndef _ATVLIB_H_
#define _ATVLIB_H_

#include "atvint.h"
#include "atvmach.h"
#include "atvutils.h"
#include "console.h"
#include "debug_console.h"
#include "handoff.h"
#include <efi.h>
#include "pci.h"

extern int vsprintf(char *buf, const char *fmt, va_list args);
extern void load_freeldr();
extern void outb(uint16_t port, uint8_t val);
extern uint8_t inb(uint16_t port);
extern void outl(uint16_t port, uint32_t val);
extern uint32_t inl(uint16_t port);

// from assembly
extern void fail();

#endif