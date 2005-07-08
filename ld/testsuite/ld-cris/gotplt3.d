#source: dso-2.s
#source: dsofnf.s
#source: gotrel1.s
#source: dso-1.s
#as: --pic --no-underscore --em=criself
#ld: -shared -m crislinux -z nocombreloc
#objdump: -sR

# Like gotplt2, but make sure we merge right when we have a
# definition of the function too.

.*:     file format elf32-cris

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
00002250 R_CRIS_GLOB_DAT   dsofn

Contents of section .*
#...
Contents of section \.rela\.got:
 01a4 50220000 0a090000 00000000           .*
Contents of section \.text:
 01b0 5f1d0c00 30096f1d 0c000000 30090000  .*
 01c0 6f0d0c00 0000611a 6f3e84df ffff0000  .*
 01d0 0f050000                             .*
Contents of section \.dynamic:
 21d4 04000000 94000000 05000000 70010000  .*
 21e4 06000000 d0000000 0a000000 33000000  .*
 21f4 0b000000 10000000 07000000 a4010000  .*
 2204 08000000 0c000000 09000000 0c000000  .*
 2214 00000000 00000000 00000000 00000000  .*
 2224 00000000 00000000 00000000 00000000  .*
 2234 00000000 00000000 00000000 00000000  .*
Contents of section \.got:
 2244 d4210000 00000000 00000000 00000000  .*
