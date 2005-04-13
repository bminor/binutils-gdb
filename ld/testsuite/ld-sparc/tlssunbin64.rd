#source: tlssunbin64.s
#as: --64
#ld: -shared -melf64_sparc tmpdir/libtlslib64.so tmpdir/tlssunbinpic64.o
#readelf: -WSsrl
#target: sparc*-*-*

There are 15 section headers, starting at offset 0x[0-9a-f]+:

Section Headers:
 +\[Nr\] Name +Type +Address +Off +Size +ES Flg Lk Inf Al
 +\[ 0\] +NULL +0+ 0+ 0+ 00 +0 +0 +0
 +\[ 1\] .interp +.*
 +\[ 2\] .hash +.*
 +\[ 3\] .dynsym +.*
 +\[ 4\] .dynstr +.*
 +\[ 5\] .rela.dyn +.*
 +\[ 6\] .text +PROGBITS +0+101000 0+1000 0+11a4 00 +AX +0 +0 4096
 +\[ 7\] .tdata +PROGBITS +0+2021a4 0+21a4 0+0060 00 WAT +0 +0 +4
 +\[ 8\] .tbss +NOBITS +0+202204 0+2204 0+40 00 WAT +0 +0 +4
 +\[ 9\] .dynamic +DYNAMIC +0+202208 0+2208 0+100 10 +WA +4 +0 +8
 +\[10\] .got +PROGBITS +0+202308 0+2308 0+28 08 +WA +0 +0 +8
 +\[11\] .plt +.*
 +\[12\] .shstrtab +.*
 +\[13\] .symtab +.*
 +\[14\] .strtab +.*
#...

Elf file type is EXEC \(Executable file\)
Entry point 0x102000
There are 6 program headers, starting at offset [0-9]+

Program Headers:
 +Type +Offset +VirtAddr +PhysAddr +FileSiz +MemSiz +Flg Align
 +PHDR +0x0+40 0x0+100040 0x0+100040 0x0+150 0x0+150 R E 0x8
 +INTERP +0x0+190 0x0+100190 0x0+100190 0x0+19 0x0+19 R +0x1
.*Requesting program interpreter.*
 +LOAD +0x0+ 0x0+100000 0x0+100000 0x0+21a4 0x0+21a4 R E 0x100000
 +LOAD +0x0+21a4 0x0+2021a4 0x0+2021a4 0x0+25c 0x0+25c RWE 0x100000
 +DYNAMIC +0x0+2208 0x0+202208 0x0+202208 0x0+100 0x0+100 RW +0x8
 +TLS +0x0+21a4 0x0+2021a4 0x0+2021a4 0x0+60 0x0+a0 R +0x4
#...

Relocation section '.rela.dyn' at offset 0x[0-9a-f]+ contains 4 entries:
 +Offset +Info +Type +Symbol's Value +Symbol's Name \+ Addend
0+202310 +0+10000004f R_SPARC_TLS_TPOFF64 +0+ +sG5 \+ 0
0+202318 +0+30000004f R_SPARC_TLS_TPOFF64 +0+ +sG2 \+ 0
0+202320 +0+70000004f R_SPARC_TLS_TPOFF64 +0+ +sG6 \+ 0
0+202328 +0+80000004f R_SPARC_TLS_TPOFF64 +0+ +sG1 \+ 0

