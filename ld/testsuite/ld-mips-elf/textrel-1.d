#name: MIPS textrel-1
#source: textrel-1.s
#as: -EB -n32
#ld: -shared -melf32btsmipn32
#readelf: -d

Dynamic section at offset 0xb4 contains 18 entries:
  Tag        Type                         Name/Value
 0x00000004 \(HASH\)                       0x16c
 0x00000005 \(STRTAB\)                     0x30c
 0x00000006 \(SYMTAB\)                     0x1fc
 0x0000000a \(STRSZ\)                      84 \(bytes\)
 0x0000000b \(SYMENT\)                     16 \(bytes\)
 0x00000015 \(DEBUG\)                      0x0
 0x00000003 \(PLTGOT\)                     0x1003e0
 0x00000011 \(REL\)                        0x360
 0x00000012 \(RELSZ\)                      8 \(bytes\)
 0x00000013 \(RELENT\)                     8 \(bytes\)
 0x70000001 \(MIPS_RLD_VERSION\)           1
 0x70000005 \(MIPS_FLAGS\)                 NOTPOT
 0x70000006 \(MIPS_BASE_ADDRESS\)          0
 0x7000000a \(MIPS_LOCAL_GOTNO\)           7
 0x70000011 \(MIPS_SYMTABNO\)              17
 0x70000012 \(MIPS_UNREFEXTNO\)            12
 0x70000013 \(MIPS_GOTSYM\)                0x11
 0x00000000 \(NULL\)                       0x0
