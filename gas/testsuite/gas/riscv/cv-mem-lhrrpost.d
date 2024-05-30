#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+0263b2ab[ 	]+cv.lh[ 	]+t0,\(t2\),t1
[ 	]+4:[ 	]+03c33eab[ 	]+cv.lh[ 	]+t4,\(t1\),t3
[ 	]+8:[ 	]+025f3e2b[ 	]+cv.lh[ 	]+t3,\(t5\),t0
