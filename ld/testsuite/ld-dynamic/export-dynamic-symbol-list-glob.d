#name: -u --export-dynamic-symbol-list fstar archive
#source: export-dynamic-symbol.s
#ld: -pie -u foo --export-dynamic-symbol-list fstar.list tmpdir/libpr25910.a
#nm: -D

#...
[0-9a-f]+ T +foo
#...
