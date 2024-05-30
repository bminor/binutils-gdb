#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+81cf102b[ 	]+cv.sh[ 	]+t3,\(t5\),-2048
[ 	]+4:[ 	]+026f9e2b[ 	]+cv.sh[ 	]+t1,\(t6\),60
[ 	]+8:[ 	]+7e7e9fab[ 	]+cv.sh[ 	]+t2,\(t4\),2047
