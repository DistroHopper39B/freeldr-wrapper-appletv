#ifndef _ATVLIB_H_
#define _ATVLIB_H_

#include "atvint.h"
#include "atvmach.h"
#include "atvutils.h"
#include "console.h"
#include "debug.h"
#include "handoff_struct.h"
#include <efi.h>

extern int vsprintf(char *buf, const char *fmt, va_list args);
extern void quirk_fixup_efi_memmap();
extern void CreateNewBootStruct();
extern HandoffBootStruct *BootStruct;

#endif