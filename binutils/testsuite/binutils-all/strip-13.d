#PROG: strip
#strip: -g
#error: .* bad value
#not-target: arm-* d10v-* dlx-* h8300-* ip2k-* rx-* xgate-*
# The D10V, DLX and XGATE targets only support REL relocations but this test uses RELA relocations.
# The ARM target does support both types, but defaults to REL.
# The H8300-*, IP2K and RX targets do not complain about unrecognised relocs, unless they are actually used
#  (which is what should really happen with the other targets...)
