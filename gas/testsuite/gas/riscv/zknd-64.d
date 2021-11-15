#as: -march=rv64i_zknd
#source: zknd-64.s
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+.*:[ 	]+.*[ 	]+aes64ds[ 	]+a0,a1,a2
[ 	]+.*:[ 	]+.*[ 	]+aes64dsm[ 	]+a0,a1,a2
[ 	]+.*:[ 	]+.*[ 	]+aes64im[ 	]+a0,a0
[ 	]+.*:[ 	]+.*[ 	]+aes64ks1i[ 	]+a0,a1,0x4
[ 	]+.*:[ 	]+.*[ 	]+aes64ks2[ 	]+a0,a1,a2
