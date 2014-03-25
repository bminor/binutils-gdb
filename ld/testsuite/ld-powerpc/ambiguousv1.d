#source: startv1.s
#source: funref.s
#as: -a64
#ld: -melf64ppc
#ld_after_inputfiles: tmpdir/funv1.so
#readelf: -r --wide
# check that we do the right thing with funref.s that doesn't have
# anything to mark it as ELFv1 or ELFv2

Relocation section .* contains 1 entries:
.*
.* R_PPC64_ADDR64 +0+ my_func \+ 0
