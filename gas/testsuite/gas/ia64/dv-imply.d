# as: -xexplicit
# objdump: -d
# name ia64 dv-mutex

.*: +file format .*

Disassembly of section \.text:

0000000000000000 <L-0xb0>:
   0:	30 20 08 00 00 21 	\[MIB\] \(p01\) mov r4=2
   6:	00 00 00 02 00 01 	            nop\.i 0x0
   c:	b0 00 00 40       	      \(p02\) br\.cond\.sptk\.few b0 <L>
  10:	11 20 1c 00 00 21 	\[MIB\]       mov r4=7
  16:	00 00 00 02 00 00 	            nop\.i 0x0
  1c:	00 00 20 00       	            rfi;;
  20:	10 20 08 00 00 21 	\[MIB\]       mov r4=2
  26:	00 00 00 02 00 01 	            nop\.i 0x0
  2c:	90 00 00 40       	      \(p02\) br\.cond\.sptk\.few b0 <L>
  30:	31 20 1c 00 00 21 	\[MIB\] \(p01\) mov r4=7
  36:	00 00 00 02 00 00 	            nop\.i 0x0
  3c:	00 00 20 00       	            rfi;;
  40:	70 08 06 04 02 78 	\[MIB\] \(p03\) cmp\.eq\.unc p1,p2=r1,r2
  46:	40 10 00 00 c2 01 	      \(p01\) mov r4=2
  4c:	70 00 00 40       	      \(p03\) br\.cond\.sptk\.few b0 <L>
  50:	11 20 1c 00 00 21 	\[MIB\]       mov r4=7
  56:	00 00 00 02 00 00 	            nop\.i 0x0
  5c:	00 00 20 00       	            rfi;;
  60:	60 08 06 04 02 38 	\[MII\] \(p03\) cmp\.eq\.unc p1,p2=r1,r2
  66:	30 28 18 88 e8 80 	            cmp\.eq\.or p3,p4=r5,r6
  6c:	20 00 00 84       	      \(p01\) mov r4=2
  70:	10 00 00 00 01 00 	\[MIB\]       nop\.m 0x0
  76:	00 00 00 02 80 01 	            nop\.i 0x0
  7c:	40 00 00 40       	      \(p03\) br\.cond\.sptk\.few b0 <L>
  80:	11 20 1c 00 00 21 	\[MIB\]       mov r4=7
  86:	00 00 00 02 00 00 	            nop\.i 0x0
  8c:	00 00 20 00       	            rfi;;
  90:	10 08 16 0c 42 70 	\[MIB\]       cmp\.ne\.and p1,p2=r5,r6
  96:	40 10 00 00 c2 01 	      \(p01\) mov r4=2
  9c:	20 00 00 40       	      \(p03\) br\.cond\.sptk\.few b0 <L>
  a0:	11 20 1c 00 00 21 	\[MIB\]       mov r4=7
  a6:	00 00 00 02 00 00 	            nop\.i 0x0
  ac:	00 00 20 00       	            rfi;;
