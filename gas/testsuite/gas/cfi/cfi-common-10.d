#objdump: -Wf
#name: CFI common 10
#...
Contents of the .eh_frame section:

0+0000 0+00(10|14) 0+0000 CIE
  Version:               1
  Augmentation:          "zR"
  Code alignment factor: .*
  Data alignment factor: .*
  Return address column: .*
  Augmentation data:     [01][abc]
#...

0+00(14|18) 0+00(18|1c) 0+00(18|1c) FDE cie=0+0000 pc=.*
#...
  DW_CFA_advance_loc: 4 to 0+0004
  DW_CFA_def_cfa_offset: 1099511627808
#...
