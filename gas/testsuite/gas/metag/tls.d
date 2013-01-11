#objdump: -dr
#name: tls

.*: +file format .*

Disassembly of section \.text:

00000000 <.text>:
   0:	00 00 18 03 	          ADD       D1Ar1,D1Ar1,#0
			0: R_METAG_TLS_GD	_a
   4:	00 00 18 03 	          ADD       D1Ar1,D1Ar1,#0
			4: R_METAG_TLS_LDM	_b
   8:	01 00 00 02 	          ADDT      D0Re0,D0Re0,#0
			8: R_METAG_TLS_LDO_HI16	_b
   c:	00 00 00 02 	          ADD       D0Re0,D0Re0,#0
			c: R_METAG_TLS_LDO_LO16	_b
  10:	0d 00 20 a7 	          GETD      D0FrT,\[A1LbP\]
			10: R_METAG_TLS_IE	_b
  14:	05 00 00 02 	          MOVT      D0Re0,#0
			14: R_METAG_TLS_IENONPIC_HI16	_b
  18:	00 00 00 02 	          ADD       D0Re0,D0Re0,#0
			18: R_METAG_TLS_IENONPIC_LO16	_b
  1c:	01 00 00 02 	          ADDT      D0Re0,D0Re0,#0
			1c: R_METAG_TLS_LE_HI16	_b
  20:	00 00 00 02 	          ADD       D0Re0,D0Re0,#0
			20: R_METAG_TLS_LE_LO16	_b
