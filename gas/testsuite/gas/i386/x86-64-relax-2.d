#source: relax-3.s
#objdump: -dwr

.*: +file format .*


Disassembly of section .text:

0+ <foo>:
[ 	]*[a-f0-9]+:	eb 1f                	jmp    21 <local>
[ 	]*[a-f0-9]+:	eb 19                	jmp    1d <hidden_def>
[ 	]*[a-f0-9]+:	e9 00 00 00 00       	jmpq   9 <foo\+0x9>	5: R_X86_64_PC32	global_def-0x4
[ 	]*[a-f0-9]+:	e9 00 00 00 00       	jmpq   e <foo\+0xe>	a: R_X86_64_PC32	weak_def-0x4
[ 	]*[a-f0-9]+:	e9 00 00 00 00       	jmpq   13 <foo\+0x13>	f: R_X86_64_PC32	weak_hidden_undef-0x4
[ 	]*[a-f0-9]+:	e9 00 00 00 00       	jmpq   18 <foo\+0x18>	14: R_X86_64_PC32	weak_hidden_def-0x4
[ 	]*[a-f0-9]+:	e9 00 00 00 00       	jmpq   1d <hidden_def>	19: R_X86_64_PC32	hidden_undef-0x4

0+1d <hidden_def>:
[ 	]*[a-f0-9]+:	c3                   	retq   

0+1e <weak_hidden_def>:
[ 	]*[a-f0-9]+:	c3                   	retq   

0+1f <global_def>:
[ 	]*[a-f0-9]+:	c3                   	retq   

0+20 <weak_def>:
[ 	]*[a-f0-9]+:	c3                   	retq   

0+21 <local>:
[ 	]*[a-f0-9]+:	c3                   	retq   
#pass