Symbol table '.dynsym' contains 11 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND *
 +1: 0+ +0 TLS +GLOBAL DEFAULT +UND sG5
 +2: 0+202208 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +3: 0+ +0 TLS +GLOBAL DEFAULT +UND sG2
 +4: [0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +ABS _PROCEDURE_LINKAGE_TABLE_
 +5: 0+ +0 FUNC +GLOBAL DEFAULT +UND __tls_get_addr
 +6: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +7: 0+ +0 TLS +GLOBAL DEFAULT +UND sG6
 +8: 0+ +0 TLS +GLOBAL DEFAULT +UND sG1
 +9: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +10: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _end

Symbol table '.symtab' contains 68 entries:
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
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +13 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +14 *
 +[0-9]+: 0+20 +0 TLS +LOCAL +DEFAULT +7 sl1
 +[0-9]+: 0+24 +0 TLS +LOCAL +DEFAULT +7 sl2
 +[0-9]+: 0+28 +0 TLS +LOCAL +DEFAULT +7 sl3
 +[0-9]+: 0+2c +0 TLS +LOCAL +DEFAULT +7 sl4
 +[0-9]+: 0+30 +0 TLS +LOCAL +DEFAULT +7 sl5
 +[0-9]+: 0+34 +0 TLS +LOCAL +DEFAULT +7 sl6
 +[0-9]+: 0+38 +0 TLS +LOCAL +DEFAULT +7 sl7
 +[0-9]+: 0+3c +0 TLS +LOCAL +DEFAULT +7 sl8
 +[0-9]+: 0+80 +0 TLS +LOCAL +DEFAULT +8 bl1
 +[0-9]+: 0+84 +0 TLS +LOCAL +DEFAULT +8 bl2
 +[0-9]+: 0+88 +0 TLS +LOCAL +DEFAULT +8 bl3
 +[0-9]+: 0+8c +0 TLS +LOCAL +DEFAULT +8 bl4
 +[0-9]+: 0+90 +0 TLS +LOCAL +DEFAULT +8 bl5
 +[0-9]+: 0+94 +0 TLS +LOCAL +DEFAULT +8 bl6
 +[0-9]+: 0+98 +0 TLS +LOCAL +DEFAULT +8 bl7
 +[0-9]+: 0+9c +0 TLS +LOCAL +DEFAULT +8 bl8
 +[0-9]+: 0+1c +0 TLS +GLOBAL DEFAULT +7 sg8
 +[0-9]+: 0+7c +0 TLS +GLOBAL DEFAULT +8 bg8
 +[0-9]+: 0+74 +0 TLS +GLOBAL DEFAULT +8 bg6
 +[0-9]+: 0+ +0 TLS +GLOBAL DEFAULT +UND sG5
 +[0-9]+: 0+68 +0 TLS +GLOBAL DEFAULT +8 bg3
 +[0-9]+: 0+202208 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +[0-9]+: 0+08 +0 TLS +GLOBAL DEFAULT +7 sg3
 +[0-9]+: 0+48 +0 TLS +GLOBAL HIDDEN +7 sh3
 +[0-9]+: 0+ +0 TLS +GLOBAL DEFAULT +UND sG2
 +[0-9]+: 0+0c +0 TLS +GLOBAL DEFAULT +7 sg4
 +[0-9]+: 0+10 +0 TLS +GLOBAL DEFAULT +7 sg5
 +[0-9]+: [0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +ABS _PROCEDURE_LINKAGE_TABLE_
 +[0-9]+: 0+70 +0 TLS +GLOBAL DEFAULT +8 bg5
 +[0-9]+: 0+ +0 FUNC +GLOBAL DEFAULT +UND __tls_get_addr
 +[0-9]+: 0+58 +0 TLS +GLOBAL HIDDEN +7 sh7
 +[0-9]+: 0+5c +0 TLS +GLOBAL HIDDEN +7 sh8
 +[0-9]+: 0+ +0 TLS +GLOBAL DEFAULT +7 sg1
 +[0-9]+: 0+102000 +0 FUNC +GLOBAL DEFAULT +6 _start
 +[0-9]+: 0+4c +0 TLS +GLOBAL HIDDEN +7 sh4
 +[0-9]+: 0+78 +0 TLS +GLOBAL DEFAULT +8 bg7
 +[0-9]+: 0+50 +0 TLS +GLOBAL HIDDEN +7 sh5
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +[0-9]+: 0+ +0 TLS +GLOBAL DEFAULT +UND sG6
 +[0-9]+: 0+101008 +0 FUNC +GLOBAL DEFAULT +6 fn2
 +[0-9]+: 0+04 +0 TLS +GLOBAL DEFAULT +7 sg2
 +[0-9]+: 0+ +0 TLS +GLOBAL DEFAULT +UND sG1
 +[0-9]+: 0+40 +0 TLS +GLOBAL HIDDEN +7 sh1
 +[0-9]+: 0+14 +0 TLS +GLOBAL DEFAULT +7 sg6
 +[0-9]+: 0+18 +0 TLS +GLOBAL DEFAULT +7 sg7
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +[0-9]+: 0+202308 +0 OBJECT +GLOBAL +HIDDEN +10 _GLOBAL_OFFSET_TABLE_
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _end
 +[0-9]+: 0+44 +0 TLS +GLOBAL HIDDEN +7 sh2
 +[0-9]+: 0+54 +0 TLS +GLOBAL HIDDEN +7 sh6
 +[0-9]+: 0+64 +0 TLS +GLOBAL DEFAULT +8 bg2
 +[0-9]+: 0+60 +0 TLS +GLOBAL DEFAULT +8 bg1
 +[0-9]+: 0+6c +0 TLS +GLOBAL DEFAULT +8 bg4
