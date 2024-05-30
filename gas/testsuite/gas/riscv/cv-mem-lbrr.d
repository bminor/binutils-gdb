#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+0863b2ab[ 	]+cv.lb[ 	]+t0,t1\(t2\)
[ 	]+4:[ 	]+09c33eab[ 	]+cv.lb[ 	]+t4,t3\(t1\)
[ 	]+8:[ 	]+085f3e2b[ 	]+cv.lb[ 	]+t3,t0\(t5\)
