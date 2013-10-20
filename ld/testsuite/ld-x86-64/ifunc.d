#name: Generation of dynamic relocs for STT_IFUNC symbols
#source: ifunc.s
#ld: --defsym _start=0 --defsym puts=0 --defsym rand=0 --defsym printf=0 
#readelf: --relocs --syms

Relocation section '.rela.ifunc.dyn' at offset 0x[0-9a-f]+ contains 2 entries:
 +Offset +Info +Type  +Sym. Value +Sym. Name \+ Addend
[0-9a-f]+  0+20+2 R_X86_64_PC32     .*
[0-9a-f]+  0+10+2 R_X86_64_PC32     .*
#...
    ..: [0-9a-f]+    .. IFUNC   GLOBAL DEFAULT   .. long_func_name
#...
    ..: [0-9a-f]+    .. IFUNC   GLOBAL DEFAULT   .. func
#pass
