#as: -EL -mdialect=pseudoc
#objdump: -dr -M hex,pseudoc
#source: atomic-pseudoc.s
#name: eBPF atomic instructions, pseudoc syntax

.*: +file format .*bpf.*

Disassembly of section .text:

0+ <.text>:
   0:	db 21 ef 1e 00 00 00 00 	\*\(u64\*\)\(r1\+0x1eef\)\+=r2
   8:	c3 21 ef 1e 00 00 00 00 	\*\(u32\*\)\(r1\+0x1eef\)\+=r2
