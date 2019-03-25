#PROG: objcopy
#source: out-of-order.s
#ld: -e v1 -Ttext-segment=0x400000
#objdump: -d
#name: Check if disassembler can handle sections in default order

.*: +file format .*aarch64.*

Disassembly of section \.func1:

0000000000400000 <v1>:
  400000:	8b010000 	add	x0, x0, x1
  400004:	00000000 	\.word	0x00000000

Disassembly of section .func2:

0000000000400008 <\.func2>:
  400008:	8b010000 	add	x0, x0, x1

Disassembly of section \.func3:

000000000040000c <\.func3>:
  40000c:	8b010000 	add	x0, x0, x1
  400010:	8b010000 	add	x0, x0, x1
  400014:	8b010000 	add	x0, x0, x1
  400018:	8b010000 	add	x0, x0, x1
  40001c:	8b010000 	add	x0, x0, x1
  400020:	00000000 	\.word	0x00000000
