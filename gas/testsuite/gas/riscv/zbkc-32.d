#as: -march=rv32i_zbkc
#source: zbkc.s
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+.*:[ 	]+.*[ 	]+clmul[ 	]+a0,a1,a2
[ 	]+.*:[ 	]+.*[ 	]+clmulh[ 	]+a0,a1,a2
