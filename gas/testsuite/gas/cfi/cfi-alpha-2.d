#objdump: -r -j .eh_frame
#name: CFI on alpha, 2

.*:     file format elf64-alpha

RELOCATION RECORDS FOR \[\.eh_frame\]:
OFFSET           TYPE              VALUE 
0*000001b SREL32            \.text
0*000002c SREL32            \.text\+0x0*0000004
