#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+8003a28b[ 	]+cv.lw[ 	]+t0,\(t2\),-2048
[ 	]+4:[ 	]+00f32e8b[ 	]+cv.lw[ 	]+t4,\(t1\),15
[ 	]+8:[ 	]+7fff2e0b[ 	]+cv.lw[ 	]+t3,\(t5\),2047
