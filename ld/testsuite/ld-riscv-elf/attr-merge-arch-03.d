#source: attr-merge-arch-03a.s
#source: attr-merge-arch-03b.s
#as:
#ld: -r -melf32lriscv
#readelf: -A

Attribute Section: riscv
File Attributes
  Tag_RISCV_arch: "rv32i2p0_m2p0_xbar2p0_xfoo2p0"
  Tag_RISCV_priv_spec: [0-9_\"].*
  Tag_RISCV_priv_spec_minor: [0-9_\"].*
#...
