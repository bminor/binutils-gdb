#as: -march=armv8.5-a+memtag
# objdump: -d

.*: .*


Disassembly of section \.text:

0+0 <func>:
.*:	9ac01000 	irg	x0, x0, x0
.*:	9ac0101b 	irg	x27, x0, x0
.*:	9ac01360 	irg	x0, x27, x0
.*:	9adb1000 	irg	x0, x0, x27
.*:	9adb137b 	irg	x27, x27, x27
.*:	9adf101f 	irg	sp, x0
.*:	9adf13e0 	irg	x0, sp
.*:	9ac01400 	gmi	x0, x0, x0
.*:	9ac0141b 	gmi	x27, x0, x0
.*:	9ac01760 	gmi	x0, x27, x0
.*:	9adb1400 	gmi	x0, x0, x27
.*:	9adb177b 	gmi	x27, x27, x27
.*:	9ac017e0 	gmi	x0, sp, x0
.*:	9ac0141f 	gmi	xzr, x0, x0
.*:	91800000 	addg	x0, x0, #0x0, #0x0
.*:	9180001b 	addg	x27, x0, #0x0, #0x0
.*:	91800360 	addg	x0, x27, #0x0, #0x0
.*:	9180037b 	addg	x27, x27, #0x0, #0x0
.*:	91bf3fe0 	addg	x0, sp, #0x3f0, #0xf
.*:	91aa3c1f 	addg	sp, x0, #0x2a0, #0xf
.*:	d1800000 	subg	x0, x0, #0x0, #0x0
.*:	d180001b 	subg	x27, x0, #0x0, #0x0
.*:	d1800360 	subg	x0, x27, #0x0, #0x0
.*:	d180037b 	subg	x27, x27, #0x0, #0x0
.*:	d1bf3fe0 	subg	x0, sp, #0x3f0, #0xf
.*:	d1bf141f 	subg	sp, x0, #0x3f0, #0x5
