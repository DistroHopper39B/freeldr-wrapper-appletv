//
// Created by distrohopper39b on 11/26/23.
// This is a small PCI driver created to switch the Prog IF of the IDE controller.
//

#include <fldrwrapper.h>

pci_slot_info *slot;

/* Read 32-bit longword from PCI bus. */
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
u8 PciGetClassCode(u8 Bus, u8 Slot, u8 Func) {
    u8 ClassCode;

    ClassCode = PciReadByte(Bus, Slot, Func, 0xB);
    return ClassCode;
}

/* Get subclass code */
u8 PciGetSubclassCode(u8 Bus, u8 Slot, u8 Func) {
    u8 SubclassCode;

    SubclassCode = PciReadByte(Bus, Slot, Func, 0xA);
    return SubclassCode;
}

/* Get PCI vendor ID */
u16 PciGetVendorId(u8 Bus, u8 Slot, u8 Func) {
    u16 VendorId;

    VendorId = PciReadWord(Bus, Slot, Func, 0x0);
    return VendorId;
}


/* Get PCI device ID */
u16 PciGetDeviceId(u8 Bus, u8 Slot, u8 Func) {
    u16 DeviceId;

    DeviceId = PciReadWord(Bus, Slot, Func, 0x2);
    return DeviceId;
}

/* Get Prog If */
u8 PciGetProgIf(u8 Bus, u8 Slot, u8 Func) {
    u8 ProgIf;

    ProgIf = PciReadByte(Bus, Slot, Func, 0x9);
    return ProgIf;
}

/* Get IRQ */
u8 PciGetIRQ(u8 Bus, u8 Slot, u8 Func) {
    u8 IRQ;

    IRQ = PciReadByte(Bus, Slot, Func, 0x3C);
    return IRQ;
}
/* Detect PCI IDE controller. */
pci_slot_info *DetectPciIdeController() {
    u8 Bus, Slot, Func;

    debug_printf("PCI: Enumerating PCI bus...\n");

    for(Bus = 0; Bus < 255; Bus++) {
        for(Slot = 0; Slot < 32; Slot++) {
            for(Func = 0; Func < 8; Func++) {
                if (PciGetVendorId(Bus, Slot, Func) != PCI_INVALID_DEVICE &&
                    PciGetClassCode(Bus, Slot, Func) == PCI_CLASS_CODE_MASS_STORAGE &&
                    PciGetSubclassCode(Bus, Slot, Func) == PCI_MASS_STORAGE_SUBCLASS_CODE_IDE_CONTROLLER) {
                    debug_printf("PCI: Found IDE controller at %02x:%lx.%lx [%lX:%lX] with Prog IF %02x\n",
                                 Bus, Slot, Func,
                                 PciGetVendorId(Bus, Slot, Func),
                                 PciGetDeviceId(Bus, Slot, Func),
                                 PciGetProgIf(Bus, Slot, Func));
                    slot->bus = Bus;
                    slot->slot = Slot;
                    slot->function = Func;
                }
            }
        }
    }
    return slot;
}

/* Change Prog IF value to work correctly with FreeLoader. */

void AppleTVFixupIdeController() {
    u32 PciOffsetOld, PciOffsetNew;
    u32 ClassCode, Subclass, ProgIf, RevId;

    /* Get IDE controller */
    slot = DetectPciIdeController();

    /* Read entire offset */
    PciOffsetOld = PciReadLong(slot->bus, slot->slot, slot->function, 0x8); // Prog IF offset

    /* Split data */
    ClassCode = (PciOffsetOld >> 24) & 0xFF;
    Subclass = (PciOffsetOld >> 16) & 0xFF;
    ProgIf = (PciOffsetOld >> 8) & 0xFF;
    RevId = (PciOffsetOld) & 0xFF;

    if(ProgIf == 0x8A) {
        /* no work needed, expected result in virtualbox */
        debug_printf("PCI: Nothing to do.\n");
    } else if(ProgIf == 0x8F) { // expected result on real hardware
        /* change value */
        ProgIf = 0x8A;

        /* reconstruct data */
        PciOffsetNew = (u32)(ClassCode << 24) | (u32)(Subclass << 16) | (u32)(ProgIf << 8) | (u32)(RevId);

        /* write out new Prog If value */
        debug_printf("PCI: Writing out new Prog IF value...\n");
        PciWriteLong(slot->bus, slot->slot, slot->function, 0x8, PciOffsetNew);
    } else {
        ChangeColors(0xFFFF0000, 0x00000000);
        printf("PCI: FATAL: Unsupported IDE controller!\n");
        fail();
    }
    // test code
    //PciWriteLong(0x00, 0x1D, 0x0, 0x3C, 0x0000100B);
    //PciWriteLong(0x00, 0x1D, 0x7, 0x3C, 0x0000100B);
    //PciWriteLong(0x00, 0x1F, 0x1, 0x3C, 0x00001012);

}