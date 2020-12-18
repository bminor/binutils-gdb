#source: restart-relax.s
#as:
#ld:
#objdump: -d

#...
Disassembly of section .text:

0+[0-9a-f]+ <_start>:
.*:[ 	]+[0-9a-f]+[ 	]+addi[ 	]+.*
#...
.*:[ 	]+[0-9a-f]+[ 	]+jal[ 	]+ra,[0-9a-f]+ <_start>
.*:[ 	]+[0-9a-f]+[ 	]+add[ 	]+a0,a1,a2
#pass
