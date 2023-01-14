#source: attr-merge-strict-align-04a.s
#source: attr-merge-strict-align-04b.s
#as: -march-attr
#ld: -r
#readelf: -A

Attribute Section: riscv
File Attributes
  Tag_RISCV_arch: [a-zA-Z0-9_\"].*
  Tag_RISCV_priv_spec: [0-9_\"].*
  Tag_RISCV_priv_spec_minor: [0-9_\"].*
#...
