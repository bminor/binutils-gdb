#as: -march=rv32i_zknd
#source: zknd-32.s
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+.*:[ 	]+.*[ 	]+aes32dsi[ 	]+a0,a1,a2,0x2
[ 	]+.*:[ 	]+.*[ 	]+aes32dsmi[ 	]+a0,a1,a2,0x2
