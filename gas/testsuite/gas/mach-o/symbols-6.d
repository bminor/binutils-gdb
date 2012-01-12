#as: -L -I $srcdir/$subdir
#objdump: -t
#target: i?86-*-darwin* powerpc-*-darwin*
#source: symbols-6.s
.*: +file format mach-o.*
#...
SYMBOL TABLE:
00000000 l.*0e SECT.*01 0000 \[.text\] Lzt0
00000002 l.*0e SECT.*01 0000 \[.text\] Lmt0
00000004 l.*0e SECT.*01 0000 \[.text\] Lat0
00000000 l.*0e SECT.*02 0000 \[.data\] Lzd0
00000002 l.*0e SECT.*02 0000 \[.data\] Lmd0
00000005 l.*0e SECT.*02 0000 \[.data\] Lad0
00000000 l.*0e SECT.*03 0000 \[.bss\] zlcomm0
00000006 l.*0e SECT.*03 0000 \[.bss\] mlcomm0
0000000c l.*0e SECT.*03 0000 \[.bss\] alcomm0
00000000 l.*0e SECT.*04 0000 \[__HERE.__there\] Lzs0
00000002 l.*0e SECT.*04 0000 \[__HERE.__there\] Lms0
00000004 l.*0e SECT.*04 0000 \[__HERE.__there\] Las0
00000012 l.*0e SECT.*01 0000 \[.text\] Lzt1
00000015 l.*0e SECT.*01 0000 \[.text\] Lmt1
00000017 l.*0e SECT.*01 0000 \[.text\] Lat1
00000012 l.*0e SECT.*02 0000 \[.data\] Lzd1
00000014 l.*0e SECT.*02 0000 \[.data\] Lmd1
00000017 l.*0e SECT.*02 0000 \[.data\] Lad1
00000012 l.*0e SECT.*03 0000 \[.bss\] zlcomm1
00000018 l.*0e SECT.*03 0000 \[.bss\] mlcomm1
0000001e l.*0e SECT.*03 0000 \[.bss\] alcomm1
00000016 l.*0e SECT.*04 0000 \[__HERE.__there\] Lzs1
0000001e l.*0e SECT.*04 0000 \[__HERE.__there\] Lms1
0000001f l.*0e SECT.*04 0000 \[__HERE.__there\] Las1
0000001b l.*0e SECT.*01 0000 \[.text\] e
0000001c l.*0e SECT.*01 0000 \[.text\] e1
0000001d l.*0e SECT.*01 0000 \[.text\] e2
00000024 l.*0e SECT.*02 0000 \[.data\] d
0000002c l.*0e SECT.*02 0000 \[.data\] d1
00000034 l.*0e SECT.*02 0000 \[.data\] d2
00000000 l.*0e SECT.*05 0000 \[__dummy.__dummy\] La
00000008 l.*0e SECT.*05 0000 \[__dummy.__dummy\] Lb
00000010 l.*0e SECT.*05 0000 \[__dummy.__dummy\] Lc
00000018 l.*0e SECT.*05 0000 \[__dummy.__dummy\] Ld
00000020 l.*0e SECT.*05 0000 \[__dummy.__dummy\] Le
00000028 l.*0e SECT.*05 0000 \[__dummy.__dummy\] Lf
00000030 l.*0e SECT.*05 0000 \[__dummy.__dummy\] Lg
00000000 l.*0e SECT.*06 0000 \[.lazy_symbol_pointer\] La1
00000004 l.*0e SECT.*06 0000 \[.lazy_symbol_pointer\] Lb1
00000008 l.*0e SECT.*06 0000 \[.lazy_symbol_pointer\] Lc1
0000000c l.*0e SECT.*06 0000 \[.lazy_symbol_pointer\] Ld1
00000010 l.*0e SECT.*06 0000 \[.lazy_symbol_pointer\] Le1
00000014 l.*0e SECT.*06 0000 \[.lazy_symbol_pointer\] Lf1
00000018 l.*0e SECT.*06 0000 \[.lazy_symbol_pointer\] Lg1
00000000 l.*0e SECT.*07 0000 \[.non_lazy_symbol_pointer\] La2
00000004 l.*0e SECT.*07 0000 \[.non_lazy_symbol_pointer\] Lb2
00000008 l.*0e SECT.*07 0000 \[.non_lazy_symbol_pointer\] Lc2
0000000c l.*0e SECT.*07 0000 \[.non_lazy_symbol_pointer\] Ld2
00000010 l.*0e SECT.*07 0000 \[.non_lazy_symbol_pointer\] Le2
00000014 l.*0e SECT.*07 0000 \[.non_lazy_symbol_pointer\] Lf2
00000018 l.*0e SECT.*07 0000 \[.non_lazy_symbol_pointer\] Lg2
0000001c l.*0e SECT.*07 0000 \[.non_lazy_symbol_pointer\] Lf11
00000020 l.*0e SECT.*07 0000 \[.non_lazy_symbol_pointer\] Lg11
00000024 l.*0e SECT.*07 0000 \[.non_lazy_symbol_pointer\] La12
00000028 l.*0e SECT.*07 0000 \[.non_lazy_symbol_pointer\] Lb12
00000004 g.*0f SECT.*02 0000 \[.data\] adg0
00000016 g.*0f SECT.*02 0000 \[.data\] adg1
00000005 g.*0f SECT.*04 0000 \[__HERE.__there\] asg0
0000001d g.*0f SECT.*04 0000 \[__HERE.__there\] asg1
00000005 g.*0f SECT.*01 0000 \[.text\] atg0
00000016 g.*0f SECT.*01 0000 \[.text\] atg1
00000018 g.*0f SECT.*01 0000 \[.text\] c
00000019 g.*0f SECT.*01 0000 \[.text\] c1
0000001a g.*0f SECT.*01 0000 \[.text\] c2
0000003c g.*1f SECT.*02 0000 \[.data\] f
00000044 g.*1f SECT.*02 0000 \[.data\] f1
0000004c g.*1f SECT.*02 0000 \[.data\] f2
00000003 g.*0f SECT.*02 0000 \[.data\] mdg0
00000015 g.*0f SECT.*02 0000 \[.data\] mdg1
00000003 g.*0f SECT.*04 0000 \[__HERE.__there\] msg0
0000001c g.*0f SECT.*04 0000 \[__HERE.__there\] msg1
00000003 g.*0f SECT.*01 0000 \[.text\] mtg0
00000014 g.*0f SECT.*01 0000 \[.text\] mtg1
00000001 g.*0f SECT.*02 0000 \[.data\] zdg0
00000013 g.*0f SECT.*02 0000 \[.data\] zdg1
00000001 g.*0f SECT.*04 0000 \[__HERE.__there\] zsg0
00000017 g.*0f SECT.*04 0000 \[__HERE.__there\] zsg1
00000001 g.*0f SECT.*01 0000 \[.text\] ztg0
00000013 g.*0f SECT.*01 0000 \[.text\] ztg1
00000000 g.*01 UND.*00 0000 _aud0
00000000 g.*01 UND.*00 0000 _aud1
00000000 g.*01 UND.*00 0000 _aus0
00000000 g.*01 UND.*00 0000 _aus1
00000000 g.*01 UND.*00 0000 _aut0
00000000 g.*01 UND.*00 0000 _mud0
00000000 g.*01 UND.*00 0000 _mud1
00000000 g.*01 UND.*00 0000 _mus0
00000000 g.*01 UND.*00 0000 _mus1
00000000 g.*01 UND.*00 0000 _mut0
00000000 g.*01 UND.*00 0000 _zud0
00000000 g.*01 UND.*00 0000 _zud1
00000000 g.*01 UND.*00 0000 _zus0
00000000 g.*01 UND.*00 0000 _zus1
00000000 g.*01 UND.*00 0000 _zut0
00000000 g.*01 UND.*00 0001 a
00000000 g.*01 UND.*00 0001 a1
00000000 g.*01 UND.*00 0000 a2
0000000a.*01 COM.*00 0300 acommon0
0000000a.*01 COM.*00 0300 acommon1
00000000 g.*01 UND.*00 0001 b
00000000 g.*01 UND.*00 0001 b1
00000000 g.*01 UND.*00 0000 b2
00000000 g.*11 UND.*00 0000 g
00000000 g.*11 UND.*00 0000 g1
00000000 g.*11 UND.*00 0000 g2
0000000a.*01 COM.*00 0300 mcommon0
0000000a.*01 COM.*00 0300 mcommon1
0000000a.*01 COM.*00 0300 zcommon0
0000000a.*01 COM.*00 0300 zcommon1
