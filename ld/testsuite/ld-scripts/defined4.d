#ld: -Tdefined4.t
#nm: -B
#source: defined4.s
#xfail: powerpc*-*-aix* rs6000-*-aix*
#xfail: mips*-*-* mmix-*-*
# We check that defined and defined1 have the same address.  MIPS targets
# use different address. MMIX puts defined and defined1 in text section.

# Check that arithmetic on DEFINED works.
#...
0+0 D defined
#...
0+0 D defined1
#pass
