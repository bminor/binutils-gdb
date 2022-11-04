# Checking that the IRELATIVE relocations which are added by CAPINIT
# static relocations end up between the __rela_iplt_start and __rela_iplt_end
# symbols and that RELATIVE relocations added by CAPINIT static relocations end
# up between __rela_dyn_start and __rela_dyn_end.
#as: -march=morello+c64
#ld:
#objdump: -Dzr --section-headers --syms

.*:     file format .*

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
#record: PCC_START
  0 \.[^ ]+ +[0-9a-f]+  ([0-9a-f]+)  [0-9a-f]+  [0-9a-f]+  2\*\*.
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
#...
 *[0-9]+ \.got\.plt *[0-9a-f]+  [0-9a-f]+ .*
                  CONTENTS, ALLOC, LOAD, DATA
#record: PCC_END
 *[0-9]+ \.[^ ]* +[0-9a-f]*  ([0-9a-f]+)  .*
#...
SYMBOL TABLE:
#record: RELA_IPLT_END
#...
([0-9a-f]+) l       \.rela\.plt	0000000000000000 __rela_iplt_end
#record: RELA_IPLT_START
([0-9a-f]+) l       \.rela\.plt	0000000000000000 __rela_iplt_start
#record: RELA_DYN_END
([0-9a-f]+) l     O \.rela\.dyn	0000000000000000 __rela_dyn_end
#record: RELA_DYN_START
([0-9a-f]+) l     O \.rela\.dyn	0000000000000000 __rela_dyn_start
#...

Disassembly of section \.rela\.dyn:

#check: RELA_START format %016x 0x$RELA_DYN_START
RELA_START <__rela_dyn_start>:
#record: CHERICAP_A_LOC
 *[0-9a-f]+:	([0-9a-f]+) 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	0000e803 	.*
 *[0-9a-f]+:	00000000 	.*
#record: CHERICAP_ADDEND
 *[0-9a-f]+:	([0-9a-f]+) 	.*
 *[0-9a-f]+:	00000000 	.*
# __rela_iplt_end and __rela_iplt_start relocations.
# These were just added in order to get the symbols in the output binary.
# There is nothing we want to check around them.
 *[0-9a-f]+:	........ 	.*
 *[0-9a-f]+:	........ 	.*
 *[0-9a-f]+:	........ 	.*
 *[0-9a-f]+:	........ 	.*
 *[0-9a-f]+:	........ 	.*
 *[0-9a-f]+:	........ 	.*
 *[0-9a-f]+:	........ 	.*
 *[0-9a-f]+:	........ 	.*
 *[0-9a-f]+:	........ 	.*
 *[0-9a-f]+:	........ 	.*
 *[0-9a-f]+:	........ 	.*
 *[0-9a-f]+:	........ 	.*
#check: CCAP_ADDEND string tolower $CHERICAP_ADDEND
#record: CHERICAP_B_LOC
 *[0-9a-f]+:	([0-9a-f]+) 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	0000e803 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	CCAP_ADDEND 	.*
 *[0-9a-f]+:	00000000 	.*
#record: CHERICAP_C_LOC
 *[0-9a-f]+:	([0-9a-f]+) 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	0000e803 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	CCAP_ADDEND 	.*
 *[0-9a-f]+:	00000000 	.*
#record: GOT_IFUNC_LOC
 *[0-9a-f]+:	([0-9a-f]+) 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	0000e803 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	CCAP_ADDEND 	.*
#check: RELA_DYN_LAST format %x [expr "0x$RELA_DYN_END - 0x4"]
 *RELA_DYN_LAST:	00000000 	.*

Disassembly of section \.rela\.plt:

[0-9a-f]+ <__rela_iplt_start>:
#record: PLTGOT_LOC 
 +[0-9a-f]+:	([0-9a-f]+) .*
 +[0-9a-f]+:	00000000 .*
 +[0-9a-f]+:	0000e804 .*
 +[0-9a-f]+:	00000000 .*
