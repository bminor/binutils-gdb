#source: dso-2.s
#source: dsofnf2.s
#source: gotrel1.s
#as: --pic --no-underscore --em=criself
#ld: -m crislinux tmpdir/libdso-1.so
#objdump: -sR

# Make sure we don't merge a PLT-specific entry
# (R_CRIS_JUMP_SLOT) with a non-PLT-GOT-specific entry
# (R_CRIS_GLOB_DAT) in an executable, since they may have
# different contents there.  (If we merge them in a DSO it's ok:
# we make a round-trip to the PLT in the executable if it's
# referenced there, but that's still perceived as better than
# having an unnecessary PLT, dynamic reloc and lookup in the
# DSO.)  In the executable, the GOT contents for the non-PLT
# reloc should be constant.

.*:     file format elf32-cris

DYNAMIC RELOCATION RECORDS
OFFSET   TYPE              VALUE 
0008229c R_CRIS_JUMP_SLOT  dsofn

Contents of section .*
#...
Contents of section \.rela\.plt:
 801ac 9c220800 0b050000 00000000           .*
Contents of section \.plt:
 801b8 fce17e7e 7f0d9422 0800307a 7f0d9822  .*
 801c8 08003009 7f0d9c22 08003009 3f7e0000  .*
 801d8 00002ffe d8ffffff                    .*
Contents of section \.text:
 801e0 5f1d0c00 30096f1d 0c000000 30090000  .*
 801f0 6f0d1000 0000611a 6f2ecc01 08000000  .*
 80200 6f3e70df ffff0000                    .*
Contents of section \.dynamic:
 82208 01000000 01000000 04000000 e4000800  .*
 82218 05000000 70010800 06000000 10010800  .*
 82228 0a000000 3b000000 0b000000 10000000  .*
 82238 15000000 00000000 03000000 90220800  .*
 82248 02000000 0c000000 14000000 07000000  .*
 82258 17000000 ac010800 00000000 00000000  .*
 82268 00000000 00000000 00000000 00000000  .*
 82278 00000000 00000000 00000000 00000000  .*
 82288 00000000 00000000                    .*
Contents of section \.got:
 82290 08220800 00000000 00000000 d4010800  .*
 822a0 cc010800                             .*
