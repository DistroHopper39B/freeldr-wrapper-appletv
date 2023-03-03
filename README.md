# Apple TV Playground
A small early stage bootloader and playground for the Apple TV (1st Generation) based on [atv-bootloader](https://github.com/davilla/atv-bootloader).

### What this can do now:
- Print text to a display (implementation from atv-bootloader)
- Print text to a serial port in e.g. VirtualBox (my own implementation)
- Basic CPU-clock-based timing (implementation from atv-bootloader)
- Compiles to a Mach-O executable and takes the place of the `mach_kernel` (implementation from atv-bootloader)
### Future implementation ideas:
- Arbitrary grid-based pixel manipulation 
- Color support
- ANSI escape code support in `printk()`
- Interactive demos (controlled by the serial terminal)
- ~~Compilation to Mach-O using a linker (*.ld) script, so that a cross compiler is not required~~ Not doable as of now
### How this works:
Like Intel Macs, the Apple TV (1st Generation) uses EFI firmware to boot into its own operating system (based on Mac OS X 10.4.7). Unlike Intel Macs, however, the Apple TV can only boot from a single EFI file (located at `/boot.efi` on the Apple TV's hard drive). This file is likely verified in the firmware with some form of hashing. However, atv-bootloader (and atv-playground) get around this by replacing the kernel file (loaded by `boot.efi`) with a file that acts like the Mac OS X kernel, but is actually either a boot loader for Linux (in the case of atv-bootloader) or a mini-environment (in the case of atv-playground). Because this file is not verified whatsoever by the firmware or `boot.efi`, it can be replaced easily.
