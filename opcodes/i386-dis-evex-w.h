  /* EVEX_W_0F10_P_1 */
  {
    { "vmovss",	{ XMVexScalar, VexScalar, EXxmm_md }, 0 },
  },
  /* EVEX_W_0F10_P_3 */
  {
    { Bad_Opcode },
    { "vmovsd",	{ XMVexScalar, VexScalar, EXxmm_mq }, 0 },
  },
  /* EVEX_W_0F11_P_1 */
  {
    { "vmovss",	{ EXdVexScalarS, VexScalar, XMScalar }, 0 },
  },
  /* EVEX_W_0F11_P_3 */
  {
    { Bad_Opcode },
    { "vmovsd",	{ EXqVexScalarS, VexScalar, XMScalar }, 0 },
  },
  /* EVEX_W_0F12_P_0_M_1 */
  {
    { "vmovhlps",	{ XMM, Vex, EXxmm_mq }, 0 },
  },
  /* EVEX_W_0F12_P_1 */
  {
    { "vmovsldup",	{ XM, EXEvexXNoBcst }, 0 },
  },
  /* EVEX_W_0F12_P_3 */
  {
    { Bad_Opcode },
    { "vmovddup",	{ XM, EXymmq }, 0 },
  },
  /* EVEX_W_0F16_P_0_M_1 */
  {
    { "vmovlhps",	{ XMM, Vex, EXx }, 0 },
  },
  /* EVEX_W_0F16_P_1 */
  {
    { "vmovshdup",	{ XM, EXx }, 0 },
  },
  /* EVEX_W_0F2A_P_3 */
  {
    { "vcvtsi2sd{%LQ|}",	{ XMScalar, VexScalar, Ed }, 0 },
    { "vcvtsi2sd{%LQ|}",	{ XMScalar, VexScalar, EXxEVexR64, Edq }, 0 },
  },
  /* EVEX_W_0F51_P_1 */
  {
    { "vsqrtss",	{ XMScalar, VexScalar, EXxmm_md, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F51_P_3 */
  {
    { Bad_Opcode },
    { "vsqrtsd",	{ XMScalar, VexScalar, EXxmm_mq, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F58_P_1 */
  {
    { "vaddss",	{ XMScalar, VexScalar, EXxmm_md, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F58_P_3 */
  {
    { Bad_Opcode },
    { "vaddsd",	{ XMScalar, VexScalar, EXxmm_mq, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F59_P_1 */
  {
    { "vmulss",	{ XMScalar, VexScalar, EXxmm_md, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F59_P_3 */
  {
    { Bad_Opcode },
    { "vmulsd",	{ XMScalar, VexScalar, EXxmm_mq, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F5A_P_0 */
  {
    { "vcvtps2pd",   { XM, EXEvexHalfBcstXmmq, EXxEVexS }, 0 },
  },
  /* EVEX_W_0F5A_P_1 */
  {
    { "vcvtss2sd",	{ XMScalar, VexScalar, EXxmm_md, EXxEVexS }, 0 },
  },
  /* EVEX_W_0F5A_P_2 */
  {
    { Bad_Opcode },
    { "vcvtpd2ps%XY",	{ XMxmmq, EXx, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F5A_P_3 */
  {
    { Bad_Opcode },
    { "vcvtsd2ss",	{ XMScalar, VexScalar, EXxmm_mq, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F5B_P_0 */
  {
    { "vcvtdq2ps",	{ XM, EXx, EXxEVexR }, 0 },
    { "vcvtqq2ps%XY",	{ XMxmmq, EXx, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F5B_P_1 */
  {
    { "vcvttps2dq",	{ XM, EXx, EXxEVexS }, 0 },
  },
  /* EVEX_W_0F5B_P_2 */
  {
    { "vcvtps2dq",	{ XM, EXx, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F5C_P_1 */
  {
    { "vsubss",	{ XMScalar, VexScalar, EXxmm_md, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F5C_P_3 */
  {
    { Bad_Opcode },
    { "vsubsd",	{ XMScalar, VexScalar, EXxmm_mq, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F5D_P_1 */
  {
    { "vminss",	{ XMScalar, VexScalar, EXxmm_md, EXxEVexS }, 0 },
  },
  /* EVEX_W_0F5D_P_3 */
  {
    { Bad_Opcode },
    { "vminsd",	{ XMScalar, VexScalar, EXxmm_mq, EXxEVexS }, 0 },
  },
  /* EVEX_W_0F5E_P_1 */
  {
    { "vdivss",	{ XMScalar, VexScalar, EXxmm_md, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F5E_P_3 */
  {
    { Bad_Opcode },
    { "vdivsd",	{ XMScalar, VexScalar, EXxmm_mq, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F5F_P_1 */
  {
    { "vmaxss",	{ XMScalar, VexScalar, EXxmm_md, EXxEVexS }, 0 },
  },
  /* EVEX_W_0F5F_P_3 */
  {
    { Bad_Opcode },
    { "vmaxsd",	{ XMScalar, VexScalar, EXxmm_mq, EXxEVexS }, 0 },
  },
  /* EVEX_W_0F62 */
  {
    { PREFIX_TABLE (PREFIX_VEX_0F62) },
  },
  /* EVEX_W_0F66_P_2 */
  {
    { "vpcmpgtd",	{ XMask, Vex, EXx }, 0 },
  },
  /* EVEX_W_0F6A */
  {
    { PREFIX_TABLE (PREFIX_VEX_0F6A) },
  },
  /* EVEX_W_0F6B */
  {
    { PREFIX_TABLE (PREFIX_VEX_0F6B) },
  },
  /* EVEX_W_0F6C */
  {
    { Bad_Opcode },
    { PREFIX_TABLE (PREFIX_VEX_0F6C) },
  },
  /* EVEX_W_0F6D */
  {
    { Bad_Opcode },
    { PREFIX_TABLE (PREFIX_VEX_0F6D) },
  },
  /* EVEX_W_0F6F_P_1 */
  {
    { "vmovdqu32",	{ XM, EXEvexXNoBcst }, 0 },
    { "vmovdqu64",	{ XM, EXEvexXNoBcst }, 0 },
  },
  /* EVEX_W_0F6F_P_2 */
  {
    { "vmovdqa32",	{ XM, EXEvexXNoBcst }, 0 },
    { "vmovdqa64",	{ XM, EXEvexXNoBcst }, 0 },
  },
  /* EVEX_W_0F6F_P_3 */
  {
    { "vmovdqu8",	{ XM, EXx }, 0 },
    { "vmovdqu16",	{ XM, EXx }, 0 },
  },
  /* EVEX_W_0F70_P_2 */
  {
    { "vpshufd",	{ XM, EXx, Ib }, 0 },
  },
  /* EVEX_W_0F72_R_2_P_2 */
  {
    { "vpsrld",	{ Vex, EXx, Ib }, 0 },
  },
  /* EVEX_W_0F72_R_6_P_2 */
  {
    { "vpslld",	{ Vex, EXx, Ib }, 0 },
  },
  /* EVEX_W_0F73_R_2_P_2 */
  {
    { Bad_Opcode },
    { "vpsrlq",	{ Vex, EXx, Ib }, 0 },
  },
  /* EVEX_W_0F73_R_6_P_2 */
  {
    { Bad_Opcode },
    { "vpsllq",	{ Vex, EXx, Ib }, 0 },
  },
  /* EVEX_W_0F76_P_2 */
  {
    { "vpcmpeqd",	{ XMask, Vex, EXx }, 0 },
  },
  /* EVEX_W_0F78_P_0 */
  {
    { "vcvttps2udq",	{ XM, EXx, EXxEVexS }, 0 },
    { "vcvttpd2udq%XY",	{ XMxmmq, EXx, EXxEVexS }, 0 },
  },
  /* EVEX_W_0F78_P_2 */
  {
    { "vcvttps2uqq",	{ XM, EXEvexHalfBcstXmmq, EXxEVexS }, 0 },
    { "vcvttpd2uqq",	{ XM, EXx, EXxEVexS }, 0 },
  },
  /* EVEX_W_0F79_P_0 */
  {
    { "vcvtps2udq",	{ XM, EXx, EXxEVexR }, 0 },
    { "vcvtpd2udq%XY",	{ XMxmmq, EXx, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F79_P_2 */
  {
    { "vcvtps2uqq",	{ XM, EXEvexHalfBcstXmmq, EXxEVexR }, 0 },
    { "vcvtpd2uqq",	{ XM, EXx, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F7A_P_1 */
  {
    { "vcvtudq2pd",	{ XM, EXEvexHalfBcstXmmq }, 0 },
    { "vcvtuqq2pd",	{ XM, EXx, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F7A_P_2 */
  {
    { "vcvttps2qq",	{ XM, EXEvexHalfBcstXmmq, EXxEVexS }, 0 },
    { "vcvttpd2qq",	{ XM, EXx, EXxEVexS }, 0 },
  },
  /* EVEX_W_0F7A_P_3 */
  {
    { "vcvtudq2ps",	{ XM, EXx, EXxEVexR }, 0 },
    { "vcvtuqq2ps%XY",	{ XMxmmq, EXx, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F7B_P_2 */
  {
    { "vcvtps2qq",	{ XM, EXEvexHalfBcstXmmq, EXxEVexR }, 0 },
    { "vcvtpd2qq",	{ XM, EXx, EXxEVexR }, 0 },
  },
  /* EVEX_W_0F7B_P_3 */
  {
    { "vcvtusi2sd{%LQ|}",	{ XMScalar, VexScalar, Ed }, 0 },
    { "vcvtusi2sd{%LQ|}",	{ XMScalar, VexScalar, EXxEVexR64, Edq }, 0 },
  },
  /* EVEX_W_0F7E_P_1 */
  {
    { Bad_Opcode },
    { "vmovq",	{ XMScalar, EXxmm_mq }, 0 },
  },
  /* EVEX_W_0F7F_P_1 */
  {
    { "vmovdqu32",	{ EXxS, XM }, 0 },
    { "vmovdqu64",	{ EXxS, XM }, 0 },
  },
  /* EVEX_W_0F7F_P_2 */
  {
    { "vmovdqa32",	{ EXxS, XM }, 0 },
    { "vmovdqa64",	{ EXxS, XM }, 0 },
  },
  /* EVEX_W_0F7F_P_3 */
  {
    { "vmovdqu8",	{ EXxS, XM }, 0 },
    { "vmovdqu16",	{ EXxS, XM }, 0 },
  },
  /* EVEX_W_0FC2_P_1 */
  {
    { "vcmpss",	{ XMask, VexScalar, EXxmm_md, EXxEVexS, CMP }, 0 },
  },
  /* EVEX_W_0FC2_P_3 */
  {
    { Bad_Opcode },
    { "vcmpsd",	{ XMask, VexScalar, EXxmm_mq, EXxEVexS, CMP }, 0 },
  },
  /* EVEX_W_0FD2 */
  {
    { PREFIX_TABLE (PREFIX_VEX_0FD2) },
  },
  /* EVEX_W_0FD3 */
  {
    { Bad_Opcode },
    { PREFIX_TABLE (PREFIX_VEX_0FD3) },
  },
  /* EVEX_W_0FD4 */
  {
    { Bad_Opcode },
    { PREFIX_TABLE (PREFIX_VEX_0FD4) },
  },
  /* EVEX_W_0FD6_P_2 */
  {
    { Bad_Opcode },
    { "vmovq",	{ EXqVexScalarS, XMScalar }, 0 },
  },
  /* EVEX_W_0FE6_P_1 */
  {
    { "vcvtdq2pd",	{ XM, EXEvexHalfBcstXmmq }, 0 },
    { "vcvtqq2pd",	{ XM, EXx, EXxEVexR }, 0 },
  },
  /* EVEX_W_0FE6_P_2 */
  {
    { Bad_Opcode },
    { "vcvttpd2dq%XY",	{ XMxmmq, EXx, EXxEVexS }, 0 },
  },
  /* EVEX_W_0FE6_P_3 */
  {
    { Bad_Opcode },
    { "vcvtpd2dq%XY",	{ XMxmmq, EXx, EXxEVexR }, 0 },
  },
  /* EVEX_W_0FE7_P_2 */
  {
    { "vmovntdq",	{ EXEvexXNoBcst, XM }, 0 },
  },
  /* EVEX_W_0FF2 */
  {
    { PREFIX_TABLE (PREFIX_VEX_0FF2) },
  },
  /* EVEX_W_0FF3 */
  {
    { Bad_Opcode },
    { PREFIX_TABLE (PREFIX_VEX_0FF3) },
  },
  /* EVEX_W_0FF4 */
  {
    { Bad_Opcode },
    { PREFIX_TABLE (PREFIX_VEX_0FF4) },
  },
  /* EVEX_W_0FFA */
  {
    { PREFIX_TABLE (PREFIX_VEX_0FFA) },
  },
  /* EVEX_W_0FFB */
  {
    { Bad_Opcode },
    { PREFIX_TABLE (PREFIX_VEX_0FFB) },
  },
  /* EVEX_W_0FFE */
  {
    { PREFIX_TABLE (PREFIX_VEX_0FFE) },
  },
  /* EVEX_W_0F380D_P_2 */
  {
    { Bad_Opcode },
    { "vpermilpd",	{ XM, Vex, EXx }, 0 },
  },
  /* EVEX_W_0F3810_P_1 */
  {
    { "vpmovuswb",	{ EXxmmq, XM }, 0 },
  },
  /* EVEX_W_0F3810_P_2 */
  {
    { Bad_Opcode },
    { "vpsrlvw",	{ XM, Vex, EXx }, 0 },
  },
  /* EVEX_W_0F3811_P_1 */
  {
    { "vpmovusdb",	{ EXxmmqd, XM }, 0 },
  },
  /* EVEX_W_0F3811_P_2 */
  {
    { Bad_Opcode },
    { "vpsravw",	{ XM, Vex, EXx }, 0 },
  },
  /* EVEX_W_0F3812_P_1 */
  {
    { "vpmovusqb",	{ EXxmmdw, XM }, 0 },
  },
  /* EVEX_W_0F3812_P_2 */
  {
    { Bad_Opcode },
    { "vpsllvw",	{ XM, Vex, EXx }, 0 },
  },
  /* EVEX_W_0F3813_P_1 */
  {
    { "vpmovusdw",	{ EXxmmq, XM }, 0 },
  },
  /* EVEX_W_0F3813_P_2 */
  {
    { "vcvtph2ps",	{ XM, EXxmmq, EXxEVexS }, 0 },
  },
  /* EVEX_W_0F3814_P_1 */
  {
    { "vpmovusqw",	{ EXxmmqd, XM }, 0 },
  },
  /* EVEX_W_0F3815_P_1 */
  {
    { "vpmovusqd",	{ EXxmmq, XM }, 0 },
  },
  /* EVEX_W_0F3819_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F3819_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F3819_P_2_W_1) },
  },
  /* EVEX_W_0F381A_P_2 */
  {
    { MOD_TABLE (MOD_EVEX_0F381A_P_2_W_0) },
    { MOD_TABLE (MOD_EVEX_0F381A_P_2_W_1) },
  },
  /* EVEX_W_0F381B_P_2 */
  {
    { MOD_TABLE (MOD_EVEX_0F381B_P_2_W_0) },
    { MOD_TABLE (MOD_EVEX_0F381B_P_2_W_1) },
  },
  /* EVEX_W_0F381E_P_2 */
  {
    { "vpabsd",	{ XM, EXx }, 0 },
  },
  /* EVEX_W_0F381F_P_2 */
  {
    { Bad_Opcode },
    { "vpabsq",	{ XM, EXx }, 0 },
  },
  /* EVEX_W_0F3820_P_1 */
  {
    { "vpmovswb",	{ EXxmmq, XM }, 0 },
  },
  /* EVEX_W_0F3821_P_1 */
  {
    { "vpmovsdb",	{ EXxmmqd, XM }, 0 },
  },
  /* EVEX_W_0F3822_P_1 */
  {
    { "vpmovsqb",	{ EXxmmdw, XM }, 0 },
  },
  /* EVEX_W_0F3823_P_1 */
  {
    { "vpmovsdw",	{ EXxmmq, XM }, 0 },
  },
  /* EVEX_W_0F3824_P_1 */
  {
    { "vpmovsqw",	{ EXxmmqd, XM }, 0 },
  },
  /* EVEX_W_0F3825_P_1 */
  {
    { "vpmovsqd",	{ EXxmmq, XM }, 0 },
  },
  /* EVEX_W_0F3825_P_2 */
  {
    { "vpmovsxdq",	{ XM, EXxmmq }, 0 },
  },
  /* EVEX_W_0F3828_P_2 */
  {
    { Bad_Opcode },
    { "vpmuldq",	{ XM, Vex, EXx }, 0 },
  },
  /* EVEX_W_0F3829_P_2 */
  {
    { Bad_Opcode },
    { "vpcmpeqq",	{ XMask, Vex, EXx }, 0 },
  },
  /* EVEX_W_0F382A_P_1 */
  {
    { Bad_Opcode },
    { "vpbroadcastmb2q",	{ XM, MaskR }, 0 },
  },
  /* EVEX_W_0F382A_P_2 */
  {
    { "vmovntdqa",	{ XM, EXEvexXNoBcst }, 0 },
  },
  /* EVEX_W_0F382B */
  {
    { PREFIX_TABLE (PREFIX_VEX_0F382B) },
  },
  /* EVEX_W_0F3830_P_1 */
  {
    { "vpmovwb",	{ EXxmmq, XM }, 0 },
  },
  /* EVEX_W_0F3831_P_1 */
  {
    { "vpmovdb",	{ EXxmmqd, XM }, 0 },
  },
  /* EVEX_W_0F3832_P_1 */
  {
    { "vpmovqb",	{ EXxmmdw, XM }, 0 },
  },
  /* EVEX_W_0F3833_P_1 */
  {
    { "vpmovdw",	{ EXxmmq, XM }, 0 },
  },
  /* EVEX_W_0F3834_P_1 */
  {
    { "vpmovqw",	{ EXxmmqd, XM }, 0 },
  },
  /* EVEX_W_0F3835_P_1 */
  {
    { "vpmovqd",	{ EXxmmq, XM }, 0 },
  },
  /* EVEX_W_0F3835_P_2 */
  {
    { "vpmovzxdq",	{ XM, EXxmmq }, 0 },
  },
  /* EVEX_W_0F3837_P_2 */
  {
    { Bad_Opcode },
    { "vpcmpgtq",	{ XMask, Vex, EXx }, 0 },
  },
  /* EVEX_W_0F383A_P_1 */
  {
    { "vpbroadcastmw2d",	{ XM, MaskR }, 0 },
  },
  /* EVEX_W_0F3852_P_1 */
  {
    { "vdpbf16ps",	{ XM, Vex, EXx }, 0 },
    { Bad_Opcode },
  },
  /* EVEX_W_0F3859_P_2 */
  {
    { "vbroadcasti32x2",	{ XM, EXxmm_mq }, 0 },
    { "vpbroadcastq",	{ XM, EXxmm_mq }, 0 },
  },
  /* EVEX_W_0F385A_P_2 */
  {
    { MOD_TABLE (MOD_EVEX_0F385A_P_2_W_0) },
    { MOD_TABLE (MOD_EVEX_0F385A_P_2_W_1) },
  },
  /* EVEX_W_0F385B_P_2 */
  {
    { MOD_TABLE (MOD_EVEX_0F385B_P_2_W_0) },
    { MOD_TABLE (MOD_EVEX_0F385B_P_2_W_1) },
  },
  /* EVEX_W_0F3862_P_2 */
  {
    { "vpexpandb", { XM, EXbScalar }, 0 },
    { "vpexpandw", { XM, EXwScalar }, 0 },
  },
  /* EVEX_W_0F3863_P_2 */
  {
    { "vpcompressb",   { EXbScalar, XM }, 0 },
    { "vpcompressw",   { EXwScalar, XM }, 0 },
  },
  /* EVEX_W_0F3870_P_2 */
  {
    { Bad_Opcode },
    { "vpshldvw",  { XM, Vex, EXx }, 0 },
  },
  /* EVEX_W_0F3872_P_1 */
  {
    { "vcvtneps2bf16%XY", { XMxmmq, EXx }, 0 },
    { Bad_Opcode },
  },
  /* EVEX_W_0F3872_P_2 */
  {
    { Bad_Opcode },
    { "vpshrdvw",  { XM, Vex, EXx }, 0 },
  },
  /* EVEX_W_0F3872_P_3 */
  {
    { "vcvtne2ps2bf16", { XM, Vex, EXx}, 0 },
    { Bad_Opcode },
  },
  /* EVEX_W_0F387A_P_2 */
  {
    { "vpbroadcastb",	{ XM, Rd }, 0 },
  },
  /* EVEX_W_0F387B_P_2 */
  {
    { "vpbroadcastw",	{ XM, Rd }, 0 },
  },
  /* EVEX_W_0F3883_P_2 */
  {
    { Bad_Opcode },
    { "vpmultishiftqb",	{ XM, Vex, EXx }, 0 },
  },
  /* EVEX_W_0F3891_P_2 */
  {
    { "vpgatherqd",	{ XMxmmq, MVexVSIBQDWpX }, 0 },
    { "vpgatherqq",	{ XM, MVexVSIBQWpX }, 0 },
  },
  /* EVEX_W_0F3893_P_2 */
  {
    { "vgatherqps",	{ XMxmmq, MVexVSIBQDWpX }, 0 },
    { "vgatherqpd",	{ XM, MVexVSIBQWpX }, 0 },
  },
  /* EVEX_W_0F38A1_P_2 */
  {
    { "vpscatterqd",	{ MVexVSIBQDWpX, XMxmmq }, 0 },
    { "vpscatterqq",	{ MVexVSIBQWpX, XM }, 0 },
  },
  /* EVEX_W_0F38A3_P_2 */
  {
    { "vscatterqps",	{ MVexVSIBQDWpX, XMxmmq }, 0 },
    { "vscatterqpd",	{ MVexVSIBQWpX, XM }, 0 },
  },
  /* EVEX_W_0F38C7_R_1_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F38C7_R_1_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F38C7_R_1_P_2_W_1) },
  },
  /* EVEX_W_0F38C7_R_2_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F38C7_R_2_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F38C7_R_2_P_2_W_1) },
  },
  /* EVEX_W_0F38C7_R_5_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F38C7_R_5_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F38C7_R_5_P_2_W_1) },
  },
  /* EVEX_W_0F38C7_R_6_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F38C7_R_6_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F38C7_R_6_P_2_W_1) },
  },
  /* EVEX_W_0F3A00_P_2 */
  {
    { Bad_Opcode },
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A00_P_2_W_1) },
  },
  /* EVEX_W_0F3A01_P_2 */
  {
    { Bad_Opcode },
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A01_P_2_W_1) },
  },
  /* EVEX_W_0F3A05_P_2 */
  {
    { Bad_Opcode },
    { "vpermilpd",	{ XM, EXx, Ib }, 0 },
  },
  /* EVEX_W_0F3A08_P_2 */
  {
    { "vrndscaleps",	{ XM, EXx, EXxEVexS, Ib }, 0 },
  },
  /* EVEX_W_0F3A09_P_2 */
  {
    { Bad_Opcode },
    { "vrndscalepd",	{ XM, EXx, EXxEVexS, Ib }, 0 },
  },
  /* EVEX_W_0F3A0A_P_2 */
  {
    { "vrndscaless",	{ XMScalar, VexScalar, EXxmm_md, EXxEVexS, Ib }, 0 },
  },
  /* EVEX_W_0F3A0B_P_2 */
  {
    { Bad_Opcode },
    { "vrndscalesd",	{ XMScalar, VexScalar, EXxmm_mq, EXxEVexS, Ib }, 0 },
  },
  /* EVEX_W_0F3A18_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A18_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A18_P_2_W_1) },
  },
  /* EVEX_W_0F3A19_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A19_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A19_P_2_W_1) },
  },
  /* EVEX_W_0F3A1A_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A1A_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A1A_P_2_W_1) },
  },
  /* EVEX_W_0F3A1B_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A1B_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A1B_P_2_W_1) },
  },
  /* EVEX_W_0F3A21_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A21_P_2_W_0) },
  },
  /* EVEX_W_0F3A23_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A23_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A23_P_2_W_1) },
  },
  /* EVEX_W_0F3A38_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A38_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A38_P_2_W_1) },
  },
  /* EVEX_W_0F3A39_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A39_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A39_P_2_W_1) },
  },
  /* EVEX_W_0F3A3A_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A3A_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A3A_P_2_W_1) },
  },
  /* EVEX_W_0F3A3B_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A3B_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A3B_P_2_W_1) },
  },
  /* EVEX_W_0F3A42_P_2 */
  {
    { "vdbpsadbw",	{ XM, Vex, EXx, Ib }, 0 },
  },
  /* EVEX_W_0F3A43_P_2 */
  {
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A43_P_2_W_0) },
    { EVEX_LEN_TABLE (EVEX_LEN_0F3A43_P_2_W_1) },
  },
  /* EVEX_W_0F3A70_P_2 */
  {
    { Bad_Opcode },
    { "vpshldw",   { XM, Vex, EXx, Ib }, 0 },
  },
  /* EVEX_W_0F3A72_P_2 */
  {
    { Bad_Opcode },
    { "vpshrdw",   { XM, Vex, EXx, Ib }, 0 },
  },
