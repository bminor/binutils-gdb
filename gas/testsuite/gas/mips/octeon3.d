#objdump: -d -r --show-raw-insn
#name: MIPS octeon3 instructions

.*: +file format .*mips.*

Disassembly of section .text:

[0-9a-f]+ <foo>:
.*:	71ec0008 	mtm0	t3,t0
.*:	71a40008 	mtm0	t1,a0
.*:	7083000c 	mtm1	a0,v1
.*:	70e1000c 	mtm1	a3,at
.*:	7022000d 	mtm2	at,v0
.*:	7083000c 	mtm1	a0,v1
.*:	70a20009 	mtp0	a1,v0
.*:	70c40009 	mtp0	a2,a0
.*:	7083000a 	mtp1	a0,v1
.*:	70e1000a 	mtp1	a3,at
.*:	7022000b 	mtp2	at,v0
.*:	7083000a 	mtp1	a0,v1
