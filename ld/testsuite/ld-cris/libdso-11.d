#source: dso-1.s
#source: dsov32-1.s
#as: --pic --no-underscore --march=v32 --em=criself
#ld: --shared -m crislinux
#objdump: -s -T

.*:     file format elf32-cris

DYNAMIC SYMBOL TABLE:
#...
0+1bc g    DF \.text	0+8 dsofn3
#...
0+1b8 g    DF \.text	0+ dsofn
#...
Contents of section \.rela\.plt:
 0178 48220000 0b070000 00000000           .*
Contents of section \.plt:
 0184 84e20401 7e7a3f7a 04f26ffa bf09b005  .*
 0194 00000000 00000000 00006f0d 0c000000  .*
 01a4 6ffabf09 b0053f7e 00000000 bf0ed4ff  .*
 01b4 ffffb005                             .*
Contents of section \.text:
 01b8 b0050000 bfbee2ff ffffb005           .*
Contents of section \.dynamic:
#...
Contents of section \.got:
 223c c4210000 00000000 00000000 aa010000  .*
