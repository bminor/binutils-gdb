#as: -march=rv32i_zbkx
#source: zbkx.s
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+.*:[ 	]+.*[ 	]+xperm4[ 	]+a0,a1,a2
[ 	]+.*:[ 	]+.*[ 	]+xperm8[ 	]+a0,a1,a2
