#source: crc64-in-section.s
#ld: -T crc64-in-section.t
#error: .*: CRC located inside checked area
#target: [is_elf_format] [is_coff_format]
#skip: tic4x-*-* tic54x-*-*
