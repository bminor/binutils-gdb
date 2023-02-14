#source: asciz.s
#ld: -T asciz.t
#objdump: -s -j .text
#target: [is_elf_format]
#skip: mips*-*-*
#skip: tilegx*-*-* tilepro-*-*
# COFF, PE and MIPS targets align code to a 16 byte boundary
# tilegx andtilepro aligns code to a 8 byte boundary.

.*:     file format .*

Contents of section .text:
 .... 01010101 54686973 20697320 61207374  ....This is a st
 .... 72696e67 00...... ........ ........  ring............
 .... 54686973 20697320 616e6f74 68657220  This is another 
 .... 0a737472 696e6753 00                 .stringS........
#pass
