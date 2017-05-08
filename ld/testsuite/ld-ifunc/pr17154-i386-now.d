#source: pr17154-x86.s
#ld: -z now -m elf_i386 -shared
#as: --32
#objdump: -dw
#target: x86_64-*-* i?86-*-*
#notarget: x86_64-*-nacl* i?86-*-nacl*

.*: +file format .*


Disassembly of section .plt:

0+1d0 <.plt>:
 +[a-f0-9]+:	ff b3 04 00 00 00    	pushl  0x4\(%ebx\)
 +[a-f0-9]+:	ff a3 08 00 00 00    	jmp    \*0x8\(%ebx\)
 +[a-f0-9]+:	00 00                	add    %al,\(%eax\)
	...

0+1e0 <\*ABS\*@plt>:
 +[a-f0-9]+:	ff a3 0c 00 00 00    	jmp    \*0xc\(%ebx\)
 +[a-f0-9]+:	68 08 00 00 00       	push   \$0x8
 +[a-f0-9]+:	e9 e0 ff ff ff       	jmp    1d0 <.plt>

0+1f0 <\*ABS\*@plt>:
 +[a-f0-9]+:	ff a3 10 00 00 00    	jmp    \*0x10\(%ebx\)
 +[a-f0-9]+:	68 00 00 00 00       	push   \$0x0
 +[a-f0-9]+:	e9 d0 ff ff ff       	jmp    1d0 <.plt>

Disassembly of section .plt.got:

0+200 <func1@plt>:
 +[a-f0-9]+:	ff a3 f8 ff ff ff    	jmp    \*-0x8\(%ebx\)
 +[a-f0-9]+:	66 90                	xchg   %ax,%ax

0+208 <func2@plt>:
 +[a-f0-9]+:	ff a3 fc ff ff ff    	jmp    \*-0x4\(%ebx\)
 +[a-f0-9]+:	66 90                	xchg   %ax,%ax

Disassembly of section .text:

0+210 <resolve1>:
 +[a-f0-9]+:	e8 eb ff ff ff       	call   200 <func1@plt>

0+215 <g1>:
 +[a-f0-9]+:	e9 d6 ff ff ff       	jmp    1f0 <\*ABS\*@plt>

0+21a <resolve2>:
 +[a-f0-9]+:	e8 e9 ff ff ff       	call   208 <func2@plt>

0+21f <g2>:
 +[a-f0-9]+:	e9 bc ff ff ff       	jmp    1e0 <\*ABS\*@plt>
#pass
