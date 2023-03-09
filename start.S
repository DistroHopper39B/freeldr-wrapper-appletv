        .text
        .globl  start

start:


		# i don't know what this code here does
		mov %ds, %bx
		mov %bx, %es
		mov %eax, %ebp
		mov %ebp, %ebx
		mov $0, %ax
		mov %ax, %fs
		mov %ax, %gs
		pushl %ebp
		xorl %ebp, %ebp

		# terrible serial code, but i dont want to use .data
		# this code sends "ATV: OK" to the serial port, so that i know that the program has gotten to this point
		movb $'\n', %al
		movw $0x3f8, %dx # serial port
		outb %al, %dx
		movb $'T', %al
		outb %al, %dx
		movb $'E', %al
		outb %al, %dx
		movb $'X', %al
		outb %al, %dx
		movb $'T', %al
		outb %al, %dx
		movb $' ', %al
		outb %al, %dx
		movb $'O', %al
		outb %al, %dx
		movb $'K', %al
		outb %al, %dx
		movb $'\n', %al
		outb %al, %dx
		
		# start program and halt the CPU in event of a failure
		call _playground_start
		hlt
