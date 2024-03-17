/*
 * PROJECT:     FreeLoader wrapper for Apple TV
 * LICENSE:     MIT (https://spdx.org/licenses/MIT)
 * PURPOSE:     PCI functions for IDE controller fixup for the original Apple TV
 * COPYRIGHT:   Copyright 2023-2024 DistroHopper39B (distrohopper39b.business@gmail.com)
 */

/* INCLUDES *******************************************************************/

#include <fldrwrapper.h>

/* GLOBALS ********************************************************************/

PPCIDEVICE PciDevice;

#define PciIsValidDevice(bus, slot, func) \
PciReadWord(bus, slot, func, VENDOR_ID) != 0xFFFF

u8 IrqCounter = 5;


/* FUNCTIONS ******************************************************************/

/* Read 32-bit longword from PCI bus. */
static
u32 PciReadLong(u8 Bus, u8 Slot, u8 Func, u8 Offset) {
    u32 Address;
    u32 LBus = (u32) Bus;
    u32 LSlot = (u32) Slot;
    u32 LFunc = (u32) Func;
    u32 Output = 0;

    /* Create configuration address */
    Address = (u32)((LBus << 16) | (LSlot << 11) | (LFunc << 8) | (Offset & 0xFC) | ((u32) 0x80000000));

    /* Write out configuration address */
    outl(PCI_TYPE1_ADDRESS_PORT, Address);

    /* Read data */
    Output = (u32)((inl(PCI_TYPE1_DATA_PORT) >> (Offset & 2) * 8) & 0xFFFFFFFF);
    return Output;
}

/* Write 32-bit longword to PCI bus. */
static
void PciWriteLong(u8 Bus, u8 Slot, u8 Func, u8 Offset, u32 Value) {
    u32 Address;
    u32 LBus = (u32) Bus;
    u32 LSlot = (u32) Slot;
    u32 LFunc = (u32) Func;

    /* Create configuration address */
    Address = (u32)((LBus << 16) | (LSlot << 11) | (LFunc << 8) | (Offset & 0xFC) | ((u32) 0x80000000));

    /* Write out configuration address */
    outl(PCI_TYPE1_ADDRESS_PORT, Address);

    /* Write out value */
    outl(PCI_TYPE1_DATA_PORT, Value);
}

/* Read 16-bit word from PCI bus. */
static
u16 PciReadWord(u8 Bus, u8 Slot, u8 Func, u8 Offset) {
    u32 Address;
    u32 LBus = (u32) Bus;
    u32 LSlot = (u32) Slot;
    u32 LFunc = (u32) Func;
    u16 Output = 0;

    /* Create configuration address */
    Address = (u32)((LBus << 16) | (LSlot << 11) | (LFunc << 8) | (Offset & 0xFC) | ((u32) 0x80000000));

    /* Write out configuration address */
    outl(PCI_TYPE1_ADDRESS_PORT, Address);

    /* Read data */
    Output = (u16)((inl(PCI_TYPE1_DATA_PORT) >> (Offset & 2) * 8) & 0xFFFF);
    return Output;
}

/* Read 8-bit byte from PCI bus. */
static
u8 PciReadByte(u8 Bus, u8 Slot, u8 Func, u8 Offset) {
    u32 Address;
    u32 LBus = (u32) Bus;
    u32 LSlot = (u32) Slot;
    u32 LFunc = (u32) Func;
    UINT8 Output = 0;

    /* Create configuration address */
    Address = (u32)((LBus << 16) | (LSlot << 11) | (LFunc << 8) | (Offset & 0xFC) | ((u32) 0x80000000));

    /* Write out configuration address */
    outl(PCI_TYPE1_ADDRESS_PORT, Address);

    /* Read data */
    Output = (u8)((inl(PCI_TYPE1_DATA_PORT) >> (Offset & 3) * 8) & 0xFF);
    return Output;
}

/* Get class code */
static
u8 PciGetClassCode(u8 Bus, u8 Slot, u8 Func) {
    u8 ClassCode;

    ClassCode = PciReadByte(Bus, Slot, Func, 0xB);
    return ClassCode;
}

/* Get subclass code */
static
u8 PciGetSubclassCode(u8 Bus, u8 Slot, u8 Func) {
    u8 SubclassCode;

    SubclassCode = PciReadByte(Bus, Slot, Func, 0xA);
    return SubclassCode;
}

/* Get PCI vendor ID */
static
u16 PciGetVendorId(u8 Bus, u8 Slot, u8 Func) {
    u16 VendorId;

    VendorId = PciReadWord(Bus, Slot, Func, 0x0);
    return VendorId;
}


/* Get PCI device ID */
static
u16 PciGetDeviceId(u8 Bus, u8 Slot, u8 Func) {
    u16 DeviceId;

    DeviceId = PciReadWord(Bus, Slot, Func, 0x2);
    return DeviceId;
}

/* Get Prog If */
static
u8 PciGetProgIf(u8 Bus, u8 Slot, u8 Func) {
    u8 ProgIf;

    ProgIf = PciReadByte(Bus, Slot, Func, 0x9);
    return ProgIf;
}

