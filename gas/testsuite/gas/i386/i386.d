#as: -J
#objdump: -dw
#name: i386
.*: +file format .*

Disassembly of section .text:

0+ <.*>:
[ 	]*[a-f0-9]+:	df e0                	fnstsw %ax
[ 	]*[a-f0-9]+:	df e0                	fnstsw %ax
[ 	]*[a-f0-9]+:	9b df e0             	fstsw  %ax
[ 	]*[a-f0-9]+:	9b df e0             	fstsw  %ax
[ 	]*[a-f0-9]+:	0f be 10             	movsbl \(%eax\),%edx
[ 	]*[a-f0-9]+:	66 0f be 10          	movsbw \(%eax\),%dx
[ 	]*[a-f0-9]+:	0f be 10             	movsbl \(%eax\),%edx
[ 	]*[a-f0-9]+:	66 0f be 10          	movsbw \(%eax\),%dx
[ 	]*[a-f0-9]+:	0f be 10             	movsbl \(%eax\),%edx
[ 	]*[a-f0-9]+:	0f bf 10             	movswl \(%eax\),%edx
[ 	]*[a-f0-9]+:	0f b6 10             	movzbl \(%eax\),%edx
[ 	]*[a-f0-9]+:	66 0f b6 10          	movzbw \(%eax\),%dx
[ 	]*[a-f0-9]+:	0f b6 10             	movzbl \(%eax\),%edx
[ 	]*[a-f0-9]+:	66 0f b6 10          	movzbw \(%eax\),%dx
[ 	]*[a-f0-9]+:	0f b6 10             	movzbl \(%eax\),%edx
[ 	]*[a-f0-9]+:	66 0f b6 10          	movzbw \(%eax\),%dx
[ 	]*[a-f0-9]+:	0f b7 10             	movzwl \(%eax\),%edx
[ 	]*[a-f0-9]+:	0f c3 00             	movnti %eax,\(%eax\)
[ 	]*[a-f0-9]+:	0f c3 00             	movnti %eax,\(%eax\)
[ 	]*[a-f0-9]+:	df e0                	fnstsw %ax
[ 	]*[a-f0-9]+:	df e0                	fnstsw %ax
[ 	]*[a-f0-9]+:	9b df e0             	fstsw  %ax
[ 	]*[a-f0-9]+:	9b df e0             	fstsw  %ax
[ 	]*[a-f0-9]+:	0f be 10             	movsbl \(%eax\),%edx
[ 	]*[a-f0-9]+:	66 0f be 10          	movsbw \(%eax\),%dx
[ 	]*[a-f0-9]+:	0f bf 10             	movswl \(%eax\),%edx
[ 	]*[a-f0-9]+:	0f b6 10             	movzbl \(%eax\),%edx
[ 	]*[a-f0-9]+:	66 0f b6 10          	movzbw \(%eax\),%dx
[ 	]*[a-f0-9]+:	0f b7 10             	movzwl \(%eax\),%edx
[ 	]*[a-f0-9]+:	f3 0f 7e 0c 24       	movq   \(%esp\),%xmm1
[ 	]*[a-f0-9]+:	f3 0f 7e 0c 24       	movq   \(%esp\),%xmm1
[ 	]*[a-f0-9]+:	66 0f d6 0c 24       	movq   %xmm1,\(%esp\)
[ 	]*[a-f0-9]+:	66 0f d6 0c 24       	movq   %xmm1,\(%esp\)
[ 	]*[a-f0-9]+:	66 0f be 00          	movsbw \(%eax\),%ax
[ 	]*[a-f0-9]+:	0f be 00             	movsbl \(%eax\),%eax
[ 	]*[a-f0-9]+:	0f bf 00             	movswl \(%eax\),%eax
[ 	]*[a-f0-9]+:	66 0f b6 00          	movzbw \(%eax\),%ax
[ 	]*[a-f0-9]+:	0f b6 00             	movzbl \(%eax\),%eax
[ 	]*[a-f0-9]+:	0f b7 00             	movzwl \(%eax\),%eax
[ 	]*[a-f0-9]+:	0f c3 00             	movnti %eax,\(%eax\)
#pass
