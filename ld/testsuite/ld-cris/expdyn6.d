#source: expdyn1.s
#source: expdref1.s --pic
#source: euwref1.s --pic
#as: --no-underscore
#ld: -m crislinux --export-dynamic tmpdir/libdso-1.so
#objdump: -s -j .got

# Like expdyn5.d, weakly referencing symbols.

.*:     file format elf32-cris
Contents of section \.got:
 822e8 80220800 00000000 00000000 00000000  .*
 822f8 4e020800 00230800                    .*
