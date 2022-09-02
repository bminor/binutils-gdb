#as: -mla-global-with-pcrel
#objdump: -Drsz

.*:[    ]+file format .*


Disassembly of section .text:

.* <foo>:
#...
[ 	]+8:[ 	]+02c04084[ 	]+addi.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+16\(0x10\)
#...
0+14 <__sec_end>:
#pass
