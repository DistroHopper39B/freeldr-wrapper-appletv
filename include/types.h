#ifndef _TYPES_H
#define _TYPES_H

typedef signed char         int8_t;
typedef short               int16_t;
typedef int                 int32_t;
typedef long long           int64_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

typedef __SIZE_TYPE__       size_t;
typedef __UINTPTR_TYPE__    uintptr_t;

typedef short			    CHAR16;
typedef void			    VOID;
typedef unsigned long       UINTN;
typedef uint8_t			    UINT8;
typedef UINT8			    CHAR8;
typedef int16_t			    INT16;
typedef uint16_t		    UINT16;
typedef int32_t			    INTN;
typedef unsigned int	    UINT32;
typedef int64_t			    INT64;
typedef unsigned long long  UINT64;

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned long       __attribute__((__may_alias__)) u32;
typedef unsigned long long  __attribute__((__may_alias__)) u64;
typedef char                s8;
typedef short               s16;
typedef long                s32;
typedef long long           s64;
typedef char                bool;

typedef unsigned char*      FRAMEBUFFER;

#define UUID_SIZE 16

typedef struct {
    u8 b[UUID_SIZE];
} guid_t;

typedef struct {
    u8 b[UUID_SIZE];
} uuid_t;

struct range {
    u64   start;
    u64   end;
};


#define __aligned(x)                    __attribute__((__aligned__(x)))

#define FALSE 0
#define TRUE  1
#define NULL  ((void *) 0)
//

#endif
