#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+2263bfab[ 	]+cv.sh[ 	]+t1,\(t2\),t6
[ 	]+4:[ 	]+22733f2b[ 	]+cv.sh[ 	]+t2,\(t1\),t5
[ 	]+8:[ 	]+23ee3eab[ 	]+cv.sh[ 	]+t5,\(t3\),t4
