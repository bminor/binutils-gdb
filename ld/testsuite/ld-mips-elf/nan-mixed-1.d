#source: nan-legacy.s
#source: nan-2008.s
#as: -march=from-abi
#ld: -r
#error: \A[^\n]*: [^\n]* linking -mnan=2008 module with previous -mnan=legacy modules\n
#error:   [^\n]*: failed to merge target specific data of file [^\n]*\.o\Z
