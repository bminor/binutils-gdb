#source: pr17154-x86.s
#as: --64
#ld: -z now -shared -melf_x86_64
#objdump: -dw
#target: x86_64-*-*
#notarget: x86_64-*-nacl*

.*: +file format .*


Disassembly of section .plt:

0+2b0 <.plt>:
 +[a-f0-9]+:	ff 35 aa 01 20 00    	pushq  0x2001aa\(%rip\)        # 200460 <_GLOBAL_OFFSET_TABLE_\+0x8>
 +[a-f0-9]+:	ff 25 ac 01 20 00    	jmpq   \*0x2001ac\(%rip\)        # 200468 <_GLOBAL_OFFSET_TABLE_\+0x10>
 +[a-f0-9]+:	0f 1f 40 00          	nopl   0x0\(%rax\)

0+2c0 <\*ABS\*\+0x2fa@plt>:
 +[a-f0-9]+:	ff 25 aa 01 20 00    	jmpq   \*0x2001aa\(%rip\)        # 200470 <_GLOBAL_OFFSET_TABLE_\+0x18>
 +[a-f0-9]+:	68 01 00 00 00       	pushq  \$0x1
 +[a-f0-9]+:	e9 e0 ff ff ff       	jmpq   2b0 <.plt>

0+2d0 <\*ABS\*\+0x2f0@plt>:
 +[a-f0-9]+:	ff 25 a2 01 20 00    	jmpq   \*0x2001a2\(%rip\)        # 200478 <_GLOBAL_OFFSET_TABLE_\+0x20>
 +[a-f0-9]+:	68 00 00 00 00       	pushq  \$0x0
 +[a-f0-9]+:	e9 d0 ff ff ff       	jmpq   2b0 <.plt>

Disassembly of section .plt.got:

0+2e0 <func1@plt>:
 +[a-f0-9]+:	ff 25 62 01 20 00    	jmpq   \*0x200162\(%rip\)        # 200448 <func1>
 +[a-f0-9]+:	66 90                	xchg   %ax,%ax

0+2e8 <func2@plt>:
 +[a-f0-9]+:	ff 25 62 01 20 00    	jmpq   \*0x200162\(%rip\)        # 200450 <func2>
 +[a-f0-9]+:	66 90                	xchg   %ax,%ax

Disassembly of section .text:

0+2f0 <resolve1>:
 +[a-f0-9]+:	e8 eb ff ff ff       	callq  2e0 <func1@plt>

0+2f5 <g1>:
 +[a-f0-9]+:	e9 d6 ff ff ff       	jmpq   2d0 <\*ABS\*\+0x2f0@plt>

0+2fa <resolve2>:
 +[a-f0-9]+:	e8 e9 ff ff ff       	callq  2e8 <func2@plt>

0+2ff <g2>:
 +[a-f0-9]+:	e9 bc ff ff ff       	jmpq   2c0 <\*ABS\*\+0x2fa@plt>
#pass
