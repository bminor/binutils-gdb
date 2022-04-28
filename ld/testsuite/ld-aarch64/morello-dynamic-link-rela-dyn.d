# Test here is to ensure that __rela_dyn_* symbols are not emitted for
# dynamically linked objects.  The code that was acting when we noticed this
# problem was correctly avoiding the behaviour for shared objects, but not for
# dynamically linked PDE's.  Hence that's what this testcase is added for.
#
# N.b. aarch64-elf.exp compiles a shared libary for this test under
# tmpdir/morello-dynamic-relocs.so.  We use that shared library for the test in
# the `ld` command below.
#
# This testcase is written partly to ensure a particular code path is
# exercised.  That is the purpose of the local `val` symbol that we have a GOT
# relocation to.
#as: -march=morello+c64
#ld: tmpdir/morello-dynamic-relocs.so
#readelf: --symbols

#failif
#...
.* __rela_dyn_start
#...
