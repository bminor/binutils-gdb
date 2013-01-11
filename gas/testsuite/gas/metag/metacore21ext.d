#as: -mcpu=metac21
#objdump: -dr
#name: metacpu21ext

.*: +file format .*

Disassembly of section \.text:

00000000 <.text>:
.*:	40 46 00 60 	          MULD      D0Re0,D0Ar6,D0Ar2
.*:	ad 41 c0 c7 	          MOVL      RABZ,\[D0Ar6\+\+\]
.*:	cd 41 c8 c7 	          MOVL      RAWZ,\[D1Ar5\+\+\]
.*:	ad 81 d0 c7 	          MOVL      RADZ,\[D0Ar4\+\+\]
.*:	cd 81 e0 c7 	          MOVL      RABX,\[D1Ar3\+\+\]
.*:	ad c1 e8 c7 	          MOVL      RAWX,\[D0Ar2\+\+\]
.*:	cd c1 f0 c7 	          MOVL      RADX,\[D1Ar1\+\+\]
.*:	ad 01 f9 c7 	          MOVL      RAMX,\[D0FrT\+\+\]
.*:	ed 81 f8 c7 	          MOVL      RAMX,\[A0\.2\+\+\]
