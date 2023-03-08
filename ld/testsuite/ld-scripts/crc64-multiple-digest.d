#source: crc64-multiple-digest.s
#ld: -T crc64-multiple-digest.t
# error: .*: Duplicate digest .*
#target: [is_elf_format] [is_coff_format]
#skip: tic4x-*-* tic54x-*-*
