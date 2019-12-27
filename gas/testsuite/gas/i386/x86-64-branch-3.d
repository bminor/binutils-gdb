#as: -J -mintel64
#objdump: -dwr -Mintel64
#name: x86-64 branch 3

.*: +file format .*

Disassembly of section .text:

0+ <bar-0x6>:
[ 	]*[a-f0-9]+:	66 e9 00 00 00 00    	data16 jmpq 6 <bar>	2: R_X86_64_PLT32	foo-0x4

0+6 <bar>:
[ 	]*[a-f0-9]+:	89 c3                	mov    %eax,%ebx
[ 	]*[a-f0-9]+:	66 e8 00 00 00 00    	data16 callq e <bar\+0x8>	a: R_X86_64_PLT32	foo-0x4
[ 	]*[a-f0-9]+:	66 c7 f8 00 00       	xbeginw 13 <bar\+0xd>	11: R_X86_64_PC16	foo-0x2
#pass
