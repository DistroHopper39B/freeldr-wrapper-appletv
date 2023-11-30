/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _FIRMWARE_H
#define _FIRMWARE_H

#include <fldrwrapper.h>
#include <handoff.h>

extern int memcmp(const void *cs,const void *ct, size_t count);

extern void quirk_fixup_efi_memmap(handoff_boot_info *handoff);
extern void AddMemoryRegion(struct mmap_entry *map, u32 *NumberOfEntries, u64 addr, u64 len, u32 type);
extern void FillMultibootMemoryMap(handoff_boot_info *handoff);
extern void PrintMultibootMemoryMap(handoff_boot_info *handoff);
extern void LegacyAcpiSmbiosFix();

// acpi/smbios code from linux_code.h
#define hi32(a) ((UINT32)((a) >> 32))
#define lo32(a) ((UINT32)(a))

// page alignment macros (include/asm-i386/page.h)
// Align the pointer to the (next) page boundary
// PAGE_SHIFT determines the page size
#define PAGE_SHIFT       12
#define PAGE_SIZE        (1UL << PAGE_SHIFT)
#define PAGE_MASK        (~(PAGE_SIZE-1))
#define PAGE_ALIGN(addr) (((addr)+PAGE_SIZE-1)&PAGE_MASK)

#define asmlinkage CPP_ASMLINKAGE


#define __noreturn                      __attribute__((__noreturn__))

// Descriptor table pointer format.
#pragma pack(1)
typedef struct {
    uint16_t limit;
    uint32_t base;
} dt_addr_t;
#pragma pack()

typedef struct {
    void *kstart;
    void *kend;
    void *kentry;
} kdesc_t;

typedef struct {
    void     *start_addr;
    uint32_t pgcnt;
    uint32_t size;
} memdesc_t;

/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
//acpi memory search
//000E0000h to 000FFFFFh
//
/* RSDP location */
#define ACPI_BIOS_ROM_BASE 0xE0000L
#define ACPI_BIOS_ROM_END  0x100000L
#define ACPI_RSDP1_SIG     0x20445352		/* 'RSD ' */
#define ACPI_RSDP2_SIG     0x20525450		/* 'PTR ' */
//
/* Root System Descriptor Pointer */
// ACPI 1.0 Root System Description Table (RSDT)
typedef struct acpi_rsdp_rev1
{
    uint8_t  signature [4];          /* ACPI signature (4 ASCII characters) */\
	uint32_t length;                 /* Length of table, in bytes, including header */\
	uint8_t  revision;               /* ACPI Specification minor version # */\
	uint8_t  checksum;               /* To make sum of entire table == 0 */\
	uint8_t  oem_id [6];             /* OEM identification */\
	uint8_t  oem_table_id [8];       /* OEM table identification */\
	uint32_t oem_revision;           /* OEM revision number */\
	uint8_t  asl_compiler_id [4];    /* ASL compiler vendor ID */\
	uint32_t asl_compiler_revision;  /* ASL compiler revision number */
    uint32_t table_offset_entry [2]; /* Array of pointers to other ACPI tables */
} acpi_rsdp_rev1_t;

/* ACPI 2.0 table RSDP */
typedef struct acpi_rsdp {
    char signature[8];     /* RSDP signature "RSD PTR" */
    u8   checksum;         /* checksum of the first 20 bytes */
    char oem_id[6];        /* OEM ID, "LXBIOS" */
    u8   revision;         /* 0 for APCI 1.0, 2 for ACPI 2.0 */
    u32  rsdt_address;     /* physical address of RSDT */
    u32  length;           /* total length of RSDP (including extended part) */
    u64  xsdt_address;     /* physical address of XSDT */
    u8   ext_checksum;     /* checksum of whole table */
    u8   reserved[3];
} __attribute__((packed)) acpi_rsdp_t;

/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
//smbios memory search
//000F0000h to 000FFFFFh
//
// SMBIOS
// linux scans 0xF0000 to 0xF0000 + 0x10000 searching for
//	the SMBIOS (DMI) signature "_DMI_" and correct dmi checksum
// SMBIOS Structure Table Entry Point.  See DSP0134 2.1.1 for more information.
//  The structure table entry point is located by searching for the anchor.

typedef struct smbios_entry {
    char     smbe_eanchor[4];		/* anchor tag (SMB_ENTRY_EANCHOR) "_SM_" */
    uint8_t  smbe_ecksum;			/* checksum of entry point structure */
    uint8_t  smbe_elen;				/* length in bytes of entry point */
    uint8_t  smbe_major;				/* major version of the SMBIOS spec */
    uint8_t  smbe_minor;				/* minor version of the SMBIOS spec */
    uint16_t smbe_maxssize;			/* maximum size in bytes of a struct */
    uint8_t  smbe_revision;			/* entry point structure revision */
    uint8_t  smbe_format[5];			/* entry point revision-specific data */
    char     smbe_ianchor[5];		/* intermed. tag (SMB_ENTRY_IANCHOR) "_DMI_" */
    uint8_t  smbe_icksum;			/* intermed. checksum */
    uint16_t smbe_stlen;				/* length in bytes of structure table */
    uint32_t smbe_staddr;			/* physical addr of structure table */
    uint16_t smbe_stnum;				/* number of structure table entries */
    uint8_t  smbe_bcdrev;			/* BCD value representing DMI version */
} __attribute__((packed)) smbios_entry_t;

// (linux/include/linux/efi.h)
typedef struct _efi_memory_desc_t {
    u32 type;
    u32 pad;
    u64 phys_addr;
    u64 virt_addr;
    u64 num_pages;
    u64 attribute;
} efi_memory_desc_t;

