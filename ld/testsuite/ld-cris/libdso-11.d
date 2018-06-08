#source: dso-1.s
#source: dsov32-1.s
#as: --pic --no-underscore --march=v32 --em=criself
#ld: --shared -m crislinux --hash-style=sysv
#objdump: -s -T

.*:     file format elf32-cris

DYNAMIC SYMBOL TABLE:
#...
0+144 g    DF \.text	0+8 dsofn3
#...
0+140 g    DF \.text	0+2 dsofn
#...
Contents of section \.rela\.plt:
 0100 d0210000 0b030000 00000000           .*
Contents of section \.plt:
 010c 84e20401 7e7a3f7a 04f26ffa bf09b005  .*
 011c 00000000 00000000 00006f0d 0c000000  .*
 012c 6ffabf09 b0053f7e 00000000 bf0ed4ff  .*
 013c ffffb005                             .*
Contents of section \.text:
 0140 b0050000 bfbee2ff ffffb005           .*
Contents of section \.dynamic:
#...
Contents of section \.got:
 21c4 4c210000 00000000 00000000 32010000  .*
