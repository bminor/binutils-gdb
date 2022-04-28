#source: emit-relocs-morello-hidden.s
#as: -march=morello+c64
#ld: -shared
#objdump: -DR -j .got -j .text

.*:     file format .*


Disassembly of section \.text:

#record: HIDDEN_ADDR
#...
([0-9a-f]*) <hidden_func>:
 .*:	028043ff 	sub	csp, csp, #0x10
 .*:	b9000fe0 	str	w0, \[csp, #12\]
 .*:	b9400fe0 	ldr	w0, \[csp, #12\]
 .*:	020043ff 	add	csp, csp, #0x10
 .*:	c2c253c0 	ret	c30

Disassembly of section \.got:

#...
	\.\.\.
#record: RELOC_BASE
   .*:	([0-9a-f]{8}) 	.*
#check: RELOC_ADDEND aarch64_required_func_addend $RELOC_BASE $HIDDEN_ADDR
			.*: R_MORELLO_RELATIVE	\*ABS\*\+0xRELOC_ADDEND
   .*:	00000000 	.*
# Check that the bounds are anything except zero (rely on other tests to ensure
# that the PCC bounds calculation is correct).
!   .*:	00000000 	.*
   .*:	04000000 	.*

