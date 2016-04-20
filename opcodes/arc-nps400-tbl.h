/****                  Bit Manipulation Instructions                  ****/

/* movl<.cl> */
{ "movh", 0x48080000, 0xf81f0000, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST, NPS_R_SRC1, NPS_UIMM16 }, { 0 }},
{ "movh", 0x48180000, 0xf81f0000, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST, NPS_UIMM16 }, { C_NPS_CL }},

/* movl<.cl> */
{ "movl", 0x48090000, 0xf81f0000, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST, NPS_R_SRC1, NPS_UIMM16 }, { 0 }},
{ "movl", 0x48190000, 0xf81f0000, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST, NPS_UIMM16 }, { C_NPS_CL }},

/* movb<.f><.cl> */
{ "movb", 0x48010000, 0xf80f8000, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_R_SRC2_3B, NPS_BITOP_DST_POS, NPS_BITOP_SRC_POS, NPS_BITOP_SIZE }, { C_NPS_F }},
{ "movb", 0x48018000, 0xf80f8000, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST_3B, NPS_R_SRC2_3B, NPS_BITOP_DST_POS, NPS_BITOP_SRC_POS, NPS_BITOP_SIZE }, { C_NPS_F, C_NPS_CL }},

/* movbi<.f><.cl> */
{ "movbi", 0x480f0000, 0xf80f8000, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST, NPS_R_SRC1, NPS_BITOP_UIMM8, NPS_BITOP_DST_POS, NPS_BITOP_SIZE_2B  }, { C_NPS_F }},
{ "movbi", 0x480f8000, 0xf80f8000, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST, NPS_BITOP_UIMM8, NPS_BITOP_DST_POS, NPS_BITOP_SIZE_2B  }, { C_NPS_F, C_NPS_CL }},

/* decode1<.f> */
{ "decode1", 0x48038040, 0xf80f83e0, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_R_SRC2_3B, NPS_BITOP_SRC_POS, NPS_BITOP_SIZE }, { C_NPS_F }},

/* decode1.cl<.f> */
{ "decode1", 0x48038060, 0xf80803e0, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST_3B, NPS_R_SRC2_3B, NPS_BITOP_DST_POS_SZ }, { C_NPS_CL, C_NPS_F }},

/* fbset<.f> */
{ "fbset", 0x48038000, 0xf80f83e0, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_R_SRC2_3B, NPS_BITOP_SRC_POS, NPS_BITOP_SIZE }, { C_NPS_F }},

/* fbclr<.f> */
{ "fbclr", 0x48030000, 0xf80f83e0, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_R_SRC2_3B, NPS_BITOP_SRC_POS, NPS_BITOP_SIZE }, { C_NPS_F }},

/* encode0<.f> */
{ "encode0", 0x48040000, 0xf80f8000, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST_3B, NPS_R_SRC2_3B, NPS_BITOP_SRC_POS, NPS_BITOP_SIZE }, { C_NPS_F }},

/* encode1<.f> */
{ "encode1", 0x48048000, 0xf80f8000, ARC_OPCODE_NPS400, BITOP, NONE, { NPS_R_DST_3B, NPS_R_SRC2_3B, NPS_BITOP_SRC_POS, NPS_BITOP_SIZE }, { C_NPS_F }},

/* rflt a,b,c   00111bbb00101110FBBBCCCCCCAAAAAA */
{ "rflt", 0x382e0000, 0xf8ff8000, ARC_OPCODE_NPS400, BITOP, NONE, { RA, RB, RC }, { 0 }},

/* rflt a,limm,c   0011111000101110F111CCCCCCAAAAAA */
{ "rflt", 0x3e2e7000, 0xfffff000, ARC_OPCODE_NPS400, BITOP, NONE, { RA, LIMM, RC }, { 0 }},

/* rflt 0,b,c   00111bbb00101110FBBBCCCCCC111110 */
{ "rflt", 0x382e003e, 0xf8ff803f, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, RB, RC }, { 0 }},

/* rflt 0,limm,c   0011111000101110F111CCCCCC111110 */
{ "rflt", 0x3e2e703e, 0xfffff03f, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, LIMM, RC }, { 0 }},

/* rflt a,b,u6   00111bbb01101110FBBBuuuuuuAAAAAA */
{ "rflt", 0x386e0000, 0xf8ff8000, ARC_OPCODE_NPS400, BITOP, NONE, { RA, RB, NPS_RFLT_UIMM6 }, { 0 }},

/* rflt a,limm,u6   0011111001101110F111uuuuuuAAAAAA */
{ "rflt", 0x3e6e7000, 0xfffff000, ARC_OPCODE_NPS400, BITOP, NONE, { RA, LIMM, NPS_RFLT_UIMM6 }, { 0 }},

