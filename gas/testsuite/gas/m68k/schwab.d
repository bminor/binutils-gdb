#name: PC-relative relocations
#objdump: -drs

.*:     file format .*

Contents of section .text:
 0000 4e714e71 4cfa0300 fffa4cfa 0300fff4  NqNqL.....L.....
 0010 4cfb0300 08ee41fa ffea41fa ffe641fa  L.....A...A...A.
 0020 ff6241fb 08de41fb 08da41fb 08d641fb  .bA...A...A...A.
 0030 0920ffd2 41fb0920 ffcc41fb 0930ffff  . ..A.. ..A..0..
 0040 ffc641fb 0930ffff ffbe4e71 61ff0000  ..A..0....Nqa...
 0050 00586100 0052614e 614c4e71 41f90000  .Xa..RaNaLNqA...
 0060 00a641fa 004241fa 00be41fb 083a41fb  ..A..BA...A..:A.
 0070 083641fb 083241fb 0920002e 41fb0920  .6A..2A.. ..A.. 
 0080 002841fb 09300000 002241fb 09300000  .\(A..0..."A..0..
 0090 001a41fb 09300000 001241fb 0920000a  ..A..0....A.. ..
 00a0 41fb0804 4e714e71 4e7141fb 088041fb  A...NqNqNqA...A.
 00b0 0920ff7f 41fb0920 800041fb 0930ffff  . ..A.. ..A..0..
 00c0 7fff4e71 41fb087f 41fb0920 008041fb  ..NqA...A.. ..A.
 00d0 09207fff 41fb0930 00008000 4e7141fa  . ..A..0....NqA.
 00e0 800041fb 0170ffff 7fff4e71 41fa7fff  ..A..p....NqA...
 00f0 41fb0170 00008000 4e7141fb 0170ffff  A..p....NqA..p..
 0100 ff0441fb 0930ffff fefc4e71 41f90000  ..A..0....NqA...
 0110 0000...............................  ................
Contents of section .data:
Disassembly of section \.text:
0+0000 <lbl_b-2> nop
0+0002 <lbl_b> nop
0+0004 <lbl_b\+2> moveml 0+0002 <lbl_b>,%a0-%a1
0+000a <lbl_b\+8> moveml 0+0002 <lbl_b>,%a0-%a1
0+0010 <lbl_b\+e> moveml 0x10002\[%d0\.l\],%a0-%a1
0+0016 <lbl_b\+14> lea 0+0002 <lbl_b>,%a0
0+001a <lbl_b\+18> lea 0+0002 <lbl_b>,%a0
0+001e <lbl_b\+1c> lea f+ff82 <lbl_a\+f+fedc>,%a0
0+0022 <lbl_b\+20> lea 0x10002\[%d0\.l\],%a0
0+0026 <lbl_b\+24> lea 0x10002\[%d0\.l\],%a0
0+002a <lbl_b\+28> lea 0x10002\[%d0\.l\],%a0
0+002e <lbl_b\+2c> lea 0x2\[%d0\.l\],%a0
0+0034 <lbl_b\+32> lea 0x2\[%d0\.l\],%a0
0+003a <lbl_b\+38> lea 0x2\[%d0\.l\],%a0
0+0042 <lbl_b\+40> lea 0x2\[%d0\.l\],%a0
0+004a <lbl_b\+48> nop
0+004c <lbl_b\+4a> bsrl 0+00a6 <lbl_a>
0+0052 <lbl_b\+50> bsr 0+00a6 <lbl_a>
0+0056 <lbl_b\+54> bsrs 0+00a6 <lbl_a>
0+0058 <lbl_b\+56> bsrs 0+00a6 <lbl_a>
0+005a <lbl_b\+58> nop
0+005c <lbl_b\+5a> lea @#0+00a6 <lbl_a>,%a0
		RELOC: 0+005e (R_68K_)?32 \.text
0+0062 <lbl_b\+60> lea 0+00a6 <lbl_a>,%a0
0+0066 <lbl_b\+64> lea 0+0126 <lbl_a\+80>,%a0
0+006a <lbl_b\+68> lea 0xa6\[%d0\.l\],%a0
0+006e <lbl_b\+6c> lea 0xa6\[%d0\.l\],%a0
0+0072 <lbl_b\+70> lea 0xa6\[%d0\.l\],%a0
0+0076 <lbl_b\+74> lea 0xa6\[%d0\.l\],%a0
0+007c <lbl_b\+7a> lea 0xa6\[%d0\.l\],%a0
0+0082 <lbl_b\+80> lea 0xa6\[%d0\.l\],%a0
0+008a <lbl_b\+88> lea 0xa6\[%d0\.l\],%a0
0+0092 <lbl_b\+90> lea 0xa6\[%d0\.l\],%a0
0+009a <lbl_b\+98> lea 0xa6\[%d0\.l\],%a0
0+00a0 <lbl_b\+9e> lea 0xa6\[%d0\.l\],%a0
0+00a4 <lbl_b\+a2> nop
0+00a6 <lbl_a> nop
0+00a8 <lbl_a\+2> nop
0+00aa <lbl_a\+4> lea 0x1002c\[%d0\.l\],%a0
0+00ae <lbl_a\+8> lea 0x2f\[%d0\.l\],%a0
0+00b4 <lbl_a\+e> lea 0xf+80b6\[%d0\.l\],%a0
0+00ba <lbl_a\+14> lea 0xf+80bb\[%d0\.l\],%a0
0+00c2 <lbl_a\+1c> nop
0+00c4 <lbl_a\+1e> lea 0x145\[%d0\.l\],%a0
0+00c8 <lbl_a\+22> lea 0x14a\[%d0\.l\],%a0
0+00ce <lbl_a\+28> lea 0x80cf\[%d0\.l\],%a0
0+00d4 <lbl_a\+2e> lea 0x80d6\[%d0\.l\],%a0
0+00dc <lbl_a\+36> nop
0+00de <lbl_a\+38> lea f+80e0 <lbl_a\+f+803a>,%a0
0+00e2 <lbl_a\+3c> lea 0xf+80e3,%a0
0+00ea <lbl_a\+44> nop
0+00ec <lbl_a\+46> lea 0+80ed <lbl_a\+8047>,%a0
0+00f0 <lbl_a\+4a> lea 0x80f2,%a0
0+00f8 <lbl_a\+52> nop
0+00fa <lbl_a\+54> lea 0x0,%a0
		RELOC: 0+00fe (R_68K_PC|DISP)32 undef
0+0102 <lbl_a\+5c> lea 0x0\[%d0\.l\],%a0
		RELOC: 0+0106 (R_68K_PC|DISP)32 undef
0+010a <lbl_a\+64> nop
0+010c <lbl_a\+66> lea @#0+0+ <lbl_b-2>,%a0
		RELOC: 0+010e (R_68K_)?32 undef
0+0112 <lbl_a\+6c> nop
\.\.\.
