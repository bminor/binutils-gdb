#source: relax-3.s
#as: -mno-shared
#objdump: -dwr

.*: +file format .*


Disassembly of section .text:

0+ <foo>:
[ 	]*[a-f0-9]+:	eb 1c                	jmp    1e <local>
[ 	]*[a-f0-9]+:	eb 16                	jmp    1a <hidden_def>
[ 	]*[a-f0-9]+:	eb 16                	jmp    1c <global_def>
[ 	]*[a-f0-9]+:	e9 00 00 00 00       	jmpq   b <foo\+0xb>	7: R_X86_64_PC32	weak_def-0x4
[ 	]*[a-f0-9]+:	e9 00 00 00 00       	jmpq   10 <foo\+0x10>	c: R_X86_64_PC32	weak_hidden_undef-0x4
[ 	]*[a-f0-9]+:	e9 00 00 00 00       	jmpq   15 <foo\+0x15>	11: R_X86_64_PC32	weak_hidden_def-0x4
[ 	]*[a-f0-9]+:	e9 00 00 00 00       	jmpq   1a <hidden_def>	16: R_X86_64_PC32	hidden_undef-0x4

0+1a <hidden_def>:
[ 	]*[a-f0-9]+:	c3                   	retq   

0+1b <weak_hidden_def>:
[ 	]*[a-f0-9]+:	c3                   	retq   

0+1c <global_def>:
[ 	]*[a-f0-9]+:	c3                   	retq   

0+1d <weak_def>:
[ 	]*[a-f0-9]+:	c3                   	retq   

0+1e <local>:
[ 	]*[a-f0-9]+:	c3                   	retq   
#pass
