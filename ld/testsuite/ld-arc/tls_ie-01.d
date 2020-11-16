#source: tls_ie-01.s
#as: -mcpu=arc700
#ld: -static
#objdump: -D -j .got

[^:]+:     file format elf32-.*arc


Disassembly of section \.got:
[0-9a-f]+ <_GLOBAL_OFFSET_TABLE_>:
\s+...
\s+[0-9a-f]+:\s+08 00 00 00.*
\s+[0-9a-f]+:\s+0c 00 00 00.*
