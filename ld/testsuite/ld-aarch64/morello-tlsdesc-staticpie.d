#source: morello-tlsdesc.s
#as: -march=morello+c64 --defsym STATIC=1
#ld: -pie
#objdump: -DR -j .got.plt -j .text -j .plt -j .rodata

.*:     file format .*


Disassembly of section \.text:

[0-9a-f]+ <f2>:
 +[0-9a-f]+:	c29bd042 	mrs	c2, ctpidr_el0
#record: RODATA_PAGE
 +[0-9a-f]+:	[0-9a-f]+ 	adrp	c0, ([0-9a-f]+) .*
#record: VAR2_OFFSET
 +[0-9a-f]+:	[0-9a-f]+ 	add	c0, c0, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400400 	ldp	x0, x1, \[c0\]
 +[0-9a-f]+:	c2a06040 	add	c0, c2, x0, uxtx
 +[0-9a-f]+:	c2c10000 	scbnds	c0, c0, x1

[0-9a-f]+ <access_hidden>:
 +[0-9a-f]+:	c29bd042 	mrs	c2, ctpidr_el0
#check: PAGE string tolower $RODATA_PAGE
 +[0-9a-f]+:	[0-9a-f]+ 	adrp	c0, PAGE .*
#record: VAR4_OFFSET
 +[0-9a-f]+:	[0-9a-f]+ 	add	c0, c0, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400400 	ldp	x0, x1, \[c0\]
 +[0-9a-f]+:	c2a06040 	add	c0, c2, x0, uxtx
 +[0-9a-f]+:	c2c10000 	scbnds	c0, c0, x1

[0-9a-f]+ <access_protected>:
 +[0-9a-f]+:	c29bd042 	mrs	c2, ctpidr_el0
#check: PAGE string tolower $RODATA_PAGE
 +[0-9a-f]+:	[0-9a-f]+ 	adrp	c0, PAGE .*
#record: VAR5_OFFSET
 +[0-9a-f]+:	[0-9a-f]+ 	add	c0, c0, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400400 	ldp	x0, x1, \[c0\]
 +[0-9a-f]+:	c2a06040 	add	c0, c2, x0, uxtx
 +[0-9a-f]+:	c2c10000 	scbnds	c0, c0, x1

[0-9a-f]+ <_start>:
 +[0-9a-f]+:	c29bd042 	mrs	c2, ctpidr_el0
#check: PAGE string tolower $RODATA_PAGE
 +[0-9a-f]+:	[0-9a-f]+ 	adrp	c0, PAGE .*
#record: VAR3_OFFSET
 +[0-9a-f]+:	[0-9a-f]+ 	add	c0, c0, #0x([0-9a-f]+)
 +[0-9a-f]+:	a9400400 	ldp	x0, x1, \[c0\]
 +[0-9a-f]+:	c2a06040 	add	c0, c2, x0, uxtx
 +[0-9a-f]+:	c2c10000 	scbnds	c0, c0, x1

Disassembly of section \.rodata:

#check: VAR2_LOC aarch64_page_plus_offset $RODATA_PAGE $VAR2_OFFSET
#check: VAR3_LOC aarch64_page_plus_offset $RODATA_PAGE $VAR3_OFFSET
#check: VAR4_LOC aarch64_page_plus_offset $RODATA_PAGE $VAR4_OFFSET
#check: VAR5_LOC aarch64_page_plus_offset $RODATA_PAGE $VAR5_OFFSET
[0-9a-f]+ <.*>:
 +VAR2_LOC:	00000020 	udf	#32
 +[0-9a-f]+:	00000000 	udf	#0
 +[0-9a-f]+:	00000004 	udf	#4
 +[0-9a-f]+:	00000000 	udf	#0
 +VAR4_LOC:	00000038 	udf	#56
 +[0-9a-f]+:	00000000 	udf	#0
 +[0-9a-f]+:	00000004 	udf	#4
 +[0-9a-f]+:	00000000 	udf	#0
 +VAR5_LOC:	0000003c 	udf	#60
 +[0-9a-f]+:	00000000 	udf	#0
 +[0-9a-f]+:	00000004 	udf	#4
 +[0-9a-f]+:	00000000 	udf	#0
 +VAR3_LOC:	00000024 	udf	#36
 +[0-9a-f]+:	00000000 	udf	#0
 +[0-9a-f]+:	00000014 	udf	#20
 +[0-9a-f]+:	00000000 	udf	#0

Disassembly of section \.got\.plt:

.*:
	\.\.\.
