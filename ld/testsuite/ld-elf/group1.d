#source: group1a.s
#source: group1b.s
#ld: -T group.ld
#readelf: -s
Symbol table '.symtab' contains .* entries:
#...
    .*: 00001000     0 NOTYPE  WEAK   DEFAULT    . foo
    .*: 00000000     0 NOTYPE  GLOBAL DEFAULT  UND bar
#...
