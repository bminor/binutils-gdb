static const struct dis386 evex_len_table[][3] = {
  /* EVEX_LEN_0F6E */
  {
    { "vmovK",	{ XMScalar, Edq }, PREFIX_DATA },
  },

  /* EVEX_LEN_0F7E_P_1 */
  {
    { VEX_W_TABLE (EVEX_W_0F7E_P_1) },
  },

  /* EVEX_LEN_0F7E_P_2 */
  {
    { "vmovK",	{ Edq, XMScalar }, 0 },
  },

  /* EVEX_LEN_0FC4 */
  {
    { "vpinsrw",	{ XM, Vex, Edqw, Ib }, PREFIX_DATA },
  },

  /* EVEX_LEN_0FC5 */
  {
    { "vpextrw",	{ Gdq, XS, Ib }, PREFIX_DATA },
  },

  /* EVEX_LEN_0FD6 */
  {
    { VEX_W_TABLE (EVEX_W_0FD6_L_0) },
  },

  /* EVEX_LEN_0F3816 */
  {
    { Bad_Opcode },
    { "vpermp%XW",	{ XM, Vex, EXx }, PREFIX_DATA },
    { "vpermp%XW",	{ XM, Vex, EXx }, PREFIX_DATA },
  },

  /* EVEX_LEN_0F3819 */
  {
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F3819_L_n) },
    { VEX_W_TABLE (EVEX_W_0F3819_L_n) },
  },

  /* EVEX_LEN_0F381A_M_0 */
  {
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F381A_M_0_L_n) },
    { VEX_W_TABLE (EVEX_W_0F381A_M_0_L_n) },
  },

  /* EVEX_LEN_0F381B_M_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F381B_M_0_L_2) },
  },

  /* EVEX_LEN_0F3836 */
  {
    { Bad_Opcode },
    { "vperm%DQ",	{ XM, Vex, EXx }, PREFIX_DATA },
    { "vperm%DQ",	{ XM, Vex, EXx }, PREFIX_DATA },
  },

  /* EVEX_LEN_0F385A_M_0 */
  {
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F385A_M_0_L_n) },
    { VEX_W_TABLE (EVEX_W_0F385A_M_0_L_n) },
  },

  /* EVEX_LEN_0F385B_M_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F385B_M_0_L_2) },
  },

  /* EVEX_LEN_0F38C6_M_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { REG_TABLE (REG_EVEX_0F38C6_M_0_L_2) },
  },

  /* EVEX_LEN_0F38C7_M_0 */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F38C7_M_0_L_2) },
  },

  /* EVEX_LEN_0F3A00 */
  {
    { Bad_Opcode },
    { VEX_W_TABLE (VEX_W_0F3A00_L_1) },
    { VEX_W_TABLE (VEX_W_0F3A00_L_1) },
  },

  /* EVEX_LEN_0F3A01 */
  {
    { Bad_Opcode },
    { VEX_W_TABLE (VEX_W_0F3A01_L_1) },
    { VEX_W_TABLE (VEX_W_0F3A01_L_1) },
  },

  /* EVEX_LEN_0F3A14 */
  {
    { "vpextrb",	{ Edqb, XM, Ib }, PREFIX_DATA },
  },

  /* EVEX_LEN_0F3A15 */
  {
    { "vpextrw",	{ Edqw, XM, Ib }, PREFIX_DATA },
  },

  /* EVEX_LEN_0F3A16 */
  {
    { "vpextrK",	{ Edq, XM, Ib }, PREFIX_DATA },
  },

  /* EVEX_LEN_0F3A17 */
  {
    { "vextractps",	{ Edqd, XMM, Ib }, PREFIX_DATA },
  },

  /* EVEX_LEN_0F3A18 */
  {
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F3A18_L_n) },
    { VEX_W_TABLE (EVEX_W_0F3A18_L_n) },
  },

  /* EVEX_LEN_0F3A19 */
  {
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F3A19_L_n) },
    { VEX_W_TABLE (EVEX_W_0F3A19_L_n) },
  },

  /* EVEX_LEN_0F3A1A */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F3A1A_L_2) },
  },

  /* EVEX_LEN_0F3A1B */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F3A1B_L_2) },
  },

  /* EVEX_LEN_0F3A20 */
  {
    { "vpinsrb",	{ XM, Vex, Edqb, Ib }, PREFIX_DATA },
  },

  /* EVEX_LEN_0F3A21_W_0 */
  {
    { "vinsertps",	{ XMM, Vex, EXxmm_md, Ib }, PREFIX_DATA },
  },

  /* EVEX_LEN_0F3A22 */
  {
    { "vpinsrK",	{ XM, Vex, Edq, Ib }, PREFIX_DATA },
  },

  /* EVEX_LEN_0F3A23 */
  {
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F3A23_L_n) },
    { VEX_W_TABLE (EVEX_W_0F3A23_L_n) },
  },

  /* EVEX_LEN_0F3A38 */
  {
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F3A38_L_n) },
    { VEX_W_TABLE (EVEX_W_0F3A38_L_n) },
  },

  /* EVEX_LEN_0F3A39 */
  {
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F3A39_L_n) },
    { VEX_W_TABLE (EVEX_W_0F3A39_L_n) },
  },

  /* EVEX_LEN_0F3A3A */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F3A3A_L_2) },
  },

  /* EVEX_LEN_0F3A3B */
  {
    { Bad_Opcode },
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F3A3B_L_2) },
  },

  /* EVEX_LEN_0F3A43 */
  {
    { Bad_Opcode },
    { VEX_W_TABLE (EVEX_W_0F3A43_L_n) },
    { VEX_W_TABLE (EVEX_W_0F3A43_L_n) },
  },
};
