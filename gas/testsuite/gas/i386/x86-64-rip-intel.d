#as: -J
#objdump: -drwMintel
#name: x86-64 rip addressing (Intel mode)
#source: x86-64-rip.s

.*: +file format .*

Disassembly of section .text:

0+ <.text>:
[ 	]*[a-f0-9]+:	8d 05 00 00 00 00    	lea    eax,\[rip\+0x0\][ 	]*#[ 	]*0x[a-f0-9]+[ 	]+[a-f0-9]+: R_X86_64_PC32	symbol-0x4
[ 	]*[a-f0-9]+:	8d 05 11 11 11 11    	lea    eax,\[rip\+0x11111111\]        # 0x1111111d
[ 	]*[a-f0-9]+:	8d 05 01 00 00 00    	lea    eax,\[rip\+0x1\]        # 0x13
[ 	]*[a-f0-9]+:	8d 05 00 00 00 00    	lea    eax,\[rip\+0x0\]        # 0x18
[ 	]*[a-f0-9]+:	48 8b 05 00 00 00 00 	mov    rax,QWORD PTR \[rip\+0x0\][ 	]*#[ 	]*0x[a-f0-9]+[ 	]+[a-f0-9]+: R_X86_64_PC32	__CTOR_END__-0xc
[ 	]*[a-f0-9]+:	48 8b 05 00 00 00 00 	mov    rax,QWORD PTR \[rip\+0x0\][ 	]*#[ 	]*0x[a-f0-9]+[ 	]+[a-f0-9]+: R_X86_64_PC32	__CTOR_END__-0xc
[ 	]*[a-f0-9]+:	48 8b 05 00 00 00 00 	mov    rax,QWORD PTR \[rip\+0x0\][ 	]*#[ 	]*0x[a-f0-9]+[ 	]+[a-f0-9]+: R_X86_64_PC32	__CTOR_END__-0xc
[ 	]*[a-f0-9]+:	48 8b 05 00 00 00 00 	mov    rax,QWORD PTR \[rip\+0x0\][ 	]*#[ 	]*0x[a-f0-9]+[ 	]+[a-f0-9]+: R_X86_64_PC32	__CTOR_END__-0xc
#pass
