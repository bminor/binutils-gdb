
tmpdir/arm-lib.so:     file format elf32-littlearm
architecture: arm, flags 0x00000150:
HAS_SYMS, DYNAMIC, D_PAGED
start address 0x.*

Disassembly of section .text:

.* <lib_func1>:
 .*:	e1a0c00d 	mov	ip, sp
 .*:	e92dd800 	stmdb	sp!, {fp, ip, lr, pc}
 .*:	ebfffffe 	bl	.* <lib_func1\+0x8>
 .*:	e89d6800 	ldmia	sp, {fp, sp, lr}
 .*:	e12fff1e 	bx	lr

.* <lib_func2>:
 .*:	e12fff1e 	bx	lr
