#PROG: objcopy
#ld: -T out-of-order.T
#objdump: -D
#name: Check if disassembler can handle sections in different order than header

.*: +file format .*aarch64.*

Disassembly of section \.global:

00000000ffe00000 <\.global>:
    ffe00000:	00000001 	\.word	0x00000001
    ffe00004:	00000000 	\.word	0x00000000
    ffe00008:	00000001 	\.word	0x00000001
    ffe0000c:	00000000 	\.word	0x00000000
    ffe00010:	00000001 	\.word	0x00000001
    ffe00014:	00000000 	\.word	0x00000000

Disassembly of section \.func2:

0000000004018280 <\.func2>:
 4018280:	8b010000 	add	x0, x0, x1

Disassembly of section \.func1:

0000000004005000 <v1>:
 4005000:	8b010000 	add	x0, x0, x1
 4005004:	00000000 	\.word	0x00000000

Disassembly of section \.func3:

0000000004015000 <\.func3>:
 4015000:	8b010000 	add	x0, x0, x1
 4015004:	8b010000 	add	x0, x0, x1
 4015008:	8b010000 	add	x0, x0, x1
 401500c:	8b010000 	add	x0, x0, x1
 4015010:	8b010000 	add	x0, x0, x1
 4015014:	00000000 	\.word	0x00000000

Disassembly of section \.rodata:

0000000004015018 <\.rodata>:
 4015018:	00000004 	\.word	0x00000004
