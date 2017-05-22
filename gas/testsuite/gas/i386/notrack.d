#objdump: -dw
#name: i386 NOTRACK prefix

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
[ 	]*[a-f0-9]+:	3e ff d0             	notrack call \*%eax
[ 	]*[a-f0-9]+:	66 3e ff d0          	notrack callw \*%ax
[ 	]*[a-f0-9]+:	3e ff e0             	notrack jmp \*%eax
[ 	]*[a-f0-9]+:	66 3e ff e0          	notrack jmpw \*%ax
[ 	]*[a-f0-9]+:	f2 3e ff d0          	bnd notrack call \*%eax
[ 	]*[a-f0-9]+:	66 f2 3e ff d0       	bnd notrack callw \*%ax
[ 	]*[a-f0-9]+:	f2 3e ff e0          	bnd notrack jmp \*%eax
[ 	]*[a-f0-9]+:	66 f2 3e ff e0       	bnd notrack jmpw \*%ax
[ 	]*[a-f0-9]+:	3e ff d0             	notrack call \*%eax
[ 	]*[a-f0-9]+:	66 3e ff d0          	notrack callw \*%ax
[ 	]*[a-f0-9]+:	3e ff e0             	notrack jmp \*%eax
[ 	]*[a-f0-9]+:	66 3e ff e0          	notrack jmpw \*%ax
[ 	]*[a-f0-9]+:	f2 3e ff d0          	bnd notrack call \*%eax
[ 	]*[a-f0-9]+:	66 f2 3e ff d0       	bnd notrack callw \*%ax
[ 	]*[a-f0-9]+:	f2 3e ff e0          	bnd notrack jmp \*%eax
[ 	]*[a-f0-9]+:	66 f2 3e ff e0       	bnd notrack jmpw \*%ax
[ 	]*[a-f0-9]+:	3e f2 ff d0          	ds bnd call \*%eax
[ 	]*[a-f0-9]+:	3e 66 ff d0          	ds callw \*%ax
#pass
