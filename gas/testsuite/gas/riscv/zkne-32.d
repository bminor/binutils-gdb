#as: -march=rv32i_zkne
#source: zkne-32.s
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+.*:[ 	]+.*[ 	]+aes32esi[ 	]+a0,a1,a2,0x2
[ 	]+.*:[ 	]+.*[ 	]+aes32esmi[ 	]+a0,a1,a2,0x2
