#objdump: -dr -m mips:micromips
#name: microMIPS insn32 move test
#source: micromips32-move.s

# Check objdump's disassembly of the move menomic for addu, daddu and or.

.*: +file format .*mips.*

Disassembly of section .text:
0+ <.*>:
   0:	001f 6a90 	move	t5,ra
   4:	001f 6950 	move	t5,ra
   8:	581f 6950 	move	t5,ra
   c:	001f 6a90 	move	t5,ra
