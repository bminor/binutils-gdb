#readelf: -wf
#name: CFI on i386

The section .eh_frame contains:

00000000 00000010 00000000 CIE
  Version:               1
  Augmentation:          ""
  Code alignment factor: 1
  Data alignment factor: -4
  Return address column: 8

  DW_CFA_def_cfa: r7 ofs 4
  DW_CFA_offset: r8 at cfa-4
  DW_CFA_nop
  DW_CFA_nop

00000014 00000014 00000018 FDE cie=00000000 pc=00000000..00000012
  DW_CFA_advance_loc: 6 to 00000006
  DW_CFA_def_cfa_offset: 4664
  DW_CFA_advance_loc: 11 to 00000011
  DW_CFA_def_cfa_offset: 4
  DW_CFA_nop

0000002c 00000010 00000000 CIE
  Version:               1
  Augmentation:          ""
  Code alignment factor: 1
  Data alignment factor: -4
  Return address column: 8

  DW_CFA_def_cfa: r7 ofs 4
  DW_CFA_offset: r8 at cfa-4
  DW_CFA_nop
  DW_CFA_nop

00000040 00000018 00000018 FDE cie=0000002c pc=00000012..0000001f
  DW_CFA_advance_loc: 1 to 00000013
  DW_CFA_def_cfa_offset: 8
  DW_CFA_offset: r6 at cfa-8
  DW_CFA_advance_loc: 2 to 00000015
  DW_CFA_def_cfa_reg: r6
  DW_CFA_advance_loc: 9 to 0000001e
  DW_CFA_def_cfa_reg: r7
  DW_CFA_nop

0000005c 00000010 00000000 CIE
  Version:               1
  Augmentation:          ""
  Code alignment factor: 1
  Data alignment factor: -4
  Return address column: 8

  DW_CFA_def_cfa: r7 ofs 4
  DW_CFA_offset: r8 at cfa-4
  DW_CFA_nop
  DW_CFA_nop

00000070 00000014 00000018 FDE cie=0000005c pc=0000001f..0000002f
  DW_CFA_advance_loc: 2 to 00000021
  DW_CFA_def_cfa_reg: r1
  DW_CFA_advance_loc: 13 to 0000002e
  DW_CFA_def_cfa: r7 ofs 4
  DW_CFA_nop

00000088 00000010 00000000 CIE
  Version:               1
  Augmentation:          ""
  Code alignment factor: 1
  Data alignment factor: -4
  Return address column: 8

  DW_CFA_def_cfa: r7 ofs 4
  DW_CFA_offset: r8 at cfa-4
  DW_CFA_nop
  DW_CFA_nop

0000009c 00000010 00000018 FDE cie=00000088 pc=0000002f..00000035
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop

000000b0 00000010 00000000 CIE
  Version:               1
  Augmentation:          ""
  Code alignment factor: 1
  Data alignment factor: -4
  Return address column: 8

  DW_CFA_def_cfa: r7 ofs 4
  DW_CFA_offset: r8 at cfa-4
  DW_CFA_nop
  DW_CFA_nop

000000c4 00000010 00000018 FDE cie=000000b0 pc=00000035..00000044
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop

