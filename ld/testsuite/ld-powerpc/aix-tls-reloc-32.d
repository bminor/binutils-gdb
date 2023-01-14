#source: aix-tls-reloc.s
#as: -a32
#ld: -b32 -shared -bE:aix-tls-reloc.ex
#objdump: -dr
#target: [is_xcoff_format]

.*

Disassembly of section \.text:

.* <\.foo>:
.*:	80 82 00 00 	l       r4,0\(r2\)
.*: R_TOC	gd-.*
.*:	80 62 00 04 	l       r3,4\(r2\)
.*: R_TOC	.gd-.*
.*:	48 00 00 03 	bla     0 <\.foo-.*>
.*: R_BA_26	__tls_get_addr
.*:	80 62 00 0c 	l       r3,12\(r2\)
.*: R_TOC	_\$TLSML-.*
.*:	48 00 00 03 	bla     0 <\.foo-.*>
.*: R_BA_26	__tls_get_mod
.*:	80 82 00 08 	l       r4,8\(r2\)
.*: R_TOC	ld-.*
.*:	7c a3 22 14 	cax     r5,r3,r4
.*:	48 00 00 03 	bla     0 <\.foo-.*>
.*: R_BA_26	__get_tpointer
.*:	80 82 00 10 	l       r4,16\(r2\)
.*: R_TOC	ie-.*
.*:	7c a3 22 14 	cax     r5,r3,r4
.*:	48 00 00 03 	bla     0 <\.foo-.*>
.*: R_BA_26	__get_tpointer
.*:	80 82 00 14 	l       r4,20\(r2\)
.*: R_TOC	le-.*
.*:	7c a3 22 14 	cax     r5,r3,r4
.*
