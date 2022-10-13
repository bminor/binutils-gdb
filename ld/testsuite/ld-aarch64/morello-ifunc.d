# Checking that GOT relocations against a PLT which requires pointer equality
# use a GOT entry pointing to the PLT stub rather than just using the .got.plt
# entry.
#
# Things we check are:
#    - __rela_dyn_{start,end} are around the RELATIVE relocation against FOO.
#    - That RELATIVE relocation initialises a GOT entry.
#    - The fragment in that GOT entry plus the addend of the RELATIVE
#      relocation total to point to the foo PLT stub.
#    - We emit a PLT stub which accesses a PLTGOT entry.
#as: -march=morello+c64
#ld:
#objdump: -Dr --section-headers --syms

.*:     file format .*

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
#record: PCC_START
  0 \.[^ \t]* +[0-9a-f]*  ([0-9a-f]+)  .*
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
#record: LAST_PCC_SEC_SIZE LAST_PCC_SEC_START
#...
 *[0-9]+ \.got\.plt *([0-9a-f]+)  ([0-9a-f]+) .*
                  CONTENTS, ALLOC, LOAD, DATA
#...
SYMBOL TABLE:
#record: RELA_DYN_END
#...
([0-9a-f]+) l     O \.rela\.dyn	0000000000000000 __rela_dyn_end
#record: RELA_DYN_START
([0-9a-f]+) l     O \.rela\.dyn	0000000000000000 __rela_dyn_start
#...
Disassembly of section \.rela\.dyn:

#check: RELA_START format %016x 0x$RELA_DYN_START
RELA_START <__rela_dyn_start>:
#record: CHERICAP_LOC
 *[0-9a-f]+:	([0-9a-f]+) 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	0000e803 	.*
 *[0-9a-f]+:	00000000 	.*
#record: CHERICAP_ADDEND
 *[0-9a-f]+:	([0-9a-f]+) 	.*
 *[0-9a-f]+:	00000000 	.*
#record: FOO_GOT
 *[0-9a-f]+:	([0-9a-f]+) 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	0000e803 	.*
 *[0-9a-f]+:	00000000 	.*
#check: CHERICAP_ADDEND string tolower $CHERICAP_ADDEND
 *[0-9a-f]+:	CHERICAP_ADDEND 	.*
#check: RELA_DYN_LAST format %x [expr "0x$RELA_DYN_END - 0x4"]
 *RELA_DYN_LAST:	00000000 	.*

Disassembly of section \.rela\.plt:

[0-9a-f]+ <.*>:
#record: PLTGOT_LOC
 *[0-9a-f]+:	([0-9a-f]+) 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	0000e804 	.*
 *[0-9a-f]+:	00000000 	.*
#record: PLTGOT_LOC_ADDEND
 *[0-9a-f]+:	([0-9a-f]+) 	.*
#...
Disassembly of section \.plt:

#check: FOO_PLT format %x [expr "0x$PCC_START + 0x$CHERICAP_ADDEND - 1"]
#check: PLTGOT_PAGE format %x [expr "0x$PLTGOT_LOC & (~0xfff)"]
#check: PLTGOT_DEC_OFF  expr "0x$PLTGOT_LOC & 0xfff"
([0-9a-f]+) <\.plt>:
#...
 *FOO_PLT:	.* 	adrp	c16, PLTGOT_PAGE .*
 *[0-9a-f]+:	.* 	ldr	c17, \[c16, #PLTGOT_DEC_OFF\]
 *[0-9a-f]+:	.* 	add	c16, c16, #0x.*
 *[0-9a-f]+:	c2c21220 	br	c17

Disassembly of section \.text:

[0-9a-f]+ <foo_1>:
 *[0-9a-f]+:	52800020 	mov	w0, #0x1                   	// #1
 *[0-9a-f]+:	c2c253c0 	ret	c30

[0-9a-f]+ <foo_2>:
 *[0-9a-f]+:	52800040 	mov	w0, #0x2                   	// #2
 *[0-9a-f]+:	c2c253c0 	ret	c30

#check: RESOLVER_ADDRESS format %016x [expr "0x$PLTGOT_LOC_ADDEND + 0x$PCC_START - 1"]
RESOLVER_ADDRESS <foo>:
 *[0-9a-f]+:	.* 	mov	x1, #0x2a                  	// #42
 *[0-9a-f]+:	.* 	tst	x0, x1
 *[0-9a-f]+:	.* 	b\.ne	.*
 *[0-9a-f]+:	.* 	adrp	c0, .*
 *[0-9a-f]+:	.* 	add	c0, c0, .*
 *[0-9a-f]+:	.* 	ret	c30
 *[0-9a-f]+:	.* 	adrp	c0, .*
 *[0-9a-f]+:	.* 	add	c0, c0, .*
 *[0-9a-f]+:	.* 	b	.*

#clearcheck:
#check: FOO_PLT format %x [expr "0x$PCC_START + 0x$CHERICAP_ADDEND - 1"]
#check: GOT_PAGE format %x [expr "0x$FOO_GOT & ~0xfff"]
#check: FOO_GOTOFF expr "0x$FOO_GOT & 0xfff"
[0-9a-f]+ <_start>:
 *[0-9a-f]+:	.* 	bl	FOO_PLT .*
 *[0-9a-f]+:	.* 	adrp	c0, GOT_PAGE .*
 *[0-9a-f]+:	.* 	ldr	c0, \[c0, #FOO_GOTOFF\]
#clearcheck:
#check: PLT_PAGE format %x [expr "(0x$PCC_START + 0x$CHERICAP_ADDEND) & ~0xfff"]
#check: FOO_PLT_OFF format %x [expr "(0x$PCC_START + 0x$CHERICAP_ADDEND) & 0xfff"]
 *[0-9a-f]+:	.* 	adrp	c0, PLT_PAGE .*
 *[0-9a-f]+:	.* 	add	c0, c0, #0xFOO_PLT_OFF
 *[0-9a-f]+:	.* 	ret	c30

#...
Disassembly of section \.got:

#clearcheck:
#check: FRAGMENT_BASE aarch64_8digit_addr $PCC_START
#check: FRAGMENT_SIZE format %08x [expr "0x$LAST_PCC_SEC_START + 0x$LAST_PCC_SEC_SIZE - 0x$PCC_START"]
#check: FRAGMENT_LOC format %x 0x$FOO_GOT
#...
 *FRAGMENT_LOC:	FRAGMENT_BASE 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	FRAGMENT_SIZE 	.*
 *[0-9a-f]+:	04000000 	.*

Disassembly of section \.got\.plt:

#check: PLTGOTLOC  format %x 0x$PLTGOT_LOC
[0-9a-f]+ <.*>:
	\.\.\.
 *PLTGOTLOC:	FRAGMENT_BASE 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	FRAGMENT_SIZE 	.*
 *[0-9a-f]+:	04000000 	.*
#pass
