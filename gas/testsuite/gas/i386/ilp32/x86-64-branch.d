#source: ../x86-64-branch.s
#as: -J
#objdump: -drw -Mintel64
#name: x86-64 (ILP32) branch

.*: +file format .*

Disassembly of section .text:

0+ <.text>:
[ 	]*[a-f0-9]+:	ff d0                	callq  \*%rax
[ 	]*[a-f0-9]+:	ff d0                	callq  \*%rax
[ 	]*[a-f0-9]+:	66 ff d0             	data16 callq \*%rax
[ 	]*[a-f0-9]+:	66 ff d0             	data16 callq \*%rax
[ 	]*[a-f0-9]+:	66 ff 10             	data16 callq \*\(%rax\)
[ 	]*[a-f0-9]+:	ff e0                	jmpq   \*%rax
[ 	]*[a-f0-9]+:	ff e0                	jmpq   \*%rax
[ 	]*[a-f0-9]+:	66 ff e0             	data16 jmpq \*%rax
[ 	]*[a-f0-9]+:	66 ff e0             	data16 jmpq \*%rax
[ 	]*[a-f0-9]+:	66 ff 20             	data16 jmpq \*\(%rax\)
[ 	]*[a-f0-9]+:	e8 00 00 00 00       	callq  0x1f	1b: R_X86_64_PC32	\*ABS\*\+0x10003c
[ 	]*[a-f0-9]+:	e9 00 00 00 00       	jmpq   0x24	20: R_X86_64_PC32	\*ABS\*\+0x10003c
[ 	]*[a-f0-9]+:	66 e8 00 00 00 00    	data16 callq 0x2a	26: R_X86_64_PLT32	foo-0x4
[ 	]*[a-f0-9]+:	66 e9 00 00 00 00    	data16 jmpq 0x30	2c: R_X86_64_PLT32	foo-0x4
[ 	]*[a-f0-9]+:	66 0f 82 00 00 00 00 	data16 jb 0x37	33: R_X86_64_PLT32	foo-0x4
[ 	]*[a-f0-9]+:	66 c3                	data16 retq *
[ 	]*[a-f0-9]+:	66 c2 08 00          	data16 retq \$0x8
[ 	]*[a-f0-9]+:	ff d0                	callq  \*%rax
[ 	]*[a-f0-9]+:	ff d0                	callq  \*%rax
[ 	]*[a-f0-9]+:	66 ff d0             	data16 callq \*%rax
[ 	]*[a-f0-9]+:	66 ff d0             	data16 callq \*%rax
[ 	]*[a-f0-9]+:	66 ff 10             	data16 callq \*\(%rax\)
[ 	]*[a-f0-9]+:	ff e0                	jmpq   \*%rax
[ 	]*[a-f0-9]+:	ff e0                	jmpq   \*%rax
[ 	]*[a-f0-9]+:	66 ff e0             	data16 jmpq \*%rax
[ 	]*[a-f0-9]+:	66 ff e0             	data16 jmpq \*%rax
[ 	]*[a-f0-9]+:	66 ff 20             	data16 jmpq \*\(%rax\)
[ 	]*[a-f0-9]+:	e8 00 00 00 00       	callq  0x[0-9a-f]*	[0-9a-f]*: R_X86_64_PC32	\*ABS\*\+0x10003c
[ 	]*[a-f0-9]+:	e9 00 00 00 00       	jmpq   0x[0-9a-f]*	[0-9a-f]*: R_X86_64_PC32	\*ABS\*\+0x10003c
[ 	]*[a-f0-9]+:	66 c3                	data16 retq *
[ 	]*[a-f0-9]+:	66 c2 08 00          	data16 retq \$0x8
#pass
