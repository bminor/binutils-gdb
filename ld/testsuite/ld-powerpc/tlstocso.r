#source: tlstoc.s
#as: -a64
#ld: -shared -melf64ppc
#readelf: -WSsrl
#target: powerpc64*-*-*

There are 20 section headers.*

Section Headers:
 +\[Nr\] Name +Type +Address +Off +Size +ES Flg Lk Inf Al
 +\[ 0\] +NULL +0+ 0+ 0+ 0+ +0 +0 +0
 +\[ 1\] \.hash +HASH +0+120 0+120 0+cc 04 +A +2 +0 +8
 +\[ 2\] \.dynsym +DYNSYM +0+1f0 0+1f0 0+300 18 +A +3 +11 +8
 +\[ 3\] \.dynstr +STRTAB +0+4f0 0+4f0 0+53 0+ +A +0 +0 +1
 +\[ 4\] \.rela\.dyn +RELA +0+548 0+548 0+108 18 +A +2 +0 +8
 +\[ 5\] \.rela\.plt +RELA +0+650 0+650 0+18 18 +A +2 +f +8
 +\[ 6\] \.text +PROGBITS +0+668 0+668 0+bc 0+ +AX +0 +0 +4
 +\[ 7\] \.data +PROGBITS +0+10728 0+728 0+ 0+ +WA +0 +0 +1
 +\[ 8\] \.branch_lt +PROGBITS +0+10728 0+728 0+ 0+ +WA +0 +0 +8
 +\[ 9\] \.tdata +PROGBITS +0+10728 0+728 0+38 0+ WAT +0 +0 +8
 +\[10\] \.tbss +NOBITS +0+10760 0+760 0+38 0+ WAT +0 +0 +8
 +\[11\] \.dynamic +DYNAMIC +0+10760 0+760 0+150 10 +WA +3 +0 +8
 +\[12\] \.got +PROGBITS +0+108b0 0+8b0 0+8 08 +WA +0 +0 +8
 +\[13\] \.toc +PROGBITS +0+108b8 0+8b8 0+50 0+ +WA +0 +0 +1
 +\[14\] \.sbss +PROGBITS +0+10908 0+908 0+ 0+ +W +0 +0 +1
 +\[15\] \.plt +NOBITS +0+10908 0+908 0+30 18 +WA +0 +0 +8
 +\[16\] \.bss +NOBITS +0+10938 0+908 0+ 0+ +WA +0 +0 +1
 +\[17\] \.shstrtab +STRTAB +0+ 0+908 0+87 0+ +0 +0 +1
 +\[18\] \.symtab +SYMTAB +0+ 0+e90 0+420 18 +19 +1d +8
 +\[19\] \.strtab +STRTAB +0+ 0+12b0 0+8c 0+ +0 +0 +1
#...

Elf file type is DYN \(Shared object file\)
Entry point 0x684
There are 4 program headers.*

Program Headers:
 +Type +Offset +VirtAddr +PhysAddr +FileSiz +MemSiz +Flg Align
 +LOAD +0x0+ 0x0+ 0x0+ 0x0+724 0x0+724 R E 0x10000
 +LOAD +0x0+728 0x0+10728 0x0+10728 0x0+1e0 0x0+210 RW +0x10000
 +DYNAMIC +0x0+760 0x0+10760 0x0+10760 0x0+150 0x0+150 RW +0x8
 +TLS +0x0+728 0x0+10728 0x0+10728 0x0+38 0x0+70 R +0x8

 Section to Segment mapping:
 +Segment Sections\.\.\.
 +0+ +\.hash \.dynsym \.dynstr \.rela\.dyn \.rela\.plt \.text 
 +01 +\.tdata \.tbss \.dynamic \.got \.toc \.plt 
 +02 +\.tbss \.dynamic 
 +03 +\.tdata \.tbss 

Relocation section '\.rela\.dyn' at offset .* contains 11 entries:
 +Offset +Info +Type +Symbol's Value +Symbol's Name \+ Addend
0+6c2 +0+d00000048 R_PPC64_TPREL16_HA +0+108b8 \.toc \+ 40
0+6d2 +0+1300000045 R_PPC64_TPREL16 +0+60 le0 \+ 0
0+6d6 +0+1600000048 R_PPC64_TPREL16_HA +0+68 le1 \+ 0
0+108b8 +0+1200000044 R_PPC64_DTPMOD64 +0+ gd \+ 0
0+108c0 +0+120000004e R_PPC64_DTPREL64 +0+ gd \+ 0
0+108c8 +0+1700000044 R_PPC64_DTPMOD64 +0+ ld \+ 0
0+108d8 +0+1e00000044 R_PPC64_DTPMOD64 +0+38 gd0 \+ 0
0+108e0 +0+1e0000004e R_PPC64_DTPREL64 +0+38 gd0 \+ 0
0+108e8 +0+1500000044 R_PPC64_DTPMOD64 +0+40 ld0 \+ 0
0+108f8 +0+190000004e R_PPC64_DTPREL64 +0+50 ld2 \+ 0
0+10900 +0+1f00000049 R_PPC64_TPREL64 +0+58 ie0 \+ 0

Relocation section '\.rela\.plt' at offset .* contains 1 entries:
 +Offset +Info +Type +Symbol's Value +Symbol's Name \+ Addend
0+10920 +0+1400000015 R_PPC64_JMP_SLOT +0+ __tls_get_addr \+ 0

