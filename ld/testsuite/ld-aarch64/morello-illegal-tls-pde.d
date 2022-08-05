#as: -march=morello+c64
#ld: tmpdir/morello-tls-shared.so

#error: .*: relocation R_MORELLO_MOVW_SIZE_G1 against `w1' must be used against a non-interposable defined symbol
#error: .*: unresolvable R_MORELLO_MOVW_SIZE_G1 relocation against symbol `w1'
#error: .*: final link failed: bad value
