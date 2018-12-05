#objdump: --dwarf=frames
# Test assembling a file with functions signed by two different pointer
# authentication keys. It must interpret .cfi_b_key_frame properly and emit a
# 'B' character into the correct CIE's augmentation string.

.+:     file .+

Contents of the .eh_frame section:

00000000 0000000000000010 00000000 CIE
  Version:               1
  Augmentation:          "zR"
  Code alignment factor: 4
  Data alignment factor: -8
  Return address column: 30
  Augmentation data:     1b
  DW_CFA_def_cfa: r31 \(sp\) ofs 0

00000014 0000000000000018 00000018 FDE cie=00000000 pc=0000000000000000..0000000000000008
  DW_CFA_advance_loc: 4 to 0000000000000004
  DW_CFA_GNU_window_save
  DW_CFA_advance_loc: 4 to 0000000000000008
  DW_CFA_def_cfa_offset: 16
  DW_CFA_offset: r29 \(x29\) at cfa-16
  DW_CFA_offset: r30 \(x30\) at cfa-8
  DW_CFA_nop
  DW_CFA_nop

00000030 0000000000000014 00000000 CIE
  Version:               1
  Augmentation:          "zRB"
  Code alignment factor: 4
  Data alignment factor: -8
  Return address column: 30
  Augmentation data:     1b
  DW_CFA_def_cfa: r31 \(sp\) ofs 0
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop

00000048 000000000000001c 0000001c FDE cie=00000030 pc=0000000000000008..0000000000000010
  DW_CFA_advance_loc: 4 to 000000000000000c
  DW_CFA_GNU_window_save
  DW_CFA_advance_loc: 4 to 0000000000000010
  DW_CFA_def_cfa_offset: 16
  DW_CFA_offset: r29 \(x29\) at cfa-16
  DW_CFA_offset: r30 \(x30\) at cfa-8
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop
  DW_CFA_nop

