#objdump : -s -j .data
#name : .quad tests

.*: .*

Contents of section .data:
 0000 (00000000 76543210 00000000 80000000|10325476 00000000 00000080 00000000) .*
 0010 (00000000 87654321 00000000 ffffffff|21436587 00000000 ffffffff 00000000) .*
 0020 (ffffffff 89abcdf0 ffffffff 80000000|f0cdab89 ffffffff 00000080 ffffffff) .*
 0030 (ffffffff 789abcdf ffffffff 00000001|dfbc9a78 ffffffff 01000000 ffffffff) .*
 0040 (01234567 89abcdef fedcba98 76543211|efcdab89 67452301 11325476 98badcfe) .*
#pass
