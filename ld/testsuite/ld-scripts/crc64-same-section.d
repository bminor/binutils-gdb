#source: crc64-same-section.s
#ld: -T crc64-same-section.t
#error: .*: The CRC digest and table should be inside the .*
#target: [is_elf_format] [is_coff_format]
#skip: tic4x-*-* tic54x-*-*
