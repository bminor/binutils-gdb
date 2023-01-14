#ld: -Toutput-section-types.t
#source: align2a.s
#objdump: -h
#target: [is_elf_format]

#...
  . \.rom.*
[ 	]+ALLOC, READONLY
  . \.ro.*
[ 	]+CONTENTS, ALLOC, LOAD, READONLY, DATA
  . \.over.*
[ 	]+CONTENTS, READONLY
#pass
