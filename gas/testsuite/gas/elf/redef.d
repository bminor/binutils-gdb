#objdump: -t
#name: .equ redefinitions (ELF)
#source: ../all/redef2.s

.*: .*

SYMBOL TABLE:
#...
0+[ 	]+l[ 	].*[ 	]here
#...
0*2+[ 	]+l[ 	]+\*ABS\*[ 	].*[ 	]sym
#...
0+[ 	]+\*UND\*[ 	].*[ 	]xtrn
#...
