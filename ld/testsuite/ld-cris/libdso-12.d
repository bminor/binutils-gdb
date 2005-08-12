#source: expdyn1.s
#source: dsov32-1.s
#source: dsov32-2.s
#as: --pic --no-underscore --march=v32 --em=criself
#ld: --shared -m crislinux -z nocombreloc
#objdump: -s -T

# Check for common DSO contents; load of GOT register, branch to
# function PLT, undefined symbol, GOT reloc.

.*:     file format elf32-cris

DYNAMIC SYMBOL TABLE:
#...
0+26e g    DF \.text	0+12 dsofn4
0+264 g    DF \.text	0+2 expfn
0+232c g    DO \.data	0+4 expobj
#...
0+266 g    DF \.text	0+8 dsofn3
#...
0+      D  \*UND\*	0+ dsofn
#...
Contents of section \.rela\.got:
 01f0 28230000 0a060000 00000000           .*
Contents of section \.rela\.plt:
 01fc 20230000 0b040000 00000000 24230000  .*
 020c 0b0c0000 00000000                    .*
Contents of section \.plt:
 0214 84e20401 7e7a3f7a 04f26ffa bf09b005  .*
 0224 00000000 00000000 00006f0d 0c000000  .*
 0234 6ffabf09 b0053f7e 00000000 bf0ed4ff  .*
 0244 ffffb005 6f0d1000 00006ffa bf09b005  .*
 0254 3f7e0c00 0000bf0e baffffff b005      .*
Contents of section \.text:
 0262 b005b005 bfbee2ff ffffb005 7f0da620  .*
 0272 00005f0d 1400bfbe b6ffffff b0050000  .*
Contents of section \.dynamic:
 2284 04000000 94000000 05000000 ac010000  .*
 2294 06000000 dc000000 0a000000 43000000  .*
 22a4 0b000000 10000000 03000000 14230000  .*
 22b4 02000000 18000000 14000000 07000000  .*
 22c4 17000000 fc010000 07000000 f0010000  .*
 22d4 08000000 0c000000 09000000 0c000000  .*
 22e4 00000000 00000000 00000000 00000000  .*
 22f4 00000000 00000000 00000000 00000000  .*
 2304 00000000 00000000 00000000 00000000  .*
Contents of section \.got:
 2314 84220000 00000000 00000000 3a020000  .*
 2324 54020000 00000000                    .*
Contents of section \.data:
 232c 00000000                             .*
