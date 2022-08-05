# The aim here is to test that
#   a) We generate the __rela_dyn_{start,end} symbols.
#   b) They span the entire .rela.dyn section.
# This particular case (the -2 file) is in case of there both .got symbols and
# local symbols that require relocations.
#source: emit-morello-reloc-markers-2.s
#as: -march=morello+c64
#ld: -Ttext-segment 0x0 -static
#objdump: --syms --section-headers --section=.rela.dyn --section=.got

.*:     file format .*

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
#record: RELADYN_START
  0 \.rela\.dyn     00000030  ([0-9a-f]+)  .*  .*  .*
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  .* \.got .*
.*

#check: STARTPOS string tolower $RELADYN_START
#check: ENDPOS format %016x [expr "0x$RELADYN_START + 0x30"]
SYMBOL TABLE:
STARTPOS l    d  \.rela\.dyn	0000000000000000 \.rela\.dyn
.*
ENDPOS l     O \.rela\.dyn	0000000000000000 __rela_dyn_end
STARTPOS l     O \.rela\.dyn	0000000000000000 __rela_dyn_start
#pass
