#source: init.s
#as: --abi=64
#ld: -shared -mshelf64
#readelf: -d
#target: sh64-*-elf

# Make sure that the lsb of DT_INIT and DT_FINI entries is set
# when _init and _fini are SHmedia code.

Dynamic section at offset 0x330 contains 8 entries:
  Tag        Type                         Name/Value
 0x000000000000000c \(INIT\)               0x30d
 0x000000000000000d \(FINI\)               0x31d
 0x0000000000000004 \(HASH\)               0xe8
 0x0000000000000005 \(STRTAB\)             0x2a0
 0x0000000000000006 \(SYMTAB\)             0x138
 0x000000000000000a \(STRSZ\)              107 \(bytes\)
 0x000000000000000b \(SYMENT\)             24 \(bytes\)
 0x0000000000000000 \(NULL\)               0x0
