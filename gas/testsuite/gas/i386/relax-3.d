#objdump: -dwr

.*: +file format .*

Disassembly of section .text:

0+ <foo>:
[ 	]*[a-f0-9]+:	eb 1f                	jmp    21 <local>
[ 	]*[a-f0-9]+:	eb 19                	jmp    1d <hidden_def>
[ 	]*[a-f0-9]+:	e9 fc ff ff ff       	jmp    5 <foo\+0x5>	5: (R_386_PC)?(DISP)?32	global_def
[ 	]*[a-f0-9]+:	e9 fc ff ff ff       	jmp    a <foo\+0xa>	a: (R_386_PC)?(DISP)?32	weak_def
[ 	]*[a-f0-9]+:	e9 fc ff ff ff       	jmp    f <foo\+0xf>	f: (R_386_PC)?(DISP)?32	weak_hidden_undef
[ 	]*[a-f0-9]+:	e9 fc ff ff ff       	jmp    14 <foo\+0x14>	14: (R_386_PC)?(DISP)?32	weak_hidden_def
[ 	]*[a-f0-9]+:	e9 fc ff ff ff       	jmp    19 <foo\+0x19>	19: (R_386_PC)?(DISP)?32	hidden_undef

0+1d <hidden_def>:
[ 	]*[a-f0-9]+:	c3                   	ret    

0+1e <weak_hidden_def>:
[ 	]*[a-f0-9]+:	c3                   	ret    

0+1f <global_def>:
[ 	]*[a-f0-9]+:	c3                   	ret    

0+20 <weak_def>:
[ 	]*[a-f0-9]+:	c3                   	ret    

0+21 <local>:
[ 	]*[a-f0-9]+:	c3                   	ret    
#pass
