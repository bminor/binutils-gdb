#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+8003d28b[ 	]+cv.lhu[ 	]+t0,\(t2\),-2048
[ 	]+4:[ 	]+00f35e8b[ 	]+cv.lhu[ 	]+t4,\(t1\),15
[ 	]+8:[ 	]+7fff5e0b[ 	]+cv.lhu[ 	]+t3,\(t5\),2047
