#as: -J
#objdump: -drw
#name: x86-64 rip addressing

.*: +file format .*

Disassembly of section .text:

0+ <.text>:
[ 	]*[a-f0-9]+:	8d 05 00 00 00 00    	lea    0x0\(%rip\),%eax[ 	]*#[ 	]*0x[a-f0-9]+[ 	]+[a-f0-9]+: R_X86_64_PC32	symbol-0x4
[ 	]*[a-f0-9]+:	8d 05 11 11 11 11    	lea    0x11111111\(%rip\),%eax        # 0x1111111d
[ 	]*[a-f0-9]+:	8d 05 01 00 00 00    	lea    0x1\(%rip\),%eax        # 0x13
[ 	]*[a-f0-9]+:	8d 05 00 00 00 00    	lea    0x0\(%rip\),%eax        # 0x18
[ 	]*[a-f0-9]+:	48 8b 05 00 00 00 00 	mov    0x0\(%rip\),%rax[ 	]*#[ 	]*0x[a-f0-9]+[ 	]+[a-f0-9]+: R_X86_64_PC32	__CTOR_END__-0xc
[ 	]*[a-f0-9]+:	48 8b 05 00 00 00 00 	mov    0x0\(%rip\),%rax[ 	]*#[ 	]*0x[a-f0-9]+[ 	]+[a-f0-9]+: R_X86_64_PC32	__CTOR_END__-0xc
[ 	]*[a-f0-9]+:	48 8b 05 00 00 00 00 	mov    0x0\(%rip\),%rax[ 	]*#[ 	]*0x[a-f0-9]+[ 	]+[a-f0-9]+: R_X86_64_PC32	__CTOR_END__-0xc
[ 	]*[a-f0-9]+:	48 8b 05 00 00 00 00 	mov    0x0\(%rip\),%rax[ 	]*#[ 	]*0x[a-f0-9]+[ 	]+[a-f0-9]+: R_X86_64_PC32	__CTOR_END__-0xc
#pass
