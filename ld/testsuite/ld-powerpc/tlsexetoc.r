#source: tlslib.s
#source: tlstoc.s
#as: -a64
#ld: -shared -melf64ppc
#readelf: -WSsrl
#target: powerpc64*-*-*

There are 21 section headers.*

Section Headers:
 +\[Nr\] Name +Type +Address +Off +Size +ES Flg Lk Inf Al
 +\[ 0\] +NULL +0+ 0+ 0+ 0+ +0 +0 +0
 +\[ 1\] .interp +PROGBITS +0+10000190 0+190 0+11 0+ +A +0 +0 +1
 +\[ 2\] .hash +HASH +0+100001a8 0+1a8 0+38 04 +A +3 +0 +8
 +\[ 3\] .dynsym +DYNSYM +0+100001e0 0+1e0 0+d8 18 +A +4 +1 +8
 +\[ 4\] .dynstr +STRTAB +0+100002b8 0+2b8 0+4d 0+ +A +0 +0 +1
 +\[ 5\] .rela.dyn +RELA +0+10000308 0+308 0+30 18 +A +3 +0 +8
 +\[ 6\] .text +PROGBITS +0+10000338 0+338 0+58 0+ +AX +0 +0 +4
 +\[ 7\] .data +PROGBITS +0+10010390 0+390 0+ 0+ +WA +0 +0 +1
 +\[ 8\] .branch_lt +PROGBITS +0+10010390 0+390 0+ 0+ +WA +0 +0 +8
 +\[ 9\] .tdata +PROGBITS +0+10010390 0+390 0+38 0+ WAT +0 +0 +8
 +\[10\] .tbss +NOBITS +0+100103c8 0+3c8 0+38 0+ WAT +0 +0 +8
 +\[11\] .dynamic +DYNAMIC +0+100103c8 0+3c8 0+100 10 +WA +4 +0 +8
 +\[12\] .ctors +PROGBITS +0+100104c8 0+520 0+ 0+ +W +0 +0 +1
 +\[13\] .dtors +PROGBITS +0+100104c8 0+520 0+ 0+ +W +0 +0 +1
 +\[14\] .got +PROGBITS +0+100104c8 0+4c8 0+8 08 +WA +0 +0 +8
 +\[15\] .toc +PROGBITS +0+100104d0 0+4d0 0+50 0+ +WA +0 +0 +1
 +\[16\] .sbss +PROGBITS +0+10010520 0+520 0+ 0+ +W +0 +0 +1
 +\[17\] .bss +NOBITS +0+10010520 0+520 0+ 0+ +WA +0 +0 +1
 +\[18\] .shstrtab +STRTAB +0+ 0+520 0+93 0+ +0 +0 +1
 +\[19\] .symtab +SYMTAB +0+ 0+af8 0+438 18 +20 +1d +8
 +\[20\] .strtab +STRTAB +0+ 0+f30 0+83 0+ +0 +0 +1
#...

Elf file type is EXEC \(Executable file\)
Entry point 0x10000338
There are 6 program headers.*

Program Headers:
 +Type +Offset +VirtAddr +PhysAddr +FileSiz +MemSiz +Flg Align
 +PHDR +0x0+40 0x0+10000040 0x0+10000040 0x0+150 0x0+150 R E 0x8
 +INTERP +0x0+190 0x0+10000190 0x0+10000190 0x0+11 0x0+11 R +0x1
 +\[Requesting program interpreter: .*\]
 +LOAD +0x0+ 0x0+10000000 0x0+10000000 0x0+390 0x0+390 R E 0x10000
 +LOAD +0x0+390 0x0+10010390 0x0+10010390 0x0+190 0x0+190 RW +0x10000
 +DYNAMIC +0x0+3c8 0x0+100103c8 0x0+100103c8 0x0+100 0x0+100 RW +0x8
 +TLS +0x0+390 0x0+10010390 0x0+10010390 0x0+38 0x0+70 R +0x8

 Section to Segment mapping:
 +Segment Sections\.\.\.
 +00 +
 +01 +\.interp 
 +02 +\.interp \.hash \.dynsym \.dynstr \.rela\.dyn \.text 
 +03 +\.tdata \.tbss \.dynamic \.got \.toc 
 +04 +\.tbss \.dynamic 
 +05 +\.tdata \.tbss 

Relocation section '\.rela\.dyn' at offset .* contains 2 entries:
 +Offset +Info +Type +Symbol's Value +Symbol's Name \+ Addend
