#objdump: -dr
#name: TIc80 control register operands

.*: +file format .*tic80.*

Disassembly of section .text:

00000000 <.text>:
   0:	34 00 02 10 	rdcr      ANASTAT,r2
   4:	39 00 02 10 	rdcr      BRK1,r2
   8:	3a 00 02 10 	rdcr      BRK2,r2
   c:	02 00 02 10 	rdcr      CONFIG,r2
  10:	00 05 02 10 	rdcr      DLRU,r2
  14:	00 04 02 10 	rdcr      DTAG0,r2
  18:	01 04 02 10 	rdcr      DTAG1,r2
  1c:	0a 04 02 10 	rdcr      DTAG10,r2
  20:	0b 04 02 10 	rdcr      DTAG11,r2
  24:	0c 04 02 10 	rdcr      DTAG12,r2
  28:	0d 04 02 10 	rdcr      DTAG13,r2
  2c:	0e 04 02 10 	rdcr      DTAG14,r2
  30:	0f 04 02 10 	rdcr      DTAG15,r2
  34:	02 04 02 10 	rdcr      DTAG2,r2
  38:	03 04 02 10 	rdcr      DTAG3,r2
  3c:	04 04 02 10 	rdcr      DTAG4,r2
  40:	05 04 02 10 	rdcr      DTAG5,r2
  44:	06 04 02 10 	rdcr      DTAG6,r2
  48:	07 04 02 10 	rdcr      DTAG7,r2
  4c:	08 04 02 10 	rdcr      DTAG8,r2
  50:	09 04 02 10 	rdcr      DTAG9,r2
  54:	33 00 02 10 	rdcr      ECOMCNTL,r2
  58:	01 00 02 10 	rdcr      EIP,r2
  5c:	00 00 02 10 	rdcr      EPC,r2
  60:	11 00 02 10 	rdcr      FLTADR,r2
  64:	14 00 02 10 	rdcr      FLTDTH,r2
  68:	13 00 02 10 	rdcr      FLTDTL,r2
  6c:	10 00 02 10 	rdcr      FLTOP,r2
  70:	12 00 02 10 	rdcr      FLTTAG,r2
  74:	08 00 02 10 	rdcr      FPST,r2
  78:	06 00 02 10 	rdcr      IE,r2
  7c:	00 03 02 10 	rdcr      ILRU,r2
  80:	00 40 02 10 	rdcr      IN0P,r2
  84:	01 40 02 10 	rdcr      IN1P,r2
  88:	04 00 02 10 	rdcr      INTPEN,r2
  8c:	00 02 02 10 	rdcr      ITAG0,r2
  90:	01 02 02 10 	rdcr      ITAG1,r2
  94:	0a 02 02 10 	rdcr      ITAG10,r2
  98:	0b 02 02 10 	rdcr      ITAG11,r2
  9c:	0c 02 02 10 	rdcr      ITAG12,r2
  a0:	0d 02 02 10 	rdcr      ITAG13,r2
  a4:	0e 02 02 10 	rdcr      ITAG14,r2
  a8:	0f 02 02 10 	rdcr      ITAG15,r2
  ac:	02 02 02 10 	rdcr      ITAG2,r2
  b0:	03 02 02 10 	rdcr      ITAG3,r2
  b4:	04 02 02 10 	rdcr      ITAG4,r2
  b8:	05 02 02 10 	rdcr      ITAG5,r2
  bc:	06 02 02 10 	rdcr      ITAG6,r2
  c0:	07 02 02 10 	rdcr      ITAG7,r2
  c4:	08 02 02 10 	rdcr      ITAG8,r2
  c8:	09 02 02 10 	rdcr      ITAG9,r2
  cc:	31 00 02 10 	rdcr      MIP,r2
  d0:	30 00 02 10 	rdcr      MPC,r2
  d4:	02 40 02 10 	rdcr      OUTP,r2
  d8:	0d 00 02 10 	rdcr      PKTREQ,r2
  dc:	0a 00 02 10 	rdcr      PPERROR,r2
  e0:	20 00 02 10 	rdcr      SYSSTK,r2
  e4:	21 00 02 10 	rdcr      SYSTMP,r2
  e8:	0e 00 02 10 	rdcr      TCOUNT,r2
  ec:	0f 00 02 10 	rdcr      TSCALE,r2
