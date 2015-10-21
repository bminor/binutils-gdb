#PROG: objcopy
#name: objcopy add-symbol
#source: symbols.s
#objcopy: --add-symbol NEW=0x1234 --add-symbol NEW_DATA=.data:0x4321,local
#objdump: --syms
# MIPS targets swap the order of the symbols in the output.
#not-target: mips-*-* tx39-*-*

.*: +file format .*

SYMBOL TABLE:
#...
0+04321 l[ 	]+.data[ 	]+0+00 NEW_DATA
#...
0+01234 g[ 	]+\*ABS\*[ 	]+0+00 NEW
#pass
