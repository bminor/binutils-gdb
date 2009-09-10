#as: -EL -mARC600
#objdump: -dr -EL

.*: +file format elf32-.*arc

Disassembly of section .text:

00000000 <text_label>:
   0:	22 20 80 0f 00 00 00 00 	jl         0 <text_label>

			4: R_ARC_32_ME	.text
   8:	e2 20 42 00             	jlnz       [r1]
   c:	e2 20 80 0f 00 00 00 00 	jl         0 <text_label>

			10: R_ARC_32_ME	.text
