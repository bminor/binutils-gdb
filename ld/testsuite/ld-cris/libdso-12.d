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
0+282 g    DF \.text	0+12 dsofn4
0+278 g    DF \.text	0+2 expfn
0+2340 g    DO \.data	0+ expobj
#...
0+27a g    DF \.text	0+8 dsofn3
#...
0+      D  \*UND\*	0+ dsofn
#...
Contents of section \.rela\.got:
 0204 3c230000 0a070000 00000000           .*
Contents of section \.rela\.plt:
 0210 34230000 0b050000 00000000 38230000  .*
 0220 0b0d0000 00000000                    .*
Contents of section \.plt:
 0228 84e20401 7e7a3f7a 04f26ffa bf09b005  .*
 0238 00000000 00000000 00006f0d 0c000000  .*
 0248 6ffabf09 b0053f7e 00000000 bf0ed4ff  .*
 0258 ffffb005 6f0d1000 00006ffa bf09b005  .*
 0268 3f7e0c00 0000bf0e baffffff b005      .*
Contents of section \.text:
 0276 b005b005 bfbee2ff ffffb005 7f0da620  .*
 0286 00005f0d 1400bfbe b6ffffff b0050000  .*
Contents of section \.dynamic:
 2298 04000000 94000000 05000000 c0010000  .*
 22a8 06000000 e0000000 0a000000 43000000  .*
 22b8 0b000000 10000000 03000000 28230000  .*
 22c8 02000000 18000000 14000000 07000000  .*
 22d8 17000000 10020000 07000000 04020000  .*
 22e8 08000000 0c000000 09000000 0c000000  .*
 22f8 00000000 00000000 00000000 00000000  .*
 2308 00000000 00000000 00000000 00000000  .*
 2318 00000000 00000000 00000000 00000000  .*
Contents of section \.got:
 2328 98220000 00000000 00000000 4e020000  .*
 2338 68020000 00000000                    .*
Contents of section \.data:
 2340 00000000                             .*
