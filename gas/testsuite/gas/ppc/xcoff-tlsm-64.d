#as: -a64
#source: xcoff-tlsm.s
#objdump: -Dr
#name: XCOFF TLSM relocation (64 bit)

.*
Disassembly of section .data:

0000000000000000 <TOC>:
   0:	00 00 00 00.*
			0: R_TLS	foo_tdata-0x20
   4:	00 00 00 20.*

0000000000000008 <.foo_tdata>:
	...
			8: R_TLSM	foo_tdata-0x20

0000000000000010 <foo_tbss>:
  10:	00 00 00 00.*
			10: R_TLS	foo_tbss-0x30
  14:	00 00 00 30.*

0000000000000018 <.foo_tbss>:
	...
			18: R_TLSM	foo_tbss-0x30

Disassembly of section .tdata:

0000000000000020 <foo_tdata>:
  20:	00 00 00 01.*
	...

Disassembly of section .tbss:

0000000000000030 <foo_tbss>:
	...
