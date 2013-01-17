#source: size32-1.s
#as: --x32
#ld: -shared -melf32_x86_64
#objdump: -R -s -j .data
#target: x86_64-*-*

.*: +file format .*

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
0+200220 R_X86_64_SIZE32   xxx
0+200224 R_X86_64_SIZE32   xxx-0x0000001e
0+200228 R_X86_64_SIZE32   xxx\+0x0000001e
0+20022c R_X86_64_SIZE32   yyy
0+200230 R_X86_64_SIZE32   zzz


Contents of section .data:
 200220 00000000 00000000 00000000 00000000  ................
 200230 00000000 00000000 00000000 00000000  ................
 200240 00000000 00000000 00000000 00000000  ................
 200250 00000000 00000000 00000000           ............    
