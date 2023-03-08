#source: crc64-bad-label-2.s
#ld: -T crc64-bad-label-2.t
# error: .*: Illegal label .* in digest command.*
#target: [is_elf_format] [is_coff_format]
#skip: tic4x-*-* tic54x-*-*
