#source: crc64-bad-label-2.s
#ld: -T crc64-bad-label-2.t
# error: .*: Illegal label .* in digest command.*
#skip: tic4x-coff, tic54x-cof
#skip: ns32k-pc532-macho, pdp11-dec-aout, powerpc-ibm-aix5.2.0
#skip: rs6000-aix4.3.3, alpha-linuxecoff
