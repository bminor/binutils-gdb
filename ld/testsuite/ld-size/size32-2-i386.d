#source: size32-2.s
#as: --32
#ld: -shared -melf_i386
#objdump: -R -s -j .data
#target: x86_64-*-* i?86-*-*

.*: +file format .*

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
0+123c R_386_SIZE32      xxx
0+1240 R_386_SIZE32      yyy
0+1244 R_386_SIZE32      zzz
0+1248 R_386_SIZE32      zzz
0+124c R_386_SIZE32      zzz


Contents of section .data:
 123c 00000000 00000000 00000000 e2ffffff  ................
 124c 1e000000                             ....            