/* rflt 0,b,u6   00111bbb01101110FBBBuuuuuu111110 */
{ "rflt", 0x386e003e, 0xf8ff803f, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, RB, NPS_RFLT_UIMM6 }, { 0 }},

/* rflt 0,limm,u6   0011111001101110F111uuuuuu111110 */
{ "rflt", 0x3e6e703e, 0xfffff03f, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, LIMM, NPS_RFLT_UIMM6 }, { 0 }},

/* crc16<.r> a,b,c  00111bbb00110011RBBBCCCCCCAAAAAA */
{ "crc16", 0x38330000, 0xf8ff0000, ARC_OPCODE_NPS400, BITOP, NONE, { RA, RB, RC }, { C_NPS_R }},

/* crc16<.r> a,limm,c  0011111000110011R111CCCCCCAAAAAA */
{ "crc16", 0x3e337000, 0xffff7000, ARC_OPCODE_NPS400, BITOP, NONE, { RA, LIMM, RC }, { C_NPS_R }},

/* crc16<.r> a,b,u6  00111bbb01110011RBBBuuuuuuAAAAAA */
{ "crc16", 0x38730000, 0xf8ff0000, ARC_OPCODE_NPS400, BITOP, NONE, { RA, RB, UIMM6_20 }, { C_NPS_R }},

/* crc16<.r> 0,b,c  00111bbb00110011RBBBCCCCCC111110 */
{ "crc16", 0x3833003e, 0xf8ff003f, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, RB, RC }, { C_NPS_R }},

/* crc16<.r> 0,limm,c  0011111000110011R111CCCCCC111110 */
{ "crc16", 0x3e33703e, 0xffff703f, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, LIMM, RC }, { C_NPS_R }},

/* crc16<.r> 0,b,u6  00111bbb01110011RBBBuuuuuu111110 */
{ "crc16", 0x3873003e, 0xf8ff003f, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, RB, UIMM6_20 }, { C_NPS_R }},

/* crc16<.r> 0,b,limm  00111bbb00110011RBBB111110111110 */
{ "crc16", 0x38330fbe, 0xf8ff0fff, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, RB, LIMM }, { C_NPS_R }},

/* crc16<.r> a,b,limm  00111bbb00110011RBBB111110AAAAAA */
{ "crc16", 0x38330f80, 0xf8ff0fc0, ARC_OPCODE_NPS400, BITOP, NONE, { RA, RB, LIMM }, { C_NPS_R }},

/* crc16<.r> a,limm,limm  0011111000110011R111111110AAAAAA */
{ "crc16", 0x3e337f80, 0xffff7fc0, ARC_OPCODE_NPS400, BITOP, NONE, { RA, LIMM, LIMMdup }, { C_NPS_R }},

/* crc16<.r> a,limm,u6  0011111001110011R111uuuuuuAAAAAA */
{ "crc16", 0x3e737000, 0xffff7000, ARC_OPCODE_NPS400, BITOP, NONE, { RA, LIMM, UIMM6_20 }, { C_NPS_R }},

/* crc16<.r> 0,limm,u6  0011111001110011R111uuuuuu111110 */
{ "crc16", 0x3e73703e, 0xffff703f, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, LIMM, UIMM6_20 }, { C_NPS_R }},

/* crc32<.r> a,b,c		00111 bbb 00 110100 R BBB CCCCCC AAAAAA */
{ "crc32", 0x38340000, 0xf8ff0000, ARC_OPCODE_NPS400, BITOP, NONE, { RA, RB, RC }, { C_NPS_R }},

/* crc32<.r> a,limm,c		00111 110 00 110100 R 111 CCCCCC AAAAAA */
{ "crc32", 0x3e347000, 0xffff7000, ARC_OPCODE_NPS400, BITOP, NONE, { RA, LIMM, RC }, { C_NPS_R }},

/* crc32<.r> a,b,u6		00111 bbb 01 110100 R BBB uuuuuu AAAAAA */
{ "crc32", 0x38740000, 0xf8ff0000, ARC_OPCODE_NPS400, BITOP, NONE, { RA, RB, UIMM6_20 }, { C_NPS_R }},

/* crc32<.r> 0,b,c		00111 bbb 00 110100 R BBB CCCCCC 111110 */
{ "crc32", 0x3834003e, 0xf8ff003f, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, RB, RC }, { C_NPS_R }},

/* crc32<.r> 0,limm,c		00111 110 00 110100 R 111 CCCCCC 111110 */
{ "crc32", 0x3e34703e, 0xffff703f, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, LIMM, RC }, { C_NPS_R }},

