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
00002278 R_CRIS_GLOB_DAT   dsofn

Contents of section .*
#...
Contents of section \.rela\.got:
 01cc 78220000 0a0b0000 00000000           .*
Contents of section \.text:
 01d8 5f1d0c00 30096f1d 0c000000 30090000  .*
 01e8 6f0d0c00 0000611a 6f3e84df ffff0000  .*
 01f8 0f050000                             .*
Contents of section \.dynamic:
 21fc 04000000 94000000 05000000 98010000  .*
 220c 06000000 d8000000 0a000000 33000000  .*
 221c 0b000000 10000000 07000000 cc010000  .*
 222c 08000000 0c000000 09000000 0c000000  .*
 223c 00000000 00000000 00000000 00000000  .*
 224c 00000000 00000000 00000000 00000000  .*
 225c 00000000 00000000 00000000 00000000  .*
Contents of section \.got:
 226c fc210000 00000000 00000000 00000000  .*
