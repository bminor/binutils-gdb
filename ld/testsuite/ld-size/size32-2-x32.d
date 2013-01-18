#source: size32-2.s
#as: --x32
#ld: -shared -melf32_x86_64
#objdump: -R -s -j .data
#target: x86_64-*-*

.*: +file format .*

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
0+200268 R_X86_64_SIZE32   zzz
0+20026c R_X86_64_SIZE32   zzz-0x0000001e
0+200270 R_X86_64_SIZE32   zzz\+0x0000001e


Contents of section .data:
 200260 28000000 28000000 00000000 00000000  ................
 200270 00000000                             ....            
