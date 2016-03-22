#ld: -shared --out-implib dx.dll.a --gc-sections
#objdump: --syms
#notarget: mcore-* arm-epoc-pe
#
# Check that the target specific entry symbol _DllMainCRTStartup is still
# a defined (sec > 0), public (scl == 2) symbol, even after garbage
# collection.
#
# Check that the symbol _testval is undefined (sec == 0) and hidden
# (scl == 106) in the output.  It should have been changed to this state when
# garbage collection was performed.

#...
.*\(sec  0\)\(fl 0x00\)\(ty   0\)\(scl 106\) \(nx 0\) 0x0+000 _testval
#...
.*\(sec  1\)\(fl 0x00\)\(ty   0\)\(scl   2\) \(nx 0\) 0x0+000 .*Startup.*
#pass