#record: IREL_ADDEND
 +[0-9a-f]+:	([0-9a-f]+) .*
 +[0-9a-f]+:	00000000 .*

Disassembly of section \.plt:

#check: PLTGOT_PAGE format %x [expr "0x$PLTGOT_LOC & (~0xfff)"]
#check: PLTGOT_DEC_OFF  expr "0x$PLTGOT_LOC & 0xfff"
#check: PLTADDR_LOC format %x [expr "0x$PCC_START + (0x$CHERICAP_ADDEND & ~1)"]
[0-9a-f]+ <\.plt>:
 *PLTADDR_LOC:	.* 	adrp	c16, PLTGOT_PAGE .*
 *[0-9a-f]+:	.* 	ldr	c17, \[c16, #PLTGOT_DEC_OFF\]
 *[0-9a-f]+:	.* 	add	c16, c16, #0x.*
 *[0-9a-f]+:	.* 	br	c17

Disassembly of section \.text:

[0-9a-f]+ <foo_1>:
#...

[0-9a-f]+ <foo_2>:
#...

#check: FOO_ADDR format %016x [expr "0x$PCC_START + (0x$IREL_ADDEND & ~1)"]
FOO_ADDR <foo>:
 *[0-9a-f]+:	.* 	mov	x1, #0x2a                  	// #42
 *[0-9a-f]+:	.* 	tst	x0, x1
 *[0-9a-f]+:	.* 	b\.ne	.* <foo\+0x18>  // b\.any
 *[0-9a-f]+:	.* 	adrp	c0, .*
 *[0-9a-f]+:	.* 	add	c0, c0, .*
 *[0-9a-f]+:	.* 	ret	c30
 *[0-9a-f]+:	.* 	adrp	c0, .*
 *[0-9a-f]+:	.* 	add	c0, .*
 *[0-9a-f]+:	.* 	b	.* <foo\+0x14>

#check: GOT_PAGE format %x [expr "0x$GOT_IFUNC_LOC & (~0xfff)"]
#check: GOT_DEC_OFF  expr "0x$GOT_IFUNC_LOC & 0xfff"
#check: PLTADDR_PAGE format %x [expr "(0x$PCC_START + 0x$CHERICAP_ADDEND) & (~0xfff)"]
#check: PLTADDR_OFF format %x [expr "(0x$PCC_START + 0x$CHERICAP_ADDEND) & 0xfff"]
[0-9a-f]+ <_start>:
 *[0-9a-f]+:	.* 	bl	PLTADDR_LOC .*
 *[0-9a-f]+:	.* 	adrp	c0, GOT_PAGE .*
 *[0-9a-f]+:	.* 	ldr	c0, \[c0, #GOT_DEC_OFF\]
 *[0-9a-f]+:	.* 	adrp	c0, PLTADDR_PAGE .*
 *[0-9a-f]+:	.* 	add	c0, c0, #0xPLTADDR_OFF
 *[0-9a-f]+:	.* 	ret	c30

Disassembly of section \.got:

#check: GOTLOC format %x 0x$GOT_IFUNC_LOC
#check: FRAGBASE format %08x 0x$PCC_START
#check: FRAGSIZE format %08x [expr "0x$PCC_END - 0x$PCC_START"]
[0-9a-f]+ <.*>:
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *GOTLOC:	FRAGBASE 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	FRAGSIZE 	.*
 *[0-9a-f]+:	04000000 	.*

Disassembly of section \.got\.plt:

#check: PLTGOT_ADDR format %x 0x$PLTGOT_LOC
[0-9a-f]+ <.*>:
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	00000000 	.*
 *PLTGOT_ADDR:	FRAGBASE 	.*
 *[0-9a-f]+:	00000000 	.*
 *[0-9a-f]+:	FRAGSIZE 	.*
 *[0-9a-f]+:	04000000 	.*
#...
Disassembly of section \.data:
#pass
