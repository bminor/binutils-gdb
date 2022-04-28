# Checking that the DT_INIT and DT_FINI entries in the dynamic section include
# the LSB when referring to functions which include the LSB.
#as: -march=morello+c64
#ld: -shared
#readelf: --symbols --dynamic --wide

Dynamic section at offset .*
  Tag        Type                         Name/Value
#record: INIT_LOC
#...
 0x000000000000000c \(INIT\)               0x([0-9a-f]+)
#record: FINI_LOC
#...
 0x000000000000000d \(FINI\)               0x([0-9a-f]+)
#...
Symbol table '.symtab' contains 18 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
#check: INIT_ADDR string tolower $INIT_LOC
#check: FINI_ADDR string tolower $FINI_LOC
#...
.*: 0+INIT_ADDR     0 FUNC    LOCAL  DEFAULT    .* _init
.*: 0+FINI_ADDR     0 FUNC    LOCAL  DEFAULT    .* _fini
#pass
