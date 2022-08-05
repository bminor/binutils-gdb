# This test is here to check that if we have a TLS LE relocation against an
# external symbol (e.g. a shared library) the linker complains.  Such a
# relocation would not generate anything useful since we don't know which order
# the shared libraries' thread local storage location would be stored at in
# relation to that of other shared libraries.
#name: Illegal :tprel*: relocation.
#as: -march=morello+c64
#source: illegal-tlsle-pde.s
#ld: tmpdir/tls-shared.so
#error: .*: Local-Exec TLS relocation R_AARCH64_TLSLE_MOVW_TPREL_G0 against non-local symbol `w1'
#error: .*: unresolvable R_AARCH64_TLSLE_MOVW_TPREL_G0 relocation against symbol `w1'
#error: .*: final link failed: bad value
