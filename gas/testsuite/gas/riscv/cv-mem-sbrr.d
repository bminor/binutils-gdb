#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+2853b32b[ 	]+cv.sb[ 	]+t0,t1\(t2\)
[ 	]+4:[ 	]+29d33e2b[ 	]+cv.sb[ 	]+t4,t3\(t1\)
[ 	]+8:[ 	]+29cf32ab[ 	]+cv.sb[ 	]+t3,t0\(t5\)
