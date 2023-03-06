#source: crc64-reverse-area.s
#ld: -T crc64-reverse-area.t
# error: .*: CRC area starts after its end location.*
#skip: tic4x-coff, tic54x-cof
#skip: ns32k-pc532-macho, pdp11-dec-aout, powerpc-ibm-aix5.2.0
#skip: rs6000-aix4.3.3, alpha-linuxecoff
