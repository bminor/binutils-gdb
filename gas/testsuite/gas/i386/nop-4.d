#objdump: -drw
#name: i386 .nop 4

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	31 c0                	xor    %eax,%eax
 +[a-f0-9]+:	85 c0                	test   %eax,%eax
 +[a-f0-9]+:	66 0f 1f 84 00 00 00 00 00 	nopw   0x0\(%eax,%eax,1\)
 +[a-f0-9]+:	31 c0                	xor    %eax,%eax
 +[a-f0-9]+:	31 c0                	xor    %eax,%eax

Disassembly of section .altinstr_replacement:

0+ <.altinstr_replacement>:
 +[a-f0-9]+:	89 c0                	mov    %eax,%eax
 +[a-f0-9]+:	89 c0                	mov    %eax,%eax
 +[a-f0-9]+:	89 c0                	mov    %eax,%eax
 +[a-f0-9]+:	e9 fc ff ff ff       	jmp    7 <.altinstr_replacement\+0x7>	7: (R_386_PC)?(DISP)?32	foo
#pass
