#as: -march=rv32i_xcvbi
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <foo>:
[ 	]+0:[ 	]+0102e00b[ 	]+cv.beqimm[ 	]+t0,-16,0 +<foo>
[ 	]+4:[ 	]+fe5eee8b[ 	]+cv.beqimm[ 	]+t4,5,0 +<foo>
[ 	]+8:[ 	]+fef3ec8b[ 	]+cv.beqimm[ 	]+t2,15,0 +<foo>
