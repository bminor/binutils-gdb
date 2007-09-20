#source: sib.s
#objdump: -dw -Mintel
#name: i386 SIB (Intel mode)

.*: +file format .*

Disassembly of section .text:

0+ <foo>:
[ 	]*[a-f0-9]+:	8b 03                	mov    eax,DWORD PTR \[ebx\]
[ 	]*[a-f0-9]+:	8b 04 23             	mov    eax,DWORD PTR \[ebx\+eiz\*1\]
[ 	]*[a-f0-9]+:	8b 04 63             	mov    eax,DWORD PTR \[ebx\+eiz\*2\]
[ 	]*[a-f0-9]+:	8b 04 a3             	mov    eax,DWORD PTR \[ebx\+eiz\*4\]
[ 	]*[a-f0-9]+:	8b 04 e3             	mov    eax,DWORD PTR \[ebx\+eiz\*8\]
[ 	]*[a-f0-9]+:	8b 04 24             	mov    eax,DWORD PTR \[esp\]
[ 	]*[a-f0-9]+:	8b 04 24             	mov    eax,DWORD PTR \[esp\]
[ 	]*[a-f0-9]+:	8b 04 64             	mov    eax,DWORD PTR \[esp\+eiz\*2\]
[ 	]*[a-f0-9]+:	8b 04 a4             	mov    eax,DWORD PTR \[esp\+eiz\*4\]
[ 	]*[a-f0-9]+:	8b 04 e4             	mov    eax,DWORD PTR \[esp\+eiz\*8\]
#pass
