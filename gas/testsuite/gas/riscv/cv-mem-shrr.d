#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+2a6ebe2b[ 	]+cv.sh[ 	]+t1,t3\(t4\)
[ 	]+4:[ 	]+2bd333ab[ 	]+cv.sh[ 	]+t4,t2\(t1\)
[ 	]+8:[ 	]+2bf3bf2b[ 	]+cv.sh[ 	]+t6,t5\(t2\)
