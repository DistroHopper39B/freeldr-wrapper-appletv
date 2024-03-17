/*
 * PROJECT:     FreeLoader wrapper for Apple TV
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     Header file for PCI functions for IDE controller fixup for the original Apple TV
 * COPYRIGHT:   Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
 */

#ifndef _PCI_H
#define _PCI_H

#include <fldrwrapper.h>

#define PCI_TYPE1_ADDRESS_PORT 0xCF8
#define PCI_TYPE1_DATA_PORT 0xCFC


/* PCI Class Codes */
#define PCI_CLASS_CODE_UNCLASSIFIED 0x0
#define PCI_CLASS_CODE_MASS_STORAGE 0x1
#define PCI_CLASS_CODE_NETWORK 0x2
#define PCI_CLASS_CODE_DISPLAY 0x3
#define PCI_CLASS_CODE_MULTIMEDIA 0x4
#define PCI_CLASS_CODE_MEMORY 0x5
#define PCI_CLASS_CODE_BRIDGE 0x6
#define PCI_CLASS_CODE_SIMPLE_COMMUNICATION_CONTROLLER 0x7
#define PCI_CLASS_CODE_BASE_SYSTEM_PERIPHERAL 0x8
#define PCI_CLASS_CODE_INPUT_DEVICE_CONTROLLER 0x9
#define PCI_CLASS_CODE_DOCKING_STATION 0xA
#define PCI_CLASS_CODE_PROCESSOR 0xB
#define PCI_CLASS_CODE_SERIAL_BUS_CONTROLLER 0xC
#define PCI_CLASS_CODE_WIRELESS_CONTROLLER 0xD
#define PCI_CLASS_CODE_INTELLIGENT_CONTROLLER 0xE
#define PCI_CLASS_CODE_SATELITE_CONTROLLER 0xF
#define PCI_CLASS_CODE_ENCRYPTION_CONTROLLER 0x10
#define PCI_CLASS_CODE_SIGNAL_PROCESSING_CONTROLLER 0x11
#define PCI_CLASS_CODE_PROCESSING_ACCELERATOR 0x12
#define PCI_CLASS_CODE_NON_ESSENTIAL_INSTRUMENTATION 0x13
#define PCI_CLASS_CODE_RESERVED1 0x14
#define PCI_CODE_COPROCESSOR 0x40
#define PCI_CLASS_CODE_RESERVED2 0x41
#define PCI_CLASS_CODE_UNASSIGNED_CLASS 0xFF

/* Mass storage class codes */
#define PCI_MASS_STORAGE_SUBCLASS_CODE_SCSI_CONTROLLER 0x0
#define PCI_MASS_STORAGE_SUBCLASS_CODE_IDE_CONTROLLER 0x1
#define PCI_MASS_STORAGE_SUBCLASS_CODE_FLOPPY_CONTROLLER 0x2
#define PCI_MASS_STORAGE_SUBCLASS_CODE_IPI_CONTROLLER 0x3
#define PCI_MASS_STORAGE_SUBCLASS_CODE_RAID_CONTROLLER 0x4
#define PCI_MASS_STORAGE_SUBCLASS_CODE_ATA_CONTROLLER 0x5
#define PCI_MASS_STORAGE_SUBCLASS_CODE_SATA_CONTROLLER 0x6
#define PCI_MASS_STORAGE_SUBCLASS_CODE_SAS_CONTROLLER 0x7
#define PCI_MASS_STORAGE_SUBCLASS_CODE_NVM_CONTROLLER 0x8
#define PCI_MASS_STORAGE_SUBCLASS_CODE_OTHER 0x80

/* PCI configuration space offsets (common) */
#define VENDOR_ID 0x0 // u16
#define DEVICE_ID 0x2 // u16

#define COMMAND 0x4 // u16
#define STATUS 0x6 // u16

#define REV_ID 0x8 // u8
#define PROG_IF 0x9 // u8
#define SUBCLASS_CODE 0xA // u8
#define CLASS_CODE 0xB // u8

#define CACHE_LINE_SIZE 0xC // u8
#define LATENCY_TIMER 0xD // u8
#define HEADER_TYPE 0xE // u8
#define BIST 0xF // u8

