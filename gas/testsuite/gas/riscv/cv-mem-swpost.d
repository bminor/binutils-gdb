#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+806ea02b[ 	]+cv.sw[ 	]+t1,\(t4\),-2048
[ 	]+4:[ 	]+07cf222b[ 	]+cv.sw[ 	]+t3,\(t5\),100
[ 	]+8:[ 	]+7e63afab[ 	]+cv.sw[ 	]+t1,\(t2\),2047
