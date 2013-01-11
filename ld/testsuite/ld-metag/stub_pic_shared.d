tmpdir/stub_pic_shared.so:     file format elf32-metag
architecture: metag, flags 0x00000150:
HAS_SYMS, DYNAMIC, D_PAGED
start address 0x.*

Disassembly of section .plt:

.* <.*>:
 .*:	01 01 90 82 	          ADDT      A0.2,CPC0,#0x20
 .*:	60 06 12 82 	          ADD       A0.2,A0.2,#0x40cc
 .*:	20 0c 10 a3 	          MOV       D0Re0,A0.2
 .*:	e3 01 00 b7 	          SETL      \[A0StP\+\+\],D0Re0,D1Re0
 .*:	2a 01 00 c6 	          GETD      PC,\[D0Re0\+#4\]
.* <_far2@plt>:
 .*:	01 01 90 82 	          ADDT      A0.2,CPC0,#0x20
 .*:	80 05 12 82 	          ADD       A0.2,A0.2,#0x40b0
 .*:	6a 80 00 c6 	          GETD      PC,\[A0.2\]
 .*:	04 00 00 03 	          MOV       D1Re0,#0
 .*:	e0 fe ff a0 	          B         .* <_far2@plt-0x14>
Disassembly of section .text:
.* <__start-0xc>:
.*:	01 01 98 82 	          ADDT      A0.3,CPC0,#0x20
.*:	00 01 18 82 	          ADD       A0.3,A0.3,#0x20
.*:	a0 0c 18 a3 	          MOV       PC,A0.3
.* <__start>:
.*:	b4 ff ff ab 	          CALLR     D1RtP,.* <_far2@plt\+0x14>
	\.\.\.
.* <pad_end>:
.*:	f9 fe 9f 82 	          ADDT      A0.3,CPC0,#0xffdf
.*:	e0 fe 1f 82 	          ADD       A0.3,A0.3,#0xffdc
.*:	a0 0c 18 a3 	          MOV       PC,A0.3
.* <_far2>:
.*:	fe ff ff a0 	          NOP
.* <_far>:
.*:	94 ff ff ab 	          CALLR     D1RtP,.* <pad_end>
