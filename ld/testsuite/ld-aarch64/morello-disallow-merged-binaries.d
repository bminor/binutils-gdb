#source: morello-disallow-merged-binaries-data.s -march=morello
#source: morello-disallow-merged-binaries-code.s -march=morello+c64
#ld: -static
#error: .*failed to merge target specific data.*
