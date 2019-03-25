#PROG: objcopy
#source: out-of-order.s
#ld: -T out-of-order.T
#objdump: -D
#name: Check if disassembler can handle all sections in different order than header

.*: +file format .*arm.*

Disassembly of section \.global:

ffe00000 <\.global>:
ffe00000:	00000001 	andeq	r0, r0, r1
ffe00004:	00000001 	andeq	r0, r0, r1
ffe00008:	00000001 	andeq	r0, r0, r1

Disassembly of section \.func2:

04018280 <\.func2>:
 4018280:	e0800001 	add	r0, r0, r1

Disassembly of section \.func1:

04005000 <v1>:
 4005000:	e0800001 	add	r0, r0, r1
 4005004:	00000000 	andeq	r0, r0, r0

Disassembly of section \.func3:

04015000 <\.func3>:
 4015000:	e0800001 	add	r0, r0, r1
 4015004:	e0800001 	add	r0, r0, r1
 4015008:	e0800001 	add	r0, r0, r1
 401500c:	e0800001 	add	r0, r0, r1
 4015010:	e0800001 	add	r0, r0, r1
 4015014:	00000000 	andeq	r0, r0, r0

Disassembly of section \.rodata:

04015018 <\.rodata>:
 4015018:	00000004 	andeq	r0, r0, r4

Disassembly of section \.ARM\.attributes:

00000000 <\.ARM\.attributes>:
   0:	00001141 	andeq	r1, r0, r1, asr #2
   4:	61656100 	cmnvs	r5, r0, lsl #2
   8:	01006962 	tsteq	r0, r2, ror #18
   c:	00000007 	andeq	r0, r0, r7
  10:	Address 0x0000000000000010 is out of bounds.

