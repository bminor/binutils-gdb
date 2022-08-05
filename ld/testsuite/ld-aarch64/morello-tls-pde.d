# Ensure the following things for dynamically linked PDE's:
#   1) We relax the global dynamic reference to an external value to an IE
#      reference.
#   2) We relax an IE reference to a value defined in the executable down to an
#      LE reference (with the correct location and emition of our data stub).
#   3) LE relocation is handled without messing up.
#as: -march=morello+c64
#ld: tmpdir/tls-shared.so
#objdump: -DR -j .text -j .got -j .rodata

.*: +file format .*


Disassembly of section \.text:

[0-9a-f]+ <_start>:
 +[0-9a-f]+:	c2c253c0 	ret	c30

[0-9a-f]+ <load_w>:
 +[0-9a-f]+:	c29bd040 	mrs	c0, ctpidr_el0
 +[0-9a-f]+:	d2a00001 	movz	x1, #0x0, lsl #16
 +[0-9a-f]+:	f2800401 	movk	x1, #0x20
 +[0-9a-f]+:	c2a16000 	add	c0, c0, x1, uxtx
 +[0-9a-f]+:	d2a00001 	movz	x1, #0x0, lsl #16
 +[0-9a-f]+:	f2800081 	movk	x1, #0x4
 +[0-9a-f]+:	c2c10000 	scbnds	c0, c0, x1
 +[0-9a-f]+:	b9400000 	ldr	w0, \[c0\]
 +[0-9a-f]+:	c2c253c0 	ret	c30

[0-9a-f]+ <load_w_ie>:
#record: RODATA_PAGE
 +[0-9a-f]+:	.* 	adrp	c0, ([0-9a-f]+) .*
#record: W_IE_OFF
 +[0-9a-f]+:	.* 	add	c0, c0, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400402 	ldp	x2, x1, \[c0\]
 +[0-9a-f]+:	c29bd040 	mrs	c0, ctpidr_el0
 +[0-9a-f]+:	c2a26000 	add	c0, c0, x2, uxtx
 +[0-9a-f]+:	c2c10000 	scbnds	c0, c0, x1
 +[0-9a-f]+:	b9400000 	ldr	w0, \[c0\]
 +[0-9a-f]+:	c2c253c0 	ret	c30

[0-9a-f]+ <load_w1>:
#record: GOTPAGE
 +[0-9a-f]+:	.* 	adrp	c0, ([0-9a-f]+) .*
#record: W1_OFF
 +[0-9a-f]+:	.* 	add	c0, c0, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400402 	ldp	x2, x1, \[c0\]
 +[0-9a-f]+:	c29bd040 	mrs	c0, ctpidr_el0
 +[0-9a-f]+:	c2a26000 	add	c0, c0, x2, uxtx
 +[0-9a-f]+:	c2c10000 	scbnds	c0, c0, x1
 +[0-9a-f]+:	b9400000 	ldr	w0, \[c0\]
 +[0-9a-f]+:	c2c253c0 	ret	c30

[0-9a-f]+ <load_w2>:
 +[0-9a-f]+:	c29bd042 	mrs	c2, ctpidr_el0
#check: PAGE string tolower $GOTPAGE
 +[0-9a-f]+:	.* 	adrp	c0, PAGE .*
#record: W2_OFF
 +[0-9a-f]+:	.* 	add	c0, c0, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400400 	ldp	x0, x1, \[c0\]
 +[0-9a-f]+:	c2a06040 	add	c0, c2, x0, uxtx
 +[0-9a-f]+:	c2c10000 	scbnds	c0, c0, x1
 +[0-9a-f]+:	b9400000 	ldr	w0, \[c0\]
 +[0-9a-f]+:	c2c253c0 	ret	c30

Disassembly of section \.rodata:

#check: W_DATA_LOC aarch64_page_plus_offset $RODATA_PAGE $W_IE_OFF
[0-9a-f]+ <.rodata>:
 +W_DATA_LOC:	00000020 	udf	#32
 +[0-9a-f]+:	00000000 	udf	#0
 +[0-9a-f]+:	00000004 	udf	#4
 +[0-9a-f]+:	00000000 	udf	#0

Disassembly of section \.got:

#check: W1_LOC aarch64_page_plus_offset $GOTPAGE $W1_OFF
#check: W2_LOC aarch64_page_plus_offset $GOTPAGE $W2_OFF
[0-9a-f]+ <.*>:
.*
	\.\.\.
			W1_LOC: R_MORELLO_TPREL128	w1
			W2_LOC: R_MORELLO_TPREL128	w2