/* crc32<.r> 0,b,u6		00111 bbb 01 110100 R BBB uuuuuu 111110 */
{ "crc32", 0x3874003e, 0xf8ff003f, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, RB, UIMM6_20 }, { C_NPS_R }},

/* crc32<.r> 0,b,limm		00111 bbb 00 110100 R BBB 111110 111110 */
{ "crc32", 0x38340fbe, 0xf8ff0fff, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, RB, LIMM }, { C_NPS_R }},

/* crc32<.r> a,b,limm		00111 bbb 00 110100 R BBB 111110 AAAAAA */
{ "crc32", 0x38340f80, 0xf8ff0fc0, ARC_OPCODE_NPS400, BITOP, NONE, { RA, RB, LIMM }, { C_NPS_R }},

/* crc32<.r> a,limm,limm	00111 110 00 110100 R 111 111110 AAAAAA */
{ "crc32", 0x3e347f80, 0xffff7fc0, ARC_OPCODE_NPS400, BITOP, NONE, { RA, LIMM, LIMMdup }, { C_NPS_R }},

/* crc32<.r> a,limm,u6		00111 110 01 110100 R 111 uuuuuu AAAAAA */
{ "crc32", 0x3e747000, 0xffff7000, ARC_OPCODE_NPS400, BITOP, NONE, { RA, LIMM, UIMM6_20 }, { C_NPS_R }},

/* crc32<.r> 0,limm,u6		00111 110 01 110100 R 111 uuuuuu 111110 */
{ "crc32", 0x3e74703e, 0xffff703f, ARC_OPCODE_NPS400, BITOP, NONE, { ZA, LIMM, UIMM6_20 }, { C_NPS_R }},

/****                 Arithmetic & Logic Instructions                 ****/

#define ADDB_LIKE(NAME,SUBOP2)                                          \
  { NAME, (0x48000000 | SUBOP2), 0xf80f001f, ARC_OPCODE_NPS400, ARITH, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_R_SRC2_3B, NPS_SRC1_POS, NPS_SRC2_POS, NPS_ADDB_SIZE }, { C_NPS_F, C_NPS_SX }},

ADDB_LIKE ("addb", 0)
ADDB_LIKE ("subb", 4)
ADDB_LIKE ("adcb", 5)
ADDB_LIKE ("sbcb", 6)

#define ANDB_LIKE(NAME,SUBOP2,SIZE_OPERAND)                             \
  { NAME, (0x48000000 | SUBOP2), 0xf80f001f, ARC_OPCODE_NPS400, ARITH, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_R_SRC2_3B, NPS_SRC1_POS, NPS_SRC2_POS, SIZE_OPERAND }, { C_NPS_F }},

ANDB_LIKE ("andb", 1, NPS_ANDB_SIZE)
ANDB_LIKE ("xorb", 2, NPS_ANDB_SIZE)
ANDB_LIKE ("orb", 3, NPS_ANDB_SIZE)
ANDB_LIKE ("fxorb", 7, NPS_FXORB_SIZE)
ANDB_LIKE ("wxorb", 8, NPS_WXORB_SIZE)
ANDB_LIKE ("shlb", 0xb, NPS_ANDB_SIZE)
ANDB_LIKE ("shrb", 0xc, NPS_ANDB_SIZE)

#define NOTB_LIKE(NAME,SUBOP2)                                          \
  { NAME, (0x48000000 | SUBOP2), 0xf80f001f, ARC_OPCODE_NPS400, ARITH, NONE, { NPS_R_DST_3B, NPS_R_SRC2_3B, NPS_SRC2_POS, NPS_ANDB_SIZE }, { C_NPS_F }},

NOTB_LIKE ("notb", 0x9)
NOTB_LIKE ("cntbb", 0xa)

#define DIV_LIKE(NAME,DIV_MODE)                                          \
  { NAME, (0x4800000d | DIV_MODE << 14), 0xf80fc3ff, ARC_OPCODE_NPS400, ARITH, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_R_SRC2_3B, NPS_SRC1_POS, NPS_SRC2_POS, }, { C_NPS_F }}, \
  { NAME, (0x4800020d | DIV_MODE << 14), 0xf8efc21f, ARC_OPCODE_NPS400, ARITH, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_DIV_UIMM4, NPS_SRC1_POS }, { C_NPS_F }},

DIV_LIKE ("div", 0x1)
DIV_LIKE ("mod", 0x2)
DIV_LIKE ("divm", 0x0)

