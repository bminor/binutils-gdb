#objdump: -dr
#as: --march=v10
#source: v32-err-8.s

# Check that USP gets the right number for V10.

.*:     file format elf32-us-cris

Disassembly of section \.text:

0+ <\.text>:
   0:	3af6                	move r10,usp
   2:	3ffe b0ab 0f00      	move 0xfabb0,usp
   8:	75fa                	move usp,\[r5\]
   a:	3cfa                	move \[r12\],usp
