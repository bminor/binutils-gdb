#name: -u --export-dynamic-symbol "f*" archive
#source: export-dynamic-symbol.s
#ld: -pie -u foo --export-dynamic-symbol "f*" tmpdir/libpr25910.a
#nm: -D

#...
[0-9a-f]+ T +foo
#...
