#source: report-reloc-1.s
#as: --32
#ld: -pie -melf_i386 -z report-relative-reloc
#warning_output: report-reloc-1.l
#readelf: -r --wide

Relocation section '.rel.dyn' at offset 0x[0-9a-f]+ contains 2 entries:
 +Offset +Info +Type +Sym.* Value +Symbol's Name
[0-9a-f]+ +[0-9a-f]+ +R_386_RELATIVE +
[0-9a-f]+ +[0-9a-f]+ +R_386_IRELATIVE +
