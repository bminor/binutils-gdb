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
0+.* <[^>]*> ec9f 0b00 	vscclrm	{VPR}
0+.* <[^>]*> ec9f fa01 	vscclrm	{s30, VPR}
0+.* <[^>]*> ec9f eb02 	vscclrm	{d14, VPR}
0+.* <[^>]*> ecdf 0a04 	vscclrm	{s1-s4, VPR}
0+.* <[^>]*> ec9f 1b08 	vscclrm	{d1-d4, VPR}
0+.* <[^>]*> ec9f 0a20 	vscclrm	{s0-s31, VPR}
0+.* <[^>]*> ec9f 0b20 	vscclrm	{d0-d15, VPR}
0+.* <[^>]*> bf18      	it	ne
0+.* <[^>]*> ecdf 1a01 	vscclrmne	{s3, VPR}
#...
