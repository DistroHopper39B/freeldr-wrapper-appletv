cmake_minimum_required(VERSION 3.22)
project(playground C ASM)

set(CMAKE_SYSTEM_NAME macOS)
set(CMAKE_SYSTEM_PROCESSOR i386)

set(CMAKE_C_STANDARD 17)

if(UNIX)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_C_COMPILER_TARGET i386-apple-darwin8)
    set(CMAKE_C_LINK_EXECUTABLE "/opt/osxcross/bin/i386-apple-darwin8-ld -static -force_cpusubtype_ALL \
    -segalign 0x1000 -segaddr __TEXT 0x2000000 \
    -sectalign __TEXT __text 0x1000 \
    -sectalign __DATA __common 0x1000 \
    -sectalign __DATA __bss 0x1000 \
    -sectcreate __PRELINK __text /dev/null \
    -sectcreate __PRELINK __symtab /dev/null \
    -sectcreate __PRELINK __info /dev/null \
    -o <TARGET> <OBJECTS>")
    set(CMAKE_C_FLAGS "-Wall -static -nostdlib -arch i386 -fno-stack-protector -O3 -isysroot /opt/osxcross/SDK/MacOSX10.4u.sdk")
    set(CMAKE_ASM_FLAGS "-Wall -static -nostdlib -arch i386 -DASSEMBLER -target i386-apple-darwin8 -isysroot /opt/osxcross/SDK/MacOSX10.4u.sdk")
    set(CMAKE_EXE_LINKER_FLAGS "")
endif(UNIX)

if(APPLE)
    set(CMAKE_C_LINK_EXECUTABLE "ld -static -o <TARGET> <OBJECTS>")
    set(CMAKE_C_FLAGS "-static -nostdlib -arch i386 -O3 -fno-stack-protector")
    set(CMAKE_ASM_FLAGS "-static -nostdlib -arch i386 -DASSEMBLER")
endif(APPLE)

include_directories(./include)
add_executable(mach_kernel start.S playground.c utils.c vsprintf.c console.c demos.c
        tvfixes.c
)