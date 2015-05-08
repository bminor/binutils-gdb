#source: relax-3.s
#as: -mno-shared
#objdump: -dwr

.*: +file format .*

Disassembly of section .text:

0+ <foo>:
[ 	]*[a-f0-9]+:	eb 1c                	jmp    1e <local>
[ 	]*[a-f0-9]+:	eb 16                	jmp    1a <hidden_def>
[ 	]*[a-f0-9]+:	eb 16                	jmp    1c <global_def>
[ 	]*[a-f0-9]+:	e9 fc ff ff ff       	jmp    7 <foo\+0x7>	7: (R_386_PC)?(DISP)?32	weak_def
[ 	]*[a-f0-9]+:	e9 fc ff ff ff       	jmp    c <foo\+0xc>	c: (R_386_PC)?(DISP)?32	weak_hidden_undef
[ 	]*[a-f0-9]+:	e9 fc ff ff ff       	jmp    11 <foo\+0x11>	11: (R_386_PC)?(DISP)?32	weak_hidden_def
[ 	]*[a-f0-9]+:	e9 fc ff ff ff       	jmp    16 <foo\+0x16>	16: (R_386_PC)?(DISP)?32	hidden_undef

0+1a <hidden_def>:
[ 	]*[a-f0-9]+:	c3                   	ret    

0+1b <weak_hidden_def>:
[ 	]*[a-f0-9]+:	c3                   	ret    

0+1c <global_def>:
[ 	]*[a-f0-9]+:	c3                   	ret    

0+1d <weak_def>:
[ 	]*[a-f0-9]+:	c3                   	ret    

0+1e <local>:
[ 	]*[a-f0-9]+:	c3                   	ret    
#pass