/******************************************************************************/

#define INTERRUPT_LINE 0x3C // u8
#define INTERRUPT_PIN 0x3D // u8
#define MIN_GRANT 0x3E // u8
#define MAX_LATENCY 0x3F // u8

/* PCI Configuration space offsets (HeaderType == 0) */
#define BAR0 0x10 // u32 (also available when HeaderType == 1)

#define BAR1 0x14 // u32 (also available when HeaderType == 1)

#define BAR2 0x18 // u32

#define BAR3 0x1C // u32

#define BAR4 0x20 // u32

#define BAR5 0x24 // u32

#define CARDBUS_CIS_POINTER 0x28 // u32

#define SUBSYSTEM_VENDOR_ID 0x2C // u16
#define SUBSYSTEM_DEVICE_ID 0x2E // u16

#define EXPANSION_ROM_ADDR 0x30 // u32

#define CAPABILITIES_PTR 0x34 // u8 (also available when HeaderType == 1)
#define RESERVED1 0x35 // u24 (RESERVED) (also available when HeaderType == 1)
#define RESERVED2 0x38 // u32 (RESERVED) (also available when HeaderType == 1)

/* PCI Configuration space offset (HeaderType == 1, PCI-PCI bridge) */
#define PRIMARY_BUS_NUMBER 0x18 // u8
#define SECONDARY_BUS_NUMBER 0x19 // u8
#define SUBORDINATE_BUS_NUMBER 0x1A // u8
#define SECONDARY_LATENCY_TIMER 0x1B // u8

#define IO_BASE 0x1C // u8
#define IO_LIMIT 0x1D // u8
#define SECONDARY_STATUS 0x1E // u16

#define MEMORY_BASE 0x20 // u16
#define MEMORY_LIMIT 0x22 // u16

#define PREFETCHABLE_MEMORY_BASE 0x24 // u16
#define PREFETCHABLE_MEMORY_LIMIT 0x26 // u16

#define PREFETCHABLE_BASE_UPPER_32_BITS 0x28 // u32

#define PREFETCHABLE_LIMIT_UPPER_32_BITS 0x2C // u32

#define IO_BASE_UPPER_16_BITS 0x30 // u16
#define IO_LIMIT_UPPER_16_BITS 0x32 // u16

/* PCI configuration space offsets (HeaderType == 2, PCI-CardBus bridge) */
/* I don't think there are any EFI machines with CardBus slots, but let's define these anyways */
#define CARDBUS_SOCKET_BASE_ADDR 0x10 // u32

#define CARDBUS_CAPABILITIES_OFFSET 0x14 // u8
#define RESERVED3 0x15 // u8
#define CARDBUS_SECONDARY_STATUS 0x16 // u16

#define CARDBUS_PCI_BUS_NUM 0x18 // u8
#define CARDBUS_BUS_NUM 0x19 // u8
#define CARDBUS_SUBORDINATE_BUS_NUM 0x1A // u8
#define CARDBUS_LATENCY_TIMER 0x1B // u8

#define CARDBUS_MEMORY_BASE_0 0x1C // u32

#define CARDBUS_MEMORY_LIMIT_0 0x20 // u32

#define CARDBUS_MEMORY_BASE_1 0x24 // u32

#define CARDBUS_MEMORY_LIMIT_1 0x28 // u32

#define CARDBUS_IO_BASE_0 0x2C // u32

#define CARDBUS_IO_LIMIT_0 0x30 // u32

#define CARDBUS_IO_BASE_1 0x34 // u32

#define CARDBUS_IO_LIMIT_1 0x38 // u32

/* INTERRUPT_LINE, INTERRUPT_PIN */
#define CARDBUS_BRIDGE_CONTROL 0x3E // u16

#define CARDBUS_SUBSYSTEM_DEVICE_ID 0x40 // u16
#define CARDBUS_SUBSYSTEM_VENDOR_ID 0x42 // u16

#define CARDBUS_PCMCIA_BASE_ADDR 0x44 // u32

typedef struct {
    u8 Bus;
    u8 Slot;
    u8 Function;
} PCIDEVICE, *PPCIDEVICE;

extern void AppleTVFixupIdeController();

#endif //_PCI_H
