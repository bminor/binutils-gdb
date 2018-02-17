#objdump: -drw
#name: i386 .nop 3

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	31 c0                	xor    %eax,%eax
 +[a-f0-9]+:	85 c0                	test   %eax,%eax
 +[a-f0-9]+:	0f 1f 00             	nopl   \(%eax\)
 +[a-f0-9]+:	31 c0                	xor    %eax,%eax
 +[a-f0-9]+:	31 c0                	xor    %eax,%eax

Disassembly of section .altinstr_replacement:

0+ <.altinstr_replacement>:
 +[a-f0-9]+:	e9 fc ff ff ff       	jmp    1 <.altinstr_replacement\+0x1>	1: (R_386_PC)?(DISP)?32	foo
#pass
