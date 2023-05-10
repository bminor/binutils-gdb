#as: --EB
#objdump: -dr
#source: atomic-v3.s
#name: eBPF V3 atomic instructions, big-endian, normal syntax

.*: +file format .*bpf.*

Disassembly of section .text:

0+ <.text>:
   0:	db 12 1e ef 00 00 00 00 	xadddw \[%r1\+0x1eef\],%r2
   8:	c3 12 1e ef 00 00 00 00 	xaddw \[%r1\+0x1eef\],%r2
  10:	db 12 1e ef 00 00 00 40 	xordw \[%r1\+0x1eef\],%r2
  18:	c3 12 1e ef 00 00 00 40 	xorw \[%r1\+0x1eef\],%r2
  20:	db 12 1e ef 00 00 00 50 	xanddw \[%r1\+0x1eef\],%r2
  28:	c3 12 1e ef 00 00 00 50 	xandw \[%r1\+0x1eef\],%r2
  30:	db 12 1e ef 00 00 00 a0 	xxordw \[%r1\+0x1eef\],%r2
  38:	c3 12 1e ef 00 00 00 a0 	xxorw \[%r1\+0x1eef\],%r2
  40:	db 12 1e ef 00 00 00 01 	xfadddw \[%r1\+0x1eef\],%r2
  48:	c3 12 1e ef 00 00 00 01 	xfaddw \[%r1\+0x1eef\],%r2
  50:	db 12 1e ef 00 00 00 41 	xfordw \[%r1\+0x1eef\],%r2
  58:	c3 12 1e ef 00 00 00 41 	xforw \[%r1\+0x1eef\],%r2
  60:	db 12 1e ef 00 00 00 51 	xfanddw \[%r1\+0x1eef\],%r2
  68:	c3 12 1e ef 00 00 00 51 	xfandw \[%r1\+0x1eef\],%r2
  70:	db 12 1e ef 00 00 00 a1 	xfxordw \[%r1\+0x1eef\],%r2
  78:	c3 12 1e ef 00 00 00 a1 	xfxorw \[%r1\+0x1eef\],%r2
  80:	db 12 1e ef 00 00 00 e1 	xchgdw \[%r1\+0x1eef\],%r2
  88:	c3 12 1e ef 00 00 00 e1 	xchgw \[%r1\+0x1eef\],%r2
  90:	db 12 1e ef 00 00 00 f1 	xcmpdw \[%r1\+0x1eef\],%r2
  98:	c3 12 1e ef 00 00 00 f1 	xcmpw \[%r1\+0x1eef\],%r2
