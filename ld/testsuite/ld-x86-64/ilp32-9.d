#as: --x32
#ld: -m elf32_x86_64 -Ttext-segment 0xe0000000
#objdump: -s -j .text

.*: +file format .*

Contents of section .text:
 e0000054 540000e0 00000000                    T.......        
#pass
