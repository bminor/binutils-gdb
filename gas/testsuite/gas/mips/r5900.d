#objdump: -dr --prefix-addresses
#name: MIPS R5900
#as: -mcpu=r5900


.*: +file format .*mips.*

Disassembly of section \.text:
0+0000 <stuff> di
0+0004 <stuff\+4> ei
0+0008 <stuff\+8> mfbpc	\$t2
0+000c <stuff\+c> mfdab	\$t3
0+0010 <stuff\+10> mfdabm	\$t4
0+0014 <stuff\+14> mfdvb	\$t5
0+0018 <stuff\+18> mfdvbm	\$t6
0+001c <stuff\+1c> mfiab	\$t7
0+0020 <stuff\+20> mfiabm	\$s0
0+0024 <stuff\+24> mfpc	\$a0,\$1
0+0028 <stuff\+28> mfps	\$a0,\$1
0+002c <stuff\+2c> mtbpc	\$t2
0+0030 <stuff\+30> mtdab	\$t3
0+0034 <stuff\+34> mtdabm	\$t4
0+0038 <stuff\+38> mtdvb	\$t5
0+003c <stuff\+3c> mtdvbm	\$t6
0+0040 <stuff\+40> mtiab	\$t7
0+0044 <stuff\+44> mtiabm	\$s0
0+0048 <stuff\+48> mtpc	\$a0,\$1
0+004c <stuff\+4c> mtps	\$a0,\$1
