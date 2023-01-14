#as: -gdwarf-4
#readelf: -wl
#name: DWARF4 .debug_line 1

Raw dump of debug contents of section \.z?debug_line:

  Offset:                      0x0
  Length:                      .*
  DWARF Version:               4
  Prologue Length:             .*
  Minimum Instruction Length:  1
  Maximum Ops per Instruction: 1
  Initial value of 'is_stmt':  1
  Line Base:                   -5
  Line Range:                  14
  Opcode Base:                 13

 Opcodes:
  Opcode 1 has 0 args
  Opcode 2 has 1 arg
  Opcode 3 has 1 arg
  Opcode 4 has 1 arg
  Opcode 5 has 1 arg
  Opcode 6 has 0 args
  Opcode 7 has 0 args
  Opcode 8 has 0 args
  Opcode 9 has 1 arg
  Opcode 10 has 0 args
  Opcode 11 has 0 args
  Opcode 12 has 1 arg

 The Directory Table \(offset 0x.*\):
  1	.*/gas/testsuite/gas/i386

 The File Name Table \(offset 0x.*\):
  Entry	Dir	Time	Size	Name
  1	0	0	0	foo.c
  2	0	0	0	foo.h

 Line Number Statements:
  \[0x.*\]  Extended opcode 2: set Address to 0x0
  \[0x.*\]  Advance Line by 81 to 82
  \[0x.*\]  Copy
  \[0x.*\]  Set File Name to entry 2 in the File Name Table
  \[0x.*\]  Advance Line by -73 to 9
  \[0x.*\]  Special opcode 19: advance Address by 1 to 0x1 and Line by 0 to 9
  \[0x.*\]  Advance PC by 3 to 0x4
  \[0x.*\]  Extended opcode 1: End of Sequence


