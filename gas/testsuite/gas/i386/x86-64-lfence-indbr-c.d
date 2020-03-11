#source: x86-64-lfence-indbr.s
#as: -mlfence-before-indirect-branch=memory
#warning_output: x86-64-lfence-indbr.e
#objdump: -dw
#name: x86-64 -mlfence-before-indirect-branch=memory

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	ff d2                	callq  \*%rdx
 +[a-f0-9]+:	ff e2                	jmpq   \*%rdx
 +[a-f0-9]+:	ff 12                	callq  \*\(%rdx\)
 +[a-f0-9]+:	ff 22                	jmpq   \*\(%rdx\)
 +[a-f0-9]+:	ff 14 25 00 00 00 00 	callq  \*0x0
 +[a-f0-9]+:	ff 24 25 00 00 00 00 	jmpq   \*0x0
#pass
