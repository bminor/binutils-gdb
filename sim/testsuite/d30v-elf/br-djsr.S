# check return address correctly set by a djsr insn
	add r3, r0, last
	sub r3, r3, start
start:	
	djsr r3, dest
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
last:
	add r2, r2, 1 || nop
nexti:	
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop
	add r2, r2, 1 || nop

.align 4
dest:
	sub r2, r62, nexti
	.long 0x0e000004, 0x00f00000
