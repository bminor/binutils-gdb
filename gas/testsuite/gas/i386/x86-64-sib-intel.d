#source: x86-64-sib.s
#as: -J
#objdump: -dw -Mintel
#name: x86-64 SIB (Intel mode)

.*: +file format .*

Disassembly of section .text:

0+ <foo>:
[ 	]*[a-f0-9]+:	8b 03                	mov    eax,DWORD PTR \[rbx\]
[ 	]*[a-f0-9]+:	8b 04 23             	mov    eax,DWORD PTR \[rbx\+riz\*1\]
[ 	]*[a-f0-9]+:	8b 04 63             	mov    eax,DWORD PTR \[rbx\+riz\*2\]
[ 	]*[a-f0-9]+:	8b 04 a3             	mov    eax,DWORD PTR \[rbx\+riz\*4\]
[ 	]*[a-f0-9]+:	8b 04 e3             	mov    eax,DWORD PTR \[rbx\+riz\*8\]
[ 	]*[a-f0-9]+:	8b 04 24             	mov    eax,DWORD PTR \[rsp\]
[ 	]*[a-f0-9]+:	8b 04 24             	mov    eax,DWORD PTR \[rsp\]
[ 	]*[a-f0-9]+:	8b 04 64             	mov    eax,DWORD PTR \[rsp\+riz\*2\]
[ 	]*[a-f0-9]+:	8b 04 a4             	mov    eax,DWORD PTR \[rsp\+riz\*4\]
[ 	]*[a-f0-9]+:	8b 04 e4             	mov    eax,DWORD PTR \[rsp\+riz\*8\]
#pass
