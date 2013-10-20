#name: FRV uClinux PIC relocs to global symbols with addends, failing pie linking
#source: fdpic8.s
#objdump: -DR -j .text -j .data -j .got -j .plt
#as: -mfdpic
#ld: -pie
#error: nonzero addend
