#as: -march=rv32g2p0 -march-attr -misa-spec=2.2
#readelf: -A
#source: attribute-06.s
Attribute Section: riscv
File Attributes
  Tag_RISCV_arch: "rv32i2p0"
  Tag_RISCV_priv_spec: [0-9_\"].*
  Tag_RISCV_priv_spec_minor: [0-9_\"].*
#...
