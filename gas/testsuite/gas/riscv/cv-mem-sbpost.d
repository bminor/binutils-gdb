#as: -march=rv32i_xcvmem
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+0:[ 	]+81f3002b[ 	]+cv.sb[ 	]+t6,\(t1\),-2048
[ 	]+4:[ 	]+07d3822b[ 	]+cv.sb[ 	]+t4,\(t2\),100
[ 	]+8:[ 	]+7fce8fab[ 	]+cv.sb[ 	]+t3,\(t4\),2047
