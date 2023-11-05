#ifndef _ATVLIB_H_
#define _ATVLIB_H_

#include "atvint.h"
#include "atvmach.h"
#include "atvutils.h"
#include "console.h"
#include "debug.h"
#include "handoff_struct.h"
#include <efi.h>
#include "multiboot.h"

extern int vsprintf(char *buf, const char *fmt, va_list args);
extern void CreateNewBootStruct();
extern HandoffBootStruct *BootStruct;
extern u32 *CreateMultibootInfoStructure(multiboot_info_t *mb);
extern void FillMultibootMemoryMap(multiboot_info_t *mb);
extern void PrintMultibootMemoryMap(multiboot_info_t *mb);
extern u32 ValidateMultibootHeader();
extern void load_multiboot(multiboot_info_t *mb);

// from assembly
extern void fail();

#endif