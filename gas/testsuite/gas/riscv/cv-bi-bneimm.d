#as: -march=rv32i_xcvbi
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <foo>:
[ 	]+0:[ 	]+0102f00b[ 	]+cv.bneimm[ 	]+t0,-16,0 +<foo>
[ 	]+4:[ 	]+fe5efe8b[ 	]+cv.bneimm[ 	]+t4,5,0 +<foo>
[ 	]+8:[ 	]+fef3fc8b[ 	]+cv.bneimm[ 	]+t2,15,0 +<foo>
