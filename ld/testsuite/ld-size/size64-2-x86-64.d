#source: size64-2.s
#as: --64
#ld: -shared -melf_x86_64
#objdump: -R -s -j .data
#target: x86_64-*-*

.*: +file format .*

DYNAMIC RELOCATION RECORDS
OFFSET           TYPE              VALUE 
0+2003d8 R_X86_64_SIZE64   xxx
0+2003e0 R_X86_64_SIZE64   yyy
0+2003e8 R_X86_64_SIZE64   zzz
0+2003f0 R_X86_64_SIZE64   zzz-0x000000000000001e
0+2003f8 R_X86_64_SIZE64   zzz\+0x000000000000001e


Contents of section .data:
 2003d8 00000000 00000000 00000000 00000000  ................
 2003e8 00000000 00000000 00000000 00000000  ................
 2003f8 00000000 00000000                    ........        
