# Checking the following:
#     tprel relocations have provided the offset.
#     tprel relocations account for the TCB (i.e. 32 extra bytes on top of the
#     size of `p` added to the thread pointer in order to get to `a`).
#
#     size relocations have given us the size.
#
# Also check the behaviour on a global and local symbol (just in case).
#source: morello-tlsle.s
#as: -march=morello+c64
#ld: -pie
#objdump: -dr

.*: +file format .*


Disassembly of section \.text:

[0-9a-f]+ <_start>:
 +[0-9a-f]+:	d2a00022 	mov	x2, #0x10000               	// #65536
 +[0-9a-f]+:	f2800482 	movk	x2, #0x24
 +[0-9a-f]+:	d2a00021 	mov	x1, #0x10000               	// #65536
 +[0-9a-f]+:	f2800701 	movk	x1, #0x38
 +[0-9a-f]+:	d2a00042 	mov	x2, #0x20000               	// #131072
 +[0-9a-f]+:	f2800b82 	movk	x2, #0x5c
 +[0-9a-f]+:	d2a00001 	movz	x1, #0x0, lsl #16
 +[0-9a-f]+:	f2800c81 	movk	x1, #0x64
 +[0-9a-f]+:	c29bd040 	mrs	c0, ctpidr_el0
 +[0-9a-f]+:	c2a26000 	add	c0, c0, x2, uxtx
 +[0-9a-f]+:	c2c10000 	scbnds	c0, c0, x1
 +[0-9a-f]+:	b9400000 	ldr	w0, \[c0\]
 +[0-9a-f]+:	c2c253c0 	ret	c30

