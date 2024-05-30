#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+0463b2ab[ 	]+cv.lw[ 	]+t0,\(t2\),t1
[ 	]+4:[ 	]+05c33eab[ 	]+cv.lw[ 	]+t4,\(t1\),t3
[ 	]+8:[ 	]+045f3e2b[ 	]+cv.lw[ 	]+t3,\(t5\),t0
