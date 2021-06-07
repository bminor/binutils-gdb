#name: i386 32-bit wrapping calculations (data/ELF)
#source: wrap32.s
#objdump: -rsj .data

.*: +file format .*

RELOCATION RECORDS FOR \[\.data\]:

OFFSET +TYPE +VALUE *
0*10 (R_386_32|dir32) *sym
0*14 (R_386_32|dir32) *sym
0*18 (R_386_32|dir32) *sym
0*1c (R_386_32|dir32) *sym
0*30 (R_386_32|dir32) *sym
0*34 (R_386_32|dir32) *sym
0*38 (R_386_32|dir32) *sym
0*3c (R_386_32|dir32) *sym

Contents of section .data:
 0+00 f4 ?00 ?00 ?00 f4 ?00 ?00 ?00 90 ?00 ?00 ?00 90 ?00 ?00 ?00 .*
 0+10 00 ?ff ?ff ?ff 00 ?ff ?ff ?ff f4 ?00 ?00 ?00 f4 ?00 ?00 ?00 .*
 0+20 f4 ?02 ?00 ?70 f4 ?00 ?00 ?80 90 ?02 ?00 ?70 90 ?00 ?00 ?80 .*
 0+30 00 ?01 ?00 ?70 00 ?ff ?ff ?7f f4 ?02 ?00 ?70 f4 ?00 ?00 ?80 .*
