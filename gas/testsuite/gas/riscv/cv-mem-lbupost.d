#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+8003c28b[ 	]+cv.lbu[ 	]+t0,\(t2\),-2048
[ 	]+4:[ 	]+00f34e8b[ 	]+cv.lbu[ 	]+t4,\(t1\),15
[ 	]+8:[ 	]+7fff4e0b[ 	]+cv.lbu[ 	]+t3,\(t5\),2047
