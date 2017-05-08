#source: bnd-ifunc-2.s
#as: --64 -madd-bnd-prefix
#ld: -z now -shared -melf_x86_64 -z bndplt
#objdump: -dw

.*: +file format .*


Disassembly of section .plt:

0+2b0 <.plt>:
 +[a-f0-9]+:	ff 35 ba 01 20 00    	pushq  0x2001ba\(%rip\)        # 200470 <_GLOBAL_OFFSET_TABLE_\+0x8>
 +[a-f0-9]+:	f2 ff 25 bb 01 20 00 	bnd jmpq \*0x2001bb\(%rip\)        # 200478 <_GLOBAL_OFFSET_TABLE_\+0x10>
 +[a-f0-9]+:	0f 1f 00             	nopl   \(%rax\)
 +[a-f0-9]+:	68 01 00 00 00       	pushq  \$0x1
 +[a-f0-9]+:	f2 e9 e5 ff ff ff    	bnd jmpq 2b0 <.plt>
 +[a-f0-9]+:	0f 1f 44 00 00       	nopl   0x0\(%rax,%rax,1\)
 +[a-f0-9]+:	68 00 00 00 00       	pushq  \$0x0
 +[a-f0-9]+:	f2 e9 d5 ff ff ff    	bnd jmpq 2b0 <.plt>
 +[a-f0-9]+:	0f 1f 44 00 00       	nopl   0x0\(%rax,%rax,1\)

Disassembly of section .plt.got:

0+2e0 <func1@plt>:
 +[a-f0-9]+:	f2 ff 25 71 01 20 00 	bnd jmpq \*0x200171\(%rip\)        # 200458 <func1>
 +[a-f0-9]+:	90                   	nop

0+2e8 <func2@plt>:
 +[a-f0-9]+:	f2 ff 25 71 01 20 00 	bnd jmpq \*0x200171\(%rip\)        # 200460 <func2>
 +[a-f0-9]+:	90                   	nop

Disassembly of section .plt.bnd:

0+2f0 <\*ABS\*\+0x30c@plt>:
 +[a-f0-9]+:	f2 ff 25 89 01 20 00 	bnd jmpq \*0x200189\(%rip\)        # 200480 <_GLOBAL_OFFSET_TABLE_\+0x18>
 +[a-f0-9]+:	90                   	nop

0+2f8 <\*ABS\*\+0x300@plt>:
 +[a-f0-9]+:	f2 ff 25 89 01 20 00 	bnd jmpq \*0x200189\(%rip\)        # 200488 <_GLOBAL_OFFSET_TABLE_\+0x20>
 +[a-f0-9]+:	90                   	nop

Disassembly of section .text:

0+300 <resolve1>:
 +[a-f0-9]+:	f2 e8 da ff ff ff    	bnd callq 2e0 <func1@plt>

0+306 <g1>:
 +[a-f0-9]+:	f2 e9 ec ff ff ff    	bnd jmpq 2f8 <\*ABS\*\+0x300@plt>

0+30c <resolve2>:
 +[a-f0-9]+:	f2 e8 d6 ff ff ff    	bnd callq 2e8 <func2@plt>

0+312 <g2>:
 +[a-f0-9]+:	f2 e9 d8 ff ff ff    	bnd jmpq 2f0 <\*ABS\*\+0x30c@plt>
#pass
