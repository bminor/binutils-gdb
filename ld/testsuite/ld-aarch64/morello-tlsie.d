# Checking the following:
#     Relaxations have occured.
#     .rodata stubs have been emitted.
#     Values are correct.
#     Include some other data in .rodata in order to check we offset correctly.
#
# Also check the behavior on a global and local symbol (just in case).
#as: -march=morello+c64
#ld: -static
#objdump: -Dr -j .rodata -j .text

.*: +file format .*


Disassembly of section \.text:

[0-9a-f]+ <_start>:
#record: RODATA_PAGE
 +[0-9a-f]+:	[0-9a-f]+ 	adrp	c2, ([0-9a-f]+) .*
#record: STUB1_OFFSET
 +[0-9a-f]+:	[0-9a-f]+ 	add	c2, c2, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400442 	ldp	x2, x1, \[c2\]
#check: PAGE string tolower $RODATA_PAGE
 +[0-9a-f]+:	[0-9a-f]+ 	adrp	c2, PAGE .*
#record: STUB2_OFFSET
 +[0-9a-f]+:	[0-9a-f]+ 	add	c2, c2, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400442 	ldp	x2, x1, \[c2\]
 +[0-9a-f]+:	[0-9a-f]+ 	adrp	c2, PAGE .*
#record: STUB3_OFFSET
 +[0-9a-f]+:	[0-9a-f]+ 	add	c2, c2, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400442 	ldp	x2, x1, \[c2\]
 +[0-9a-f]+:	[0-9a-f]+ 	adrp	c2, PAGE .*
#record: STUB4_OFFSET
 +[0-9a-f]+:	[0-9a-f]+ 	add	c2, c2, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400442 	ldp	x2, x1, \[c2\]

Disassembly of section \.rodata:

[0-9a-f]+ <.*>:
 +[0-9a-f]+:	6c6c6548 	ldnp	d8, d25, \[x10, #-320\]
 +[0-9a-f]+:	6f77206f 	umlal2	v15.4s, v3.8h, v7.h\[3\]
 +[0-9a-f]+:	0a646c72 	bic	w18, w3, w4, lsr #27
 +[0-9a-f]+:	00000000 	udf	#0
#check: STUB1_LOC aarch64_page_plus_offset $RODATA_PAGE $STUB1_OFFSET
#check: HEX_OFF1  format %08x [expr "65540 + 32"]
#check: HEX_SIZE1  format %08x 65592
 +STUB1_LOC:	HEX_OFF1 	.*
 +[0-9a-f]+:	00000000 	udf	#0
 +[0-9a-f]+:	HEX_SIZE1 	.*
 +[0-9a-f]+:	00000000 	udf	#0
#check: STUB2_LOC aarch64_page_plus_offset $RODATA_PAGE $STUB2_OFFSET
#check: HEX_OFF2  format %08x [expr "65540 + 32 + 65592"]
#check: HEX_SIZE2  format %08x 100
 +STUB2_LOC:	HEX_OFF2 	.*
 +[0-9a-f]+:	00000000 	udf	#0
 +[0-9a-f]+:	HEX_SIZE2 	.*
 +[0-9a-f]+:	00000000 	udf	#0
#check: STUB3_LOC aarch64_page_plus_offset $RODATA_PAGE $STUB3_OFFSET
#check: HEX_OFF3  format %08x [expr "65540 + 32 + 65592 + 100"]
#check: HEX_SIZE3  format %08x 10
 +STUB3_LOC:	HEX_OFF3 	.*
 +[0-9a-f]+:	00000000 	udf	#0
 +[0-9a-f]+:	HEX_SIZE3 	.*
 +[0-9a-f]+:	00000000 	udf	#0
#check: STUB4_LOC aarch64_page_plus_offset $RODATA_PAGE $STUB4_OFFSET
#check: HEX_OFF4  format %08x [expr "65540 + 32 + 65592 + 100 + 10"]
#check: HEX_SIZE4  format %08x 15
 +STUB4_LOC:	HEX_OFF4 	.*
 +[0-9a-f]+:	00000000 	udf	#0
 +[0-9a-f]+:	HEX_SIZE4 	.*
 +[0-9a-f]+:	00000000 	udf	#0
