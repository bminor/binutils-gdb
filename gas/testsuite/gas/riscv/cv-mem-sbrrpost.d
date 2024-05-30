#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+2053b32b[ 	]+cv.sb[ 	]+t0,\(t2\),t1
[ 	]+4:[ 	]+21d33e2b[ 	]+cv.sb[ 	]+t4,\(t1\),t3
[ 	]+8:[ 	]+21cf32ab[ 	]+cv.sb[ 	]+t3,\(t5\),t0
