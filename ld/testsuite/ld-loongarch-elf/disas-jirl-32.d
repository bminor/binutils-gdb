#as:
#objdump: -dr
#skip: loongarch64-*-*

.*:[    ]+file format .*


Disassembly of section .text:

00000000.* <_start>:
[ 	]+0:[ 	]+1a000014[ 	]+pcalau12i[ 	]+\$t8,[ 	]+0
[ 	]+0:[ 	]+R_LARCH_PCALA_HI20[ 	]+_start
[ 	]+4:[ 	]+02800294[ 	]+addi.w[ 	]+\$t8,[ 	]+\$t8,[ 	]+0
[ 	]+4:[ 	]+R_LARCH_PCALA_LO12[ 	]+_start
[ 	]+8:[ 	]+4c000281[ 	]+jirl[ 	]+\$ra,[ 	]+\$t8,[ 	]+0
