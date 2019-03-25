#PROG: objcopy
#source: out-of-order.s
#ld: -e v1 -Ttext-segment=0x400000
#objdump: -d
#name: Check if disassembler can handle sections in default order

.*: +file format .*arm.*

Disassembly of section \.func1:

00400000 <v1>:
  400000:	e0800001 	add	r0, r0, r1
  400004:	00000000 	\.word	0x00000000

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
  400020:	00000000 	\.word	0x00000000
