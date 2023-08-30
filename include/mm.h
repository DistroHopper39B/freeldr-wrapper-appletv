//
// Created by distrohopper39b on 8/29/23.
// reactos/sdk/include/xdk/wdm.h
//

#ifndef PLAYGROUND_MM_H
#define PLAYGROUND_MM_H

#include "atvint.h"
#include "freeldr.h"
#include "ms_sal.h"

extern void *memmove(void *dest, const void *src, size_t n);
extern void* memset(void *s, int c, size_t count);
/******************************************************************************
 *                       Memory manager Functions                             *
 ******************************************************************************/
/* Alignment Macros */
#define ALIGN_DOWN_BY(size, align) \
    ((ULONG_PTR)(size) & ~((ULONG_PTR)(align) - 1))

#define ALIGN_UP_BY(size, align) \
    (ALIGN_DOWN_BY(((ULONG_PTR)(size) + align - 1), align))

#define ALIGN_DOWN_POINTER_BY(ptr, align) \
    ((PVOID)ALIGN_DOWN_BY(ptr, align))

#define ALIGN_UP_POINTER_BY(ptr, align) \
    ((PVOID)ALIGN_UP_BY(ptr, align))

#define ALIGN_DOWN(size, type) \
    ALIGN_DOWN_BY(size, sizeof(type))

#define ALIGN_UP(size, type) \
    ALIGN_UP_BY(size, sizeof(type))

#define ALIGN_DOWN_POINTER(ptr, type) \
    ALIGN_DOWN_POINTER_BY(ptr, sizeof(type))

#define ALIGN_UP_POINTER(ptr, type) \
    ALIGN_UP_POINTER_BY(ptr, sizeof(type))

#ifndef FIELD_OFFSET
#define FIELD_OFFSET(type, field) ((ULONG)&(((type *)0)->field))
#endif

#ifndef FIELD_SIZE
#define FIELD_SIZE(type, field) (sizeof(((type *)0)->field))
#endif

#define POOL_TAGGING                             1

#if DBG
#define IF_DEBUG if (TRUE)
#else
#define IF_DEBUG if (FALSE)
#endif /* DBG */

/* ULONG
 * BYTE_OFFSET(
 *     _In_ PVOID Va)
 */
#define BYTE_OFFSET(Va) \
  ((ULONG) ((ULONG_PTR) (Va) & (PAGE_SIZE - 1)))

/* ULONG
 * BYTES_TO_PAGES(
 *     _In_ ULONG Size)
 *
 * Note: This needs to be like this to avoid overflows!
 */
#define BYTES_TO_PAGES(Size) \
  (((Size) >> PAGE_SHIFT) + (((Size) & (PAGE_SIZE - 1)) != 0))

/* PVOID
 * PAGE_ALIGN(
 *     _In_ PVOID Va)
 */
#define PAGE_ALIGN(Va) \
  ((PVOID) ((ULONG_PTR)(Va) & ~(PAGE_SIZE - 1)))

/* ULONG_PTR
 * ROUND_TO_PAGES(
 *     _In_ ULONG_PTR Size)
 */
