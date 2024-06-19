#objdump: -dr
#name: Condition Trap convert IMM0 to REG0 (MIPSr6)
#as: -32

.*: +file format .*

Disassembly of section \.text:

00000000 <f>:
   0:	00c00034 	teq	a2,zero
   4:	00c00030 	tge	a2,zero
   8:	00c00031 	tgeu	a2,zero
   c:	00c00032 	tlt	a2,zero
  10:	00c00033 	tltu	a2,zero
  14:	00c00036 	tne	a2,zero
	\.\.\.
