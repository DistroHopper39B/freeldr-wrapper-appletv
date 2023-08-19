#ifndef _ATVLIB_H_
#define _ATVLIB_H_

#include "atvint.h"
#include "atvmach.h"
#include "atvutils.h"
#include "new_console.h"
#include "demos.h"

extern int sprintf(char *buf, const char *fmt, ...);
extern int printk(const char *szFormat, ...);

extern volatile uint32_t VIDEO_CURSOR_POSY;
extern volatile uint32_t VIDEO_CURSOR_POSX;
extern volatile uint32_t VIDEO_ATTR;


#endif