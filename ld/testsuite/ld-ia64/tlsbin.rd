#source: tlsbinpic.s
#source: tlsbin.s
#as:
#ld: -shared
#readelf: -WSsrl
#target: ia64-*-*

There are 19 section headers, starting at offset 0x[0-9a-f]+:

Section Headers:
  \[Nr\] Name +Type +Address +Off +Size +ES Flg Lk Inf Al
  \[ 0\] +NULL +0+ 0+ 0+ 00 +0 +0 +0
  \[ 1\] .interp +.*
  \[ 2\] .hash +.*
  \[ 3\] .dynsym +.*
  \[ 4\] .dynstr +.*
  \[ 5\] .rela.dyn +.*
  \[ 6\] .rela.IA_64.pltof +.*
  \[ 7\] .plt +.*
  \[ 8\] .text +PROGBITS +40+1000 0+1000 0+140 00 +AX +0 +0 4096
  \[ 9\] .IA_64.unwind_inf +.*
  \[10\] .IA_64.unwind +.*
  \[11\] .tdata +PROGBITS +60+1[0-9a-f]+ 0+1[0-9a-f]+ 0+60 00 WAT +0 +0 +4
  \[12\] .tbss +NOBITS +60+1[0-9a-f]+ 0+1[0-9a-f]+ 0+40 00 WAT +0 +0 +1
  \[13\] .dynamic +DYNAMIC +60+1[0-9a-f]+ 0+1[0-9a-f]+ 0+150 10 +WA +4 +0 +8
  \[14\] .got +PROGBITS +60+2000 0+2000 0+48 00 WAp +0 +0 +8
  \[15\] .IA_64.pltoff +.*
  \[16\] .shstrtab +.*
  \[17\] .symtab +.*
  \[18\] .strtab +.*
#...

Elf file type is EXEC \(Executable file\)
Entry point 0x40+10d0
There are 7 program headers, starting at offset [0-9]+

Program Headers:
  Type +Offset +VirtAddr +PhysAddr +FileSiz +MemSiz +Flg Align
  PHDR +0x0+40 0x40+40 0x40+40 0x0+188 0x0+188 R E 0x8
  INTERP +0x0+1c8 0x40+1c8 0x40+1c8 0x[0-9a-f]+ 0x[0-9a-f]+ R +0x1
.*Requesting program interpreter.*
  LOAD +0x0+ 0x40+ 0x40+ 0x0+1[0-9a-f]+ 0x0+1[0-9a-f]+ R E 0x10000
  LOAD +0x0+1[0-9a-f]+ 0x60+1[0-9a-f]+ 0x60+1[0-9a-f]+ 0x0+0[0-9a-f]+ 0x0+0[0-9a-f]+ RW +0x10000
  DYNAMIC +0x0+1[0-9a-f]+ 0x60+1[0-9a-f]+ 0x60+1[0-9a-f]+ 0x0+150 0x0+150 RW +0x8
  TLS +0x0+1[0-9a-f]+ 0x60+1[0-9a-f]+ 0x60+1[0-9a-f]+ 0x0+60 0x0+a0 R +0x4
  IA_64_UNWIND .* R +0x8
#...

Relocation section '.rela.dyn' at offset 0x[0-9a-f]+ contains 3 entries:
 +Offset +Info +Type +Symbol's Value  Symbol's Name \+ Addend
60+2018  0+200000097 R_IA64_TPREL64LSB +0+ sG2 \+ 0
60+2020  0+5000000a7 R_IA64_DTPMOD64LSB +0+ sG1 \+ 0
60+2028  0+5000000b7 R_IA64_DTPREL64LSB +0+ sG1 \+ 0

Relocation section '.rela.IA_64.pltoff' at offset 0x[0-9a-f]+ contains 1 entries:
 +Offset +Info +Type +Symbol's Value  Symbol's Name \+ Addend
60+2050  0+300000081 R_IA64_IPLTLSB +0+ __tls_get_addr \+ 0

