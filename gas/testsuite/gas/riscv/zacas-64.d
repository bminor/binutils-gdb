#as: -march=rv64i_zacas
#source: zacas-64.s
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+[0-9a-f]+:[ 	]+28c5b52f[ 	]+amocas.d[ 	]+a0,a1,a2