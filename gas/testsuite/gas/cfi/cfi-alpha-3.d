#readelf: -wf
#name: CFI on alpha, 3
The section .eh_frame contains:

00000000 0000000f 00000000 CIE
  Version:               1
  Augmentation:          "zR"
  Code alignment factor: 4
  Data alignment factor: -8
  Return address column: 26
  Augmentation data:     1b

  DW_CFA_def_cfa_reg: r30

00000013 00000029 00000017 FDE cie=00000000 pc=0000001b..0000005b
  DW_CFA_advance_loc: 4 to 0000001f
  DW_CFA_def_cfa_offset: 32
  DW_CFA_advance_loc: 4 to 00000023
  DW_CFA_offset: r26 at cfa-32
  DW_CFA_advance_loc: 4 to 00000027
  DW_CFA_offset: r9 at cfa-24
  DW_CFA_advance_loc: 4 to 0000002b
  DW_CFA_offset: r15 at cfa-16
  DW_CFA_advance_loc: 4 to 0000002f
  DW_CFA_offset: r34 at cfa-8
  DW_CFA_advance_loc: 4 to 00000033
  DW_CFA_def_cfa_reg: r15
  DW_CFA_advance_loc: 36 to 00000057
  DW_CFA_def_cfa: r30 ofs 0
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop

