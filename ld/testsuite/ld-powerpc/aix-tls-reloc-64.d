#source: aix-tls-reloc.s
#as: -a32
#ld: -b32 -shared -bE:aix-tls-reloc.ex
#objdump: -dr
#target: [is_xcoff_format]

.*

Disassembly of section \.text:

.* <\.foo>:
.*:	e8 82 00 00 	ld      r4,0\(r2\)
.*: R_TOC	gd-.*
.*:	e8 62 00 08 	ld      r3,8\(r2\)
.*: R_TOC	.gd-.*
.*:	48 00 00 03 	bla     0 <\.foo-.*>
.*: R_BA_26	__tls_get_addr
.*:	e8 62 00 18 	ld      r3,24\(r2\)
.*: R_TOC	_\$TLSML-.*
.*:	48 00 00 03 	bla     0 <\.foo-.*>
.*: R_BA_26	__tls_get_mod
.*:	80 82 00 10 	lwz     r4,16\(r2\)
.*: R_TOC	ld-.*
.*:	7c a3 22 14 	add     r5,r3,r4
.*:	e8 82 00 20 	ld      r4,32\(r2\)
.*: R_TOC	ie-.*
.*:	7c a4 6a 14 	add     r5,r4,r13
.*:	e8 82 00 28 	ld      r4,40\(r2\)
.*: R_TOC	le-.*
.*:	7c a3 6a 14 	add     r5,r3,r13
.*
