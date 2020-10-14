#objdump: -dw
#name: i386 AVX VNNI insns

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	62 f2 5d 08 50 d2    	vpdpbusd %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	62 f2 5d 08 50 d2    	vpdpbusd %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 50 d2       	\{vex3\} vpdpbusd %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 50 d2       	\{vex3\} vpdpbusd %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 50 d2       	\{vex3\} vpdpbusd %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 50 11       	\{vex3\} vpdpbusd \(%ecx\),%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 50 11       	\{vex3\} vpdpbusd \(%ecx\),%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 50 11       	\{vex3\} vpdpbusd \(%ecx\),%xmm4,%xmm2
 +[a-f0-9]+:	62 f2 5d 08 52 d2    	vpdpwssd %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	62 f2 5d 08 52 d2    	vpdpwssd %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 52 d2       	\{vex3\} vpdpwssd %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 52 d2       	\{vex3\} vpdpwssd %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 52 d2       	\{vex3\} vpdpwssd %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 52 11       	\{vex3\} vpdpwssd \(%ecx\),%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 52 11       	\{vex3\} vpdpwssd \(%ecx\),%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 52 11       	\{vex3\} vpdpwssd \(%ecx\),%xmm4,%xmm2
 +[a-f0-9]+:	62 f2 5d 08 51 d2    	vpdpbusds %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	62 f2 5d 08 51 d2    	vpdpbusds %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 51 d2       	\{vex3\} vpdpbusds %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 51 d2       	\{vex3\} vpdpbusds %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 51 d2       	\{vex3\} vpdpbusds %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 51 11       	\{vex3\} vpdpbusds \(%ecx\),%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 51 11       	\{vex3\} vpdpbusds \(%ecx\),%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 51 11       	\{vex3\} vpdpbusds \(%ecx\),%xmm4,%xmm2
 +[a-f0-9]+:	62 f2 5d 08 53 d2    	vpdpwssds %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	62 f2 5d 08 53 d2    	vpdpwssds %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 53 d2       	\{vex3\} vpdpwssds %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 53 d2       	\{vex3\} vpdpwssds %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 53 d2       	\{vex3\} vpdpwssds %xmm2,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 53 11       	\{vex3\} vpdpwssds \(%ecx\),%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 53 11       	\{vex3\} vpdpwssds \(%ecx\),%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 53 11       	\{vex3\} vpdpwssds \(%ecx\),%xmm4,%xmm2
 +[a-f0-9]+:	62 f2 5d 08 50 d2    	vpdpbusd %xmm2,%xmm4,%xmm2
#pass
