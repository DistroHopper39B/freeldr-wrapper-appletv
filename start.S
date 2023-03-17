        .text
        .globl  start

start:
		pushl %eax
		call _playground_start
		hlt
