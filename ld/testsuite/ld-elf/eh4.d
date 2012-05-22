#source: eh4.s
#source: eh4a.s
#as: --64
#ld: -melf_x86_64 -shared -Ttext 0x400
#readelf: -wf
#notarget: x86_64-*-linux-gnux32 x86_64-*-nacl*
#target: x86_64-*-*

Contents of the .eh_frame section:

00000000 00000014 00000000 CIE
  Version:               1
  Augmentation:          "zR"
  Code alignment factor: 1
  Data alignment factor: -8
  Return address column: 16
  Augmentation data:     1b

  DW_CFA_def_cfa: r7 \(rsp\) ofs 8
  DW_CFA_offset: r16 \(rip\) at cfa-8
  DW_CFA_nop
  DW_CFA_nop

00000018 00000014 0000001c FDE cie=00000000 pc=00000400..00000413
  DW_CFA_set_loc: 00000404
  DW_CFA_def_cfa_offset: 80

00000030 00000014 00000034 FDE cie=00000000 pc=00000413..00000426
  DW_CFA_set_loc: 00000417
  DW_CFA_def_cfa_offset: 80

00000048 00000024 0000004c FDE cie=00000000 pc=00000240..00000260
  DW_CFA_def_cfa_offset: 16
  DW_CFA_advance_loc: 6 to 00000246
  DW_CFA_def_cfa_offset: 24
  DW_CFA_advance_loc: 10 to 00000250
  DW_CFA_def_cfa_expression \(DW_OP_breg7 \(rsp\): 8; DW_OP_breg16 \(rip\): 0; DW_OP_lit15; DW_OP_and; DW_OP_lit11; DW_OP_ge; DW_OP_lit3; DW_OP_shl; DW_OP_plus\)
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop

00000070 ZERO terminator
#pass

