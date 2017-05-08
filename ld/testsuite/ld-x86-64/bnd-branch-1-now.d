#source: bnd-branch-1.s
#as: --64
#ld: -z now -shared -melf_x86_64
#objdump: -dw

.*: +file format .*


Disassembly of section .plt:

0+290 <.plt>:
 +[a-f0-9]+:	ff 35 82 01 20 00    	pushq  0x200182\(%rip\)        # 200418 <_GLOBAL_OFFSET_TABLE_\+0x8>
 +[a-f0-9]+:	ff 25 84 01 20 00    	jmpq   \*0x200184\(%rip\)        # 200420 <_GLOBAL_OFFSET_TABLE_\+0x10>
 +[a-f0-9]+:	0f 1f 40 00          	nopl   0x0\(%rax\)

Disassembly of section .plt.got:

0+2a0 <foo2@plt>:
 +[a-f0-9]+:	ff 25 4a 01 20 00    	jmpq   \*0x20014a\(%rip\)        # 2003f0 <foo2>
 +[a-f0-9]+:	66 90                	xchg   %ax,%ax

0+2a8 <foo3@plt>:
 +[a-f0-9]+:	ff 25 4a 01 20 00    	jmpq   \*0x20014a\(%rip\)        # 2003f8 <foo3>
 +[a-f0-9]+:	66 90                	xchg   %ax,%ax

0+2b0 <foo1@plt>:
 +[a-f0-9]+:	ff 25 4a 01 20 00    	jmpq   \*0x20014a\(%rip\)        # 200400 <foo1>
 +[a-f0-9]+:	66 90                	xchg   %ax,%ax

0+2b8 <foo4@plt>:
 +[a-f0-9]+:	ff 25 4a 01 20 00    	jmpq   \*0x20014a\(%rip\)        # 200408 <foo4>
 +[a-f0-9]+:	66 90                	xchg   %ax,%ax

Disassembly of section .text:

0+2c0 <_start>:
 +[a-f0-9]+:	f2 e9 ea ff ff ff    	bnd jmpq 2b0 <foo1@plt>
 +[a-f0-9]+:	e8 d5 ff ff ff       	callq  2a0 <foo2@plt>
 +[a-f0-9]+:	e9 d8 ff ff ff       	jmpq   2a8 <foo3@plt>
 +[a-f0-9]+:	e8 e3 ff ff ff       	callq  2b8 <foo4@plt>
 +[a-f0-9]+:	f2 e8 cd ff ff ff    	bnd callq 2a8 <foo3@plt>
 +[a-f0-9]+:	e9 d8 ff ff ff       	jmpq   2b8 <foo4@plt>
#pass