Symbol table '\.dynsym' contains 32 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND 
 +1: 0+120 +0 SECTION LOCAL +DEFAULT +1 
 +2: 0+1f0 +0 SECTION LOCAL +DEFAULT +2 
 +3: 0+4f0 +0 SECTION LOCAL +DEFAULT +3 
 +4: 0+548 +0 SECTION LOCAL +DEFAULT +4 
 +5: 0+650 +0 SECTION LOCAL +DEFAULT +5 
 +6: 0+668 +0 SECTION LOCAL +DEFAULT +6 
 +7: 0+10728 +0 SECTION LOCAL +DEFAULT +7 
 +8: 0+10728 +0 SECTION LOCAL +DEFAULT +8 
 +9: 0+10728 +0 SECTION LOCAL +DEFAULT +9 
 +10: 0+10760 +0 SECTION LOCAL +DEFAULT +10 
 +11: 0+10760 +0 SECTION LOCAL +DEFAULT +11 
 +12: 0+108b0 +0 SECTION LOCAL +DEFAULT +12 
 +13: 0+108b8 +0 SECTION LOCAL +DEFAULT +13 
 +14: 0+10908 +0 SECTION LOCAL +DEFAULT +14 
 +15: 0+10908 +0 SECTION LOCAL +DEFAULT +15 
 +16: 0+10938 +0 SECTION LOCAL +DEFAULT +16 
 +17: 0+10760 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +18: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND gd
 +19: 0+60 +0 TLS +GLOBAL DEFAULT +10 le0
 +20: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND __tls_get_addr
 +21: 0+40 +0 TLS +GLOBAL DEFAULT +10 ld0
 +22: 0+68 +0 TLS +GLOBAL DEFAULT +10 le1
 +23: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND ld
 +24: 0+684 +0 NOTYPE +GLOBAL DEFAULT +6 _start
 +25: 0+50 +0 TLS +GLOBAL DEFAULT +10 ld2
 +26: 0+48 +0 TLS +GLOBAL DEFAULT +10 ld1
 +27: 0+10908 +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +28: 0+10908 +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +29: 0+10938 +0 NOTYPE +GLOBAL DEFAULT +ABS _end
 +30: 0+38 +0 TLS +GLOBAL DEFAULT +10 gd0
 +31: 0+58 +0 TLS +GLOBAL DEFAULT +10 ie0

Symbol table '\.symtab' contains 44 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND 
 +1: 0+120 +0 SECTION LOCAL +DEFAULT +1 
 +2: 0+1f0 +0 SECTION LOCAL +DEFAULT +2 
 +3: 0+4f0 +0 SECTION LOCAL +DEFAULT +3 
 +4: 0+548 +0 SECTION LOCAL +DEFAULT +4 
 +5: 0+650 +0 SECTION LOCAL +DEFAULT +5 
 +6: 0+668 +0 SECTION LOCAL +DEFAULT +6 
 +7: 0+10728 +0 SECTION LOCAL +DEFAULT +7 
 +8: 0+10728 +0 SECTION LOCAL +DEFAULT +8 
 +9: 0+10728 +0 SECTION LOCAL +DEFAULT +9 
 +10: 0+10760 +0 SECTION LOCAL +DEFAULT +10 
 +11: 0+10760 +0 SECTION LOCAL +DEFAULT +11 
 +12: 0+108b0 +0 SECTION LOCAL +DEFAULT +12 
 +13: 0+108b8 +0 SECTION LOCAL +DEFAULT +13 
 +14: 0+10908 +0 SECTION LOCAL +DEFAULT +14 
 +15: 0+10908 +0 SECTION LOCAL +DEFAULT +15 
 +16: 0+10938 +0 SECTION LOCAL +DEFAULT +16 
 +17: 0+ +0 SECTION LOCAL +DEFAULT +17 
 +18: 0+ +0 SECTION LOCAL +DEFAULT +18 
 +19: 0+ +0 SECTION LOCAL +DEFAULT +19 
 +20: 0+ +0 TLS +LOCAL +DEFAULT +9 gd4
 +21: 0+8 +0 TLS +LOCAL +DEFAULT +9 ld4
 +22: 0+10 +0 TLS +LOCAL +DEFAULT +9 ld5
 +23: 0+18 +0 TLS +LOCAL +DEFAULT +9 ld6
 +24: 0+20 +0 TLS +LOCAL +DEFAULT +9 ie4
 +25: 0+28 +0 TLS +LOCAL +DEFAULT +9 le4
 +26: 0+30 +0 TLS +LOCAL +DEFAULT +9 le5
 +27: 0+10900 +0 NOTYPE +LOCAL +DEFAULT +13 \.Lie0
 +28: 0+668 +0 NOTYPE +LOCAL +DEFAULT +6 \.__tls_get_addr
 +29: 0+10760 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +30: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND gd
 +31: 0+60 +0 TLS +GLOBAL DEFAULT +10 le0
 +32: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND __tls_get_addr
 +33: 0+40 +0 TLS +GLOBAL DEFAULT +10 ld0
 +34: 0+68 +0 TLS +GLOBAL DEFAULT +10 le1
 +35: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND ld
 +36: 0+684 +0 NOTYPE +GLOBAL DEFAULT +6 _start
 +37: 0+50 +0 TLS +GLOBAL DEFAULT +10 ld2
 +38: 0+48 +0 TLS +GLOBAL DEFAULT +10 ld1
 +39: 0+10908 +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +40: 0+10908 +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +41: 0+10938 +0 NOTYPE +GLOBAL DEFAULT +ABS _end
 +42: 0+38 +0 TLS +GLOBAL DEFAULT +10 gd0
 +43: 0+58 +0 TLS +GLOBAL DEFAULT +10 ie0
