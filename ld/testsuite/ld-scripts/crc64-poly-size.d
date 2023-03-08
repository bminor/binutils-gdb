#source: crc64-poly-size.s
#ld: -T crc64-poly-size.t
# error: .*: Illegal Size in DIGEST: .*
#target: [is_elf_format] [is_coff_format]
#skip: tic4x-*-* tic54x-*-*
