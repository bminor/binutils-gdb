#source: group3b.s
#source: group3a.s
#ld: -T group.ld
#readelf: -s
#notarget arc-*-* d30v-*-* dlx-*-* i960-*-* or32-*-* pj*-*-*

Symbol table '.symtab' contains .* entries:
#...
.*: 0+1000 +0 +OBJECT +GLOBAL +HIDDEN +. foo
#...
