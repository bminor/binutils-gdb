#source: crc64-poly.s
#ld: -T crc64-poly.t
#objdump: -s -j .text
#target: [is_elf_format] [is_coff_format]
#skip: tic4x-*-* tic54x-*-*

.*:     file format .*

Contents of section .text:
 1200 434f4445 deadbeef 00000000 00000000  CODE............
 1210 6c40df5f 0b497347 00000000 00000000  l@._.IsG........
 1220 ee5e1ecd 02f31206 00000000 00000000  ................
 1230 00000000 00000000 deadbeef 434f4445  ............CODE
 1240 31323334 35363738 3900ffff ffffffff  123456789.......
 1250 434f4445 00000000 00000000 00000000  CODE............
 1260 ffffffff ffffffff ffffffff ffffffff  .*
#...
 17e0 434f4445 deadbeef 00000000 00000000  CODE............
 17f0 44494745 53542054 41424c45 00000000  DIGEST TABLE....
#...
 2000 454e4420 5441424c 45000000 00000000  END TABLE.......
 2010 00000000 00000000 deadbeef 434f4445  ............CODE
#pass