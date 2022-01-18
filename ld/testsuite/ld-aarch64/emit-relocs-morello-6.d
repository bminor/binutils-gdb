# Check handling relocations into the got that require a GOT entry.
# This case handles PIE binaries.
#
# This testcase uses exact values in order to check that of the two GOT entries
# created, the one that is referenced by the first instruction in _start is
# the one which has the LSB set in its value.
#
# It's difficult to check this in the DejaGNU testsuite without checking for
# specific values that we know are good.  However this is susceptible to
# defaults changing where the .text and .got sections end up.
#
# If this testcase prooves to be too flaky while the linker gets updated then
# we should look harder for some solution, but for now we'll take this
# tradeoff.
#source: emit-relocs-morello-6.s
#as: -march=morello+c64
#ld: -Ttext-segment 0x0 -pie -static
#objdump: -DR -j .got -j .text


.*:     file format .*


Disassembly of section \.text:

00000000000001e8 <_start>:
 1e8:	c240c400 	ldr	c0, \[c0, #784\]
 1ec:	c240c000 	ldr	c0, \[c0, #768\]

Disassembly of section \.got:

00000000000102f0 <\.got>:
   102f0:	000101f0 	\.inst	0x000101f0 ; undefined
	\.\.\.
   10300:	000001e8 	udf	#488
			10300: R_MORELLO_RELATIVE	\*ABS\*
   10304:	00000000 	udf	#0
   10308:	.*
   1030c:	.*
   10310:	000001e9 	udf	#489
			10310: R_MORELLO_RELATIVE	\*ABS\*
   10314:	.*
   10318:	.*
   1031c:	.*
