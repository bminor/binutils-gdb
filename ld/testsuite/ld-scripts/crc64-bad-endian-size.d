#source: crc64-bad-endian-size.s
#ld: -T crc64-bad-endian-size.t
# error: .*: bad 'endian' .* in digest label.*
#target: [is_elf_format] [is_coff_format]
#skip: tic4x-*-* tic54x-*-*
