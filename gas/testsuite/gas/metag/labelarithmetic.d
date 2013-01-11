#as: -mcpu=metac21
#objdump: -dr
#name: labelarithmetic

.*: +file format .*

Disassembly of section \.text:

00000000 <lbl1>:
.*:	20 04 2c 86 	          ADD       TXL1START,CPC0,#0x10

00000004 <lbl2>:
.*:	04 20 00 80 	          MOV       A0StP,CPC0
.*:	a0 00 00 82 	          ADD       A0StP,A0StP,#0x14
.*:	00 8c 01 a3 	          MOV       TXL1END,A0StP

00000010 <loop_start>:
.*:	04 04 18 00 	          MOV       D0Ar2,D0Ar4
.*:	fe ff ff a0 	          NOP

00000018 <loop_end>:
.*:	04 02 18 01 	          MOV       D1Ar1,D1Ar5
