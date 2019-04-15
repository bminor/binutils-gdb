#name: Armv8.1-M Mainline Security Extensions instructions
#source: archv8m_1m-cmse-main.s
#as: -march=armv8.1-m.main -mimplicit-it=always
#objdump: -dr --prefix-addresses --show-raw-insn -marmv8.1-m.main

.*: +file format .*arm.*

Disassembly of section .text:
0+.* <[^>]*> e89f 0005 	clrm	{r0, r2}
0+.* <[^>]*> e89f 8000 	clrm	{APSR}
0+.* <[^>]*> e89f 8008 	clrm	{r3, APSR}
0+.* <[^>]*> bf08      	it	eq
0+.* <[^>]*> e89f 0010 	clrmeq	{r4}
#...
