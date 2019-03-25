#PROG: objcopy
#ld: -T out-of-order.T
#objdump: -d
#name: Check if disassembler can handle sections in different order than header

.*: +file format .*arm.*

Disassembly of section \.func2:

04018280 <\.func2>:
 4018280:	e0800001 	add	r0, r0, r1

Disassembly of section \.func1:

04005000 <v1>:
 4005000:	e0800001 	add	r0, r0, r1
 4005004:	00000000 	\.word	0x00000000

Disassembly of section \.func3:

04015000 <\.func3>:
 4015000:	e0800001 	add	r0, r0, r1
 4015004:	e0800001 	add	r0, r0, r1
 4015008:	e0800001 	add	r0, r0, r1
 401500c:	e0800001 	add	r0, r0, r1
 4015010:	e0800001 	add	r0, r0, r1
 4015014:	00000000 	\.word	0x00000000
