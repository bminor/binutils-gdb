#PROG: strip
#strip: -g
#error: .* bad value
#not-target: arm-* d10v-* dlx-* h8300-* hppa*-* ip2k-* rx-* xgate-*
# The D10V, DLX and XGATE targets only support REL relocations but this test uses RELA relocations.
# The ARM target does support both types, but defaults to REL.
# The HPPA targets use reloc 241, which is the value this test uses as its "unknown" reloc.
# The H8300-*, IP2K and RX targets do not complain about unrecognised relocs, unless they are actually used
#  (which is what should really happen with the other targets...)
