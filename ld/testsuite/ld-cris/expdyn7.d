#source: expdyn1.s
#source: expdref1.s --pic
#source: comref1.s --pic
#as: --no-underscore
#ld: -m crislinux --export-dynamic tmpdir/libdso-1.so
#objdump: -s -j .got

# Like expdyn5.d, referencing COMMON symbols.

.*:     file format elf32-cris
Contents of section \.got:
 822e8 80220800 00000000 00000000 04230800  .*
 822f8 4b020800 00230800                    .*
