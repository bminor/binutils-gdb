#as: -march=rv64i_zkne
#source: zkne-64.s
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+.*:[ 	]+.*[ 	]+aes64es[ 	]+a0,a1,a2
[ 	]+.*:[ 	]+.*[ 	]+aes64esm[ 	]+a0,a1,a2
[ 	]+.*:[ 	]+.*[ 	]+aes64ks1i[ 	]+a0,a1,0x4
[ 	]+.*:[ 	]+.*[ 	]+aes64ks2[ 	]+a0,a1,a2
