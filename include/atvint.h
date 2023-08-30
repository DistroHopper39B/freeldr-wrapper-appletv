#ifndef _ATVINT_H_
#define _ATVINT_H_

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef __SIZE_TYPE__      size_t;
typedef __UINTPTR_TYPE__ uintptr_t;

typedef short			   CHAR16;
typedef void			   VOID;
typedef unsigned long      UINTN;
typedef uint8_t			   UINT8;
typedef UINT8			   CHAR8;
typedef int16_t			   INT16;
typedef uint16_t		   UINT16;
typedef int32_t			   INTN;
typedef unsigned int	   UINT32;
typedef int64_t			   INT64;
typedef unsigned long long UINT64;

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef short s16;
typedef int                bool;
typedef unsigned int       boolean_t;
typedef u32 phys_addr_t;

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


// reactos/sdk/include/xdk/ntdef.h, a few other files here too
// required types for code from ReactOS

#define POINTER_64
#define POINTER_32

#define _Return_type_success_(expr)      _SAL2_Source_(_Return_type_success_, (expr), _Success_impl_(expr))

/* Void Pointers */
typedef void *PVOID;
typedef void * POINTER_64 PVOID64;

/* Handle Type */
typedef void *HANDLE, **PHANDLE;
#ifdef STRICT
#define DECLARE_HANDLE(n) typedef struct n##__{int unused;} *n
#else
#define DECLARE_HANDLE(n) typedef HANDLE n
#endif

/* Upper-Case Versions of Some Standard C Types */
#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;

#if defined(__ROS_LONG64__)
typedef int LONG;
#else
typedef long LONG;
#endif

#if !defined(MIDL_PASS)
typedef int INT;
#endif /* !MIDL_PASS */
#endif /* VOID */

/* Unsigned Types */
typedef unsigned char UCHAR, *PUCHAR;
typedef unsigned short USHORT, *PUSHORT;
typedef unsigned long ULONG, *PULONG;

typedef double DOUBLE;

/* Signed Types */
typedef SHORT *PSHORT;
typedef LONG *PLONG;

/* Flag types */
typedef unsigned char FCHAR;
typedef unsigned short FSHORT;
typedef unsigned long FLONG;

typedef unsigned char BOOLEAN, *PBOOLEAN;
typedef ULONG LOGICAL, *PLOGICAL;
typedef signed char SCHAR, *PSCHAR;

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
#endif /* _HRESULT_DEFINED */

#define __int3264   __int32
#define ADDRESS_TAG_BIT 0x80000000UL
typedef unsigned short UHALF_PTR, *PUHALF_PTR;
typedef short HALF_PTR, *PHALF_PTR;
typedef long SHANDLE_PTR;
typedef unsigned long HANDLE_PTR;
#define HandleToUlong( h ) ((ULONG)(ULONG_PTR)(h) )
#define HandleToLong( h ) ((LONG)(LONG_PTR) (h) )
#define ULongToHandle( h) ((HANDLE)(ULONG_PTR) (h))
#define LongToHandle( h) ((HANDLE)(LONG_PTR) (h))
#define PtrToUlong( p ) ((ULONG)(ULONG_PTR) (p) )
#define PtrToLong( p ) ((LONG)(LONG_PTR) (p) )
#define PtrToUint( p ) ((UINT)(UINT_PTR) (p) )
#define PtrToInt( p ) ((INT)(INT_PTR) (p) )
#define PtrToUshort( p ) ((unsigned short)(ULONG_PTR)(p) )
#define PtrToShort( p ) ((short)(LONG_PTR)(p) )
#define IntToPtr( i )    ((VOID*)(INT_PTR)((int)i))
#define UIntToPtr( ui )  ((VOID*)(UINT_PTR)((unsigned int)ui))
#define LongToPtr( l )   ((VOID*)(LONG_PTR)((long)l))
#define ULongToPtr( ul )  ((VOID*)(ULONG_PTR)((unsigned long)ul))

typedef long INT_PTR, *PINT_PTR;
typedef unsigned long UINT_PTR, *PUINT_PTR;
typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;

#define __int64 long long

typedef ULONG_PTR SIZE_T, *PSIZE_T;
typedef LONG_PTR SSIZE_T, *PSSIZE_T;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
typedef __int64 LONG64, *PLONG64;
typedef __int64 INT64,  *PINT64;
typedef unsigned __int64 ULONG64, *PULONG64;
typedef unsigned __int64 DWORD64, *PDWORD64;
typedef unsigned __int64 UINT64,  *PUINT64;

typedef signed char INT8, *PINT8;
typedef unsigned char UINT8, *PUINT8;
typedef signed short INT16, *PINT16;
typedef unsigned short UINT16, *PUINT16;

typedef ULONG_PTR KAFFINITY;
typedef KAFFINITY *PKAFFINITY;

#define _ULONGLONG_
__extension__ typedef __int64 LONGLONG, *PLONGLONG;
__extension__ typedef unsigned __int64 ULONGLONG, *PULONGLONG;
#define _DWORDLONG_
typedef ULONGLONG DWORDLONG, *PDWORDLONG;

#define FORCEINLINE __attribute__((always_inline))

/* ANSI (Multi-byte Character) types */
typedef CHAR *PCHAR, *LPCH, *PCH, *PNZCH;
typedef const CHAR *LPCCH, *PCCH, *PCNZCH;
typedef CHAR *NPSTR, *LPSTR, *PSTR;
typedef PSTR *PZPSTR;
typedef const PSTR *PCZPSTR;
typedef const CHAR *LPCSTR, *PCSTR;
typedef PCSTR *PZPCSTR;

#define MAXUCHAR  0xff
#define MAXUSHORT 0xffff
#define MAXULONG  0xffffffff

#endif
