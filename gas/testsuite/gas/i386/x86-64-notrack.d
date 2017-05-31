#objdump: -dw
#name: x86-64 NOTRACK prefix

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
[ 	]*[a-f0-9]+:	3e ff d0             	notrack callq \*%rax
[ 	]*[a-f0-9]+:	3e 41 ff d0          	notrack callq \*%r8
[ 	]*[a-f0-9]+:	3e ff e0             	notrack jmpq \*%rax
[ 	]*[a-f0-9]+:	3e 41 ff e0          	notrack jmpq \*%r8
[ 	]*[a-f0-9]+:	f2 3e ff d0          	bnd notrack callq \*%rax
[ 	]*[a-f0-9]+:	f2 3e 41 ff d0       	bnd notrack callq \*%r8
[ 	]*[a-f0-9]+:	f2 3e ff e0          	bnd notrack jmpq \*%rax
[ 	]*[a-f0-9]+:	f2 3e 41 ff e0       	bnd notrack jmpq \*%r8
[ 	]*[a-f0-9]+:	3e ff d0             	notrack callq \*%rax
[ 	]*[a-f0-9]+:	3e 41 ff d0          	notrack callq \*%r8
[ 	]*[a-f0-9]+:	3e ff e0             	notrack jmpq \*%rax
[ 	]*[a-f0-9]+:	3e 41 ff e0          	notrack jmpq \*%r8
[ 	]*[a-f0-9]+:	f2 3e ff d0          	bnd notrack callq \*%rax
[ 	]*[a-f0-9]+:	f2 3e 41 ff d0       	bnd notrack callq \*%r8
[ 	]*[a-f0-9]+:	f2 3e ff e0          	bnd notrack jmpq \*%rax
[ 	]*[a-f0-9]+:	f2 3e 41 ff e0       	bnd notrack jmpq \*%r8
[ 	]*[a-f0-9]+:	3e f2 ff d0          	ds bnd callq \*%rax
[ 	]*[a-f0-9]+:	3e 66 ff d0          	ds callw \*%ax
[ 	]*[a-f0-9]+:	66 3e ff d0          	ds callw \*%ax
#pass
