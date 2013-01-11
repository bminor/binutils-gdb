
tmpdir/shared.so:     file format elf32-metag
architecture: metag, flags 0x00000150:
HAS_SYMS, DYNAMIC, D_PAGED
start address 0x.*

Disassembly of section .plt:

.* <.*>:
 .*:	01 00 90 82 	          ADDT      A0.2,CPC0,#0
 .*:	60 08 12 82 	          ADD       A0.2,A0.2,#0x410c
 .*:	20 0c 10 a3 	          MOV       D0Re0,A0.2
 .*:	e3 01 00 b7 	          SETL      \[A0StP\+\+\],D0Re0,D1Re0
 .*:	2a 01 00 c6 	          GETD      PC,\[D0Re0\+#4\]
.* <app_func2@plt>:
 .*:	01 00 90 82 	          ADDT      A0.2,CPC0,#0
 .*:	80 07 12 82 	          ADD       A0.2,A0.2,#0x40f0
 .*:	6a 80 00 c6 	          GETD      PC,\[A0.2\]
 .*:	04 00 00 03 	          MOV       D1Re0,#0
 .*:	e0 fe ff a0 	          B         184 <app_func2@plt-0x14>
Disassembly of section .text:

.* <lib_func1>:
 .*:	05 32 20 00 	          MOV       D0FrT,A0FrP
 .*:	26 00 08 86 	          ADD       A0FrP,A0StP,#0
 .*:	e3 01 20 b7 	          SETL      \[A0StP\+\+\],D0FrT,D1RtP
 .*:	e9 02 08 b6 	          SETD      \[A0StP\+#8\+\+\],A1LbP
 .*:	40 00 00 82 	          ADD       A0StP,A0StP,#0x8
 .*:	01 00 88 83 	          ADDT      A1LbP,CPC1,#0
 .*:	60 06 0b 83 	          ADD       A1LbP,A1LbP,#0x60cc
 .*:	94 fe ff ab 	          CALLR     D1RtP,198 <app_func2@plt>
 .*:	8d 01 0c a7 	          GETD      D0Ar6,\[A1LbP\+#-8180\]
 .*:	00 02 00 00 	          ADD       D0Re0,D0Re0,D0Ar6
 .*:	05 02 00 01 	          MOV       D1Re0,A1LbP
 .*:	f9 ff 07 03 	          ADDT      D1Re0,D1Re0,#0xffff
 .*:	c0 ff 06 03 	          ADD       D1Re0,D1Re0,#0xdff8
 .*:	64 fe 0f a7 	          GETD      A1LbP,\[A0StP\+#-16\]
 .*:	e3 41 20 c7 	          GETL      D0FrT,D1RtP,\[A0FrP\+\+\]
 .*:	26 42 00 8e 	          SUB       A0StP,A0FrP,#0x8
 .*:	05 18 08 80 	          MOV       A0FrP,D0FrT
 .*:	a0 08 20 a3 	          MOV       PC,D1RtP
