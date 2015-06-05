#source: ppc476-shared.s
#as: -a32
#ld: -melf32ppc -q -shared --ppc476-workaround -T ppc476-shared.lnk
#objdump: -dr
#target: powerpc*-*-*

.*:     file format .*

Disassembly of section \.text:

0+fffc <\.text>:
    fffc:	(48 01 00 04|04 00 01 48) 	b       20000 .*
   10000:	(38 63 00 00|00 00 63 38) 	addi    r3,r3,0
			1000[02]: R_PPC_ADDR16_LO	.bss
	\.\.\.
   1fff0:	(42 9f 00 05|05 00 9f 42) 	bcl     .*
   1fff4:	(7d 28 02 a6|a6 02 28 7d) 	mflr    r9
   1fff8:	(3d 29 00 00|00 00 29 3d) 	addis   r9,r9,0
			1fff[8a]: R_PPC_REL16_HA	.bss\+0x[46]
   1fffc:	(48 00 00 14|14 00 00 48) 	b       20010 .*
   20000:	(3c 60 00 00|00 00 60 3c) 	lis     r3,0
			2000[02]: R_PPC_ADDR16_HA	.bss
   20004:	(4b fe ff fc|fc ff fe 4b) 	b       10000 .*
   20008:	(48 00 00 02|02 00 00 48) 	ba      0 .*
   2000c:	(48 00 00 02|02 00 00 48) 	ba      0 .*
   20010:	(39 29 01 00|00 01 29 39) 	addi    r9,r9,256
			2001[02]: R_PPC_REL16_LO	.bss\+0x1[ce]
   20014:	(4b ff ff ec|ec ff ff 4b) 	b       20000 .*
   20018:	(48 00 00 02|02 00 00 48) 	ba      0 .*
   2001c:	(48 00 00 02|02 00 00 48) 	ba      0 .*
