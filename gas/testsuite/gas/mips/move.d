#objdump: -dr
#name: MIPS move disassembly test
#source: move.s

# Check objdump's disassembly of the move menomic for addu, daddu and or.

.*: +file format .*mips.*

Disassembly of section .text:
0+ <.*>:
   0:	03e06825 	move	t5,ra
   4:	03e06821 	move	t5,ra
   8:	03e0682d 	move	t5,ra
   c:	03e06825 	move	t5,ra
