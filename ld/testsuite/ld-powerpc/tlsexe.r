#source: tls.s
#source: tlslib.s
#as: -a64
#ld: -shared -melf64ppc
#readelf: -WSsrl
#target: powerpc64*-*-*

There are 20 section headers.*

Section Headers:
 +\[Nr\] Name +Type +Address +Off +Size +ES Flg Lk Inf Al
 +\[ 0\] +NULL +0+ 0+ 0+ 0+ +0 +0 +0
 +\[ 1\] \.interp +PROGBITS +0+10000190 0+190 0+11 0+ +A +0 +0 +1
 +\[ 2\] \.hash +HASH +0+100001a8 0+1a8 0+3c 04 +A +3 +0 +8
 +\[ 3\] \.dynsym +DYNSYM +0+100001e8 0+1e8 0+f0 18 +A +4 +1 +8
 +\[ 4\] \.dynstr +STRTAB +0+100002d8 0+2d8 0+51 0+ +A +0 +0 +1
 +\[ 5\] \.rela\.dyn +RELA +0+10000330 0+330 0+48 18 +A +3 +0 +8
 +\[ 6\] \.text +PROGBITS +0+10000378 0+378 0+98 0+ +AX +0 +0 +4
 +\[ 7\] \.data +PROGBITS +0+10010410 0+410 0+ 0+ +WA +0 +0 +1
 +\[ 8\] \.branch_lt +PROGBITS +0+10010410 0+410 0+ 0+ +WA +0 +0 +8
 +\[ 9\] \.tdata +PROGBITS +0+10010410 0+410 0+38 0+ WAT +0 +0 +8
 +\[10\] \.tbss +NOBITS +0+10010448 0+448 0+38 0+ WAT +0 +0 +8
 +\[11\] \.dynamic +DYNAMIC +0+10010448 0+448 0+100 10 +WA +4 +0 +8
 +\[12\] \.ctors +PROGBITS +0+10010548 0+570 0+ 0+ +W +0 +0 +1
 +\[13\] \.dtors +PROGBITS +0+10010548 0+570 0+ 0+ +W +0 +0 +1
 +\[14\] \.got +PROGBITS +0+10010548 0+548 0+28 08 +WA +0 +0 +8
 +\[15\] \.sbss +PROGBITS +0+10010570 0+570 0+ 0+ +W +0 +0 +1
 +\[16\] \.bss +NOBITS +0+10010570 0+570 0+ 0+ +WA +0 +0 +1
 +\[17\] \.shstrtab +STRTAB +0+ 0+570 0+8e 0+ +0 +0 +1
 +\[18\] \.symtab +SYMTAB +0+ 0+b00 0+408 18 +19 +1b +8
 +\[19\] \.strtab +STRTAB +0+ 0+f08 0+7d 0+ +0 +0 +1
#...

Elf file type is EXEC \(Executable file\)
Entry point 0x10000378
There are 6 program headers.*

Program Headers:
 +Type +Offset +VirtAddr +PhysAddr +FileSiz +MemSiz +Flg Align
 +PHDR +0x0+40 0x0+10000040 0x0+10000040 0x0+150 0x0+150 R E 0x8
 +INTERP +0x0+190 0x0+10000190 0x0+10000190 0x0+11 0x0+11 R +0x1
 +\[Requesting program interpreter: .*\]
 +LOAD +0x0+ 0x0+10000000 0x0+10000000 0x0+410 0x0+410 R E 0x10000
 +LOAD +0x0+410 0x0+10010410 0x0+10010410 0x0+160 0x0+160 RW +0x10000
 +DYNAMIC +0x0+448 0x0+10010448 0x0+10010448 0x0+100 0x0+100 RW +0x8
 +TLS +0x0+410 0x0+10010410 0x0+10010410 0x0+38 0x0+70 R +0x8

 Section to Segment mapping:
 +Segment Sections\.\.\.
 +00 +
 +01 +\.interp 
 +02 +\.interp \.hash \.dynsym \.dynstr \.rela\.dyn \.text 
 +03 +\.tdata \.tbss \.dynamic \.got 
 +04 +\.tbss \.dynamic 
 +05 +\.tdata \.tbss 

