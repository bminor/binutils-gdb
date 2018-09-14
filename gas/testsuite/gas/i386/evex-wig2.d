#as: -mevexwig=1
#objdump: -dw
#name: i386 non-WIG EVEX insns with -mevexwig=1

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	62 f1 56 38 2a f0    	vcvtsi2ss %eax,\{rd-sae\},%xmm5,%xmm6
 +[a-f0-9]+:	62 f1 56 08 2a f0    	vcvtsi2ss %eax,%xmm5,%xmm6
 +[a-f0-9]+:	62 f1 57 08 2a f0    	vcvtsi2sd %eax,%xmm5,%xmm6
 +[a-f0-9]+:	62 f1 56 38 7b f0    	vcvtusi2ss %eax,\{rd-sae\},%xmm5,%xmm6
 +[a-f0-9]+:	62 f1 56 08 7b f0    	vcvtusi2ss %eax,%xmm5,%xmm6
 +[a-f0-9]+:	62 f1 57 08 7b f0    	vcvtusi2sd %eax,%xmm5,%xmm6
#pass