// EFI Memory types
#define EFI_RESERVED_TYPE                0
#define EFI_LOADER_CODE                  1
#define EFI_LOADER_DATA                  2
#define EFI_BOOT_SERVICES_CODE           3
#define EFI_BOOT_SERVICES_DATA           4
#define EFI_RUNTIME_SERVICES_CODE        5
#define EFI_RUNTIME_SERVICES_DATA        6
#define EFI_CONVENTIONAL_MEMORY          7
#define EFI_UNUSABLE_MEMORY              8
#define EFI_ACPI_RECLAIM_MEMORY          9
#define EFI_ACPI_MEMORY_NVS             10
#define EFI_MEMORY_MAPPED_IO            11
#define EFI_MEMORY_MAPPED_IO_PORT_SPACE 12
#define EFI_PAL_CODE                    13
#define EFI_MAX_MEMORY_TYPE             14

/* Attribute values: */
#define EFI_MEMORY_UC           ((u64)0x0000000000000001ULL)    /* uncached */
#define EFI_MEMORY_WC           ((u64)0x0000000000000002ULL)    /* write-coalescing */
#define EFI_MEMORY_WT           ((u64)0x0000000000000004ULL)    /* write-through */
#define EFI_MEMORY_WB           ((u64)0x0000000000000008ULL)    /* write-back */
#define EFI_MEMORY_WP           ((u64)0x0000000000001000ULL)    /* write-protect */
#define EFI_MEMORY_RP           ((u64)0x0000000000002000ULL)    /* read-protect */
#define EFI_MEMORY_XP           ((u64)0x0000000000004000ULL)    /* execute-protect */
#define EFI_MEMORY_RUNTIME      ((u64)0x8000000000000000ULL)    /* range requires runtime mapping */
#define EFI_MEMORY_DESCRIPTOR_VERSION 1

#define EFI_PAGE_SHIFT 12

#define NextEFIMemoryDescriptor(Ptr,Size) ((efi_memory_desc_t *) (((UINT8 *) Ptr) + Size))

typedef struct {
    u64 signature;
    u32 revision;
    u32 headersize;
    u32 crc32;
    u32 reserved;
} efi_table_hdr_t;
//
typedef struct {
    efi_table_hdr_t hdr;
    unsigned long   get_time;
    unsigned long   set_time;
    unsigned long   get_wakeup_time;
    unsigned long   set_wakeup_time;
    unsigned long   set_virtual_address_map;
    unsigned long   convert_pointer;
    unsigned long   get_variable;
    unsigned long   get_next_variable;
    unsigned long   set_variable;
    unsigned long   get_next_high_mono_count;
    unsigned long   reset_system;
} efi_runtime_services_t;
//
typedef struct {
    efi_table_hdr_t        hdr;
    unsigned long          fw_vendor;        // physical addr of CHAR16 vendor string
    u32                    fw_revision;
    unsigned long          con_in_handle;
    unsigned long          con_in;
    unsigned long          con_out_handle;
    unsigned long          con_out;
    unsigned long          stderr_handle;
    unsigned long          stderr;
    efi_runtime_services_t *runtime;
    unsigned long          boottime;
    unsigned long          nr_tables;
    unsigned long          tables;
} efi_system_table_t;

typedef struct {
    u8 b[16];
} efi_guid_t;

#define EFI_GUID(a,b,c,d0,d1,d2,d3,d4,d5,d6,d7) \
	((efi_guid_t) \
		{{ (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff, \
			(b) & 0xff, ((b) >> 8) & 0xff, \
			(c) & 0xff, ((c) >> 8) & 0xff, \
			(d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7) }})

static inline int efi_guidcmp (efi_guid_t left, efi_guid_t right)
{
    return (memcmp(&left, &right, sizeof(efi_guid_t)));
}
/*
*  EFI Configuration Table and GUID definitions
*/
#define NULL_GUID \
	EFI_GUID(  0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 )

#define MPS_TABLE_GUID    \
	EFI_GUID(  0xeb9d2d2f, 0x2d88, 0x11d3, 0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d )

#define ACPI_TABLE_GUID    \
	EFI_GUID(  0xeb9d2d30, 0x2d88, 0x11d3, 0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d )

#define ACPI_20_TABLE_GUID    \
	EFI_GUID(  0x8868e871, 0xe4f1, 0x11d3, 0xbc, 0x22, 0x0, 0x80, 0xc7, 0x3c, 0x88, 0x81 )

#define SMBIOS_TABLE_GUID    \
	EFI_GUID(  0xeb9d2d31, 0x2d88, 0x11d3, 0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d )

#define SAL_SYSTEM_TABLE_GUID    \
	EFI_GUID(  0xeb9d2d32, 0x2d88, 0x11d3, 0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d )

#define HCDP_TABLE_GUID \
	EFI_GUID(  0xf951938d, 0x620b, 0x42ef, 0x82, 0x79, 0xa8, 0x4b, 0x79, 0x61, 0x78, 0x98 )

#define UGA_IO_PROTOCOL_GUID \
	EFI_GUID(  0x61a4d49e, 0x6f68, 0x4f1b, 0xb9, 0x22, 0xa8, 0x6e, 0xed, 0xb, 0x7, 0xa2 )

#define EFI_GLOBAL_VARIABLE_GUID \
	EFI_GUID(  0x8be4df61, 0x93ca, 0x11d2, 0xaa, 0x0d, 0x00, 0xe0, 0x98, 0x03, 0x2b, 0x8c )
//
typedef struct {
    efi_guid_t		guid;
    unsigned long	table;
} efi_config_table_t;

typedef struct efi_tables {
    void *mps;
    void *acpi;
    void *acpi20;
    void *smbios;
    void *uga;
} efi_tables_t;

#endif