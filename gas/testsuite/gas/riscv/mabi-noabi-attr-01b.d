#as: -march-attr -march=rv64ifd
#objdump: -d
#source: mabi-attr-01.s

.*:[ 	]+file format elf32.*


Disassembly of section .text:

0+000 <foo>:
#...
[ 	]+[0-9a-f]+:[ 	]+00052503[ 	]+lw[ 	]+a0,0\(a0\) # .*