#define ROUND_TO_PAGES(Size) \
  (((ULONG_PTR) (Size) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

/* ULONG
 * ADDRESS_AND_SIZE_TO_SPAN_PAGES(
 *     _In_ PVOID Va,
 *     _In_ ULONG Size)
 */
#define ADDRESS_AND_SIZE_TO_SPAN_PAGES(_Va, _Size) \
  ((ULONG) ((((ULONG_PTR) (_Va) & (PAGE_SIZE - 1)) \
    + (_Size) + (PAGE_SIZE - 1)) >> PAGE_SHIFT))

#define COMPUTE_PAGES_SPANNED(Va, Size) \
    ADDRESS_AND_SIZE_TO_SPAN_PAGES(Va,Size)

/*
 * ULONG
 * MmGetMdlByteCount(
 *     _In_ PMDL  Mdl)
 */
#define MmGetMdlByteCount(_Mdl) \
  ((_Mdl)->ByteCount)

/*
 * ULONG
 * MmGetMdlByteOffset(
 *   IN PMDL  Mdl)
 */
#define MmGetMdlByteOffset(_Mdl) \
  ((_Mdl)->ByteOffset)

#define MmGetMdlBaseVa(Mdl) ((Mdl)->StartVa)

/*
 * PPFN_NUMBER
 * MmGetMdlPfnArray(
 *     _In_ PMDL  Mdl)
 */
#define MmGetMdlPfnArray(_Mdl) \
  ((PPFN_NUMBER) ((_Mdl) + 1))

/*
 * PVOID
 * MmGetMdlVirtualAddress(
 *     _In_ PMDL  Mdl)
 */
#define MmGetMdlVirtualAddress(_Mdl) \
  ((PVOID) ((PCHAR) ((_Mdl)->StartVa) + (_Mdl)->ByteOffset))

#define MmGetProcedureAddress(Address) (Address)
#define MmLockPagableCodeSection(Address) MmLockPagableDataSection(Address)

/* PVOID MmGetSystemAddressForMdl(
 *     _In_ PMDL Mdl);
 */
#define MmGetSystemAddressForMdl(Mdl) \
  (((Mdl)->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA | \
    MDL_SOURCE_IS_NONPAGED_POOL)) ? \
      ((Mdl)->MappedSystemVa) : \
      (MmMapLockedPages((Mdl), KernelMode)))

/* PVOID
 * MmGetSystemAddressForMdlSafe(
 *     _In_ PMDL Mdl,
 *     _In_ MM_PAGE_PRIORITY Priority)
 */
#define MmGetSystemAddressForMdlSafe(_Mdl, _Priority) \
  (((_Mdl)->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA \
    | MDL_SOURCE_IS_NONPAGED_POOL)) ? \
    (_Mdl)->MappedSystemVa : \
    (PVOID) MmMapLockedPagesSpecifyCache((_Mdl), \
      KernelMode, MmCached, NULL, FALSE, (_Priority)))

/*
 * VOID
 * MmInitializeMdl(
 *     _In_ PMDL MemoryDescriptorList,
 *     _In_ PVOID BaseVa,
 *     _In_ SIZE_T Length)
 */
#define MmInitializeMdl(_MemoryDescriptorList, \
                        _BaseVa, \
                        _Length) \
{ \
  (_MemoryDescriptorList)->Next = (PMDL) NULL; \
  (_MemoryDescriptorList)->Size = (CSHORT) (sizeof(MDL) + \
    (sizeof(PFN_NUMBER) * ADDRESS_AND_SIZE_TO_SPAN_PAGES(_BaseVa, _Length))); \
  (_MemoryDescriptorList)->MdlFlags = 0; \
  (_MemoryDescriptorList)->StartVa = (PVOID) PAGE_ALIGN(_BaseVa); \
  (_MemoryDescriptorList)->ByteOffset = BYTE_OFFSET(_BaseVa); \
  (_MemoryDescriptorList)->ByteCount = (ULONG) _Length; \
}

/*
 * VOID
 * MmPrepareMdlForReuse(
 *     _In_ PMDL Mdl)
 */
#define MmPrepareMdlForReuse(_Mdl) \
{ \
  if (((_Mdl)->MdlFlags & MDL_PARTIAL_HAS_BEEN_MAPPED) != 0) { \
    ASSERT(((_Mdl)->MdlFlags & MDL_PARTIAL) != 0); \
    MmUnmapLockedPages((_Mdl)->MappedSystemVa, (_Mdl)); \
  } else if (((_Mdl)->MdlFlags & MDL_PARTIAL) == 0) { \
    ASSERT(((_Mdl)->MdlFlags & MDL_MAPPED_TO_SYSTEM_VA) == 0); \
  } \
}

#pragma once

extern char __ImageBase;
#ifdef __GNUC__
#ifdef _M_AMD64
/* .text/.data/.rdata, and .bss */
    #define FREELDR_SECTION_COUNT 2
#else
/* .text/.data/.rdata, .edata and .bss */
#define FREELDR_SECTION_COUNT 3
#endif
#else
#ifdef _M_AMD64
/* .text, .rdata/.edata, .pdata and .data/.bss */
#define FREELDR_SECTION_COUNT 4
#else
/* .text, .rdata/.edata and .data/.bss */
#define FREELDR_SECTION_COUNT 3
#endif
#endif

typedef ULONG PFN_COUNT;
typedef LONG_PTR SPFN_NUMBER, *PSPFN_NUMBER;
typedef ULONG_PTR PFN_NUMBER, *PPFN_NUMBER;

