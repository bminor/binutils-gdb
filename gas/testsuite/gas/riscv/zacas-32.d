#as: -march=rv32g_zacas
#source: zacas-32.s
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+[0-9a-f]+:[ 	]+28c5a52f[ 	]+amocas.w[ 	]+a0,a1,a2