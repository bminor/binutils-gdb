#objdump: -d
#name:    Test of disassembler behaviour by with invalid bit manipulation instructions
#source:  bit-manip-invalid.s


.*:     file format elf32-s12z


Disassembly of section \.text:

00000000 <\.text>:
   0:	01          	nop
   1:	03 a5 10 04 	brset\.w 4100, d4, \*\+6
   5:	06 
   6:	01          	nop
   7:	01          	nop
   8:	03 65 12    	brset d1, #4, \*\+18
   b:	01          	nop
   c:	01          	nop
