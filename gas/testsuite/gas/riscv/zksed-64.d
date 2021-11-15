#as: -march=rv64i_zksed
#source: zksed.s
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+.*:[ 	]+.*[ 	]+sm4ed[ 	]+a0,a1,a2,0x2
[ 	]+.*:[ 	]+.*[ 	]+sm4ks[ 	]+a0,a1,a2,0x2