typedef struct _FREELDR_MEMORY_DESCRIPTOR
{
    TYPE_OF_MEMORY MemoryType;
    PFN_NUMBER BasePage;
    PFN_NUMBER PageCount;
} FREELDR_MEMORY_DESCRIPTOR, *PFREELDR_MEMORY_DESCRIPTOR;


#if  defined(__i386__) || defined(_PPC_) || defined(_MIPS_) || defined(_ARM_)

#define MM_PAGE_SIZE    4096
#define MM_PAGE_MASK    0xFFF
#define MM_PAGE_SHIFT    12
#if defined(_X86PAE_)
#define MM_MAX_PAGE        0x3FFFFFF /* 26 bits for the PFN */
#else
#define MM_MAX_PAGE        0xFFFFF /* 20 bits for the PFN */
#endif
#define MM_MAX_PAGE_LOADER 0xFFFFF /* 4 GB flat address range */

#define MM_SIZE_TO_PAGES(a)  \
    ( ((a) >> MM_PAGE_SHIFT) + ((a) & MM_PAGE_MASK ? 1 : 0) )

#endif // defined __i386__ or _PPC_ or _MIPS_

#if defined (_AMD64_)

#define MM_PAGE_SIZE    4096
#define MM_PAGE_MASK    0xFFF
#define MM_PAGE_SHIFT    12
#define MM_MAX_PAGE        0xFFFFFFFFF /* 36 bits for the PFN */
#define MM_MAX_PAGE_LOADER 0x3FFFF /* on x64 freeldr only maps 1 GB */

#define MM_SIZE_TO_PAGES(a)  \
    ( ((a) >> MM_PAGE_SHIFT) + ((a) & MM_PAGE_MASK ? 1 : 0) )

#endif

// HEAP and STACK size
#define HEAP_PAGES    0x400
#define STACK_PAGES    0x00

typedef struct
{
    TYPE_OF_MEMORY    PageAllocated;                    // Type of allocated memory (LoaderFree if this memory is free)
    PFN_NUMBER            PageAllocationLength;            // Number of pages allocated (or zero if this isn't the first page in the chain)
} PAGE_LOOKUP_TABLE_ITEM, *PPAGE_LOOKUP_TABLE_ITEM;

//
// Define this to 1 if you want the entire contents
// of the memory allocation bitmap displayed
// when a chunk is allocated or freed
//
#define DUMP_MEM_MAP_ON_VERIFY    0

extern PVOID PageLookupTableAddress;
extern PFN_NUMBER TotalPagesInLookupTable;
extern PFN_NUMBER FreePagesInLookupTable;
extern PFN_NUMBER LastFreePageHint;

#if DBG
PCSTR MmGetSystemMemoryMapTypeString(TYPE_OF_MEMORY Type);
#endif

PFN_NUMBER MmGetPageNumberFromAddress(PVOID Address);    // Returns the page number that contains a linear address
PFN_NUMBER MmGetAddressablePageCountIncludingHoles(VOID);    // Returns the count of addressable pages from address zero including any memory holes and reserved memory regions
PVOID MmFindLocationForPageLookupTable(PFN_NUMBER TotalPageCount);    // Returns the address for a memory chunk big enough to hold the page lookup table (starts search from end of memory)
VOID MmInitPageLookupTable(PVOID PageLookupTable, PFN_NUMBER TotalPageCount);    // Inits the page lookup table according to the memory types in the memory map
VOID MmMarkPagesInLookupTable(PVOID PageLookupTable, PFN_NUMBER StartPage, PFN_NUMBER PageCount, TYPE_OF_MEMORY PageAllocated);    // Marks the specified pages as allocated or free in the lookup table
VOID MmAllocatePagesInLookupTable(PVOID PageLookupTable, PFN_NUMBER StartPage, PFN_NUMBER PageCount, TYPE_OF_MEMORY MemoryType);    // Allocates the specified pages in the lookup table
PFN_NUMBER MmCountFreePagesInLookupTable(PVOID PageLookupTable, PFN_NUMBER TotalPageCount);    // Returns the number of free pages in the lookup table
PFN_NUMBER MmFindAvailablePages(PVOID PageLookupTable, PFN_NUMBER TotalPageCount, PFN_NUMBER PagesNeeded, BOOLEAN FromEnd);    // Returns the page number of the first available page range from the beginning or end of memory
PFN_NUMBER MmFindAvailablePagesBeforePage(PVOID PageLookupTable, PFN_NUMBER TotalPageCount, PFN_NUMBER PagesNeeded, PFN_NUMBER LastPage);    // Returns the page number of the first available page range before the specified page
VOID MmUpdateLastFreePageHint(PVOID PageLookupTable, PFN_NUMBER TotalPageCount);    // Sets the LastFreePageHint to the last usable page of memory
BOOLEAN MmAreMemoryPagesAvailable(PVOID PageLookupTable, PFN_NUMBER TotalPageCount, PVOID PageAddress, PFN_NUMBER PageCount);    // Returns TRUE if the specified pages of memory are available, otherwise FALSE
VOID MmSetMemoryType(PVOID MemoryAddress, SIZE_T MemorySize, TYPE_OF_MEMORY NewType); // Use with EXTREME caution!

