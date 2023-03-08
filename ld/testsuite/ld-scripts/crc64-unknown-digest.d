#source: crc64-unknown-digest.s
#ld: -T crc64-unknown-digest.t
# error: .*: Unknown digest.*
#target: [is_elf_format] [is_coff_format]
#skip: tic4x-*-* tic54x-*-*
