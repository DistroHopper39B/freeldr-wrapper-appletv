//
// Created by distrohopper39b on 11/26/23.
// This is a small PCI driver created to switch the Prog IF of the IDE controller.
//

#ifndef PLAYGROUND_PCI_H
#define PLAYGROUND_PCI_H

#include <fldrwrapper.h>

#define PCI_TYPE1_ADDRESS_PORT 0xCF8
#define PCI_TYPE1_DATA_PORT 0xCFC

//
// PCI Class Codes
//
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

//
// Mass storage controller subclasses
//

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

#define PCI_INVALID_DEVICE 0xFFFF

typedef struct _pci_device {
    u8 bus;
    u8 slot;
    u8 function;
} pci_slot_info;

extern void AppleTVFixupIdeController();

#endif //PLAYGROUND_PCI_H