Symbol table '.dynsym' contains 8 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +[0-9]+: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND *
 +[0-9]+: 60+1[0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +[0-9]+: 0+ +0 TLS +GLOBAL DEFAULT +UND sG2
 +[0-9]+: 0+ +16 FUNC +GLOBAL DEFAULT +UND __tls_get_addr
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +[0-9]+: 0+ +0 TLS +GLOBAL DEFAULT +UND sG1
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _end

Symbol table '.symtab' contains 69 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +[0-9]+: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +1 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +2 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +3 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +4 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +5 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +6 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +7 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +8 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +9 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +10 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +11 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +12 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +13 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +14 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +15 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +16 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +17 *
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +18 *
 +[0-9]+: 0+20 +0 TLS +LOCAL +DEFAULT +11 sl1
 +[0-9]+: 0+24 +0 TLS +LOCAL +DEFAULT +11 sl2
 +[0-9]+: 0+28 +0 TLS +LOCAL +DEFAULT +11 sl3
 +[0-9]+: 0+2c +0 TLS +LOCAL +DEFAULT +11 sl4
 +[0-9]+: 0+30 +0 TLS +LOCAL +DEFAULT +11 sl5
 +[0-9]+: 0+34 +0 TLS +LOCAL +DEFAULT +11 sl6
 +[0-9]+: 0+38 +0 TLS +LOCAL +DEFAULT +11 sl7
 +[0-9]+: 0+3c +0 TLS +LOCAL +DEFAULT +11 sl8
 +[0-9]+: 0+80 +0 TLS +LOCAL +DEFAULT +12 bl1
 +[0-9]+: 0+84 +0 TLS +LOCAL +DEFAULT +12 bl2
 +[0-9]+: 0+88 +0 TLS +LOCAL +DEFAULT +12 bl3
 +[0-9]+: 0+8c +0 TLS +LOCAL +DEFAULT +12 bl4
 +[0-9]+: 0+90 +0 TLS +LOCAL +DEFAULT +12 bl5
 +[0-9]+: 0+94 +0 TLS +LOCAL +DEFAULT +12 bl6
 +[0-9]+: 0+98 +0 TLS +LOCAL +DEFAULT +12 bl7
 +[0-9]+: 0+9c +0 TLS +LOCAL +DEFAULT +12 bl8
 +[0-9]+: 0+1c +0 TLS +GLOBAL DEFAULT +11 sg8
 +[0-9]+: 0+7c +0 TLS +GLOBAL DEFAULT +12 bg8
 +[0-9]+: 0+74 +0 TLS +GLOBAL DEFAULT +12 bg6
 +[0-9]+: 0+68 +0 TLS +GLOBAL DEFAULT +12 bg3
 +[0-9]+: 60+1[0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +[0-9]+: 0+8 +0 TLS +GLOBAL DEFAULT +11 sg3
 +[0-9]+: 0+48 +0 TLS +GLOBAL HIDDEN +11 sh3
 +[0-9]+: 0+ +0 TLS +GLOBAL DEFAULT +UND sG2
 +[0-9]+: 0+c +0 TLS +GLOBAL DEFAULT +11 sg4
 +[0-9]+: 0+10 +0 TLS +GLOBAL DEFAULT +11 sg5
 +[0-9]+: 0+70 +0 TLS +GLOBAL DEFAULT +12 bg5
 +[0-9]+: 0+ +16 FUNC +GLOBAL DEFAULT +UND __tls_get_addr
 +[0-9]+: 0+58 +0 TLS +GLOBAL HIDDEN +11 sh7
 +[0-9]+: 0+5c +0 TLS +GLOBAL HIDDEN +11 sh8
 +[0-9]+: 0+ +0 TLS +GLOBAL DEFAULT +11 sg1
 +[0-9]+: 40+10d0 +112 FUNC +GLOBAL DEFAULT +8 _start
 +[0-9]+: 0+4c +0 TLS +GLOBAL HIDDEN +11 sh4
 +[0-9]+: 0+78 +0 TLS +GLOBAL DEFAULT +12 bg7
 +[0-9]+: 0+50 +0 TLS +GLOBAL HIDDEN +11 sh5
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +[0-9]+: 40+1000 +208 FUNC +GLOBAL DEFAULT +8 fn2
 +[0-9]+: 0+4 +0 TLS +GLOBAL DEFAULT +11 sg2
 +[0-9]+: 0+ +0 TLS +GLOBAL DEFAULT +UND sG1
 +[0-9]+: 0+40 +0 TLS +GLOBAL HIDDEN +11 sh1
 +[0-9]+: 0+14 +0 TLS +GLOBAL DEFAULT +11 sg6
 +[0-9]+: 0+18 +0 TLS +GLOBAL DEFAULT +11 sg7
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +[0-9]+: [0-9a-f]+ +0 OBJECT +GLOBAL HIDDEN +14 _GLOBAL_OFFSET_TABLE_
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _end
 +[0-9]+: 0+44 +0 TLS +GLOBAL HIDDEN +11 sh2
 +[0-9]+: 0+54 +0 TLS +GLOBAL HIDDEN +11 sh6
 +[0-9]+: 0+64 +0 TLS +GLOBAL DEFAULT +12 bg2
 +[0-9]+: 0+60 +0 TLS +GLOBAL DEFAULT +12 bg1
 +[0-9]+: 0+6c +0 TLS +GLOBAL DEFAULT +12 bg4
