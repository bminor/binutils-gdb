#as: -EL -mdialect=normal
#objdump: -dr -M hex
#source: atomic.s
#name: eBPF atomic instructions, normal syntax

.*: +file format .*bpf.*

Disassembly of section .text:

0+ <.text>:
   0:	db 21 ef 1e 00 00 00 00 	aadd \[%r1\+0x1eef\],%r2
   8:	c3 21 ef 1e 00 00 00 00 	aadd32 \[%r1\+0x1eef\],%r2
