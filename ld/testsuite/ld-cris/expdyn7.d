#source: expdyn1.s
#source: expdref1.s --pic
#source: comref1.s --pic
#as: --no-underscore --em=criself
#ld: -m crislinux --export-dynamic tmpdir/libdso-1.so
#objdump: -s -j .got

# Like expdyn5.d, referencing COMMON symbols.

.*:     file format elf32-cris
Contents of section \.got:
 822b8 50220800 00000000 00000000 d4220800  .*
 822c8 21020800 d0220800                    .*
