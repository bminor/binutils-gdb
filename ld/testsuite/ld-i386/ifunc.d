#name: Generation of dynamic relocs for STT_IFUNC symbols
#source: ifunc.s
#as: --32
#ld: -melf_i386 -lc --defsym _start=0 -L/usr/lib
#readelf: --relocs --syms

Relocation section '.rel.ifunc.dyn' at offset 0x[0-9a-f]+ contains 2 entries:
 +Offset +Info +Type +Sym.Value +Sym. Name
[0-9a-f]+  0+0202 R_386_PC32        func\(\)     func
[0-9a-f]+  0+0102 R_386_PC32        long_fun\(\) long_func_name
#...
    ..: [0-9a-f]+    .. IFUNC   GLOBAL DEFAULT   .. long_func_name
    ..: [0-9a-f]+    .. IFUNC   GLOBAL DEFAULT   .. func
#pass
