#source: tls32.s
#as: -a32
#ld: -shared -melf32ppc
#readelf: -WSsrl
#target: powerpc*-*-*

There are 20 section headers.*

Section Headers:
 +\[Nr\] Name +Type +Addr +Off +Size +ES Flg Lk Inf Al
 +\[ 0\] +NULL +0+ 0+ 0+ 0+ +0 +0 +0
 +\[ 1\] \.hash +.*
 +\[ 2\] \.dynsym +.*
 +\[ 3\] \.dynstr +.*
 +\[ 4\] \.rela\.dyn +.*
 +\[ 5\] \.rela\.plt +.*
 +\[ 6\] \.text +PROGBITS +0+498 0+498 0+70 0+ +AX +0 +0 +1
 +\[ 7\] \.tdata +PROGBITS +0+10508 0+508 0+1c 0+ WAT +0 +0 +4
 +\[ 8\] \.tbss +NOBITS +0+10524 0+524 0+1c 0+ WAT +0 +0 +4
 +\[ 9\] \.dynamic +DYNAMIC +0+10524 0+524 0+a0 08 +WA +3 +0 +4
 +\[10\] \.data +PROGBITS +0+105c4 0+5c4 0+ 0+ +WA +0 +0 +1
 +\[11\] \.got +PROGBITS +0+105c4 0+5c4 0+34 04 WAX +0 +0 +4
 +\[12\] \.sdata2 +.*
 +\[13\] \.sdata +.*
 +\[14\] \.sbss +.*
 +\[15\] \.plt +.*
 +\[16\] \.bss +.*
 +\[17\] \.shstrtab +.*
 +\[18\] \.symtab +.*
 +\[19\] \.strtab +.*
#...

Elf file type is DYN \(Shared object file\)
Entry point 0x[0-9a-f]+
There are 4 program headers.*

Program Headers:
 +Type +Offset +VirtAddr +PhysAddr +FileSiz MemSiz +Flg Align
 +LOAD +0x0+ 0x0+ 0x0+ 0x0+508 0x0+508 R E 0x10000
 +LOAD +0x0+508 0x0+10508 0x0+10508 0x0+f0 0x0+144 RWE 0x10000
 +DYNAMIC +0x0+524 0x0+10524 0x0+10524 0x0+a0 0x0+a0 RW +0x4
 +TLS +0x0+508 0x0+10508 0x0+10508 0x0+1c 0x0+38 R +0x4

 Section to Segment mapping:
 +Segment Sections\.\.\.
 +0+ +\.hash \.dynsym \.dynstr \.rela\.dyn \.rela\.plt \.text 
 +01 +\.tdata \.tbss \.dynamic \.got \.plt 
 +02 +\.tbss \.dynamic 
 +03 +\.tdata \.tbss 

Relocation section '\.rela\.dyn' at offset 0x[0-9a-f]+ contains 18 entries:
 Offset +Info +Type +Sym\. Value +Symbol's Name \+ Addend
0+49c +0+c0a R_PPC_REL24 +0+ +__tls_get_addr \+ 0
0+4a4 +0+c0a R_PPC_REL24 +0+ +__tls_get_addr \+ 0
0+4dc +0+c0a R_PPC_REL24 +0+ +__tls_get_addr \+ 0
0+4e4 +0+c0a R_PPC_REL24 +0+ +__tls_get_addr \+ 0
0+4ce +0+b45 R_PPC_TPREL16 +0+30 +le0 \+ 0
0+4d2 +0+e48 R_PPC_TPREL16_HA +0+34 +le1 \+ 0
0+4d6 +0+e46 R_PPC_TPREL16_LO +0+34 +le1 \+ 0
0+4fe +0+245 R_PPC_TPREL16 +0+10508 +\.tdata \+ 1051c
0+502 +0+248 R_PPC_TPREL16_HA +0+10508 +\.tdata \+ 10520
0+506 +0+246 R_PPC_TPREL16_LO +0+10508 +\.tdata \+ 10520
0+105d4 +0+44 R_PPC_DTPMOD32 +0+
0+105dc +0+44 R_PPC_DTPMOD32 +0+
0+105e0 +0+4e R_PPC_DTPREL32 +0+
0+105e4 +0+a44 R_PPC_DTPMOD32 +0+ +gd \+ 0
0+105e8 +0+a4e R_PPC_DTPREL32 +0+ +gd \+ 0
0+105ec +0+1944 R_PPC_DTPMOD32 +0+1c +gd0 \+ 0
0+105f0 +0+194e R_PPC_DTPREL32 +0+1c +gd0 \+ 0
0+105f4 +0+1a49 R_PPC_TPREL32 +0+2c +ie0 \+ 0

Relocation section '\.rela\.plt' at offset 0x[0-9a-f]+ contains 1 entries:
 Offset +Info +Type +Sym\. Value +Symbol's Name \+ Addend
0+10640 +0+c15 R_PPC_JMP_SLOT +0+ +__tls_get_addr \+ 0

