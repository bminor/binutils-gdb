# This testcase is just to exercise some code rather than to test for an
# observable.  We may as well check that the __rela_dyn_start symbol does not
# exists.
#source: morello-static-got.s
#target: [check_shared_lib_support]
#as: -march=morello+c64
#ld: -shared
#readelf: --symbols

#failif
#...
.* __rela_dyn_start
#...
