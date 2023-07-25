#as: -EB -mdialect=pseudoc
#objdump: -dr -M hex,pseudoc
#source: atomic-pseudoc.s
#name: eBPF atomic instructions, pseudoc syntax, big endian

.*: +file format .*bpf.*

Disassembly of section .text:

0+ <.text>:
   0:	db 12 1e ef 00 00 00 00 	lock \*\(u64\*\)\(r1\+0x1eef\)\+=r2
   8:	c3 12 1e ef 00 00 00 00 	lock \*\(u32\*\)\(r1\+0x1eef\)\+=r2
  10:	db 12 1e ef 00 00 00 00 	lock \*\(u64\*\)\(r1\+0x1eef\)\+=r2
  18:	c3 12 1e ef 00 00 00 00 	lock \*\(u32\*\)\(r1\+0x1eef\)\+=r2
  20:	db 12 1e ef 00 00 00 50 	lock \*\(u64\*\)\(r1\+0x1eef\)\&=r2
  28:	c3 12 1e ef 00 00 00 50 	lock \*\(u32\*\)\(r1\+0x1eef\)\&=r2
  30:	db 12 1e ef 00 00 00 40 	lock \*\(u64\*\)\(r1\+0x1eef\)\|=r2
  38:	c3 12 1e ef 00 00 00 40 	lock \*\(u32\*\)\(r1\+0x1eef\)\|=r2
  40:	db 12 1e ef 00 00 00 a0 	lock \*\(u64\*\)\(r1\+0x1eef\)\^=r2
  48:	c3 12 1e ef 00 00 00 a0 	lock \*\(u32\*\)\(r1\+0x1eef\)\^=r2
  50:	db 12 1e ef 00 00 00 01 	r2=atomic_fetch_add\(\(u64\*\)\(r1\+0x1eef\),r2\)
  58:	c3 12 1e ef 00 00 00 01 	w2=atomic_fetch_add\(\(u32\*\)\(r1\+0x1eef\),w2\)
  60:	db 12 1e ef 00 00 00 51 	r2=atomic_fetch_and\(\(u64\*\)\(r1\+0x1eef\),r2\)
  68:	c3 12 1e ef 00 00 00 51 	w2=atomic_fetch_and\(\(u32\*\)\(r1\+0x1eef\),w2\)
  70:	db 12 1e ef 00 00 00 41 	r2=atomic_fetch_or\(\(u64\*\)\(r1\+0x1eef\),r2\)
  78:	c3 12 1e ef 00 00 00 41 	w2=atomic_fetch_or\(\(u32\*\)\(r1\+0x1eef\),w2\)
  80:	db 12 1e ef 00 00 00 a1 	r2=atomic_fetch_xor\(\(u64\*\)\(r1\+0x1eef\),r2\)
  88:	c3 12 1e ef 00 00 00 a1 	w2=atomic_fetch_xor\(\(u32\*\)\(r1\+0x1eef\),w2\)
