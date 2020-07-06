static const struct dis386 evex_len_table[][3] = {
  /* EVEX_LEN_0F6E_P_2 */
  {
    { "vmovK",	{ XMScalar, Edq }, 0 },
  },

  /* EVEX_LEN_0F7E_P_1 */
  {
    { VEX_W_TABLE (EVEX_W_0F7E_P_1) },
  },

  /* EVEX_LEN_0F7E_P_2 */
  {
    { "vmovK",	{ Edq, XMScalar }, 0 },
  },

  /* EVEX_LEN_0FC4_P_2 */
  {
    { "vpinsrw",	{ XM, Vex128, Edqw, Ib }, 0 },
  },

  /* EVEX_LEN_0FC5_P_2 */
  {
    { "vpextrw",	{ Gdq, XS, Ib }, 0 },
  },

  /* EVEX_LEN_0FD6_P_2 */
  {
    { VEX_W_TABLE (EVEX_W_0FD6_P_2) },
  },

  /* EVEX_LEN_0F3816_P_2 */
  {
    { Bad_Opcode },
    { "vpermp%XW",	{ XM, Vex, EXx }, 0 },
    { "vpermp%XW",	{ XM, Vex, EXx }, 0 },
  },

  /* EVEX_LEN_0F3819_P_2_W_0 */
  {
    { Bad_Opcode },
    { "vbroadcastf32x2",	{ XM, EXxmm_mq }, 0 },
    { "vbroadcastf32x2",	{ XM, EXxmm_mq }, 0 },
  },

  /* EVEX_LEN_0F3819_P_2_W_1 */
  {
    { Bad_Opcode },
    { "vbroadcastsd",	{ XM, EXxmm_mq }, 0 },
    { "vbroadcastsd",	{ XM, EXxmm_mq }, 0 },
  },

  /* EVEX_LEN_0F381A_P_2_W_0_M_0 */
  {
    { Bad_Opcode },
    { "vbroadcastf32x4",	{ XM, EXxmm }, 0 },
    { "vbroadcastf32x4",	{ XM, EXxmm }, 0 },
  },

  /* EVEX_LEN_0F381A_P_2_W_1_M_0 */
  {
    { Bad_Opcode },
    { "vbroadcastf64x2",	{ XM, EXxmm }, 0 },
    { "vbroadcastf64x2",	{ XM, EXxmm }, 0 },
  },

  /* EVEX_LEN_0F381B_P_2_W_0_M_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vbroadcastf32x8",	{ XM, EXymm }, 0 },
  },

  /* EVEX_LEN_0F381B_P_2_W_1_M_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vbroadcastf64x4",	{ XM, EXymm }, 0 },
  },

  /* EVEX_LEN_0F3836_P_2 */
  {
    { Bad_Opcode },
    { "vperm%LW",	{ XM, Vex, EXx }, 0 },
    { "vperm%LW",	{ XM, Vex, EXx }, 0 },
  },

  /* EVEX_LEN_0F385A_P_2_W_0_M_0 */
  {
    { Bad_Opcode },
    { "vbroadcasti32x4",	{ XM, EXxmm }, 0 },
    { "vbroadcasti32x4",	{ XM, EXxmm }, 0 },
  },

  /* EVEX_LEN_0F385A_P_2_W_1_M_0 */
  {
    { Bad_Opcode },
    { "vbroadcasti64x2",	{ XM, EXxmm }, 0 },
    { "vbroadcasti64x2",	{ XM, EXxmm }, 0 },
  },

  /* EVEX_LEN_0F385B_P_2_W_0_M_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vbroadcasti32x8",	{ XM, EXymm }, 0 },
  },

  /* EVEX_LEN_0F385B_P_2_W_1_M_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vbroadcasti64x4",	{ XM, EXymm }, 0 },
  },

  /* EVEX_LEN_0F38C6_REG_1_PREFIX_2 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vgatherpf0dp%XW",  { MVexVSIBDWpX }, 0 },
  },

  /* EVEX_LEN_0F38C6_REG_2_PREFIX_2 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vgatherpf1dp%XW",  { MVexVSIBDWpX }, 0 },
  },

  /* EVEX_LEN_0F38C6_REG_5_PREFIX_2 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vscatterpf0dp%XW",  { MVexVSIBDWpX }, 0 },
  },

  /* EVEX_LEN_0F38C6_REG_6_PREFIX_2 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vscatterpf1dp%XW",  { MVexVSIBDWpX }, 0 },
  },

  /* EVEX_LEN_0F38C7_R_1_P_2_W_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vgatherpf0qps",  { MVexVSIBDQWpX }, 0 },
  },

  /* EVEX_LEN_0F38C7_R_1_P_2_W_1 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vgatherpf0qpd",  { MVexVSIBQWpX }, 0 },
  },

  /* EVEX_LEN_0F38C7_R_2_P_2_W_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vgatherpf1qps",  { MVexVSIBDQWpX }, 0 },
  },

  /* EVEX_LEN_0F38C7_R_2_P_2_W_1 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vgatherpf1qpd",  { MVexVSIBQWpX }, 0 },
  },

  /* EVEX_LEN_0F38C7_R_5_P_2_W_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vscatterpf0qps",  { MVexVSIBDQWpX }, 0 },
  },

  /* EVEX_LEN_0F38C7_R_5_P_2_W_1 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vscatterpf0qpd",  { MVexVSIBQWpX }, 0 },
  },

  /* EVEX_LEN_0F38C7_R_6_P_2_W_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vscatterpf1qps",  { MVexVSIBDQWpX }, 0 },
  },

  /* EVEX_LEN_0F38C7_R_6_P_2_W_1 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vscatterpf1qpd",  { MVexVSIBQWpX }, 0 },
  },

  /* EVEX_LEN_0F3A00_P_2_W_1 */
  {
    { Bad_Opcode },
    { "vpermq",	{ XM, EXx, Ib }, 0 },
    { "vpermq",	{ XM, EXx, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A01_P_2_W_1 */
  {
    { Bad_Opcode },
    { "vpermpd",	{ XM, EXx, Ib }, 0 },
    { "vpermpd",	{ XM, EXx, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A14_P_2 */
  {
    { "vpextrb",	{ Edqb, XM, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A15_P_2 */
  {
    { "vpextrw",	{ Edqw, XM, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A16_P_2 */
  {
    { "vpextrK",	{ Edq, XM, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A17_P_2 */
  {
    { "vextractps",	{ Edqd, XMM, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A18_P_2_W_0 */
  {
    { Bad_Opcode },
    { "vinsertf32x4",	{ XM, Vex, EXxmm, Ib }, 0 },
    { "vinsertf32x4",	{ XM, Vex, EXxmm, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A18_P_2_W_1 */
  {
    { Bad_Opcode },
    { "vinsertf64x2",	{ XM, Vex, EXxmm, Ib }, 0 },
    { "vinsertf64x2",	{ XM, Vex, EXxmm, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A19_P_2_W_0 */
  {
    { Bad_Opcode },
    { "vextractf32x4",	{ EXxmm, XM, Ib }, 0 },
    { "vextractf32x4",	{ EXxmm, XM, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A19_P_2_W_1 */
  {
    { Bad_Opcode },
    { "vextractf64x2",	{ EXxmm, XM, Ib }, 0 },
    { "vextractf64x2",	{ EXxmm, XM, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A1A_P_2_W_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vinsertf32x8",	{ XM, Vex, EXymm, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A1A_P_2_W_1 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vinsertf64x4",	{ XM, Vex, EXymm, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A1B_P_2_W_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vextractf32x8",	{ EXymm, XM, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A1B_P_2_W_1 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vextractf64x4",	{ EXymm, XM, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A20_P_2 */
  {
    { "vpinsrb",	{ XM, Vex128, Edqb, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A21_P_2_W_0 */
  {
    { "vinsertps",	{ XMM, Vex, EXxmm_md, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A22_P_2 */
  {
    { "vpinsrK",	{ XM, Vex128, Edq, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A23_P_2_W_0 */
  {
    { Bad_Opcode },
    { "vshuff32x4",	{ XM, Vex, EXx, Ib }, 0 },
    { "vshuff32x4",	{ XM, Vex, EXx, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A23_P_2_W_1 */
  {
    { Bad_Opcode },
    { "vshuff64x2",	{ XM, Vex, EXx, Ib }, 0 },
    { "vshuff64x2",	{ XM, Vex, EXx, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A38_P_2_W_0 */
  {
    { Bad_Opcode },
    { "vinserti32x4",	{ XM, Vex, EXxmm, Ib }, 0 },
    { "vinserti32x4",	{ XM, Vex, EXxmm, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A38_P_2_W_1 */
  {
    { Bad_Opcode },
    { "vinserti64x2",	{ XM, Vex, EXxmm, Ib }, 0 },
    { "vinserti64x2",	{ XM, Vex, EXxmm, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A39_P_2_W_0 */
  {
    { Bad_Opcode },
    { "vextracti32x4",	{ EXxmm, XM, Ib }, 0 },
    { "vextracti32x4",	{ EXxmm, XM, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A39_P_2_W_1 */
  {
    { Bad_Opcode },
    { "vextracti64x2",	{ EXxmm, XM, Ib }, 0 },
    { "vextracti64x2",	{ EXxmm, XM, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A3A_P_2_W_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vinserti32x8",	{ XM, Vex, EXymm, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A3A_P_2_W_1 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vinserti64x4",	{ XM, Vex, EXymm, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A3B_P_2_W_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vextracti32x8",	{ EXymm, XM, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A3B_P_2_W_1 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { "vextracti64x4",	{ EXymm, XM, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A43_P_2_W_0 */
  {
    { Bad_Opcode },
    { "vshufi32x4",	{ XM, Vex, EXx, Ib }, 0 },
    { "vshufi32x4",	{ XM, Vex, EXx, Ib }, 0 },
  },

  /* EVEX_LEN_0F3A43_P_2_W_1 */
  {
    { Bad_Opcode },
    { "vshufi64x2",	{ XM, Vex, EXx, Ib }, 0 },
    { "vshufi64x2",	{ XM, Vex, EXx, Ib }, 0 },
  },
};
