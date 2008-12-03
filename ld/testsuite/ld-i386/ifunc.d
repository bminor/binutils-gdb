#name: Generatiion of dynamic relocs for STT_IFUNC symbols
#source: ifunc.s
#as: --32
#ld: -melf_i386 -lc --defsym _start=0 -L/usr/lib
#readelf: --relocs --syms

Relocation section '.rel.ifunc.dyn' at offset 0x[0-9a-f]+ contains 2 entries:
 Offset     Info    Type            Sym.Value  Sym. Name
[0-9a-f]+  00000202 R_386_PC32        func\(\)     func
[0-9a-f]+  00000102 R_386_PC32        long_fun\(\) long_func_name
#...
    ..: 080482e0    29 IFUNC   GLOBAL DEFAULT   10 long_func_name
    ..: 08048300    29 IFUNC   GLOBAL DEFAULT   10 func
#pass
