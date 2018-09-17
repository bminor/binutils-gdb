#as: -mevexlig=256
#objdump: -dw
#name: i386 EVEX non-LIG insns with -mevexlig=256

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	62 f1 7d 08 7e 21    	vmovd  %xmm4,\(%ecx\)
 +[a-f0-9]+:	62 f1 7d 08 7e e1    	vmovd  %xmm4,%ecx
 +[a-f0-9]+:	62 f1 7d 08 6e 21    	vmovd  \(%ecx\),%xmm4
 +[a-f0-9]+:	62 f1 7d 08 6e e1    	vmovd  %ecx,%xmm4
 +[a-f0-9]+:	62 f1 fd 08 d6 21    	vmovq  %xmm4,\(%ecx\)
 +[a-f0-9]+:	62 f1 fe 08 7e 21    	vmovq  \(%ecx\),%xmm4
 +[a-f0-9]+:	62 f1 fe 08 7e f4    	vmovq  %xmm4,%xmm6
#pass
