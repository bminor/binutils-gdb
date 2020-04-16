# source: pr18963.s
# ld: -T pr18963.t
# nm: -B -n
# notarget: tic*-*-*  ia64-*-* *-*-aix* microblaze-*-* *-*-vms
# Skip this test on targets which have sizeof(byte) != sizeof(octet).
# On these targets the ".lcomm _bss,0x100" directive does not create
# a 0x100 byte long section as needed by the test.
# Also skip it for AIX and IA64 targets because they use a non-standard version of .lcomm.
# And skip for the microblaze target which does not support a .bss section at all.
# Plus skip for VMS based targets as the linker automatically adds extra libraries that may not be present in a cross build.

#...
0+a00 . D
#...
0+a00 . E
#...
[02]+b00 T A
#...
[02]+c00 D B
#...
[02]+d00 . C
#pass
