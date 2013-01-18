#source: size32-2.s
#as: --32
#ld: -shared -melf_i386
#objdump: -R -s -j .data
#target: x86_64-*-* i?86-*-*

.*: +file format .*

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
0+1234 R_386_SIZE32      zzz
0+1238 R_386_SIZE32      zzz
0+123c R_386_SIZE32      zzz


Contents of section .data:
 122c 28000000 28000000 00000000 e2ffffff  ................
 123c 1e000000                             ....            
