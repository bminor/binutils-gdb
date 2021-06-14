#as: -a32
#source: xcoff-tlsm.s
#objdump: -Dr
#name: XCOFF TLSM relocation (32 bit)

.*
Disassembly of section .data:

00000000 <TOC>:
   0:	00 00 00 10.*
			0: R_TLS	foo_tdata-0x10

00000004 <.foo_tdata>:
   4:	00 00 00 00.*
			4: R_TLSM	foo_tdata-0x10

00000008 <foo_tbss>:
   8:	00 00 00 20.*
			8: R_TLS	foo_tbss-0x20

0000000c <.foo_tbss>:
   c:	00 00 00 00.*
			c: R_TLSM	foo_tbss-0x20

Disassembly of section .tdata:

00000010 <foo_tdata>:
  10:	00 00 00 01.*
	...

Disassembly of section .tbss:

00000020 <foo_tbss>:
	...
