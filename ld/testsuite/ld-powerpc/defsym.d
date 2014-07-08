#source: defsym.s
#as: -a64
#ld: -melf64ppc --defsym bar=foo
#objdump: -Dr

.*:     file format elf64-powerpc.*

Disassembly of section \.text:

0+100000b0 <_start>:
    100000b0:	(15 00 00 48|48 00 00 15) 	bl      100000c4 <(foo|bar)\+0x8>
    100000b4:	(11 00 00 48|48 00 00 11) 	bl      100000c4 <(foo|bar)\+0x8>
    100000b8:	(00 00 00 60|60 00 00 00) 	nop

0+100000bc <(foo|bar)>:
    100000bc:	(02 10 40 3c|3c 40 10 02) 	lis     r2,4098
    100000c0:	(c8 80 42 38|38 42 80 c8) 	addi    r2,r2,-32568
    100000c4:	(20 00 80 4e|4e 80 00 20) 	blr

Disassembly of section \.data:

0+100100c8 .*:
    100100c8:	(bc 00 00 10|00 00 00 00) 	.*
    100100cc:	(00 00 00 00|10 00 00 bc) 	.*
    100100d0:	(bc 00 00 10|00 00 00 00) 	.*
    100100d4:	(00 00 00 00|10 00 00 bc) 	.*
