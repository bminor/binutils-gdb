#name: Morello ABI arguments accepted but ignored.
#as: -mabi=purecap
#as: -mabi=lp64
#as: -mabi=hybrid
#readelf: --syms

Symbol table '\.symtab' contains 10 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 SECTION LOCAL  DEFAULT    1 
     2: 0000000000000000     0 SECTION LOCAL  DEFAULT    2 
     3: 0000000000000000     0 SECTION LOCAL  DEFAULT    3 
     4: 0000000000000001     0 FUNC    LOCAL  DEFAULT    1 foo
     5: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT    1 \$c
     6: 0000000000000008     0 FUNC    LOCAL  DEFAULT    1 bar
     7: 0000000000000008     0 NOTYPE  LOCAL  DEFAULT    1 \$x
     8: 0000000000000011     0 FUNC    LOCAL  DEFAULT    1 baz
     9: 0000000000000010     0 NOTYPE  LOCAL  DEFAULT    1 \$c
