#source: expdyn1.s
#source: expdref1.s --pic
#as: --no-underscore --em=criself
#ld: -m crislinux --export-dynamic tmpdir/libdso-1.so
#objdump: -s -j .got

# Like expdyn2.d, but testing that the .got contents is correct.  There
# needs to be a .got due to the GOT relocs, but the entry is constant.

.*:     file format elf32-cris
Contents of section \.got:
 82260 f8210800 00000000 00000000 dc010800  .*
 82270 74220800                             .*