{ "qcmp", 0x4810000e, 0xf81f001e, ARC_OPCODE_NPS400, ARITH, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_R_SRC2_3B, NPS_SRC2_POS, NPS_QCMP_SIZE, NPS_QCMP_M1, NPS_QCMP_M2, NPS_QCMP_M3 }, { C_NPS_AR_AL }},
{ "qcmp", 0x481001ee, 0xf81f01fe, ARC_OPCODE_NPS400, ARITH, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_R_SRC2_3B, NPS_SRC2_POS, NPS_QCMP_SIZE, NPS_QCMP_M1, NPS_QCMP_M2 }, { C_NPS_AR_AL }},
{ "qcmp", 0x481001ee, 0xf81f81fe, ARC_OPCODE_NPS400, ARITH, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_R_SRC2_3B, NPS_SRC2_POS, NPS_QCMP_SIZE, NPS_QCMP_M1  }, { C_NPS_AR_AL }},
{ "qcmp", 0x481001ee, 0xf81fc1fe, ARC_OPCODE_NPS400, ARITH, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_R_SRC2_3B, NPS_SRC2_POS, NPS_QCMP_SIZE  }, { C_NPS_AR_AL }},

{ "calcsd", 0x48000010, 0xf80f407f, ARC_OPCODE_NPS400, ARITH, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_R_SRC2_3B, NPS_CALC_ENTRY_SIZE }, { C_NPS_F }},
{ "calcxd", 0x48004010, 0xf80f407f, ARC_OPCODE_NPS400, ARITH, NONE, { NPS_R_DST_3B, NPS_R_SRC1_3B, NPS_R_SRC2_3B, NPS_CALC_ENTRY_SIZE }, { C_NPS_F }},

/****                  Protocol Decoder Instructions                  ****/

/* dctcp b,c  00111bbb001011110bbbcccccc000000 */
{ "dctcp", 0x382f0000, 0xf8ff803f, ARC_OPCODE_NPS400, NET, NONE, { RB, RC }, { 0 }},

/* dcip a,b,c  00111bbb001011110bbbccccccaaaaaa */
{ "dcip", 0x38290000, 0xf8ff8000, ARC_OPCODE_NPS400, NET, NONE, { RA, RB, RC }, { 0 }},

/* dcet b,c  00111bbb001011110bbbcccccc000010 */
{ "dcet", 0x382f0002, 0xf8ff803f, ARC_OPCODE_NPS400, NET, NONE, { RB, RC }, { 0 }},

/* dcet a,b,c  00111bbb001000000bbbccccccaaaaaa */
{ "dcet", 0x38200000, 0xf8ff8000, ARC_OPCODE_NPS400, NET, NONE, { RA, RB, RC }, { 0 }},

/****                        ACL Instructions                         ****/

/* dcacl<.f> a,b,c  00111bbb001001010bbbccccccaaaaaa */
{ "dcacl", 0x38250000, 0xf8ff0000, ARC_OPCODE_NPS400, ACL, NONE, { RA, RB, RC }, { C_F }},

/****                  Pipeline Control Instructions                  ****/

/* schd<.rw|.rd> */
{ "schd", 0x3e6f7004, 0xffffff7f, ARC_OPCODE_NPS400, CONTROL, NONE, { 0 }, { C_NPS_SCHD_RW }},

/* schd.wft.<.ie1|.ie2|.ie12> */
{ "schd", 0x3e6f7044, 0xfffffcff, ARC_OPCODE_NPS400, CONTROL, NONE, { 0 }, { C_NPS_SCHD_TRIG, C_NPS_SCHD_IE }},

/* sync<.rd|.wr> */
{ "sync", 0x3e6f703f, 0xffffffbf, ARC_OPCODE_NPS400, CONTROL, NONE, { 0 }, { C_NPS_SYNC }},

/* hwscd.off B */
{ "hwschd", 0x386f00bf, 0xf8ff8fff, ARC_OPCODE_NPS400, CONTROL, NONE, { RB }, { C_NPS_HWS_OFF }},

/* hwscd.restore 0,C */
{ "hwschd", 0x3e6f7003, 0xfffff03f, ARC_OPCODE_NPS400, CONTROL, NONE, { ZA, RC }, { C_NPS_HWS_RESTORE }},

/****      Load / Store From (0x57f00000 + Offset) Instructions       ****/

#define XLDST_LIKE(NAME,SUBOP2)                                          \
  { NAME, (0x58000000 | (SUBOP2 << 16)), 0xf81f0000, ARC_OPCODE_NPS400, MEMORY, NONE, { NPS_R_DST, BRAKET, NPS_XLDST_UIMM16, BRAKETdup }, { 0 }},

XLDST_LIKE("xldb", 0x8)
XLDST_LIKE("xldw", 0x9)
XLDST_LIKE("xld", 0xa)
XLDST_LIKE("xstb", 0xc)
XLDST_LIKE("xstw", 0xd)
XLDST_LIKE("xst", 0xe)
