#source: tls32.s
#source: tlslib32.s
#as: -a32
#ld: -melf32ppc
#readelf: -WSsrl
#target: powerpc*-*-*

There are 18 section headers.*

Section Headers:
 +\[Nr\] Name +Type +Addr +Off +Size +ES Flg Lk Inf Al
 +\[ 0\] +NULL +00000000 000000 000000 00 +0 +0 +0
 +\[ 1\] \.interp +.*
 +\[ 2\] \.hash +.*
 +\[ 3\] \.dynsym +.*
 +\[ 4\] \.dynstr +.*
 +\[ 5\] \.rela\.dyn +.*
 +\[ 6\] \.rela\.plt +.*
 +\[ 7\] \.text +PROGBITS +01800244 000244 000070 00 +AX +0 +0 +1
 +\[ 8\] \.sdata2 +PROGBITS +.*
 +\[ 9\] \.tdata +PROGBITS +018102b4 0002b4 00001c 00 WAT +0 +0 +4
 +\[10\] \.tbss +NOBITS +018102d0 0002d0 00001c 00 WAT +0 +0 +4
 +\[11\] \.dynamic +DYNAMIC +018102d0 0002d0 0000a0 08 +WA +4 +0 +4
 +\[12\] \.got +PROGBITS +01810370 000370 00001c 04 WAX +0 +0 +4
 +\[13\] \.sdata +PROGBITS +0181038c 00038c 000000 00 +WA +0 +0 +4
 +\[14\] \.plt +NOBITS +.*
 +\[15\] \.shstrtab +STRTAB +.*
 +\[16\] \.symtab +SYMTAB +.*
 +\[17\] \.strtab +STRTAB +.*
#...

Elf file type is EXEC \(Executable file\)
Entry point 0x1800244
There are 6 program headers, starting at offset 52

Program Headers:
 +Type +Offset +VirtAddr +PhysAddr +FileSiz MemSiz +Flg Align
 +PHDR +0x000034 0x01800034 0x01800034 0x000c0 0x000c0 R E 0x4
 +INTERP +0x0000f4 0x018000f4 0x018000f4 0x00011 0x00011 R +0x1
 +\[Requesting program interpreter: .*\]
 +LOAD +0x000000 0x01800000 0x01800000 0x002b4 0x002b4 R E 0x10000
 +LOAD +0x0002b4 0x018102b4 0x018102b4 0x000d8 0x0012c RWE 0x10000
 +DYNAMIC +0x0002d0 0x018102d0 0x018102d0 0x000a0 0x000a0 RW +0x4
 +TLS +0x0002b4 0x018102b4 0x018102b4 0x0001c 0x00038 R +0x4

 Section to Segment mapping:
 +Segment Sections\.\.\.
 +00 +
 +01 +\.interp 
 +02 +\.interp \.hash \.dynsym \.dynstr \.rela\.dyn \.rela\.plt \.text 
 +03 +\.tdata \.dynamic \.got \.plt 
 +04 +\.dynamic 
 +05 +\.tdata \.tbss 

Relocation section '\.rela\.dyn' at offset 0x220 contains 2 entries:
 Offset +Info +Type +Sym\. Value +Symbol's Name \+ Addend
01810380 +00000249 R_PPC_TPREL32 +00000000 +gd \+ 0
01810384 +00000444 R_PPC_DTPMOD32 +00000000 +ld \+ 0

Relocation section '\.rela\.plt' at offset 0x238 contains 1 entries:
 Offset +Info +Type +Sym\. Value +Symbol's Name \+ Addend
018103d4 +00000315 R_PPC_JMP_SLOT +018103d4 +__tls_get_addr \+ 0

Symbol table '\.dynsym' contains 9 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 00000000 +0 NOTYPE +LOCAL +DEFAULT +UND 
 +1: 018102d0 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +2: 00000000 +0 TLS +GLOBAL DEFAULT +UND gd
 +3: 018103d4 +0 FUNC +GLOBAL DEFAULT +UND __tls_get_addr
 +4: 00000000 +0 TLS +GLOBAL DEFAULT +UND ld
 +5: 018103e0 +0 NOTYPE +GLOBAL DEFAULT +ABS __end
 +6: 0181038c +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +7: 0181038c +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +8: 018103e0 +0 NOTYPE +GLOBAL DEFAULT +ABS _end

Symbol table '\.symtab' contains 44 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +[0-9]+: 00000000 +0 NOTYPE +LOCAL +DEFAULT +UND 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +1 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +2 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +3 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +4 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +5 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +6 
 +[0-9]+: 01800244 +0 SECTION LOCAL +DEFAULT +7 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +8 
 +[0-9]+: 018102b4 +0 SECTION LOCAL +DEFAULT +9 
 +[0-9]+: 018102d0 +0 SECTION LOCAL +DEFAULT +10 
 +[0-9]+: 018102d0 +0 SECTION LOCAL +DEFAULT +11 
 +[0-9]+: 01810370 +0 SECTION LOCAL +DEFAULT +12 
 +[0-9]+: 0181038c +0 SECTION LOCAL +DEFAULT +13 
 +[0-9]+: 0181038c +0 SECTION LOCAL +DEFAULT +14 
 +[0-9]+: 00000000 +0 SECTION LOCAL +DEFAULT +15 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +16 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +17 
 +[0-9]+: 00000000 +0 TLS +LOCAL +DEFAULT +9 gd4
 +[0-9]+: 00000004 +0 TLS +LOCAL +DEFAULT +9 ld4
 +[0-9]+: 00000008 +0 TLS +LOCAL +DEFAULT +9 ld5
 +[0-9]+: 0000000c +0 TLS +LOCAL +DEFAULT +9 ld6
 +[0-9]+: 00000010 +0 TLS +LOCAL +DEFAULT +9 ie4
 +[0-9]+: 00000014 +0 TLS +LOCAL +DEFAULT +9 le4
 +[0-9]+: 00000018 +0 TLS +LOCAL +DEFAULT +9 le5
 +[0-9]+: 018102d0 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +[0-9]+: 00000000 +0 TLS +GLOBAL DEFAULT +UND gd
 +[0-9]+: 00000030 +0 TLS +GLOBAL DEFAULT +10 le0
 +[0-9]+: [0-9a-f]+ +0 FUNC +GLOBAL DEFAULT +UND __tls_get_addr
 +[0-9]+: 00000020 +0 TLS +GLOBAL DEFAULT +10 ld0
 +[0-9]+: 00000034 +0 TLS +GLOBAL DEFAULT +10 le1
 +[0-9]+: 00000000 +0 TLS +GLOBAL DEFAULT +UND ld
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +7 _start
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __end
 +[0-9]+: [0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +13 _SDA_BASE_
 +[0-9]+: 00000028 +0 TLS +GLOBAL DEFAULT +10 ld2
 +[0-9]+: 00000024 +0 TLS +GLOBAL DEFAULT +10 ld1
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +[0-9]+: 01810374 +0 OBJECT +GLOBAL +HIDDEN +12 _GLOBAL_OFFSET_TABLE_
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _end
 +[0-9]+: 0000001c +0 TLS +GLOBAL DEFAULT +10 gd0
 +[0-9]+: 0000002c +0 TLS +GLOBAL DEFAULT +10 ie0
 +[0-9]+: [0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +8 _SDA2_BASE_
