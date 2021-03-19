#source: pr27590.s
#ld: -r tmpdir/pr27590.o --reduce-memory-overheads
#readelf: -rW
#xfail: [is_generic]

Relocation section '\.rel.*\.gnu\.debuglto_\.debug_macro' at offset 0x[0-9a-z]+ contains 2 entries:
[ \t]+Offset[ \t]+Info[ \t]+Type[ \t]+Sym.*
[0-9a-f]+[ \t]+[0-9a-f]+[ \t]+R_.*[ \t]+[0]+[ \t]+\.gnu\.debuglto_\.debug_macro.*
[0-9a-f]+[ \t]+[0-9a-f]+[ \t]+R_.*[ \t]+[0]+[ \t]+\.gnu\.debuglto_\.debug_macro.*
#pass
