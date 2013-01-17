#source: size32-1.s
#as: --64
#ld: -shared -melf_x86_64
#objdump: -R -s -j .data
#target: x86_64-*-*

.*: +file format .*

DYNAMIC RELOCATION RECORDS
OFFSET           TYPE              VALUE 
0+200360 R_X86_64_SIZE32   xxx
0+200364 R_X86_64_SIZE32   xxx-0x000000000000001e
0+200368 R_X86_64_SIZE32   xxx\+0x000000000000001e
0+20036c R_X86_64_SIZE32   yyy
0+200370 R_X86_64_SIZE32   zzz


Contents of section .data:
 200360 00000000 00000000 00000000 00000000  ................
 200370 00000000 00000000 00000000 00000000  ................
 200380 00000000 00000000 00000000 00000000  ................
 200390 00000000 00000000 00000000           ............    
