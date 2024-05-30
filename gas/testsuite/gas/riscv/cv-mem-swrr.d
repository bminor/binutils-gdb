#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+2c6eb3ab[ 	]+cv.sw[ 	]+t1,t2\(t4\)
[ 	]+4:[ 	]+2dcf33ab[ 	]+cv.sw[ 	]+t3,t2\(t5\)
[ 	]+8:[ 	]+2c63bf2b[ 	]+cv.sw[ 	]+t1,t5\(t2\)