Relocation section '\.rela\.dyn' at offset .* contains 3 entries:
 +Offset +Info +Type +Symbol's Value +Symbol's Name \+ Addend
0+10010558 +0+200000049 R_PPC64_TPREL64 +0+ gd \+ 0
0+10010560 +0+400000049 R_PPC64_TPREL64 +0+ ld \+ 0
0+10010568 +0+60000004e R_PPC64_DTPREL64 +0+50 ld2 \+ 0

Symbol table '\.dynsym' contains 10 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND 
 +1: 0+10010448 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +2: 0+ +0 TLS +GLOBAL DEFAULT +UND gd
 +3: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND \.__tls_get_addr
 +4: 0+ +0 TLS +GLOBAL DEFAULT +UND ld
 +5: 0+10010570 +0 NOTYPE +GLOBAL DEFAULT +ABS __end
 +6: 0+50 +0 TLS +GLOBAL DEFAULT +10 ld2
 +7: 0+10010570 +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +8: 0+10010570 +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +9: 0+10010570 +0 NOTYPE +GLOBAL DEFAULT +ABS _end

Symbol table '\.symtab' contains 43 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND 
 +1: 0+10000190 +0 SECTION LOCAL +DEFAULT +1 
 +2: 0+100001a8 +0 SECTION LOCAL +DEFAULT +2 
 +3: 0+100001e8 +0 SECTION LOCAL +DEFAULT +3 
 +4: 0+100002d8 +0 SECTION LOCAL +DEFAULT +4 
 +5: 0+10000330 +0 SECTION LOCAL +DEFAULT +5 
 +6: 0+10000378 +0 SECTION LOCAL +DEFAULT +6 
 +7: 0+10010410 +0 SECTION LOCAL +DEFAULT +7 
 +8: 0+10010410 +0 SECTION LOCAL +DEFAULT +8 
 +9: 0+10010410 +0 SECTION LOCAL +DEFAULT +9 
 +10: 0+10010448 +0 SECTION LOCAL +DEFAULT +10 
 +11: 0+10010448 +0 SECTION LOCAL +DEFAULT +11 
 +12: 0+10010548 +0 SECTION LOCAL +DEFAULT +12 
 +13: 0+10010548 +0 SECTION LOCAL +DEFAULT +13 
 +14: 0+10010548 +0 SECTION LOCAL +DEFAULT +14 
 +15: 0+10010570 +0 SECTION LOCAL +DEFAULT +15 
 +16: 0+10010570 +0 SECTION LOCAL +DEFAULT +16 
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
 +27: 0+10010448 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +28: 0+ +0 TLS +GLOBAL DEFAULT +UND gd
 +29: 0+60 +0 TLS +GLOBAL DEFAULT +10 le0
 +30: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND \.__tls_get_addr
 +31: 0+40 +0 TLS +GLOBAL DEFAULT +10 ld0
 +32: 0+68 +0 TLS +GLOBAL DEFAULT +10 le1
 +33: 0+ +0 TLS +GLOBAL DEFAULT +UND ld
 +34: 0+10000378 +0 NOTYPE +GLOBAL DEFAULT +6 _start
 +35: 0+10010570 +0 NOTYPE +GLOBAL DEFAULT +ABS __end
 +36: 0+50 +0 TLS +GLOBAL DEFAULT +10 ld2
 +37: 0+48 +0 TLS +GLOBAL DEFAULT +10 ld1
 +38: 0+10010570 +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +39: 0+10010570 +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +40: 0+10010570 +0 NOTYPE +GLOBAL DEFAULT +ABS _end
 +41: 0+38 +0 TLS +GLOBAL DEFAULT +10 gd0
 +42: 0+58 +0 TLS +GLOBAL DEFAULT +10 ie0
