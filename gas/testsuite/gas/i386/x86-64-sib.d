#as: -J
#objdump: -dw
#name: x86-64 SIB

.*: +file format .*

Disassembly of section .text:

0+ <foo>:
[ 	]*[a-f0-9]+:	8b 03                	mov    \(%rbx\),%eax
[ 	]*[a-f0-9]+:	8b 04 23             	mov    \(%rbx,%riz,1\),%eax
[ 	]*[a-f0-9]+:	8b 04 63             	mov    \(%rbx,%riz,2\),%eax
[ 	]*[a-f0-9]+:	8b 04 a3             	mov    \(%rbx,%riz,4\),%eax
[ 	]*[a-f0-9]+:	8b 04 e3             	mov    \(%rbx,%riz,8\),%eax
[ 	]*[a-f0-9]+:	8b 04 24             	mov    \(%rsp\),%eax
[ 	]*[a-f0-9]+:	8b 04 24             	mov    \(%rsp\),%eax
[ 	]*[a-f0-9]+:	8b 04 64             	mov    \(%rsp,%riz,2\),%eax
[ 	]*[a-f0-9]+:	8b 04 a4             	mov    \(%rsp,%riz,4\),%eax
[ 	]*[a-f0-9]+:	8b 04 e4             	mov    \(%rsp,%riz,8\),%eax
#pass
