#source: x86-64-notrack.s
#objdump: -dw -Mintel
#name: x86-64 NOTRACK prefix (Intel disassembly)

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
[ 	]*[a-f0-9]+:	3e ff d0             	notrack call rax
[ 	]*[a-f0-9]+:	3e 41 ff d0          	notrack call r8
[ 	]*[a-f0-9]+:	3e ff e0             	notrack jmp rax
[ 	]*[a-f0-9]+:	3e 41 ff e0          	notrack jmp r8
[ 	]*[a-f0-9]+:	f2 3e ff d0          	bnd notrack call rax
[ 	]*[a-f0-9]+:	f2 3e 41 ff d0       	bnd notrack call r8
[ 	]*[a-f0-9]+:	f2 3e ff e0          	bnd notrack jmp rax
[ 	]*[a-f0-9]+:	f2 3e 41 ff e0       	bnd notrack jmp r8
[ 	]*[a-f0-9]+:	3e ff d0             	notrack call rax
[ 	]*[a-f0-9]+:	3e 41 ff d0          	notrack call r8
[ 	]*[a-f0-9]+:	3e ff e0             	notrack jmp rax
[ 	]*[a-f0-9]+:	3e 41 ff e0          	notrack jmp r8
[ 	]*[a-f0-9]+:	f2 3e ff d0          	bnd notrack call rax
[ 	]*[a-f0-9]+:	f2 3e 41 ff d0       	bnd notrack call r8
[ 	]*[a-f0-9]+:	f2 3e ff e0          	bnd notrack jmp rax
[ 	]*[a-f0-9]+:	f2 3e 41 ff e0       	bnd notrack jmp r8
[ 	]*[a-f0-9]+:	3e f2 ff d0          	ds bnd call rax
[ 	]*[a-f0-9]+:	3e 66 ff d0          	ds call ax
[ 	]*[a-f0-9]+:	66 3e ff d0          	ds call ax
#pass
