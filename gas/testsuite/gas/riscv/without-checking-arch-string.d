#as: -march=rv64gc
#source: without-checking-arch-string.s
#objdump: -dr -Mmax

.*:[ 	]+file format .*

Disassembly of section .text:
0+000 <.text>:
[ 	]+[0-9a-f]+:[ 	]+052f72d7[ 	]+vsetvli[ 	]+t0,t5,e32,m4,ta,mu