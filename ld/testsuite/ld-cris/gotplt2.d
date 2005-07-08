#source: dso-2.s
#source: dsofnf.s
#source: gotrel1.s
#as: --pic --no-underscore --em=criself
#ld: -shared -m crislinux -z nocombreloc
#objdump: -sR

# Make sure we merge a PLT-specific entry (usually
# R_CRIS_JUMP_SLOT) with a GOT-specific entry (R_CRIS_GLOB_DAT)
# in a DSO.  It's ok: we make a round-trip to the PLT in the
# executable if it's referenced there, but that's still
# perceived as better than having an unnecessary PLT, dynamic
# reloc and lookup in the DSO.)

.*:     file format elf32-cris

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
0000224c R_CRIS_GLOB_DAT   dsofn

Contents of section .*
#...
Contents of section \.rela\.got:
 01a4 4c220000 0a090000 00000000           .*
Contents of section \.text:
 01b0 5f1d0c00 30096f1d 0c000000 30090000  .*
 01c0 6f0d0c00 0000611a 6f3e88df ffff0000  .*
Contents of section \.dynamic:
 21d0 04000000 94000000 05000000 70010000  .*
 21e0 06000000 d0000000 0a000000 33000000  .*
 21f0 0b000000 10000000 07000000 a4010000  .*
 2200 08000000 0c000000 09000000 0c000000  .*
 2210 00000000 00000000 00000000 00000000  .*
 2220 00000000 00000000 00000000 00000000  .*
 2230 00000000 00000000 00000000 00000000  .*
Contents of section \.got:
 2240 d0210000 00000000 00000000 00000000  .*
