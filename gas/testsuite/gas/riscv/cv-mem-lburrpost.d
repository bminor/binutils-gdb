#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+1063b2ab[ 	]+cv.lbu[ 	]+t0,\(t2\),t1
[ 	]+4:[ 	]+11c33eab[ 	]+cv.lbu[ 	]+t4,\(t1\),t3
[ 	]+8:[ 	]+105f3e2b[ 	]+cv.lbu[ 	]+t3,\(t5\),t0