/* Detect PCI IDE controller. */
static
PPCIDEVICE DetectPciIdeController() {
    u8 Bus, Slot, Func;

    debug_printf("PCI: Enumerating PCI bus...\n");

    for (Bus = 0; Bus < 255; Bus++) {
        for (Slot = 0; Slot < 32; Slot++) {
            for (Func = 0; Func < 8; Func++) {
                if (PciIsValidDevice(Bus, Slot, Func) &&
                    PciGetClassCode(Bus, Slot, Func) == PCI_CLASS_CODE_MASS_STORAGE &&
                    PciGetSubclassCode(Bus, Slot, Func) == PCI_MASS_STORAGE_SUBCLASS_CODE_IDE_CONTROLLER) {
                    debug_printf("PCI: Found IDE controller at %02x:%lx.%lx [%lX:%lX] with Prog IF %02x\n",
                                 Bus, Slot, Func,
                                 PciGetVendorId(Bus, Slot, Func),
                                 PciGetDeviceId(Bus, Slot, Func),
                                 PciGetProgIf(Bus, Slot, Func));
                    PciDevice->Bus = Bus;
                    PciDevice->Slot = Slot;
                    PciDevice->Function = Func;
                    goto Done;
                }
            }
        }
    }
    printf("No IDE controller found, are you running this on a normal Mac?\n");
    return NULL;

    Done:
    return PciDevice;
}

void PciAssignIrq(u8 Bus, u8 Slot, u8 Func, u8 Irq) {
    // don't do this. seriously.
    u8 UnusedByte0;
    u8 UnusedByte1;
    u8 UnusedByte2;
    u8 InterruptLine;

    u32 IrqOffsetOld, IrqOffsetNew;

    IrqOffsetOld = PciReadLong(Bus, Slot, Func, INTERRUPT_LINE);

    UnusedByte0 = (IrqOffsetOld >> 24) & 0xFF;
    UnusedByte1 = (IrqOffsetOld >> 16) & 0xFF;
    UnusedByte2 = (IrqOffsetOld >> 8) & 0xFF;
    InterruptLine = (IrqOffsetOld) & 0xFF;

    debug_printf("PCI: Assigning IRQ...\n");
    InterruptLine = Irq;

    IrqOffsetNew = (u32)(UnusedByte0 << 24) | (u32)(UnusedByte1 << 24) | (u32)(UnusedByte2 << 8) | (u32)(InterruptLine);
    PciWriteLong(Bus, Slot, Func, INTERRUPT_LINE, IrqOffsetNew);
}

/* Change Prog IF value to work correctly with FreeLoader. */
void AppleTVFixupIdeController() {
    u32 PciOffsetOld, PciOffsetNew;
    u32 ClassCode, Subclass, ProgIf, RevId;

    /* Get IDE controller */
    PciDevice = DetectPciIdeController();

    /* Read entire offset */
    PciOffsetOld = PciReadLong(PciDevice->Bus, PciDevice->Slot, PciDevice->Function, 0x8); // Prog IF offset

    /* Split data */
    ClassCode = (PciOffsetOld >> 24) & 0xFF;
    Subclass = (PciOffsetOld >> 16) & 0xFF;
    ProgIf = (PciOffsetOld >> 8) & 0xFF;
    RevId = (PciOffsetOld) & 0xFF;

    if (ProgIf == 0x8A) {
        /* no work needed, expected result in virtualbox */
        debug_printf("PCI: Nothing to do.\n");
    } else if (ProgIf == 0x8F) { // expected result on real hardware
        /* change value */
        ProgIf = 0x8A;

        /* reconstruct data */
        PciOffsetNew = (u32)(ClassCode << 24) | (u32)(Subclass << 16) | (u32)(ProgIf << 8) | (u32)(RevId);

        /* write out new Prog If value */
        debug_printf("PCI: Writing out new Prog IF value...\n");
        PciWriteLong(PciDevice->Bus, PciDevice->Slot, PciDevice->Function, 0x8, PciOffsetNew);
    } else {
        ChangeColors(0xFFFF0000, 0x00000000);
        printf("PCI: FATAL: Unsupported IDE controller!\n");
        fail();
    }

    //PciAssignIrq(0x00, 0x01, 0x0, 1);
    //PciAssignIrq(0x00, 0x07, 0x0, 1);
    //PciAssignIrq(0x00, 0x1b, 0x0, 1);
    //PciAssignIrq(0x00, 0x1c, 0x0, 1);
    //PciAssignIrq(0x00, 0x1c, 0x1, 1);
    //PciAssignIrq(0x00, 0x1c, 0x2, 1);
    //PciAssignIrq(0x00, 0x1c, 0x3, 1);
    //PciAssignIrq(0x00, 0x1d, 0x0, 1);
    //PciAssignIrq(0x00, 0x1d, 0x7, 1);
    //PciAssignIrq(0x00, 0x1f, 0x1, 1);
    //PciAssignIrq(0x00, 0x1f, 0x3, 1);
    //PciAssignIrq(0x01, 0x00, 0x0, 1);
    //PciAssignIrq(0x02, 0x00, 0x0, 1);
    //PciAssignIrq(0x06, 0x03, 0x0, 1);

    //PciAssignIrq(0x00, 0x02, 0x0, 0);
    //PciAssignIrq(0x00, 0x03, 0x0, 0);
    //PciAssignIrq(0x00, 0x04, 0x0, 0);
    //PciAssignIrq(0x00, 0x05, 0x0, 0);
    //PciAssignIrq(0x00, 0x07, 0x0, 0);
    //PciAssignIrq(0x00, 0x1f, 0x0, 0);
    //PciAssignIrq(0x00, 0x1f, 0x1, 0);
    //PciAssignIrq(0x00, 0x1f, 0x2, 0);







}