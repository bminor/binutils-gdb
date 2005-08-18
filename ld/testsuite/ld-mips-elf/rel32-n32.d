#name: MIPS rel32 n32
#source: rel32.s
#as: -KPIC -EB -n32
#readelf: -x 5 -r
#ld: -shared -melf32btsmipn32

Relocation section '.rel.dyn' at offset 0x2c0 contains 2 entries:
 Offset     Info    Type            Sym.Value  Sym. Name
00000000  00000000 R_MIPS_NONE      
000002e0  00000003 R_MIPS_REL32     

Hex dump of section '.text':
  0x000002d0 00000000 00000000 00000000 00000000 ................
  0x000002e0 000002e0 00000000 00000000 00000000 ................
  0x000002f0 00000000 00000000 00000000 00000000 ................
