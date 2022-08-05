# Checking that:
#    1) TLS sequences are not relaxed.
#    2) TLS sequences trigger a TLSDESC relocation.
#    3) TLS sequences end up referencing that TLSDESC relocation.
#    4) TLSDESC relocation contains a size fragment when the static linker
#       knows the size of this variable.
#source: morello-tlsdesc.s
#target: [check_shared_lib_support]
#as: -march=morello+c64
#ld: -shared
#objdump: -DR -j .got.plt -j .text -j .plt


.*:     file format .*


Disassembly of section .plt:

.*<.plt>:
.*:	62bf7bf0 	stp	c16, c30, \[csp, #-32\]!
.*:	908...90 	adrp	c16, .*
.*:	c2....11 	ldr	c17, \[c16, #[0-9]+\]
.*:	02....10 	add	c16, c16, #0x[0-9a-f]+
.*:	c2c21220 	br	c17
.*:	d503201f 	nop
.*:	d503201f 	nop
.*:	d503201f 	nop

.*<extf@plt>:
.*:	908...90 	adrp	c16, .*
.*:	c2....11 	ldr	c17, \[c16, #[0-9]+\]
.*:	02....10 	add	c16, c16, #0x[0-9a-f]+
.*:	c2c21220 	br	c17

.*<var1@plt>:
.*:	62bf8fe2 	stp	c2, c3, \[csp, #-16\]!
.*:	908...82 	adrp	c2, .*
.*:	908...83 	adrp	c3, .*
.*:	c2....42 	ldr	c2, \[c2, #[0-9]+\]

.*<var2@plt>:
.*:	02....63 	add	c3, c3, #0x[0-9a-f]+
.*:	c2c21040 	br	c2
.*:	d503201f 	nop
.*:	d503201f 	nop

Disassembly of section .text:

.*<f1>:
.*:	97fffff4 	bl	.* <extf@plt>
.*:	c29bd042 	mrs	c2, ctpidr_el0
#record: GOTPLT_PAGE
.*:	908...80 	adrp	c0, ([0-9a-f]+) .*
#record: VAR1_OFFSET
.*:	c2....01 	ldr	c1, \[c0, #([0-9]+)\]
#check: VAR1_HEX format %x $VAR1_OFFSET
.*:	02....00 	add	c0, c0, #0xVAR1_HEX
.*:	d503201f 	nop
.*:	c2c23020 	blr	c1

#check: GPAGE string tolower $GOTPLT_PAGE
.*<f2>:
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	908...80 	adrp	c0, GPAGE .*
#record: VAR2_OFFSET
.*:	c2....01 	ldr	c1, \[c0, #([0-9]+)\]
#check: VAR2_HEX format %x $VAR2_OFFSET
.*:	02....00 	add	c0, c0, #0xVAR2_HEX
.*:	d503201f 	nop
.*:	c2c23020 	blr	c1

.*<access_hidden>:
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	908...80 	adrp	c0, GPAGE .*
#record: VAR4_OFFSET
.*:	c2....01 	ldr	c1, \[c0, #([0-9]+)\]
#check: VAR4_HEX format %x $VAR4_OFFSET
.*:	02....00 	add	c0, c0, #0xVAR4_HEX
.*:	d503201f 	nop
.*:	c2c23020 	blr	c1

.*<access_protected>:
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	908...80 	adrp	c0, GPAGE .*
#record: VAR5_OFFSET
.*:	c2....01 	ldr	c1, \[c0, #([0-9]+)\]
#check: VAR5_HEX format %x $VAR5_OFFSET
.*:	02....00 	add	c0, c0, #0xVAR5_HEX
.*:	d503201f 	nop
.*:	c2c23020 	blr	c1

.*<_start>:
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	908...80 	adrp	c0, GPAGE .*
#record: VAR3_OFFSET
.*:	c2....01 	ldr	c1, \[c0, #([0-9]+)\]
#check: VAR3_HEX format %x $VAR3_OFFSET
.*:	02....00 	add	c0, c0, #0xVAR3_HEX
.*:	d503201f 	nop
.*:	c2c23020 	blr	c1

Disassembly of section .got.plt:
#check: VAR1_LOC aarch64_page_plus_offset $GOTPLT_PAGE [format %x $VAR1_OFFSET]
#check: VAR2_LOC aarch64_page_plus_offset $GOTPLT_PAGE [format %x $VAR2_OFFSET]
#check: VAR3_LOC aarch64_page_plus_offset $GOTPLT_PAGE [format %x $VAR3_OFFSET]
#check: VAR4_LOC aarch64_page_plus_offset $GOTPLT_PAGE [format %x $VAR4_OFFSET]
#check: VAR5_LOC aarch64_page_plus_offset $GOTPLT_PAGE [format %x $VAR5_OFFSET]
#check: VAR3_SIZE_LOC format %x [expr "24 + 0x$GOTPLT_PAGE + $VAR3_OFFSET"]
#check: VAR4_SIZE_LOC format %x [expr "24 + 0x$GOTPLT_PAGE + $VAR4_OFFSET"]
#check: VAR5_SIZE_LOC format %x [expr "24 + 0x$GOTPLT_PAGE + $VAR5_OFFSET"]

.*:
	\.\.\.
.*:	[0-9a-f]+ 	.*
			.*: R_MORELLO_JUMP_SLOT	extf
	\.\.\.
			VAR3_LOC: R_MORELLO_TLSDESC	\*ABS\*\+0x4
 +VAR3_SIZE_LOC:	00000014 .*
	\.\.\.
			VAR5_LOC: R_MORELLO_TLSDESC	var5
 +VAR5_SIZE_LOC:	00000004 .*
	\.\.\.
			VAR1_LOC: R_MORELLO_TLSDESC	var1
			VAR4_LOC: R_MORELLO_TLSDESC	\*ABS\*\+0x18
 +VAR4_SIZE_LOC:	00000004 .*
	\.\.\.
			VAR2_LOC: R_MORELLO_TLSDESC	var2
