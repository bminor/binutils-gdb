# Checking that we have two RELATIVE relocations, indicating that both the
# CAPINIT and the GOT entry were given a RELATIVE relocation.  As opposed to
# before when the linker generated one RELATIVE relocation and an
# R_AARCH64_NONE relocation.
#as: -march=morello+c64
#ld:
#readelf: --relocs

Relocation section '\.rela\.dyn' at offset .* contains 2 entries:
  Offset          Info           Type           Sym\. Value    Sym\. Name \+ Addend
[0-9a-f]+  [0-9a-f]+ R_MORELLO_RELATIV                    0
[0-9a-f]+  [0-9a-f]+ R_MORELLO_RELATIV                    0
