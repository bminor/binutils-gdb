#objdump: -ldr
#name: D30V optimization test
#as: 

.*: +file format elf32-d30v

Disassembly of section .text:

00000000 <.text>:
   0:	08801080 08803100 	abs	r1, r2	||	abs	r3, r4
   8:	02900100 02901080 	notfg	f0, s	||	notfg	f1, f2
  10:	08801080 02901080 	abs	r1, r2	||	notfg	f1, f2
  18:	08001083 82907000 	add	r1, r2, r3	->	notfg	c, f0
  20:	08001083 829001c0 	add	r1, r2, r3	->	notfg	f0, c
  28:	00080000 08801080 	bra	28	||	abs	r1, r2
  30:	00080000 08801080 	bra	30	||	abs	r1, r2
  38:	00280000 00f00000 	bsr	38	||	nop	
  40:	08801080 88801080 	abs	r1, r2	->	abs	r1, r2
  48:	00280000 08801080 	bsr	48	||	abs	r1, r2
  50:	04001083 85007209 	ldb	r1, \@\(r2, r3\)	->	stb	r7, \@\(r8, r9\)
  58:	05007209 84001083 	stb	r7, \@\(r8, r9\)	->	ldb	r1, \@\(r2, r3\)
  60:	04007209 84001083 	ldb	r7, \@\(r8, r9\)	->	ldb	r1, \@\(r2, r3\)
  68:	05007209 85001083 	stb	r7, \@\(r8, r9\)	->	stb	r1, \@\(r2, r3\)
