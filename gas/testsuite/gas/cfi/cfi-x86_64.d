#readelf: -wf
#name: CFI on x86-64
The section .eh_frame contains:

00000000 00000012 00000000 CIE
  Version:               1
  Augmentation:          "zR"
  Code alignment factor: 1
  Data alignment factor: -8
  Return address column: 16
  Augmentation data:     1b

  DW_CFA_def_cfa: r7 ofs 8
  DW_CFA_offset: r16 at cfa-8

00000016 00000014 0000001a FDE cie=00000000 pc=0000001e..00000032
  DW_CFA_advance_loc: 7 to 00000025
  DW_CFA_def_cfa_offset: 4668
  DW_CFA_advance_loc: 12 to 00000031
  DW_CFA_def_cfa_offset: 8

0000002e 00000019 00000032 FDE cie=00000000 pc=00000036..00000045
  DW_CFA_advance_loc: 1 to 00000037
  DW_CFA_def_cfa_offset: 16
  DW_CFA_offset: r6 at cfa-16
  DW_CFA_advance_loc: 3 to 0000003a
  DW_CFA_def_cfa_reg: r6
  DW_CFA_advance_loc: 10 to 00000044
  DW_CFA_def_cfa: r7 ofs 8

0000004b 00000013 0000004f FDE cie=00000000 pc=00000053..00000066
  DW_CFA_advance_loc: 3 to 00000056
  DW_CFA_def_cfa_reg: r12
  DW_CFA_advance_loc: 15 to 00000065
  DW_CFA_def_cfa_reg: r7

00000062 0000000d 00000066 FDE cie=00000000 pc=0000006a..00000070

00000073 00000011 00000077 FDE cie=00000000 pc=0000007b..0000008d
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop

