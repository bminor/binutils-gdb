#source: size64-2.s
#as: --64
#ld: -shared -melf_x86_64
#objdump: -R -s -j .data
#target: x86_64-*-*

.*: +file format .*

DYNAMIC RELOCATION RECORDS
OFFSET           TYPE              VALUE 
0+2003b8 R_X86_64_SIZE64   zzz
0+2003c0 R_X86_64_SIZE64   zzz-0x000000000000001e
0+2003c8 R_X86_64_SIZE64   zzz\+0x000000000000001e


Contents of section .data:
 2003a8 28000000 00000000 28000000 00000000  ................
 2003b8 00000000 00000000 00000000 00000000  ................
 2003c8 00000000 00000000                    ........        
