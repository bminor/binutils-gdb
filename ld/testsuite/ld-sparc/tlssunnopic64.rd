#source: tlssunnopic64.s
#source: tlsnopic.s
#as: --64 -Av9
#ld: -shared -melf64_sparc
#readelf: -WSsrl
#target: sparc-*-*

There are 13 section headers, starting at offset 0x[0-9a-f]+:

Section Headers:
 +\[Nr\] Name +Type +Address +Off +Size +ES Flg Lk Inf Al
 +\[ 0\] +NULL +0+ 0+ 0+ 0+ +0 +0 +0
 +\[ 1\] .hash +.*
 +\[ 2\] .dynsym +.*
 +\[ 3\] .dynstr +.*
 +\[ 4\] .rela.dyn +.*
 +\[ 5\] .text +PROGBITS +0+1000 0+1000 0+1000 0+ +AX +0 +0 4096
 +\[ 6\] .tbss +NOBITS +0+102000 0+2000 0+24 0+ WAT +0 +0 +4
 +\[ 7\] .dynamic +DYNAMIC +0+102000 0+2000 0+100 10 +WA +3 +0 +8
 +\[ 8\] .got +PROGBITS +0+102100 0+2100 0+38 08 +WA +0 +0 +8
 +\[ 9\] .plt +.*
 +\[10\] .shstrtab +.*
 +\[11\] .symtab +.*
 +\[12\] .strtab +.*
#...
Elf file type is DYN \(Shared object file\)
Entry point 0x1000
There are 4 program headers, starting at offset [0-9a-f]+

Program Headers:
 +Type +Offset +VirtAddr +PhysAddr +FileSiz +MemSiz +Flg Align
 +LOAD +0x0+ 0x0+ 0x0+ 0x0+2000 0x0+2000 R E 0x100000
 +LOAD +0x0+2000 0x0+102000 0x0+102000 0x0+200 0x0+200 RWE 0x100000
 +DYNAMIC +0x0+2000 0x0+102000 0x0+102000 0x0+100 0x0+100 RW +0x8
 +TLS +0x0+2000 0x0+102000 0x0+102000 0x0+ 0x0+24 R +0x4
#...

Relocation section '.rela.dyn' at offset 0x[0-9a-f]+ contains 14 entries:
 +Offset +Info +Type +Symbol's Value +Symbol's Name \+ Addend
0+1004 +0+300000022 R_SPARC_HH22 +0+102100 +\.got \+ 102100
0+1008 +0+300000024 R_SPARC_LM22 +0+102100 +\.got \+ 102100
0+100c +0+300000023 R_SPARC_HM10 +0+102100 +\.got \+ 102100
0+1010 +0+30000000c R_SPARC_LO10 +0+102100 +\.got \+ 102100
0+10ec +0+48 R_SPARC_TLS_LE_HIX22 +0+9
0+10f0 +0+49 R_SPARC_TLS_LE_LOX10 +0+9
0+1108 +0+48 R_SPARC_TLS_LE_HIX22 +0+1c
0+110c +0+49 R_SPARC_TLS_LE_LOX10 +0+1c
0+102108 +0+4f R_SPARC_TLS_TPOFF64 +0+
0+102110 +0+4f R_SPARC_TLS_TPOFF64 +0+4
0+102128 +0+4f R_SPARC_TLS_TPOFF64 +0+14
0+102130 +0+4f R_SPARC_TLS_TPOFF64 +0+18
0+102118 +0+90000004f R_SPARC_TLS_TPOFF64 +0+ +sg1 \+ 0
0+102120 +0+b0000004f R_SPARC_TLS_TPOFF64 +0+ +sg2 \+ 0

Symbol table '.dynsym' contains 14 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND *
 +1: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +5 *
 +2: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +6 *
 +3: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +8 *
 +4: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND *
 +5: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND *
 +6: 0+102000 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +7: 0+1000 +0 FUNC +GLOBAL DEFAULT +5 fn3
 +8: [0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +ABS _PROCEDURE_LINKAGE_TABLE_
 +9: 0+ +0 TLS +GLOBAL DEFAULT +UND sg1
 +10: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +11: 0+ +0 TLS +GLOBAL DEFAULT +UND sg2
 +12: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +13: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _end

Symbol table '.symtab' contains 31 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND *
 +1: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +1 *
 +2: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +2 *
 +3: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +3 *
 +4: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +4 *
 +5: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +5 *
 +6: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +6 *
 +7: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +7 *
 +8: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +8 *
 +9: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +9 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +10 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +11 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +12 *
 +[0-9]+: 0+ +0 TLS +LOCAL +DEFAULT +6 bl1
 +[0-9]+: 0+4 +0 TLS +LOCAL +DEFAULT +6 bl2
 +[0-9]+: 0+8 +0 TLS +LOCAL +DEFAULT +6 bl3
 +[0-9]+: 0+c +0 TLS +LOCAL +DEFAULT +6 bl4
 +[0-9]+: 0+10 +0 TLS +LOCAL +DEFAULT +6 bl5
 +[0-9]+: 0+1c +0 TLS +LOCAL +HIDDEN +6 sh3
 +[0-9]+: 0+20 +0 TLS +LOCAL +HIDDEN +6 sh4
 +[0-9]+: 0+14 +0 TLS +LOCAL +HIDDEN +6 sh1
 +[0-9]+: 0+102100 +0 OBJECT +LOCAL +HIDDEN +ABS _GLOBAL_OFFSET_TABLE_
 +[0-9]+: 0+18 +0 TLS +LOCAL +HIDDEN +6 sh2
 +[0-9]+: 0+102000 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +[0-9]+: 0+1000 +0 FUNC +GLOBAL DEFAULT +5 fn3
 +[0-9]+: [0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +ABS _PROCEDURE_LINKAGE_TABLE_
 +[0-9]+: 0+ +0 TLS +GLOBAL DEFAULT +UND sg1
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +[0-9]+: 0+ +0 TLS +GLOBAL DEFAULT +UND sg2
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _end
