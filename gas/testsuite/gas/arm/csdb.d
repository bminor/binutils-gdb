#name: CSDB
#source: csdb.s
#objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*arm.*

Disassembly of section .text:
0+000 <[^>]*> f3af 8014 ?	csdb
0+004 <[^>]*> e320f014 ?	csdb

