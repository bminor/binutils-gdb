#objdump: -dr
#name: D30V relocations

.*: +file format elf32-d30v

Disassembly of section .text:

00000000 <start>:
   0:	88082000 80000028 	add	r2, 0, 0x28
			0: R_D30V_32	.text
   8:	88084000 80000000 	add	r4, 0, 0x0
			8: R_D30V_32	.data
  10:	88084000 80000006 	add	r4, 0, 0x6
			10: R_D30V_32	.data
  18:	88084000 80000000 	add	r4, 0, 0x0
			18: R_D30V_32	unk
  20:	80080000 80000018 	bra	38 <cont>

00000028 <hello>:
  28:	48656c6c 6f20576f 	.long	0x48656c6c	||	.long	0x6f20576f
  30:	726c640a 00000000 	.long	0x726c640a	||	bra	0

00000038 <cont>:
  38:	80180000 80000048 	jmp	48 <cont2>
			38: R_D30V_32	.text
  40:	088020c0 00f00000 	abs	r2, r3	||	nop	

00000048 <cont2>:
  48:	000bfff7 00f00000 	bra	0 <start>	||	nop	
  50:	00180000 00f00000 	jmp	0 <start>	||	nop	
			50: R_D30V_21	.text
  58:	000bfff5 80180000 	bra	0 <start>	->	jmp	0 <start>
			5c: R_D30V_21	.text
  60:	00180000 800bfff4 	jmp	0 <start>	->	bra	0 <start>
			60: R_D30V_21	.text
  68:	80080000 80000000 	bra	68 <cont2\+20>
			68: R_D30V_32_PCREL	unknown
  70:	80180000 80000000 	jmp	0 <start>
			70: R_D30V_32	unknown
