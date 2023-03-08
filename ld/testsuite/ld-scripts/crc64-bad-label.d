#source: crc64-bad-label.s
#ld: -T crc64-bad-label.t
# error: .*: Illegal label .* in digest command.*
#target: [is_elf_format] [is_coff_format]
#skip: tic4x-*-* tic54x-*-*
