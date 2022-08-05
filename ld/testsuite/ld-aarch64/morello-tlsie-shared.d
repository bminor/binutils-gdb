# Checking the following:
#     Relaxations have not occured.
#     Values point to the GOT.
#     Fragment in GOT contains size hint if known.
#     Fragment in GOT does not contain any offset (except for the relocation
#     against a local symbol where the fragment is populated in order to give
#     the dynamic loader a base to use).
#
# Check the disassembly of rodata just to demonstrate we haven't emitted the
# TLS data stubs for an IE -> LE relaxation.
#
# Also check the behavior on a global and local symbol to check the behaviour
# of a local symbol.
#source: morello-tlsie.s
#target: [check_shared_lib_support]
#as: -march=morello+c64
#ld: -shared
#objdump: -DR -j .rodata -j .text -j .got

.*: +file format .*


Disassembly of section \.text:

[0-9a-f]+ <_start>:
#record: GOT_PAGE
 +[0-9a-f]+:	[0-9a-f]+ 	adrp	c2, ([0-9a-f]+) .*
#record: VAR1_OFFSET
 +[0-9a-f]+:	[0-9a-f]+ 	add	c2, c2, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400442 	ldp	x2, x1, \[c2\]
#check: PAGE string tolower $GOT_PAGE
 +[0-9a-f]+:	[0-9a-f]+ 	adrp	c2, PAGE .*
#record: VAR2_OFFSET
 +[0-9a-f]+:	[0-9a-f]+ 	add	c2, c2, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400442 	ldp	x2, x1, \[c2\]
 +[0-9a-f]+:	[0-9a-f]+ 	adrp	c2, PAGE .*
#record: VAR3_OFFSET
 +[0-9a-f]+:	[0-9a-f]+ 	add	c2, c2, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400442 	ldp	x2, x1, \[c2\]
 +[0-9a-f]+:	[0-9a-f]+ 	adrp	c2, PAGE .*
#record: VAR4_OFFSET
 +[0-9a-f]+:	[0-9a-f]+ 	add	c2, c2, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400442 	ldp	x2, x1, \[c2\]

Disassembly of section \.rodata:

# This is just the "Hello world\n" we put there to cause some interference with
# the .rodata section we create for the static tests.
[0-9a-f]+ <.*>:
 +[0-9a-f]+:	6c6c6548 	ldnp	d8, d25, \[x10, #-320\]
 +[0-9a-f]+:	6f77206f 	umlal2	v15.4s, v3.8h, v7.h\[3\]
 +[0-9a-f]+:	0a646c72 	bic	w18, w3, w4, lsr #27
	\.\.\.

Disassembly of section \.got:

[0-9a-f]+ <\.got>:
 +[0-9a-f]+:	.*
	\.\.\.
#check: VAR4_LOC aarch64_page_plus_offset $GOT_PAGE $VAR4_OFFSET
#check: VAR3_LOC aarch64_page_plus_offset $GOT_PAGE $VAR3_OFFSET
#check: VAR2_LOC aarch64_page_plus_offset $GOT_PAGE $VAR2_OFFSET
#check: VAR1_LOC aarch64_page_plus_offset $GOT_PAGE $VAR1_OFFSET
#check: VAR4_TLS_OFFSET format %08x [expr "65592 + 65540 + 100 + 10"]
#check: VAR4_TLS_OFF format %x [expr "65592 + 65540 + 100 + 10"]
#check: VAR3_TLS_OFFSET format %08x [expr "65592 + 65540 + 100"]
#check: VAR3_TLS_OFF format %x [expr "65592 + 65540 + 100"]
#check: VAR2_TLS_OFFSET format %08x [expr "65592 + 65540"]
#check: VAR2_TLS_OFF format %x [expr "65592 + 65540"]
 +VAR2_LOC:	VAR2_TLS_OFFSET 	.*
			VAR2_LOC: R_MORELLO_TPREL128	\*ABS\*\+0xVAR2_TLS_OFF
 +[0-9a-f]+:	00000000 .*
 +[0-9a-f]+:	00000064 .*
 +[0-9a-f]+:	00000000 .*
 +VAR3_LOC:	VAR3_TLS_OFFSET 	.*
			VAR3_LOC: R_MORELLO_TPREL128	\*ABS\*\+0xVAR3_TLS_OFF
 +[0-9a-f]+:	00000000 .*
 +[0-9a-f]+:	0000000a .*
	\.\.\.
			VAR4_LOC: R_MORELLO_TPREL128	y
 +[0-9a-f]+:	0000000f .*
	...
			VAR1_LOC: R_MORELLO_TPREL128	a

