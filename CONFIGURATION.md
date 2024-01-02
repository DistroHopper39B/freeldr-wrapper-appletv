# Configurating this bootloader

Boot-time configuration is done within the [com.apple.Boot.plist](USBData/com.apple.Boot.plist) file. The following
keys are defined within this file:

`Background Color`: Controls the background color. This is an RGB value that must stored in decimal form, meaning that 0
is black and 16777215 (0xFFFFFF) is white.

`Boot Logo`: This is an image file displayed on the boot screen. If invalid or left blank, a grey Apple logo is
shown.

`Kernel`: This is the name of the kernel file to be loaded by `boot.efi`.

`Kernel Flags`:  These are the flags sent to the kernel. In `freeldr-wrapper-appletv`, the valid commands are:

* `timeout=[seconds]`: This is how long FreeLoader will pause at the loading screen before booting the OS. Generally, 
this is used to allow choosing advanced options or other OSes, however the Apple TV build of FreeLoader has no keyboard
support, so there is no way to choose these options without using `defaultos` or editing `freeldr.ini`. The
`com.apple.Boot.plist` file has this set to `0` by default to always boot to the default option.
* `defaultos=[name]`: This is the name of the OS to load as specified in `freeldr.ini`. The name must be the non-spaced
name (e.g. `ReactOS_Debug` instead of "ReactOS (Debug)") as specified in the `Operating Systems` section in `freeldr.ini`.
* `debug=[options]`: This enables debug printing to different locations in FreeLoader. See
`boot/freeldr/freeldr/lib/debug.c` within ReactOS for more information, but the only one of use on the Apple TV is screen
debugging, which can be enabled either by holding Command-V or Command-S at startup (see below) or by adding
`debug=debugport=screen` to the kernel flags (note that unlike Command-V or Command-S, this will not turn on first-stage
loader logging)
* `-v`: This enables debug printing to the screen in FreeLoader and this loader. In Mac OS X, this argument is used for
verbose mode, and holding Command/Windows-V at boot has the same effect.
* `-s`: This enables debug printing to the screen in FreeLoader and this loader. In Mac OS X, this argument is used for
single-user mode, and holding Command/Windows-S at boot has the same effect.