PPAGE_LOOKUP_TABLE_ITEM MmGetMemoryMap(PFN_NUMBER *NoEntries);            // Returns a pointer to the memory mapping table and a number of entries in it


//BOOLEAN    MmInitializeMemoryManager(ULONG LowMemoryStart, ULONG LowMemoryLength);
BOOLEAN    MmInitializeMemoryManager(VOID);
VOID    MmInitializeHeap(PVOID PageLookupTable);
PVOID    MmAllocateMemory(SIZE_T MemorySize);
PVOID    MmAllocateMemoryWithType(SIZE_T MemorySize, TYPE_OF_MEMORY MemoryType);
VOID    MmFreeMemory(PVOID MemoryPointer);
PVOID    MmAllocateMemoryAtAddress(SIZE_T MemorySize, PVOID DesiredAddress, TYPE_OF_MEMORY MemoryType);
PVOID    MmAllocateHighestMemoryBelowAddress(SIZE_T MemorySize, PVOID DesiredAddress, TYPE_OF_MEMORY MemoryType);

/* Heap */
#define DEFAULT_HEAP_SIZE (1024 * 1024)
#define TEMP_HEAP_SIZE (32 * 1024 * 1024)

extern PVOID FrLdrDefaultHeap;
extern PVOID FrLdrTempHeap;
extern SIZE_T FrLdrImageSize;

#define HEAP_ZERO_MEMORY                0x00000008

PVOID
FrLdrHeapCreate(
        SIZE_T MaximumSize,
        TYPE_OF_MEMORY MemoryType);

VOID
FrLdrHeapDestroy(
        PVOID HeapHandle);

VOID
FrLdrHeapRelease(
        PVOID HeapHandle);

VOID
FrLdrHeapVerify(
        PVOID HeapHandle);

VOID
FrLdrHeapCleanupAll(VOID);

PVOID
FrLdrHeapAllocateEx(
        PVOID HeapHandle,
        SIZE_T ByteSize,
        ULONG Tag);

VOID
FrLdrHeapFreeEx(
        PVOID HeapHandle,
        PVOID Pointer,
        ULONG Tag);

FORCEINLINE
        PVOID
FrLdrHeapAlloc(SIZE_T MemorySize, ULONG Tag)
{
return FrLdrHeapAllocateEx(FrLdrDefaultHeap, MemorySize, Tag);
}

FORCEINLINE
        VOID
FrLdrHeapFree(PVOID MemoryPointer, ULONG Tag)
{
FrLdrHeapFreeEx(FrLdrDefaultHeap, MemoryPointer, Tag);
}

FORCEINLINE
        PVOID
FrLdrTempAlloc(
        _In_ SIZE_T Size,
        _In_ ULONG Tag)
{
return FrLdrHeapAllocateEx(FrLdrTempHeap, Size, Tag);
}

FORCEINLINE
        VOID
FrLdrTempFree(
        PVOID Allocation, ULONG Tag)
{
FrLdrHeapFreeEx(FrLdrTempHeap, Allocation, Tag);
}

#define RtlMoveMemory(Dest,Source,Length) memmove((Dest),(Source),(Length))
#define RtlFillMemory(Dest,Length,Fill) memset((Dest),(Fill),(Length))
#define RtlZeroMemory(Dest,Length) RtlFillMemory((Dest),(Length),0)

//
// Page-related Macros
//
#define PAGE_SIZE                         0x1000
#define PAGE_SHIFT                        12L
#define MM_ALLOCATION_GRANULARITY         0x10000
#define MM_ALLOCATION_GRANULARITY_SHIFT   16L
#define MM_PAGE_FRAME_NUMBER_SIZE         20

#endif //PLAYGROUND_MM_H
