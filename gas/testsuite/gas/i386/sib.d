#objdump: -dw
#name: i386 SIB

.*: +file format .*

Disassembly of section .text:

0+ <foo>:
[ 	]*[a-f0-9]+:	8b 03                	mov    \(%ebx\),%eax
[ 	]*[a-f0-9]+:	8b 04 23             	mov    \(%ebx,%eiz,1\),%eax
[ 	]*[a-f0-9]+:	8b 04 63             	mov    \(%ebx,%eiz,2\),%eax
[ 	]*[a-f0-9]+:	8b 04 a3             	mov    \(%ebx,%eiz,4\),%eax
[ 	]*[a-f0-9]+:	8b 04 e3             	mov    \(%ebx,%eiz,8\),%eax
[ 	]*[a-f0-9]+:	8b 04 24             	mov    \(%esp\),%eax
[ 	]*[a-f0-9]+:	8b 04 24             	mov    \(%esp\),%eax
[ 	]*[a-f0-9]+:	8b 04 64             	mov    \(%esp,%eiz,2\),%eax
[ 	]*[a-f0-9]+:	8b 04 a4             	mov    \(%esp,%eiz,4\),%eax
[ 	]*[a-f0-9]+:	8b 04 e4             	mov    \(%esp,%eiz,8\),%eax
#pass
