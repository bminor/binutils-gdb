# ld: --defsym data_align=16 -T align2.t
# objdump: --section-headers

[^:]+: +file format.*

Sections:
Idx +Name +Size +VMA +LMA +File +off +Algn
 +0 +\.text +00000004 +00000000 +00000000 +00001000 +2\*\*2
 +CONTENTS, +ALLOC, +LOAD, +READONLY, +CODE
 +1 +\.data +00000004 +00000010 +00000010 +00001010 +2\*\*2
 +CONTENTS, +ALLOC, +LOAD, +DATA
 +2 +\.bss +00000000 +00000014 +00000014 +00001014 +2\*\*2
 +ALLOC