0+100104d0 +0+200000049 R_PPC64_TPREL64 +0+ gd \+ 0
0+100104e0 +0+400000049 R_PPC64_TPREL64 +0+ ld \+ 0

Symbol table '\.dynsym' contains 9 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND 
 +1: 0+100103c8 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +2: 0+ +0 TLS +GLOBAL DEFAULT +UND gd
 +3: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND \.__tls_get_addr
 +4: 0+ +0 TLS +GLOBAL DEFAULT +UND ld
 +5: 0+10010520 +0 NOTYPE +GLOBAL DEFAULT +ABS __end
 +6: 0+10010520 +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +7: 0+10010520 +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +8: 0+10010520 +0 NOTYPE +GLOBAL DEFAULT +ABS _end

Symbol table '\.symtab' contains 45 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND 
 +1: 0+10000190 +0 SECTION LOCAL +DEFAULT +1 
 +2: 0+100001a8 +0 SECTION LOCAL +DEFAULT +2 
 +3: 0+100001e0 +0 SECTION LOCAL +DEFAULT +3 
 +4: 0+100002b8 +0 SECTION LOCAL +DEFAULT +4 
 +5: 0+10000308 +0 SECTION LOCAL +DEFAULT +5 
 +6: 0+10000338 +0 SECTION LOCAL +DEFAULT +6 
 +7: 0+10010390 +0 SECTION LOCAL +DEFAULT +7 
 +8: 0+10010390 +0 SECTION LOCAL +DEFAULT +8 
 +9: 0+10010390 +0 SECTION LOCAL +DEFAULT +9 
 +10: 0+100103c8 +0 SECTION LOCAL +DEFAULT +10 
 +11: 0+100103c8 +0 SECTION LOCAL +DEFAULT +11 
 +12: 0+100104c8 +0 SECTION LOCAL +DEFAULT +12 
 +13: 0+100104c8 +0 SECTION LOCAL +DEFAULT +13 
 +14: 0+100104c8 +0 SECTION LOCAL +DEFAULT +14 
 +15: 0+100104d0 +0 SECTION LOCAL +DEFAULT +15 
 +16: 0+10010520 +0 SECTION LOCAL +DEFAULT +16 
 +17: 0+10010520 +0 SECTION LOCAL +DEFAULT +17 
 +18: 0+ +0 SECTION LOCAL +DEFAULT +18 
 +19: 0+ +0 SECTION LOCAL +DEFAULT +19 
 +20: 0+ +0 SECTION LOCAL +DEFAULT +20 
 +21: 0+ +0 TLS +LOCAL +DEFAULT +9 gd4
 +22: 0+8 +0 TLS +LOCAL +DEFAULT +9 ld4
 +23: 0+10 +0 TLS +LOCAL +DEFAULT +9 ld5
 +24: 0+18 +0 TLS +LOCAL +DEFAULT +9 ld6
 +25: 0+20 +0 TLS +LOCAL +DEFAULT +9 ie4
 +26: 0+28 +0 TLS +LOCAL +DEFAULT +9 le4
 +27: 0+30 +0 TLS +LOCAL +DEFAULT +9 le5
 +28: 0+10010518 +0 NOTYPE +LOCAL +DEFAULT +15 \.Lie0
 +29: 0+100103c8 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +30: 0+ +0 TLS +GLOBAL DEFAULT +UND gd
 +31: 0+60 +0 TLS +GLOBAL DEFAULT +10 le0
 +32: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND \.__tls_get_addr
 +33: 0+40 +0 TLS +GLOBAL DEFAULT +10 ld0
 +34: 0+68 +0 TLS +GLOBAL DEFAULT +10 le1
 +35: 0+ +0 TLS +GLOBAL DEFAULT +UND ld
 +36: 0+10000338 +0 NOTYPE +GLOBAL DEFAULT +6 _start
 +37: 0+10010520 +0 NOTYPE +GLOBAL DEFAULT +ABS __end
 +38: 0+50 +0 TLS +GLOBAL DEFAULT +10 ld2
 +39: 0+48 +0 TLS +GLOBAL DEFAULT +10 ld1
 +40: 0+10010520 +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +41: 0+10010520 +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +42: 0+10010520 +0 NOTYPE +GLOBAL DEFAULT +ABS _end
 +43: 0+38 +0 TLS +GLOBAL DEFAULT +10 gd0
 +44: 0+58 +0 TLS +GLOBAL DEFAULT +10 ie0
