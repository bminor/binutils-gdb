#objdump: -dr --prefix-addresses -mmips:5900
#name: MIPS R5900
#as: -mcpu=r5900


.*: +file format .*mips.*

Disassembly of section \.text:
0+0000 <stuff> di
0+0004 <stuff\+(0x|)4> ei
0+0008 <stuff\+(0x|)8> mfbpc	\$t2
0+000c <stuff\+(0x|)c> mfdab	\$t3
0+0010 <stuff\+(0x|)10> mfdabm	\$t4
0+0014 <stuff\+(0x|)14> mfdvb	\$t5
0+0018 <stuff\+(0x|)18> mfdvbm	\$t6
0+001c <stuff\+(0x|)1c> mfiab	\$t7
0+0020 <stuff\+(0x|)20> mfiabm	\$s0
0+0024 <stuff\+(0x|)24> mfpc	\$a0,1
0+0028 <stuff\+(0x|)28> mfps	\$a0,1
0+002c <stuff\+(0x|)2c> mtbpc	\$t2
0+0030 <stuff\+(0x|)30> mtdab	\$t3
0+0034 <stuff\+(0x|)34> mtdabm	\$t4
0+0038 <stuff\+(0x|)38> mtdvb	\$t5
0+003c <stuff\+(0x|)3c> mtdvbm	\$t6
0+0040 <stuff\+(0x|)40> mtiab	\$t7
0+0044 <stuff\+(0x|)44> mtiabm	\$s0
0+0048 <stuff\+(0x|)48> mtpc	\$a0,1
0+004c <stuff\+(0x|)4c> mtps	\$a0,1
0+0050 <stuff\+(0x|)50> adda.s	\$f12,\$f14
0+0054 <stuff\+(0x|)54> suba.s	\$f10,\$f12
0+0058 <stuff\+(0x|)58> msuba.s	\$f10,\$f12
0+005c <stuff\+(0x|)5c> mula.s	\$f10,\$f12
0+0060 <stuff\+(0x|)60> madda.s	\$f10,\$f12
0+0064 <stuff\+(0x|)64> max.s	\$f10,\$f12,\$f14
0+0068 <stuff\+(0x|)68> min.s	\$f10,\$f12,\$f14
0+006c <stuff\+(0x|)6c> mult1	\$v1,\$a0,\$zero
0+0070 <stuff\+(0x|)70> mult1	\$a0,\$v1
0+0074 <stuff\+(0x|)74> multu1	\$v1,\$a0,\$zero
0+0078 <stuff\+(0x|)78> multu1	\$a0,\$v1
0+007c <stuff\+(0x|)7c> div1	\$zero,\$v1,\$a0
0+0080 <stuff\+(0x|)80> div1	\$zero,\$a0,\$v1
0+0084 <stuff\+(0x|)84> divu1	\$zero,\$v1,\$a0
0+0088 <stuff\+(0x|)88> divu1	\$zero,\$a0,\$v1
0+008c <stuff\+(0x|)8c> nop
