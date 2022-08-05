# Checking that:
#    1) TLS sequences are relaxed.
#    2) Relaxed TLS sequences trigger a TPREL128 relocation.
#    3) Relaxed TLS sequences end up referencing that TPREL128 relocation.
#    4) TPREL128 relocation contains a size fragment when the static linker
#       knows the size of this variable.
#target: [check_shared_lib_support]
#as: -march=morello+c64
#ld: -shared
#objdump: -DR -j .got.plt -j .text -j .plt -j .got -j .rodata


.*:     file format .*


Disassembly of section \.plt:

.*<\.plt>:
.*:	62bf7bf0 	stp	c16, c30, \[csp, #-32\]!
.*:	908...90 	adrp	c16, .*
.*:	c2....11 	ldr	c17, \[c16, #[0-9]+\]
.*:	02....10 	add	c16, c16, #0x[0-9a-f]+
.*:	c2c21220 	br	c17
.*:	d503201f 	nop
.*:	d503201f 	nop
.*:	d503201f 	nop

.*<_start@plt>:
.*:	908...90 	adrp	c16, .*
.*:	c2....11 	ldr	c17, \[c16, #[0-9]+\]
.*:	02....10 	add	c16, c16, #0x[0-9a-f]+
.*:	c2c21220 	br	c17

.*<extf@plt>:
.*:	908...90 	adrp	c16, .*
.*:	c2....11 	ldr	c17, \[c16, #[0-9]+\]
.*:	02....10 	add	c16, c16, #0x[0-9a-f]+
.*:	c2c21220 	br	c17

Disassembly of section \.text:

[0-9a-f]+ <IE_function>:
#record: GOT_PAGE
 +[0-9a-f]+:	908...80 	adrp	c0, ([0-9a-f]+) .*
#check: GPAGE string tolower $GOT_PAGE
 +[0-9a-f]+:	908...80 	adrp	c0, GPAGE .*
 +[0-9a-f]+:	908...80 	adrp	c0, GPAGE .*

.*<f1>:
.*:	97fffff9 	bl	.* <extf@plt>
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	908...80 	adrp	c0, GPAGE .*
#record: VAR1_OFFSET
.*:	........ 	add	c0, c0, #0x([0-9a-f]+)
.*:	........ 	ldp	x0, x1, \[c0\]
.*:	........ 	add	c0, c2, x0, uxtx
.*:	c2c10000 	scbnds	c0, c0, x1

.*<f2>:
.*:	97ffffee 	bl	.* <_start@plt>
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	908...80 	adrp	c0, GPAGE .*
#record: VAR2_OFFSET
.*:	........ 	add	c0, c0, #0x([0-9a-f]+)
.*:	........ 	ldp	x0, x1, \[c0\]
.*:	........ 	add	c0, c2, x0, uxtx
.*:	c2c10000 	scbnds	c0, c0, x1

.*<_start>:
.*:	c29bd042 	mrs	c2, ctpidr_el0
.*:	908...80 	adrp	c0, GPAGE .*
#record: VAR3_OFFSET
.*:	........ 	add	c0, c0, #0x([0-9a-f]+)
.*:	........ 	ldp	x0, x1, \[c0\]
.*:	........ 	add	c0, c2, x0, uxtx
.*:	c2c10000 	scbnds	c0, c0, x1

Disassembly of section \.got:
#check: VAR1_LOC aarch64_page_plus_offset $GOT_PAGE $VAR1_OFFSET
#check: VAR2_LOC aarch64_page_plus_offset $GOT_PAGE $VAR2_OFFSET
#check: VAR3_LOC aarch64_page_plus_offset $GOT_PAGE $VAR3_OFFSET
#check: VAR3_SIZE_LOC format %x [expr "8 + 0x$GOT_PAGE + 0x$VAR3_OFFSET"]

.* <\.got>:
  +[0-9a-f]+:	.*
	\.\.\.
  +VAR3_LOC:	00000004 	udf	#4
			VAR3_LOC: R_MORELLO_TPREL128	\*ABS\*\+0x4
  +[0-9a-f]+:	00000000 	udf	#0
  +VAR3_SIZE_LOC:	00000014 	udf	#20
	\.\.\.
			VAR1_LOC: R_MORELLO_TPREL128	var1
			VAR2_LOC: R_MORELLO_TPREL128	var2

Disassembly of section \.got\.plt:

.* <.*>:
	\.\.\.
   .*
			[0-9a-f]+: R_MORELLO_JUMP_SLOT	_start
	\.\.\.
   .*
			[0-9a-f]+: R_MORELLO_JUMP_SLOT	extf
	\.\.\.
