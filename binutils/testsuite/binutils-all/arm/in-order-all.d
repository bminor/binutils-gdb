#PROG: objcopy
#source: out-of-order.s
#ld: -e v1 -Ttext-segment=0x400000
#objdump: -D
#name: Check if disassembler can handle all sections in default order

.*: +file format .*arm.*

Disassembly of section \.func1:

00400000 <v1>:
  400000:	e0800001 	add	r0, r0, r1
  400004:	00000000 	andeq	r0, r0, r0

Disassembly of section \.func2:

00400008 <\.func2>:
  400008:	e0800001 	add	r0, r0, r1

Disassembly of section \.func3:

0040000c <\.func3>:
  40000c:	e0800001 	add	r0, r0, r1
  400010:	e0800001 	add	r0, r0, r1
  400014:	e0800001 	add	r0, r0, r1
  400018:	e0800001 	add	r0, r0, r1
  40001c:	e0800001 	add	r0, r0, r1
  400020:	00000000 	andeq	r0, r0, r0

Disassembly of section \.rodata:

00400024 <\.rodata>:
  400024:	00000004 	andeq	r0, r0, r4

Disassembly of section \.global:

00410028 <__data_start>:
  410028:	00000001 	andeq	r0, r0, r1
  41002c:	00000001 	andeq	r0, r0, r1
  410030:	00000001 	andeq	r0, r0, r1

Disassembly of section \.ARM\.attributes:

00000000 <\.ARM\.attributes>:
   0:	00001141 	andeq	r1, r0, r1, asr #2
   4:	61656100 	cmnvs	r5, r0, lsl #2
   8:	01006962 	tsteq	r0, r2, ror #18
   c:	00000007 	andeq	r0, r0, r7
  10:	Address 0x0000000000000010 is out of bounds.

