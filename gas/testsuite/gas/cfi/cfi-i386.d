#readelf: -wf
#name: CFI on i386
The section .eh_frame contains:

00000000 00000012 00000000 CIE
  Version:               1
  Augmentation:          "zR"
  Code alignment factor: 1
  Data alignment factor: -4
  Return address column: 8
  Augmentation data:     1b

  DW_CFA_def_cfa: r4 ofs 4
  DW_CFA_offset: r8 at cfa-4

00000016 00000014 0000001a FDE cie=00000000 pc=0000001e..00000030
  DW_CFA_advance_loc: 6 to 00000024
  DW_CFA_def_cfa_offset: 4664
  DW_CFA_advance_loc: 11 to 0000002f
  DW_CFA_def_cfa_offset: 4

0000002e 00000018 00000032 FDE cie=00000000 pc=00000048..00000055
  DW_CFA_advance_loc: 1 to 00000049
  DW_CFA_def_cfa_offset: 8
  DW_CFA_offset: r5 at cfa-8
  DW_CFA_advance_loc: 2 to 0000004b
  DW_CFA_def_cfa_reg: r5
  DW_CFA_advance_loc: 9 to 00000054
  DW_CFA_def_cfa_reg: r4

0000004a 00000014 0000004e FDE cie=00000000 pc=00000071..00000081
  DW_CFA_advance_loc: 2 to 00000073
  DW_CFA_def_cfa_reg: r3
  DW_CFA_advance_loc: 13 to 00000080
  DW_CFA_def_cfa: r4 ofs 4

00000062 0000000d 00000066 FDE cie=00000000 pc=00000099..0000009f

00000073 0000000d 00000077 FDE cie=00000000 pc=000000b0..000000bf

