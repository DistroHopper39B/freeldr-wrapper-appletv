#!/bin/bash
sudo mount ~/atv-debug/debug.img /mnt/atv_debug
cp $1 /mnt/atv_debug/mach_kernel
sudo umount /mnt/atv_debug
rm ~/atv-debug/debug.vdi
VBoxManage convertfromraw ~/atv-debug/debug.img ~/atv-debug/debug.vdi
VBoxManage internalcommands sethduuid ~/atv-debug/debug.vdi 6f9cc44f-f35a-435a-ae16-6a5407fad38d
VBoxManage startvm "atv-bootloader Debug VM" 
socat stdio /tmp/vbox-serial