#source: pr23633.s
#PROG: objcopy
#objcopy: --set-section-alignment .text=16
#objdump: --section-headers
#notarget: *-*-*aout *-*-*coff *-*-cygwin* *-*-darwin *-*-mingw* *-*-go32 *-*-*pe hppa*-*-hpux* ns32k-*-* powerpc-*-aix* rs6000-*-* rx-*-* *-*-vms

#...
.*\.text.*2\*\*4
#pass
