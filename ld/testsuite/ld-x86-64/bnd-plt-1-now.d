#source: bnd-branch-1.s
#as: --64
#ld: -z now -shared -melf_x86_64 -z bndplt
#objdump: -dw

.*: +file format .*


Disassembly of section .plt:

0+290 <.plt>:
 +[a-f0-9]+:	ff 35 82 01 20 00    	pushq  0x200182\(%rip\)        # 200418 <_GLOBAL_OFFSET_TABLE_\+0x8>
 +[a-f0-9]+:	f2 ff 25 83 01 20 00 	bnd jmpq \*0x200183\(%rip\)        # 200420 <_GLOBAL_OFFSET_TABLE_\+0x10>
 +[a-f0-9]+:	0f 1f 00             	nopl   \(%rax\)

Disassembly of section .plt.got:

0+2a0 <foo2@plt>:
 +[a-f0-9]+:	f2 ff 25 49 01 20 00 	bnd jmpq \*0x200149\(%rip\)        # 2003f0 <foo2>
 +[a-f0-9]+:	90                   	nop

0+2a8 <foo3@plt>:
 +[a-f0-9]+:	f2 ff 25 49 01 20 00 	bnd jmpq \*0x200149\(%rip\)        # 2003f8 <foo3>
 +[a-f0-9]+:	90                   	nop

0+2b0 <foo1@plt>:
 +[a-f0-9]+:	f2 ff 25 49 01 20 00 	bnd jmpq \*0x200149\(%rip\)        # 200400 <foo1>
 +[a-f0-9]+:	90                   	nop

0+2b8 <foo4@plt>:
 +[a-f0-9]+:	f2 ff 25 49 01 20 00 	bnd jmpq \*0x200149\(%rip\)        # 200408 <foo4>
 +[a-f0-9]+:	90                   	nop

Disassembly of section .text:

0+2c0 <_start>:
 +[a-f0-9]+:	f2 e9 ea ff ff ff    	bnd jmpq 2b0 <foo1@plt>
 +[a-f0-9]+:	e8 d5 ff ff ff       	callq  2a0 <foo2@plt>
 +[a-f0-9]+:	e9 d8 ff ff ff       	jmpq   2a8 <foo3@plt>
 +[a-f0-9]+:	e8 e3 ff ff ff       	callq  2b8 <foo4@plt>
 +[a-f0-9]+:	f2 e8 cd ff ff ff    	bnd callq 2a8 <foo3@plt>
 +[a-f0-9]+:	e9 d8 ff ff ff       	jmpq   2b8 <foo4@plt>
#pass
