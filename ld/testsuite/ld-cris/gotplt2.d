#source: dso-2.s
#source: dsofnf.s
#source: gotrel1.s
#as: --pic --no-underscore
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
000022bc R_CRIS_GLOB_DAT   dsofn

Contents of section .*
#...
Contents of section \.rela\.got:
 01f8 bc220000 0a0c0000 00000000           .*
Contents of section \.text:
 0204 5f1d0c00 30096f1d 0c000000 30090000  .*
 0214 6f0d0c00 0000611a 6f3e6cdf ffff0000  .*
Contents of section \.dynamic:
 2240 04000000 94000000 05000000 ac010000  .*
 2250 06000000 dc000000 0a000000 49000000  .*
 2260 0b000000 10000000 07000000 f8010000  .*
 2270 08000000 0c000000 09000000 0c000000  .*
 2280 00000000 00000000 00000000 00000000  .*
 2290 00000000 00000000 00000000 00000000  .*
 22a0 00000000 00000000 00000000 00000000  .*
Contents of section \.got:
 22b0 40220000 00000000 00000000 00000000  .*
