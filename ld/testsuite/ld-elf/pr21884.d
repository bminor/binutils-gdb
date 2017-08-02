#source: pr21884a.s
#source: pr21884b.s
#ld: -T pr21884.t
#objdump: -b binary -s
#notarget: aarch64*-*-* arm*-*-* nds32*-*-*
# Skip targets which can't change output format to binary.

.*:     file format binary

Contents of section .data:
#pass
