# name: H8300 GCC section test case
# ld: --gc-sections  -m h8300helf
# objdump: -d --no-show-raw-insn
.*:     file format .*-h8300

Disassembly of section .text:

00000100 <_functionWeUse>:
 100:	01 00 6d f6       mov.l	er6,@-er7
 104:	0f f6             mov.l	er7,er6
 106:	1b 97             subs	#4,er7
 108:	6f e0 ff fe       mov.w	r0,@\(0xfffe:16,er6\)
 10c:	6f 62 ff fe       mov.w	@\(0xfffe:16,er6\),r2
 110:	0d 20             mov.w	r2,r0
 112:	0b 97             adds	#4,er7
 114:	01 00 6d 76       mov.l	@er7\+,er6
 118:	54 70             rts	

0000011a <_start>:
 11a:	01 00 6d f6       mov.l	er6,@-er7
 11e:	0f f6             mov.l	er7,er6
 120:	79 00 00 4b       mov.w	#0x4b,r0
 124:	5e 00 01 00       jsr	@0x100:24
 128:	0d 02             mov.w	r0,r2
 12a:	0d 20             mov.w	r2,r0
 12c:	01 00 6d 76       mov.l	@er7\+,er6
 130:	54 70             rts	
