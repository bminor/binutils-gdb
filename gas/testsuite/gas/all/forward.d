#objdump: -s -j .data
#name: forward references
# Some targets don't manage to resolve BFD_RELOC_8 for constants.
#xfail: *c30-*-* *c4x-*-* d10v-*-* d30v-*-* pdp11-*-*

.*: .*

Contents of section .data:
 0000 01020304 ff0203fc 01020304 ff0203fc  ................
#pass
