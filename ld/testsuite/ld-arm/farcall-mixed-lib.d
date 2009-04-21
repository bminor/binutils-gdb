tmpdir/farcall-mixed-lib.so:     file format elf32-(little|big)arm
architecture: arm, flags 0x00000150:
HAS_SYMS, DYNAMIC, D_PAGED
start address 0x.*

Disassembly of section .plt:

.* <.plt>:
 .*:	e52de004 	push	{lr}		; \(str lr, \[sp, #-4\]!\)
 .*:	e59fe004 	ldr	lr, \[pc, #4\]	; .* <lib_func1-0x.*>
 .*:	e08fe00e 	add	lr, pc, lr
 .*:	e5bef008 	ldr	pc, \[lr, #8\]!
 .*:	.*
 .*:	e28fc6.* 	add	ip, pc, #.*	; 0x.*
 .*:	e28cca.* 	add	ip, ip, #.*	; 0x.*
 .*:	e5bcf.* 	ldr	pc, \[ip, #.*\]!
 .*:	e28fc6.* 	add	ip, pc, #.*	; 0x.*
 .*:	e28cca.* 	add	ip, ip, #.*	; 0x.*
 .*:	e5bcf.* 	ldr	pc, \[ip, #.*\]!
Disassembly of section .text:

.* <lib_func1>:
 .*:	e1a0c00d 	mov	ip, sp
 .*:	e92dd800 	push	{fp, ip, lr, pc}
 .*:	ebfffff. 	bl	.* <lib_func1-0x..?>
 .*:	ebfffff. 	bl	.* <lib_func1-0x..?>
 .*:	e89d6800 	ldm	sp, {fp, sp, lr}
 .*:	e12fff1e 	bx	lr
	...
 .*:	e1a00000 	.word	0xe1a00000
 .*:	e1a00000 	.word	0xe1a00000

.* <lib_func2>:
 .*:	f000 e806 	blx	1000300 <__app_func_from_thumb>
 .*:	f000 e80a 	blx	100030c <__app_func_weak_from_thumb>
 .*:	4770      	bx	lr
 .*:	46c0      	nop			\(mov r8, r8\)
 .*:	46c0      	nop			\(mov r8, r8\)
 .*:	46c0      	nop			\(mov r8, r8\)

.* <__app_func_from_thumb>:
 .*:	e59fc000 	ldr	ip, \[pc, #0\]	; 1000308 <__app_func_from_thumb\+0x8>
 .*:	e08ff00c 	add	pc, pc, ip
 .*:	feffffa8 	.word	0xfeffffa8

.* <__app_func_weak_from_thumb>:
 .*:	e59fc000 	ldr	ip, \[pc, #0\]	; 1000314 <__app_func_weak_from_thumb\+0x8>
 .*:	e08ff00c 	add	pc, pc, ip
 .*:	feffffa8 	.word	0xfeffffa8
	...

.* <lib_func3>:
 .*:	f000 e80c 	blx	200033c <__app_func_from_thumb>
 .*:	f000 e804 	blx	2000330 <__app_func_weak_from_thumb>
 .*:	4770      	bx	lr
 .*:	46c0      	nop			\(mov r8, r8\)
 .*:	46c0      	nop			\(mov r8, r8\)
 .*:	46c0      	nop			\(mov r8, r8\)

.* <__app_func_weak_from_thumb>:
 .*:	e59fc000 	ldr	ip, \[pc, #0\]	; 2000338 <__app_func_weak_from_thumb\+0x8>
 .*:	e08ff00c 	add	pc, pc, ip
 .*:	fdffff84 	.word	0xfdffff84

.* <__app_func_from_thumb>:
 .*:	e59fc000 	ldr	ip, \[pc, #0\]	; 2000344 <__app_func_from_thumb\+0x8>
 .*:	e08ff00c 	add	pc, pc, ip
 .*:	fdffff6c 	.word	0xfdffff6c
	...
