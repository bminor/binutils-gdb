tmpdir/stub_pic_app:     file format elf32-metag
architecture: metag, flags 0x00000112:
EXEC_P, HAS_SYMS, D_PAGED
start address 0x.*

Disassembly of section .plt:

.* <.*>:
.*:	05 81 00 02 	          MOVT      D0Re0,#0x1020
.*:	20 97 04 02 	          ADD       D0Re0,D0Re0,#0x92e4
.*:	e3 01 00 b7 	          SETL      \[A0StP\+\+\],D0Re0,D1Re0
.*:	2a 01 00 c6 	          GETD      PC,\[D0Re0\+#4\]
.*:	fe ff ff a0 	          NOP
.* <_lib_func@plt>:
.*:	05 81 10 82 	          MOVT      A0.2,#0x1020
.*:	e0 96 14 82 	          ADD       A0.2,A0.2,#0x92dc
.*:	6a 80 00 c6 	          GETD      PC,\[A0.2\]
.*:	04 00 00 03 	          MOV       D1Re0,#0
.*:	e0 fe ff a0 	          B         .* <_lib_func@plt-0x14>
Disassembly of section .text:
.* <__start-0x10>:
.*:	05 81 18 82 	          MOVT      A0.3,#0x1020
.*:	a3 91 1a ac 	          JUMP      A0.3,#0x5234
.*:	05 81 18 82 	          MOVT      A0.3,#0x1020
.*:	83 91 1a ac 	          JUMP      A0.3,#0x5230
.* <__start>:
.*:	94 ff ff ab 	          CALLR     D1RtP,.* <_lib_func@plt\+0x14>
.*:	d4 fe ff ab 	          CALLR     D1RtP,.* <_lib_func@plt>
.*:	94 ff ff ab 	          CALLR     D1RtP,.* <_lib_func@plt\+0x1c>
	\.\.\.
.* <_far2>:
.*:	fe ff ff a0 	          NOP
.* <_far>:
.*:	f4 ff ff ab 	          CALLR     D1RtP,.* <_far2>
