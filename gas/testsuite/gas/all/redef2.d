#objdump: -rsj .data
#name: .equ redefinitions (2)

.*: .*

RELOCATION RECORDS FOR .*
.*
0+0.*(here|\.data)
0+8.*xtrn
#...
Contents of section \.data:
 0000 00000000 11111111 00000000 22222222[ 	]+................[ 	]*
#pass
