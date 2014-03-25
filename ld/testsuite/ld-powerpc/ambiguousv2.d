#source: startv2.s
#source: funref.s
#as: -a64
#ld: -melf64ppc
#ld_after_inputfiles: tmpdir/funv2.so
#readelf: -r --wide
# check that we do the right thing with funref.s that doesn't have
# anything to mark it as ELFv1 or ELFv2

Relocation section .*contains 1 entries:
.*
.* R_PPC64_JMP_SLOT .* my_func \+ 0
