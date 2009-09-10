#as: -EL -mARC700
#objdump: -dr -EL

.*: +file format elf32-.*arc

Disassembly of section .text:

00000000 <text_label>:
   0:	20 20 80 0f 00 00 00 00 	j          0 <text_label>

			4: R_ARC_32_ME	.text
   8:	e0 20 80 0f 00 00 00 00 	j          0 <text_label>

			c: R_ARC_32_ME	.text
  10:	e0 20 80 0f 00 00 00 00 	j          0 <text_label>

			14: R_ARC_32_ME	.text
  18:	e0 20 81 0f 00 00 00 00 	jz         0 <text_label>

			1c: R_ARC_32_ME	.text
  20:	e0 20 81 0f 00 00 00 00 	jz         0 <text_label>

			24: R_ARC_32_ME	.text
  28:	e0 20 82 0f 00 00 00 00 	jnz        0 <text_label>

			2c: R_ARC_32_ME	.text
  30:	e0 20 82 0f 00 00 00 00 	jnz        0 <text_label>

			34: R_ARC_32_ME	.text
  38:	e0 20 83 0f 00 00 00 00 	jp         0 <text_label>

			3c: R_ARC_32_ME	.text
  40:	e0 20 83 0f 00 00 00 00 	jp         0 <text_label>

			44: R_ARC_32_ME	.text
  48:	e0 20 84 0f 00 00 00 00 	jn         0 <text_label>

			4c: R_ARC_32_ME	.text
  50:	e0 20 84 0f 00 00 00 00 	jn         0 <text_label>

			54: R_ARC_32_ME	.text
  58:	e0 20 85 0f 00 00 00 00 	jc         0 <text_label>

			5c: R_ARC_32_ME	.text
  60:	e0 20 85 0f 00 00 00 00 	jc         0 <text_label>

			64: R_ARC_32_ME	.text
  68:	e0 20 85 0f 00 00 00 00 	jc         0 <text_label>

			6c: R_ARC_32_ME	.text
  70:	e0 20 86 0f 00 00 00 00 	jnc        0 <text_label>

			74: R_ARC_32_ME	.text
  78:	e0 20 86 0f 00 00 00 00 	jnc        0 <text_label>

			7c: R_ARC_32_ME	.text
  80:	e0 20 86 0f 00 00 00 00 	jnc        0 <text_label>

			84: R_ARC_32_ME	.text
  88:	e0 20 87 0f 00 00 00 00 	jv         0 <text_label>

			8c: R_ARC_32_ME	.text
  90:	e0 20 87 0f 00 00 00 00 	jv         0 <text_label>

			94: R_ARC_32_ME	.text
  98:	e0 20 88 0f 00 00 00 00 	jnv        0 <text_label>

			9c: R_ARC_32_ME	.text
  a0:	e0 20 88 0f 00 00 00 00 	jnv        0 <text_label>

			a4: R_ARC_32_ME	.text
  a8:	e0 20 89 0f 00 00 00 00 	jgt        0 <text_label>

			ac: R_ARC_32_ME	.text
  b0:	e0 20 8a 0f 00 00 00 00 	jge        0 <text_label>

			b4: R_ARC_32_ME	.text
  b8:	e0 20 8b 0f 00 00 00 00 	jlt        0 <text_label>

			bc: R_ARC_32_ME	.text
  c0:	e0 20 8c 0f 00 00 00 00 	jle        0 <text_label>

			c4: R_ARC_32_ME	.text
  c8:	e0 20 8d 0f 00 00 00 00 	jhi        0 <text_label>

			cc: R_ARC_32_ME	.text
  d0:	e0 20 8e 0f 00 00 00 00 	jls        0 <text_label>

			d4: R_ARC_32_ME	.text
  d8:	e0 20 8f 0f 00 00 00 00 	jpnz       0 <text_label>

			dc: R_ARC_32_ME	.text
  e0:	20 20 80 0f 00 00 00 00 	j          0 <text_label>

			e4: R_ARC_32_ME	external_text_label
  e8:	60 20 00 00             	j          0 <text_label>

