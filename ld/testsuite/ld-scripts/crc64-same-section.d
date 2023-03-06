#source: crc64-same-section.s
#ld: -T crc64-same-section.t
#error: .*: The CRC digest and table should be inside the .*
#skip: tic4x-coff, tic54x-cof
#skip: ns32k-pc532-macho, pdp11-dec-aout, powerpc-ibm-aix5.2.0
#skip: rs6000-aix4.3.3, alpha-linuxecoff
