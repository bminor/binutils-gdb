#PROG: objcopy
#source: out-of-order.s
#ld: -e v1 -Ttext-segment=0x400000
#objdump: -D
#name: Check if disassembler can handle all sections in default order

.*: +file format .*aarch64.*

Disassembly of section \.func1:

0000000000400000 <v1>:
  400000:	8b010000 	add	x0, x0, x1
  400004:	00000000 	\.inst	0x00000000 ; undefined

Disassembly of section \.func2:

0000000000400008 <\.func2>:
  400008:	8b010000 	add	x0, x0, x1

Disassembly of section \.func3:

000000000040000c <\.func3>:
  40000c:	8b010000 	add	x0, x0, x1
  400010:	8b010000 	add	x0, x0, x1
  400014:	8b010000 	add	x0, x0, x1
  400018:	8b010000 	add	x0, x0, x1
  40001c:	8b010000 	add	x0, x0, x1
  400020:	00000000 	\.inst	0x00000000 ; undefined

Disassembly of section \.rodata:

0000000000400024 <\.rodata>:
  400024:	00000004 	\.inst	0x00000004 ; undefined

Disassembly of section .global:

0000000000410028 <__data_start>:
  410028:	00000001 	\.inst	0x00000001 ; undefined
  41002c:	00000000 	\.inst	0x00000000 ; undefined
  410030:	00000001 	\.inst	0x00000001 ; undefined
  410034:	00000000 	\.inst	0x00000000 ; undefined
  410038:	00000001 	\.inst	0x00000001 ; undefined
  41003c:	00000000 	\.inst	0x00000000 ; undefined
