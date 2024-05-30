#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+8003828b[ 	]+cv.lb[ 	]+t0,\(t2\),-2048
[ 	]+4:[ 	]+00f30e8b[ 	]+cv.lb[ 	]+t4,\(t1\),15
[ 	]+8:[ 	]+7fff0e0b[ 	]+cv.lb[ 	]+t3,\(t5\),2047
