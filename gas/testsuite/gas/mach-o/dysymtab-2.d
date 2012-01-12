#as: -L -I $srcdir/$subdir
#objdump: -P dysymtab
#target: i?86-*-darwin* powerpc-*-darwin*
#source: symbols-6.s
.*: +file format mach-o.*
#...
Load command dysymtab:
( )+local symbols: idx:( )+0( )+num: 55( )+\(nxtidx: 55\)
( )+external symbols: idx:( )+55( )+num: 24( )+\(nxtidx: 79\)
( )+undefined symbols: idx:( )+79( )+num: 30( )+\(nxtidx: 109\)
( )+table of content: off: 0x00000000( )+num: 0( )+\(endoff: 0x00000000\)
( )+module table: off: 0x00000000( )+num: 0( )+\(endoff: 0x00000000\)
( )+external reference table: off: 0x00000000( )+num: 0( )+\(endoff: 0x00000000\)
( )+indirect symbol table: off: 0x000003b0( )+num: 25( )+\(endoff: 0x00000414\)
( )+external relocation table: off: 0x00000000( )+num: 0( )+\(endoff: 0x00000000\)
( )+local relocation table: off: 0x00000000( )+num: 0( )+\(endoff: 0x00000000\)
( )+indirect symbols:
( )+for section __dummy.__dummy:
( )+0000000000000000( )+0: 0x0000005e a
( )+0000000000000008( )+1: 0x00000063 b
( )+0000000000000010( )+2: 0x0000003d c
( )+0000000000000018( )+3: 0x0000001b d
( )+0000000000000020( )+4: 0x00000018 e
( )+0000000000000028( )+5: 0x00000040 f
( )+0000000000000030( )+6: 0x00000066 g
( )+for section __DATA.__la_symbol_ptr:
( )+0000000000000000( )+7: 0x0000005f a1
( )+0000000000000004( )+8: 0x00000064 b1
( )+0000000000000008( )+9: 0x0000003e c1
( )+000000000000000c( )+10: 0x0000001c d1
( )+0000000000000010( )+11: 0x00000019 e1
( )+0000000000000014( )+12: 0x00000041 f1
( )+0000000000000018( )+13: 0x00000067 g1
( )+for section __DATA.__nl_symbol_ptr:
( )+0000000000000000( )+14: 0x00000060 a2
( )+0000000000000004( )+15: 0x00000065 b2
( )+0000000000000008( )+16: 0x0000003f c2
( )+000000000000000c( )+17: 0x80000000 LOCAL
( )+0000000000000010( )+18: 0x80000000 LOCAL
( )+0000000000000014( )+19: 0x00000042 f2
( )+0000000000000018( )+20: 0x00000068 g2
( )+000000000000001c( )+21: 0x00000041 f1
( )+0000000000000020( )+22: 0x00000067 g1
( )+0000000000000024( )+23: 0x00000060 a2
( )+0000000000000028( )+24: 0x00000065 b2
