#readelf: -wf
#name: CFI on x86-64

The section .eh_frame contains:

00000000 00000014 00000000 CIE
  Version:               1
  Augmentation:          ""
  Code alignment factor: 1
  Data alignment factor: -8
  Return address column: 16

  DW_CFA_def_cfa: r7 ofs 8
  DW_CFA_offset: r16 at cfa-8
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop

00000018 0000001c 0000001c FDE cie=00000000 pc=00000000..00000014
  DW_CFA_advance_loc: 7 to 00000007
  DW_CFA_def_cfa_offset: 4668
  DW_CFA_advance_loc: 12 to 00000013
  DW_CFA_def_cfa_offset: 8
  DW_CFA_nop

00000038 00000024 0000003c FDE cie=00000000 pc=00000000..0000000f
  DW_CFA_advance_loc: 1 to 00000001
  DW_CFA_def_cfa_offset: 16
  DW_CFA_offset: r6 at cfa-16
  DW_CFA_advance_loc: 3 to 00000004
  DW_CFA_def_cfa_reg: r6
  DW_CFA_advance_loc: 10 to 0000000e
  DW_CFA_def_cfa: r7 ofs 8
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop

00000060 0000001c 00000064 FDE cie=00000000 pc=00000000..00000013
  DW_CFA_advance_loc: 3 to 00000003
  DW_CFA_def_cfa_reg: r12
  DW_CFA_advance_loc: 15 to 00000012
  DW_CFA_def_cfa_reg: r7
  DW_CFA_nop
  DW_CFA_nop

00000080 0000001c 00000084 FDE cie=00000000 pc=00000000..00000006
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop

000000a0 0000001c 000000a4 FDE cie=00000000 pc=00000000..00000012
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop

