# The aim here is to test that
#   a) We generate the __rela_dyn_{start,end} symbols.
#   b) They span the entire .rela.dyn section.
# This particular case (the -3 file) is in case of there being only .got
# symbols that need dynamic relocations.
#source: emit-morello-reloc-markers-3.s
#as: -march=morello+c64
#ld: -Ttext-segment 0x0 -static
#objdump: --syms --section-headers --section=.rela.dyn --section=.got

.*:     file format .*

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 \.rela\.dyn     00000018  0000000000000000  0000000000000000  00010000  2\*\*3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  .* \.got .*
.*

SYMBOL TABLE:
0000000000000000 l    d  \.rela\.dyn	0000000000000000 \.rela\.dyn
.*
0000000000000018 l     O \.rela\.dyn	0000000000000000 __rela_dyn_end
0000000000000000 l     O \.rela\.dyn	0000000000000000 __rela_dyn_start
#pass
