#name: undefined weak symbol overflow
#source: undefweak-overflow.s
#ld: -Ttext=0x20000000 -e start
#objdump: -dr --show-raw-insn
#...
[0-9a-f]+ <_ftext>:
[ 0-9a-f]+:	d85fffff 	beqzc	v0,20000000 <_ftext>
[ 0-9a-f]+:	00000000 	nop
[ 0-9a-f]+:	f85ffffd 	bnezc	v0,20000000 <_ftext>
[ 0-9a-f]+:	ec4ffffd 	lwpc	v0,20000000 <_ftext>
[ 0-9a-f]+:	ec5bfffe 	ldpc	v0,20000000 <_ftext>
[ 0-9a-f]+:	cbfffffa 	bc	20000000 <_ftext>
[ 0-9a-f]+:	ec9ee000 	auipc	a0,0xe000
[ 0-9a-f]+:	2484ffe8 	addiu	a0,a0,-24
[ 0-9a-f]+:	1000fff7 	b	20000000 <_ftext>
[ 0-9a-f]+:	00000000 	nop
[ 0-9a-f]+:	0411fff5 	bal	20000000 <_ftext>
[ 0-9a-f]+:	3c...... 	lui	a0,0x....

[0-9a-f]+ <micro>:
[ 0-9a-f]+:	8e67      	beqz	a0,20000000 <_ftext>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	cfe5      	b	20000000 <_ftext>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	9400 ffe2 	b	20000000 <_ftext>
[ 0-9a-f]+:	0c00      	nop
#pass
