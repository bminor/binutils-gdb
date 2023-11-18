#as: -march=rv32if_xtheadvector
#objdump: -dr

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <.text>:
[ 	]+[0-9a-f]+:[ 	]+80c5f557[ 	]+th.vsetvl[ 	]+a0,a1,a2
[ 	]+[0-9a-f]+:[ 	]+0005f557[ 	]+th.vsetvli[ 	]+a0,a1,e8,m1,tu,mu
[ 	]+[0-9a-f]+:[ 	]+7ff5f557[ 	]+th.vsetvli[ 	]+a0,a1,2047
