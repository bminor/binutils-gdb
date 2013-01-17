#source: size32-2.s
#as: --x32
#ld: -shared -melf32_x86_64
#objdump: -R -s -j .data
#target: x86_64-*-*

.*: +file format .*

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
0+200278 R_X86_64_SIZE32   xxx
0+20027c R_X86_64_SIZE32   yyy
0+200280 R_X86_64_SIZE32   zzz
0+200284 R_X86_64_SIZE32   zzz-0x0000001e
0+200288 R_X86_64_SIZE32   zzz\+0x0000001e


Contents of section .data:
 200278 00000000 00000000 00000000 00000000  ................
 200288 00000000                             ....            
