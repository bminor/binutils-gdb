#objdump: -ldr
#name: D30V guarded execution test
#as: 

.*: +file format elf32-d30v

Disassembly of section .text:

00000000 <.text>:
   0:	08001083 88001083 	add	r1, r2, r3	->	add	r1, r2, r3
   8:	18001083 a8001083 	add/tx	r1, r2, r3	->	add/fx	r1, r2, r3
  10:	38001083 c8001083 	add/xt	r1, r2, r3	->	add/xf	r1, r2, r3
  18:	58001083 e8001083 	add/tt	r1, r2, r3	->	add/tf	r1, r2, r3
  20:	08001083 88001083 	add	r1, r2, r3	->	add	r1, r2, r3
  28:	18001083 a8001083 	add/tx	r1, r2, r3	->	add/fx	r1, r2, r3
  30:	38001083 c8001083 	add/xt	r1, r2, r3	->	add/xf	r1, r2, r3
  38:	58001083 e8001083 	add/tt	r1, r2, r3	->	add/tf	r1, r2, r3
