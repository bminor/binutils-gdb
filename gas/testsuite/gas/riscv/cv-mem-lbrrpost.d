#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+0063b2ab[ 	]+cv.lb[ 	]+t0,\(t2\),t1
[ 	]+4:[ 	]+01c33eab[ 	]+cv.lb[ 	]+t4,\(t1\),t3
[ 	]+8:[ 	]+005f3e2b[ 	]+cv.lb[ 	]+t3,\(t5\),t0
