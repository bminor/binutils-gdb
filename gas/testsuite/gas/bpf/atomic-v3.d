#as: --EL
#objdump: -dr
#source: atomic-v3.s
#name: eBPF V3 atomic instructions, litle-endian, normal syntax

.*: +file format .*bpf.*

Disassembly of section .text:

0+ <.text>:
   0:	db 21 ef 1e 00 00 00 00 	xadddw \[%r1\+0x1eef\],%r2
   8:	c3 21 ef 1e 00 00 00 00 	xaddw \[%r1\+0x1eef\],%r2
  10:	db 21 ef 1e 40 00 00 00 	xordw \[%r1\+0x1eef\],%r2
  18:	c3 21 ef 1e 40 00 00 00 	xorw \[%r1\+0x1eef\],%r2
  20:	db 21 ef 1e 50 00 00 00 	xanddw \[%r1\+0x1eef\],%r2
  28:	c3 21 ef 1e 50 00 00 00 	xandw \[%r1\+0x1eef\],%r2
  30:	db 21 ef 1e a0 00 00 00 	xxordw \[%r1\+0x1eef\],%r2
  38:	c3 21 ef 1e a0 00 00 00 	xxorw \[%r1\+0x1eef\],%r2
  40:	db 21 ef 1e 01 00 00 00 	xfadddw \[%r1\+0x1eef\],%r2
  48:	c3 21 ef 1e 01 00 00 00 	xfaddw \[%r1\+0x1eef\],%r2
  50:	db 21 ef 1e 41 00 00 00 	xfordw \[%r1\+0x1eef\],%r2
  58:	c3 21 ef 1e 41 00 00 00 	xforw \[%r1\+0x1eef\],%r2
  60:	db 21 ef 1e 51 00 00 00 	xfanddw \[%r1\+0x1eef\],%r2
  68:	c3 21 ef 1e 51 00 00 00 	xfandw \[%r1\+0x1eef\],%r2
  70:	db 21 ef 1e a1 00 00 00 	xfxordw \[%r1\+0x1eef\],%r2
  78:	c3 21 ef 1e a1 00 00 00 	xfxorw \[%r1\+0x1eef\],%r2
  80:	db 21 ef 1e e1 00 00 00 	xchgdw \[%r1\+0x1eef\],%r2
  88:	c3 21 ef 1e e1 00 00 00 	xchgw \[%r1\+0x1eef\],%r2
  90:	db 21 ef 1e f1 00 00 00 	xcmpdw \[%r1\+0x1eef\],%r2
  98:	c3 21 ef 1e f1 00 00 00 	xcmpw \[%r1\+0x1eef\],%r2
