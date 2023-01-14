#name: jirl zero-offset symbols
#source: disas-jirl.s
#ld: --no-relax
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0000000120000078 <_start>:
[ 	]+120000078:[ 	]+1c000014 [ 	]+pcaddu12i[ 	]+[ 	]+\$t8, 0
[ 	]+12000007c:[ 	]+02c00294 [ 	]+addi.d[ 	]+[ 	]+\$t8, \$t8, 0
[ 	]+120000080:[ 	]+4c000281 [ 	]+jirl[ 	]+[ 	]+\$ra, \$t8, 0
