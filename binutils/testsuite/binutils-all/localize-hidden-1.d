#PROG: objcopy
#readelf: --symbols
#objcopy: --localize-hidden
#name: --localize-hidden test 1
#...
.*: 0+1200 +0 +NOTYPE +LOCAL +HIDDEN +ABS +Lhidden
.*: 0+1300 +0 +NOTYPE +LOCAL +INTERNAL +ABS +Linternal
.*: 0+1400 +0 +NOTYPE +LOCAL +PROTECTED +ABS +Lprotected
.*: 0+1100 +0 +NOTYPE +LOCAL +DEFAULT +ABS +Ldefault
.*: 0+2200 +0 +NOTYPE +LOCAL +HIDDEN +ABS +Ghidden
.*: 0+2300 +0 +NOTYPE +LOCAL +INTERNAL +ABS +Ginternal
.*: 0+3200 +0 +NOTYPE +LOCAL +HIDDEN +ABS +Whidden
.*: 0+3300 +0 +NOTYPE +LOCAL +INTERNAL +ABS +Winternal
.*: 0+2100 +0 +NOTYPE +GLOBAL +DEFAULT +ABS +Gdefault
.*: 0+2400 +0 +NOTYPE +GLOBAL +PROTECTED +ABS +Gprotected
.*: 0+3100 +0 +NOTYPE +WEAK +DEFAULT +ABS +Wdefault
.*: 0+3400 +0 +NOTYPE +WEAK +PROTECTED +ABS +Wprotected
