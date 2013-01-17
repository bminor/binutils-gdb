#source: size32-1.s
#as: --32
#ld: -shared -melf_i386
#objdump: -R -s -j .data
#target: x86_64-*-* i?86-*-*

.*: +file format .*

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
0+11f4 R_386_SIZE32      xxx
0+11f8 R_386_SIZE32      xxx
0+11fc R_386_SIZE32      xxx
0+1200 R_386_SIZE32      yyy
0+1204 R_386_SIZE32      zzz


Contents of section .data:
 11f4 00000000 e2ffffff 1e000000 00000000  ................
 1204 00000000 00000000 00000000 00000000  ................
 1214 00000000 00000000 00000000 00000000  ................
 1224 00000000 00000000 00000000           ............    
