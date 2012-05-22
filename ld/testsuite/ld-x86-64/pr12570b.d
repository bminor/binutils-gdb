#name: PR ld/12570
#as: --64
#ld: -melf_x86_64 -shared
#readelf: -wf --wide

#failif
#...
  DW_CFA_def_cfa_expression \(DW_OP_breg7 \(rsp\): 8; DW_OP_breg16 \(rip\): 0; DW_OP_lit15; DW_OP_and; DW_OP_lit11; DW_OP_ge; DW_OP_lit3; DW_OP_shl; DW_OP_plus\)
#...