Symbol table '\.dynsym' contains 28 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +[0-9]+: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND 
 +[0-9]+: 0+498 +0 SECTION LOCAL +DEFAULT +6 
 +[0-9]+: 0+10508 +0 SECTION LOCAL +DEFAULT +7 
 +[0-9]+: 0+10524 +0 SECTION LOCAL +DEFAULT +8 
 +[0-9]+: 0+105c4 +0 SECTION LOCAL +DEFAULT +10 
 +[0-9]+: 0+105f8 +0 SECTION LOCAL +DEFAULT +12 
 +[0-9]+: 0+105f8 +0 SECTION LOCAL +DEFAULT +13 
 +[0-9]+: 0+105f8 +0 SECTION LOCAL +DEFAULT +14 
 +[0-9]+: 0+1064c +0 SECTION LOCAL +DEFAULT +16 
 +[0-9]+: 0+10524 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +[0-9]+: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND gd
 +[0-9]+: 0+30 +0 TLS +GLOBAL DEFAULT +8 le0
 +[0-9]+: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND __tls_get_addr
 +[0-9]+: 0+20 +0 TLS +GLOBAL DEFAULT +8 ld0
 +[0-9]+: 0+34 +0 TLS +GLOBAL DEFAULT +8 le1
 +[0-9]+: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND ld
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +6 _start
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __end
 +[0-9]+: [0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +13 _SDA_BASE_
 +[0-9]+: 0+28 +0 TLS +GLOBAL DEFAULT +8 ld2
 +[0-9]+: 0+24 +0 TLS +GLOBAL DEFAULT +8 ld1
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +[0-9]+: 0+105c8 +0 OBJECT +GLOBAL DEFAULT +ABS _GLOBAL_OFFSET_TABLE_
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _end
 +[0-9]+: 0+1c +0 TLS +GLOBAL DEFAULT +8 gd0
 +[0-9]+: 0+2c +0 TLS +GLOBAL DEFAULT +8 ie0
 +[0-9]+: [0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +12 _SDA2_BASE_

Symbol table '\.symtab' contains 46 entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +[0-9]+: 0+ +0 NOTYPE +LOCAL +DEFAULT +UND 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +1 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +2 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +3 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +4 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +5 
 +[0-9]+: 0+498 +0 SECTION LOCAL +DEFAULT +6 
 +[0-9]+: 0+10508 +0 SECTION LOCAL +DEFAULT +7 
 +[0-9]+: 0+10524 +0 SECTION LOCAL +DEFAULT +8 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +9 
 +[0-9]+: 0+105c4 +0 SECTION LOCAL +DEFAULT +10 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +11 
 +[0-9]+: 0+105f8 +0 SECTION LOCAL +DEFAULT +12 
 +[0-9]+: 0+105f8 +0 SECTION LOCAL +DEFAULT +13 
 +[0-9]+: 0+105f8 +0 SECTION LOCAL +DEFAULT +14 
 +[0-9]+: [0-9a-f]+ +0 SECTION LOCAL +DEFAULT +15 
 +[0-9]+: 0+1064c +0 SECTION LOCAL +DEFAULT +16 
 +[0-9]+: 0+ +0 SECTION LOCAL +DEFAULT +17 
 +[0-9]+: 0+ +0 SECTION LOCAL +DEFAULT +18 
 +[0-9]+: 0+ +0 SECTION LOCAL +DEFAULT +19 
 +[0-9]+: 0+ +0 TLS +LOCAL +DEFAULT +7 gd4
 +[0-9]+: 0+4 +0 TLS +LOCAL +DEFAULT +7 ld4
 +[0-9]+: 0+8 +0 TLS +LOCAL +DEFAULT +7 ld5
 +[0-9]+: 0+c +0 TLS +LOCAL +DEFAULT +7 ld6
 +[0-9]+: 0+10 +0 TLS +LOCAL +DEFAULT +7 ie4
 +[0-9]+: 0+14 +0 TLS +LOCAL +DEFAULT +7 le4
 +[0-9]+: 0+18 +0 TLS +LOCAL +DEFAULT +7 le5
 +[0-9]+: 0+10524 +0 OBJECT +GLOBAL DEFAULT +ABS _DYNAMIC
 +[0-9]+: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND gd
 +[0-9]+: 0+30 +0 TLS +GLOBAL DEFAULT +8 le0
 +[0-9]+: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND __tls_get_addr
 +[0-9]+: 0+20 +0 TLS +GLOBAL DEFAULT +8 ld0
 +[0-9]+: 0+34 +0 TLS +GLOBAL DEFAULT +8 le1
 +[0-9]+: 0+ +0 NOTYPE +GLOBAL DEFAULT +UND ld
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +6 _start
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __end
 +[0-9]+: [0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +13 _SDA_BASE_
 +[0-9]+: 0+28 +0 TLS +GLOBAL DEFAULT +8 ld2
 +[0-9]+: 0+24 +0 TLS +GLOBAL DEFAULT +8 ld1
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS __bss_start
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _edata
 +[0-9]+: 0+105c8 +0 OBJECT +GLOBAL DEFAULT +ABS _GLOBAL_OFFSET_TABLE_
 +[0-9]+: [0-9a-f]+ +0 NOTYPE +GLOBAL DEFAULT +ABS _end
 +[0-9]+: 0+1c +0 TLS +GLOBAL DEFAULT +8 gd0
 +[0-9]+: 0+2c +0 TLS +GLOBAL DEFAULT +8 ie0
 +[0-9]+: [0-9a-f]+ +0 OBJECT +GLOBAL DEFAULT +12 _SDA2_BASE_
