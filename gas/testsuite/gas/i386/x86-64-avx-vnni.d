#objdump: -dw
#name: x86-64 AVX VNNI insns

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	62 d2 5d 08 50 d4    	vpdpbusd %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	62 d2 5d 08 50 d4    	vpdpbusd %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	c4 c2 59 50 d4       	\{vex3\} vpdpbusd %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	c4 c2 59 50 d4       	\{vex3\} vpdpbusd %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 50 11       	\{vex3\} vpdpbusd \(%rcx\),%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 50 11       	\{vex3\} vpdpbusd \(%rcx\),%xmm4,%xmm2
 +[a-f0-9]+:	62 b2 5d 08 50 d6    	vpdpbusd %xmm22,%xmm4,%xmm2
 +[a-f0-9]+:	62 d2 5d 08 52 d4    	vpdpwssd %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	62 d2 5d 08 52 d4    	vpdpwssd %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	c4 c2 59 52 d4       	\{vex3\} vpdpwssd %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	c4 c2 59 52 d4       	\{vex3\} vpdpwssd %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 52 11       	\{vex3\} vpdpwssd \(%rcx\),%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 52 11       	\{vex3\} vpdpwssd \(%rcx\),%xmm4,%xmm2
 +[a-f0-9]+:	62 b2 5d 08 52 d6    	vpdpwssd %xmm22,%xmm4,%xmm2
 +[a-f0-9]+:	62 d2 5d 08 51 d4    	vpdpbusds %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	62 d2 5d 08 51 d4    	vpdpbusds %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	c4 c2 59 51 d4       	\{vex3\} vpdpbusds %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	c4 c2 59 51 d4       	\{vex3\} vpdpbusds %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 51 11       	\{vex3\} vpdpbusds \(%rcx\),%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 51 11       	\{vex3\} vpdpbusds \(%rcx\),%xmm4,%xmm2
 +[a-f0-9]+:	62 b2 5d 08 51 d6    	vpdpbusds %xmm22,%xmm4,%xmm2
 +[a-f0-9]+:	62 d2 5d 08 53 d4    	vpdpwssds %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	62 d2 5d 08 53 d4    	vpdpwssds %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	c4 c2 59 53 d4       	\{vex3\} vpdpwssds %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	c4 c2 59 53 d4       	\{vex3\} vpdpwssds %xmm12,%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 53 11       	\{vex3\} vpdpwssds \(%rcx\),%xmm4,%xmm2
 +[a-f0-9]+:	c4 e2 59 53 11       	\{vex3\} vpdpwssds \(%rcx\),%xmm4,%xmm2
 +[a-f0-9]+:	62 b2 5d 08 53 d6    	vpdpwssds %xmm22,%xmm4,%xmm2
 +[a-f0-9]+:	62 d2 5d 08 50 d4    	vpdpbusd %xmm12,%xmm4,%xmm2
#pass
