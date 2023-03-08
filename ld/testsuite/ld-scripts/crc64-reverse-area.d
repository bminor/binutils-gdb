#source: crc64-reverse-area.s
#ld: -T crc64-reverse-area.t
# error: .*: CRC area starts after its end location.*
#target: [is_elf_format] [is_coff_format]
#skip: tic4x-*-* tic54x-*-*
