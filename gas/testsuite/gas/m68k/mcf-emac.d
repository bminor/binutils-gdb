#name: mcf-emac
#objdump: -d --architecture=m68k:cfv4e
#as: -mcfv4e

.*:     file format .*

Disassembly of section .text:

00000000 <.text>:
       0:	a241 0280      	macw %d1l,%a1u,<<,%acc0
       4:	aad1 0280      	macw %d1u,%a5u,<<,%a1@,%a5,%acc1
       8:	a6d5 02a0      	macw %d5u,%a3u,>>,%a5@&,%a3,%acc1
       c:	a13c 00bc 614e 	movel #12345678,%acc0
      12:	a301           	movel %d1,%acc1
      14:	a33c 00bc 614e 	movel #12345678,%acc1
      1a:	a53c 00bc 614e 	movel #12345678,%acc2
      20:	a309           	movel %a1,%acc1
      22:	a73c 00bc 614e 	movel #12345678,%acc3
      28:	a8c3 0640      	macw %d3u,%a4l,,%acc0
      2c:	acc5 0040      	macw %d5u,%fpl,%acc0
      30:	a602 0800      	macl %d2,%d3,%acc0
      34:	a682 0800      	macl %d2,%d3,%acc0
      38:	a602 0810      	macl %d2,%d3,%acc2
      3c:	a682 0810      	macl %d2,%d3,%acc2
      40:	a1c1           	movclrl %acc0,%d1
      42:	a3ca           	movclrl %acc1,%a2
      44:	a5c3           	movclrl %acc2,%d3
      46:	a7cd           	movclrl %acc3,%a5
      48:	a381           	movel %acc1,%d1
      4a:	a78b           	movel %acc3,%a3
      4c:	a185           	movel %acc0,%d5
      4e:	a38f           	movel %acc1,%sp
      50:	a110           	movel %acc1,%acc0
      52:	a310           	movel %acc3,%acc1
      54:	a510           	movel %acc1,%acc2
      56:	a710           	movel %acc3,%acc3
      58:	a111           	movel %acc1,%acc0
      5a:	a311           	movel %acc3,%acc1
      5c:	a511           	movel %acc1,%acc2
      5e:	a711           	movel %acc3,%acc3
      60:	a112           	movel %acc1,%acc0
      62:	a312           	movel %acc3,%acc1
      64:	a512           	movel %acc1,%acc2
      66:	a712           	movel %acc3,%acc3
      68:	a113           	movel %acc1,%acc0
      6a:	a313           	movel %acc3,%acc1
      6c:	a513           	movel %acc1,%acc2
      6e:	a713           	movel %acc3,%acc3
      70:	ab88           	movel %accext23,%a0
      72:	af8f           	movel %accext23,%sp
      74:	a180           	movel %acc0,%d0
      76:	a389           	movel %acc1,%a1
      78:	a582           	movel %acc2,%d2
      7a:	a78b           	movel %acc3,%a3
      7c:	a4c9 0080      	macw %a1l,%a2u,%acc0
      80:	a449 0090      	macw %a1l,%a2u,%acc2
      84:	a4c9 0280      	macw %a1l,%a2u,,%acc0
      88:	a449 0290      	macw %a1l,%a2u,,%acc2
      8c:	a4c9 0680      	macw %a1l,%a2u,,%acc0
      90:	a449 0690      	macw %a1l,%a2u,,%acc2
      94:	a4c9 0280      	macw %a1l,%a2u,,%acc0
      98:	a449 0290      	macw %a1l,%a2u,,%acc2
      9c:	a4c9 0680      	macw %a1l,%a2u,,%acc0
      a0:	a449 0690      	macw %a1l,%a2u,,%acc2
      a4:	a689 0000      	macw %a1l,%d3l,%acc0
      a8:	a609 0010      	macw %a1l,%d3l,%acc2
      ac:	a689 0200      	macw %a1l,%d3l,>>,%acc0
      b0:	a609 0210      	macw %a1l,%d3l,>>,%acc2
      b4:	a689 0600      	macw %a1l,%d3l,>>,%acc0
      b8:	a609 0610      	macw %a1l,%d3l,>>,%acc2
      bc:	a689 0200      	macw %a1l,%d3l,>>,%acc0
      c0:	a609 0210      	macw %a1l,%d3l,>>,%acc2
      c4:	a689 0600      	macw %a1l,%d3l,>>,%acc0
      c8:	a609 0610      	macw %a1l,%d3l,>>,%acc2
      cc:	aec9 0080      	macw %a1l,%spu,%acc0
      d0:	ae49 0090      	macw %a1l,%spu,%acc2
      d4:	aec9 0280      	macw %a1l,%spu,>>,%acc0
      d8:	ae49 0290      	macw %a1l,%spu,>>,%acc2
      dc:	aec9 0680      	macw %a1l,%spu,>>,%acc0
      e0:	ae49 0690      	macw %a1l,%spu,>>,%acc2
      e4:	aec9 0280      	macw %a1l,%spu,>>,%acc0
      e8:	ae49 0290      	macw %a1l,%spu,>>,%acc2
      ec:	aec9 0680      	macw %a1l,%spu,>>,%acc0
      f0:	ae49 0690      	macw %a1l,%spu,>>,%acc2
      f4:	a289 0000      	macw %a1l,%d1l,%acc0
      f8:	a209 0010      	macw %a1l,%d1l,%acc2
      fc:	a289 0200      	macw %a1l,%d1l,<<,%acc0
     100:	a209 0210      	macw %a1l,%d1l,<<,%acc2
     104:	a289 0600      	macw %a1l,%d1l,<<,%acc0
     108:	a209 0610      	macw %a1l,%d1l,<<,%acc2
     10c:	a289 0200      	macw %a1l,%d1l,<<,%acc0
     110:	a209 0210      	macw %a1l,%d1l,<<,%acc2
     114:	a289 0600      	macw %a1l,%d1l,<<,%acc0
     118:	a209 0610      	macw %a1l,%d1l,<<,%acc2
     11c:	a4c2 00c0      	macw %d2u,%a2u,%acc0
     120:	a442 00d0      	macw %d2u,%a2u,%acc2
     124:	a4c2 02c0      	macw %d2u,%a2u,,%acc0
     128:	a442 02d0      	macw %d2u,%a2u,,%acc2
     12c:	a4c2 06c0      	macw %d2u,%a2u,,%acc0
     130:	a442 06d0      	macw %d2u,%a2u,,%acc2
     134:	a4c2 02c0      	macw %d2u,%a2u,,%acc0
     138:	a442 02d0      	macw %d2u,%a2u,,%acc2
     13c:	a4c2 06c0      	macw %d2u,%a2u,,%acc0
     140:	a442 06d0      	macw %d2u,%a2u,,%acc2
     144:	a682 0040      	macw %d2u,%d3l,%acc0
     148:	a602 0050      	macw %d2u,%d3l,%acc2
     14c:	a682 0240      	macw %d2u,%d3l,>>,%acc0
     150:	a602 0250      	macw %d2u,%d3l,>>,%acc2
     154:	a682 0640      	macw %d2u,%d3l,>>,%acc0
     158:	a602 0650      	macw %d2u,%d3l,>>,%acc2
     15c:	a682 0240      	macw %d2u,%d3l,>>,%acc0
     160:	a602 0250      	macw %d2u,%d3l,>>,%acc2
     164:	a682 0640      	macw %d2u,%d3l,>>,%acc0
     168:	a602 0650      	macw %d2u,%d3l,>>,%acc2
     16c:	aec2 00c0      	macw %d2u,%spu,%acc0
     170:	ae42 00d0      	macw %d2u,%spu,%acc2
     174:	aec2 02c0      	macw %d2u,%spu,>>,%acc0
     178:	ae42 02d0      	macw %d2u,%spu,>>,%acc2
     17c:	aec2 06c0      	macw %d2u,%spu,>>,%acc0
     180:	ae42 06d0      	macw %d2u,%spu,>>,%acc2
     184:	aec2 02c0      	macw %d2u,%spu,>>,%acc0
     188:	ae42 02d0      	macw %d2u,%spu,>>,%acc2
     18c:	aec2 06c0      	macw %d2u,%spu,>>,%acc0
     190:	ae42 06d0      	macw %d2u,%spu,>>,%acc2
     194:	a282 0040      	macw %d2u,%d1l,%acc0
     198:	a202 0050      	macw %d2u,%d1l,%acc2
     19c:	a282 0240      	macw %d2u,%d1l,<<,%acc0
     1a0:	a202 0250      	macw %d2u,%d1l,<<,%acc2
     1a4:	a282 0640      	macw %d2u,%d1l,<<,%acc0
     1a8:	a202 0650      	macw %d2u,%d1l,<<,%acc2
     1ac:	a282 0240      	macw %d2u,%d1l,<<,%acc0
     1b0:	a202 0250      	macw %d2u,%d1l,<<,%acc2
     1b4:	a282 0640      	macw %d2u,%d1l,<<,%acc0
     1b8:	a202 0650      	macw %d2u,%d1l,<<,%acc2
     1bc:	a4cd 0080      	macw %a5l,%a2u,%acc0
     1c0:	a44d 0090      	macw %a5l,%a2u,%acc2
     1c4:	a4cd 0280      	macw %a5l,%a2u,,%acc0
     1c8:	a44d 0290      	macw %a5l,%a2u,,%acc2
     1cc:	a4cd 0680      	macw %a5l,%a2u,,%acc0
     1d0:	a44d 0690      	macw %a5l,%a2u,,%acc2
     1d4:	a4cd 0280      	macw %a5l,%a2u,,%acc0
     1d8:	a44d 0290      	macw %a5l,%a2u,,%acc2
     1dc:	a4cd 0680      	macw %a5l,%a2u,,%acc0
     1e0:	a44d 0690      	macw %a5l,%a2u,,%acc2
     1e4:	a68d 0000      	macw %a5l,%d3l,%acc0
     1e8:	a60d 0010      	macw %a5l,%d3l,%acc2
     1ec:	a68d 0200      	macw %a5l,%d3l,>>,%acc0
     1f0:	a60d 0210      	macw %a5l,%d3l,>>,%acc2
     1f4:	a68d 0600      	macw %a5l,%d3l,>>,%acc0
     1f8:	a60d 0610      	macw %a5l,%d3l,>>,%acc2
     1fc:	a68d 0200      	macw %a5l,%d3l,>>,%acc0
     200:	a60d 0210      	macw %a5l,%d3l,>>,%acc2
     204:	a68d 0600      	macw %a5l,%d3l,>>,%acc0
     208:	a60d 0610      	macw %a5l,%d3l,>>,%acc2
     20c:	aecd 0080      	macw %a5l,%spu,%acc0
     210:	ae4d 0090      	macw %a5l,%spu,%acc2
     214:	aecd 0280      	macw %a5l,%spu,>>,%acc0
     218:	ae4d 0290      	macw %a5l,%spu,>>,%acc2
     21c:	aecd 0680      	macw %a5l,%spu,>>,%acc0
     220:	ae4d 0690      	macw %a5l,%spu,>>,%acc2
     224:	aecd 0280      	macw %a5l,%spu,>>,%acc0
     228:	ae4d 0290      	macw %a5l,%spu,>>,%acc2
     22c:	aecd 0680      	macw %a5l,%spu,>>,%acc0
     230:	ae4d 0690      	macw %a5l,%spu,>>,%acc2
     234:	a28d 0000      	macw %a5l,%d1l,%acc0
     238:	a20d 0010      	macw %a5l,%d1l,%acc2
     23c:	a28d 0200      	macw %a5l,%d1l,<<,%acc0
     240:	a20d 0210      	macw %a5l,%d1l,<<,%acc2
     244:	a28d 0600      	macw %a5l,%d1l,<<,%acc0
     248:	a20d 0610      	macw %a5l,%d1l,<<,%acc2
     24c:	a28d 0200      	macw %a5l,%d1l,<<,%acc0
     250:	a20d 0210      	macw %a5l,%d1l,<<,%acc2
     254:	a28d 0600      	macw %a5l,%d1l,<<,%acc0
     258:	a20d 0610      	macw %a5l,%d1l,<<,%acc2
     25c:	a4c6 00c0      	macw %d6u,%a2u,%acc0
     260:	a446 00d0      	macw %d6u,%a2u,%acc2
     264:	a4c6 02c0      	macw %d6u,%a2u,,%acc0
     268:	a446 02d0      	macw %d6u,%a2u,,%acc2
     26c:	a4c6 06c0      	macw %d6u,%a2u,,%acc0
     270:	a446 06d0      	macw %d6u,%a2u,,%acc2
     274:	a4c6 02c0      	macw %d6u,%a2u,,%acc0
     278:	a446 02d0      	macw %d6u,%a2u,,%acc2
     27c:	a4c6 06c0      	macw %d6u,%a2u,,%acc0
     280:	a446 06d0      	macw %d6u,%a2u,,%acc2
     284:	a686 0040      	macw %d6u,%d3l,%acc0
     288:	a606 0050      	macw %d6u,%d3l,%acc2
     28c:	a686 0240      	macw %d6u,%d3l,>>,%acc0
     290:	a606 0250      	macw %d6u,%d3l,>>,%acc2
     294:	a686 0640      	macw %d6u,%d3l,>>,%acc0
     298:	a606 0650      	macw %d6u,%d3l,>>,%acc2
     29c:	a686 0240      	macw %d6u,%d3l,>>,%acc0
     2a0:	a606 0250      	macw %d6u,%d3l,>>,%acc2
     2a4:	a686 0640      	macw %d6u,%d3l,>>,%acc0
     2a8:	a606 0650      	macw %d6u,%d3l,>>,%acc2
     2ac:	aec6 00c0      	macw %d6u,%spu,%acc0
     2b0:	ae46 00d0      	macw %d6u,%spu,%acc2
     2b4:	aec6 02c0      	macw %d6u,%spu,>>,%acc0
     2b8:	ae46 02d0      	macw %d6u,%spu,>>,%acc2
     2bc:	aec6 06c0      	macw %d6u,%spu,>>,%acc0
     2c0:	ae46 06d0      	macw %d6u,%spu,>>,%acc2
     2c4:	aec6 02c0      	macw %d6u,%spu,>>,%acc0
     2c8:	ae46 02d0      	macw %d6u,%spu,>>,%acc2
     2cc:	aec6 06c0      	macw %d6u,%spu,>>,%acc0
     2d0:	ae46 06d0      	macw %d6u,%spu,>>,%acc2
     2d4:	a286 0040      	macw %d6u,%d1l,%acc0
     2d8:	a206 0050      	macw %d6u,%d1l,%acc2
     2dc:	a286 0240      	macw %d6u,%d1l,<<,%acc0
     2e0:	a206 0250      	macw %d6u,%d1l,<<,%acc2
     2e4:	a286 0640      	macw %d6u,%d1l,<<,%acc0
     2e8:	a206 0650      	macw %d6u,%d1l,<<,%acc2
     2ec:	a286 0240      	macw %d6u,%d1l,<<,%acc0
     2f0:	a206 0250      	macw %d6u,%d1l,<<,%acc2
     2f4:	a286 0640      	macw %d6u,%d1l,<<,%acc0
     2f8:	a206 0650      	macw %d6u,%d1l,<<,%acc2
     2fc:	a65b 0080      	macw %a3u,%a3u,%a3@\+,%a3,%acc1
     300:	a6db 0090      	macw %a3u,%a3u,%a3@\+,%a3,%acc3
     304:	a65b 0080      	macw %a3u,%a3u,%a3@\+,%a3,%acc1
     308:	a6db 0090      	macw %a3u,%a3u,%a3@\+,%a3,%acc3
     30c:	a45b 0080      	macw %a3u,%a2u,%a3@\+,%a2,%acc1
     310:	a4db 0090      	macw %a3u,%a2u,%a3@\+,%a2,%acc3
     314:	ae5b 0080      	macw %a3u,%spu,%a3@\+,%sp,%acc1
     318:	aedb 0090      	macw %a3u,%spu,%a3@\+,%sp,%acc3
     31c:	a65b 00a0      	macw %a3u,%a3u,%a3@\+&,%a3,%acc1
     320:	a6db 00b0      	macw %a3u,%a3u,%a3@\+&,%a3,%acc3
     324:	a65b 00a0      	macw %a3u,%a3u,%a3@\+&,%a3,%acc1
     328:	a6db 00b0      	macw %a3u,%a3u,%a3@\+&,%a3,%acc3
     32c:	a45b 00a0      	macw %a3u,%a2u,%a3@\+&,%a2,%acc1
     330:	a4db 00b0      	macw %a3u,%a2u,%a3@\+&,%a2,%acc3
     334:	ae5b 00a0      	macw %a3u,%spu,%a3@\+&,%sp,%acc1
     338:	aedb 00b0      	macw %a3u,%spu,%a3@\+&,%sp,%acc3
     33c:	a65b 0080      	macw %a3u,%a3u,%a3@\+,%a3,%acc1
     340:	a6db 0090      	macw %a3u,%a3u,%a3@\+,%a3,%acc3
     344:	a65b 0080      	macw %a3u,%a3u,%a3@\+,%a3,%acc1
     348:	a6db 0090      	macw %a3u,%a3u,%a3@\+,%a3,%acc3
     34c:	a45b 0080      	macw %a3u,%a2u,%a3@\+,%a2,%acc1
     350:	a4db 0090      	macw %a3u,%a2u,%a3@\+,%a2,%acc3
     354:	ae5b 0080      	macw %a3u,%spu,%a3@\+,%sp,%acc1
     358:	aedb 0090      	macw %a3u,%spu,%a3@\+,%sp,%acc3
     35c:	a65b 00a0      	macw %a3u,%a3u,%a3@\+&,%a3,%acc1
     360:	a6db 00b0      	macw %a3u,%a3u,%a3@\+&,%a3,%acc3
     364:	a65b 00a0      	macw %a3u,%a3u,%a3@\+&,%a3,%acc1
     368:	a6db 00b0      	macw %a3u,%a3u,%a3@\+&,%a3,%acc3
     36c:	a45b 00a0      	macw %a3u,%a2u,%a3@\+&,%a2,%acc1
     370:	a4db 00b0      	macw %a3u,%a2u,%a3@\+&,%a2,%acc3
     374:	ae5b 00a0      	macw %a3u,%spu,%a3@\+&,%sp,%acc1
     378:	aedb 00b0      	macw %a3u,%spu,%a3@\+&,%sp,%acc3
     37c:	a66f 0080 000a 	macw %spl,%a3u,%sp@\(10\),%a3,%acc1
     382:	a6ef 0090 000a 	macw %spl,%a3u,%sp@\(10\),%a3,%acc3
     388:	a66f 0080 000a 	macw %spl,%a3u,%sp@\(10\),%a3,%acc1
     38e:	a6ef 0090 000a 	macw %spl,%a3u,%sp@\(10\),%a3,%acc3
     394:	a46f 0080 000a 	macw %spl,%a2u,%sp@\(10\),%a2,%acc1
     39a:	a4ef 0090 000a 	macw %spl,%a2u,%sp@\(10\),%a2,%acc3
     3a0:	ae6f 0080 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc1
     3a6:	aeef 0090 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc3
     3ac:	a66f 00a0 000a 	macw %spl,%a3u,%sp@\(10\)&,%a3,%acc1
     3b2:	a6ef 00b0 000a 	macw %spl,%a3u,%sp@\(10\)&,%a3,%acc3
     3b8:	a66f 00a0 000a 	macw %spl,%a3u,%sp@\(10\)&,%a3,%acc1
     3be:	a6ef 00b0 000a 	macw %spl,%a3u,%sp@\(10\)&,%a3,%acc3
     3c4:	a46f 00a0 000a 	macw %spl,%a2u,%sp@\(10\)&,%a2,%acc1
     3ca:	a4ef 00b0 000a 	macw %spl,%a2u,%sp@\(10\)&,%a2,%acc3
     3d0:	ae6f 00a0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc1
     3d6:	aeef 00b0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc3
     3dc:	a669 0080 a6e9 	macw %a1l,%a3u,%a1@\(-22807\),%a3,%acc1
     3e2:	0090 a669 0080 	oril #-1503068032,%d0
     3e8:	a6e9 0090 a469 	macw %a1l,%a3u,%a1@\(-23447\),%a3,%acc3
     3ee:	0080 a4e9 0090 	oril #-1528233840,%d0
     3f4:	ae69 0080 aee9 	macw %a1l,%spu,%a1@\(-20759\),%sp,%acc1
     3fa:	0090 a669 00a0 	oril #-1503068000,%d0
     400:	a6e9 00b0 a669 	macw %a1l,%a3u,%a1@\(-22935\)&,%a3,%acc3
     406:	00a0 a6e9 00b0 	oril #-1494679376,%d0
     40c:	a469 00a0 a4e9 	macw %a1l,%a2u,%a1@\(-23319\)&,%a2,%acc1
     412:	00b0 ae69 00a0 	oril #-1368850272,%d0
     418:	aee9 00b0 a65b 	macw %a1l,%spu,%a1@\(-22949\)&,%sp,%acc3
     41e:	0280 a6db 0290 	andil #-1495596400,%d0
     424:	a65b 0280      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc1
     428:	a6db 0290      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc3
     42c:	a45b 0280      	macw %a3u,%a2u,,%a3@\+,%a2,%acc1
     430:	a4db 0290      	macw %a3u,%a2u,,%a3@\+,%a2,%acc3
     434:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     438:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     43c:	a65b 02a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     440:	a6db 02b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     444:	a65b 02a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     448:	a6db 02b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     44c:	a45b 02a0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc1
     450:	a4db 02b0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc3
     454:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     458:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     45c:	a65b 0280      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc1
     460:	a6db 0290      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc3
     464:	a65b 0280      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc1
     468:	a6db 0290      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc3
     46c:	a45b 0280      	macw %a3u,%a2u,,%a3@\+,%a2,%acc1
     470:	a4db 0290      	macw %a3u,%a2u,,%a3@\+,%a2,%acc3
     474:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     478:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     47c:	a65b 02a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     480:	a6db 02b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     484:	a65b 02a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     488:	a6db 02b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     48c:	a45b 02a0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc1
     490:	a4db 02b0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc3
     494:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     498:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     49c:	a66f 0280 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
     4a2:	a6ef 0290 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
     4a8:	a66f 0280 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
     4ae:	a6ef 0290 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
     4b4:	a46f 0280 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc1
     4ba:	a4ef 0290 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc3
     4c0:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
     4c6:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
     4cc:	a66f 02a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
     4d2:	a6ef 02b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
     4d8:	a66f 02a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
     4de:	a6ef 02b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
     4e4:	a46f 02a0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc1
     4ea:	a4ef 02b0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc3
     4f0:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
     4f6:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
     4fc:	a669 0280 a6e9 	macw %a1l,%a3u,>>,%a1@\(-22807\),%a3,%acc1
     502:	0290 a669 0280 	andil #-1503067520,%d0
     508:	a6e9 0290 a469 	macw %a1l,%a3u,>>,%a1@\(-23447\),%a3,%acc3
     50e:	0280 a4e9 0290 	andil #-1528233328,%d0
     514:	ae69 0280 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\),%sp,%acc1
     51a:	0290 a669 02a0 	andil #-1503067488,%d0
     520:	a6e9 02b0 a669 	macw %a1l,%a3u,>>,%a1@\(-22935\)&,%a3,%acc3
     526:	02a0 a6e9 02b0 	andil #-1494678864,%d0
     52c:	a469 02a0 a4e9 	macw %a1l,%a2u,,%a1@\(-23319\)&,%a2,%acc1
     532:	02b0 ae69 02a0 	andil #-1368849760,%d0
     538:	aee9 02b0 a65b 	macw %a1l,%spu,>>,%a1@\(-22949\)&,%sp,%acc3
     53e:	0680 a6db 0690 	addil #-1495595376,%d0
     544:	a65b 0680      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc1
     548:	a6db 0690      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc3
     54c:	a45b 0680      	macw %a3u,%a2u,,%a3@\+,%a2,%acc1
     550:	a4db 0690      	macw %a3u,%a2u,,%a3@\+,%a2,%acc3
     554:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     558:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     55c:	a65b 06a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     560:	a6db 06b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     564:	a65b 06a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     568:	a6db 06b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     56c:	a45b 06a0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc1
     570:	a4db 06b0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc3
     574:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     578:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     57c:	a65b 0680      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc1
     580:	a6db 0690      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc3
     584:	a65b 0680      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc1
     588:	a6db 0690      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc3
     58c:	a45b 0680      	macw %a3u,%a2u,,%a3@\+,%a2,%acc1
     590:	a4db 0690      	macw %a3u,%a2u,,%a3@\+,%a2,%acc3
     594:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     598:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     59c:	a65b 06a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     5a0:	a6db 06b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     5a4:	a65b 06a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     5a8:	a6db 06b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     5ac:	a45b 06a0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc1
     5b0:	a4db 06b0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc3
     5b4:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     5b8:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     5bc:	a66f 0680 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
     5c2:	a6ef 0690 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
     5c8:	a66f 0680 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
     5ce:	a6ef 0690 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
     5d4:	a46f 0680 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc1
     5da:	a4ef 0690 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc3
     5e0:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
     5e6:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
     5ec:	a66f 06a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
     5f2:	a6ef 06b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
     5f8:	a66f 06a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
     5fe:	a6ef 06b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
     604:	a46f 06a0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc1
     60a:	a4ef 06b0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc3
     610:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
     616:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
     61c:	a669 0680 a6e9 	macw %a1l,%a3u,>>,%a1@\(-22807\),%a3,%acc1
     622:	0690 a669 0680 	addil #-1503066496,%d0
     628:	a6e9 0690 a469 	macw %a1l,%a3u,>>,%a1@\(-23447\),%a3,%acc3
     62e:	0680 a4e9 0690 	addil #-1528232304,%d0
     634:	ae69 0680 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\),%sp,%acc1
     63a:	0690 a669 06a0 	addil #-1503066464,%d0
     640:	a6e9 06b0 a669 	macw %a1l,%a3u,>>,%a1@\(-22935\)&,%a3,%acc3
     646:	06a0 a6e9 06b0 	addil #-1494677840,%d0
     64c:	a469 06a0 a4e9 	macw %a1l,%a2u,,%a1@\(-23319\)&,%a2,%acc1
     652:	06b0 ae69 06a0 	addil #-1368848736,%d0
     658:	aee9 06b0 a65b 	macw %a1l,%spu,>>,%a1@\(-22949\)&,%sp,%acc3
     65e:	0280 a6db 0290 	andil #-1495596400,%d0
     664:	a65b 0280      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc1
     668:	a6db 0290      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc3
     66c:	a45b 0280      	macw %a3u,%a2u,,%a3@\+,%a2,%acc1
     670:	a4db 0290      	macw %a3u,%a2u,,%a3@\+,%a2,%acc3
     674:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     678:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     67c:	a65b 02a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     680:	a6db 02b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     684:	a65b 02a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     688:	a6db 02b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     68c:	a45b 02a0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc1
     690:	a4db 02b0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc3
     694:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     698:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     69c:	a65b 0280      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc1
     6a0:	a6db 0290      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc3
     6a4:	a65b 0280      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc1
     6a8:	a6db 0290      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc3
     6ac:	a45b 0280      	macw %a3u,%a2u,,%a3@\+,%a2,%acc1
     6b0:	a4db 0290      	macw %a3u,%a2u,,%a3@\+,%a2,%acc3
     6b4:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     6b8:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     6bc:	a65b 02a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     6c0:	a6db 02b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     6c4:	a65b 02a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     6c8:	a6db 02b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     6cc:	a45b 02a0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc1
     6d0:	a4db 02b0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc3
     6d4:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     6d8:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     6dc:	a66f 0280 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
     6e2:	a6ef 0290 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
     6e8:	a66f 0280 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
     6ee:	a6ef 0290 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
     6f4:	a46f 0280 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc1
     6fa:	a4ef 0290 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc3
     700:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
     706:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
     70c:	a66f 02a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
     712:	a6ef 02b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
     718:	a66f 02a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
     71e:	a6ef 02b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
     724:	a46f 02a0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc1
     72a:	a4ef 02b0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc3
     730:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
     736:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
     73c:	a669 0280 a6e9 	macw %a1l,%a3u,>>,%a1@\(-22807\),%a3,%acc1
     742:	0290 a669 0280 	andil #-1503067520,%d0
     748:	a6e9 0290 a469 	macw %a1l,%a3u,>>,%a1@\(-23447\),%a3,%acc3
     74e:	0280 a4e9 0290 	andil #-1528233328,%d0
     754:	ae69 0280 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\),%sp,%acc1
     75a:	0290 a669 02a0 	andil #-1503067488,%d0
     760:	a6e9 02b0 a669 	macw %a1l,%a3u,>>,%a1@\(-22935\)&,%a3,%acc3
     766:	02a0 a6e9 02b0 	andil #-1494678864,%d0
     76c:	a469 02a0 a4e9 	macw %a1l,%a2u,,%a1@\(-23319\)&,%a2,%acc1
     772:	02b0 ae69 02a0 	andil #-1368849760,%d0
     778:	aee9 02b0 a65b 	macw %a1l,%spu,>>,%a1@\(-22949\)&,%sp,%acc3
     77e:	0680 a6db 0690 	addil #-1495595376,%d0
     784:	a65b 0680      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc1
     788:	a6db 0690      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc3
     78c:	a45b 0680      	macw %a3u,%a2u,,%a3@\+,%a2,%acc1
     790:	a4db 0690      	macw %a3u,%a2u,,%a3@\+,%a2,%acc3
     794:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     798:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     79c:	a65b 06a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     7a0:	a6db 06b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     7a4:	a65b 06a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     7a8:	a6db 06b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     7ac:	a45b 06a0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc1
     7b0:	a4db 06b0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc3
     7b4:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     7b8:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     7bc:	a65b 0680      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc1
     7c0:	a6db 0690      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc3
     7c4:	a65b 0680      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc1
     7c8:	a6db 0690      	macw %a3u,%a3u,>>,%a3@\+,%a3,%acc3
     7cc:	a45b 0680      	macw %a3u,%a2u,,%a3@\+,%a2,%acc1
     7d0:	a4db 0690      	macw %a3u,%a2u,,%a3@\+,%a2,%acc3
     7d4:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     7d8:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     7dc:	a65b 06a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     7e0:	a6db 06b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     7e4:	a65b 06a0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc1
     7e8:	a6db 06b0      	macw %a3u,%a3u,>>,%a3@\+&,%a3,%acc3
     7ec:	a45b 06a0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc1
     7f0:	a4db 06b0      	macw %a3u,%a2u,,%a3@\+&,%a2,%acc3
     7f4:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     7f8:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     7fc:	a66f 0680 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
     802:	a6ef 0690 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
     808:	a66f 0680 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
     80e:	a6ef 0690 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
     814:	a46f 0680 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc1
     81a:	a4ef 0690 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc3
     820:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
     826:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
     82c:	a66f 06a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
     832:	a6ef 06b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
     838:	a66f 06a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
     83e:	a6ef 06b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
     844:	a46f 06a0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc1
     84a:	a4ef 06b0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc3
     850:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
     856:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
     85c:	a669 0680 a6e9 	macw %a1l,%a3u,>>,%a1@\(-22807\),%a3,%acc1
     862:	0690 a669 0680 	addil #-1503066496,%d0
     868:	a6e9 0690 a469 	macw %a1l,%a3u,>>,%a1@\(-23447\),%a3,%acc3
     86e:	0680 a4e9 0690 	addil #-1528232304,%d0
     874:	ae69 0680 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\),%sp,%acc1
     87a:	0690 a669 06a0 	addil #-1503066464,%d0
     880:	a6e9 06b0 a669 	macw %a1l,%a3u,>>,%a1@\(-22935\)&,%a3,%acc3
     886:	06a0 a6e9 06b0 	addil #-1494677840,%d0
     88c:	a469 06a0 a4e9 	macw %a1l,%a2u,,%a1@\(-23319\)&,%a2,%acc1
     892:	06b0 ae69 06a0 	addil #-1368848736,%d0
     898:	aee9 06b0 a61b 	macw %a1l,%spu,>>,%a1@\(-23013\)&,%sp,%acc3
     89e:	0000           	00
     8a0:	a69b 0010      	macw %a3u,%d3l,%a3@\+,%d3,%acc3
     8a4:	a65b 0000      	macw %a3u,%a3l,%a3@\+,%a3,%acc1
     8a8:	a6db 0010      	macw %a3u,%a3l,%a3@\+,%a3,%acc3
     8ac:	a61b 0000      	macw %a3u,%d3l,%a3@\+,%d3,%acc1
     8b0:	a69b 0010      	macw %a3u,%d3l,%a3@\+,%d3,%acc3
     8b4:	ae5b 0000      	macw %a3u,%spl,%a3@\+,%sp,%acc1
     8b8:	aedb 0010      	macw %a3u,%spl,%a3@\+,%sp,%acc3
     8bc:	a61b 0020      	macw %a3u,%d3l,%a3@\+&,%d3,%acc1
     8c0:	a69b 0030      	macw %a3u,%d3l,%a3@\+&,%d3,%acc3
     8c4:	a65b 0020      	macw %a3u,%a3l,%a3@\+&,%a3,%acc1
     8c8:	a6db 0030      	macw %a3u,%a3l,%a3@\+&,%a3,%acc3
     8cc:	a61b 0020      	macw %a3u,%d3l,%a3@\+&,%d3,%acc1
     8d0:	a69b 0030      	macw %a3u,%d3l,%a3@\+&,%d3,%acc3
     8d4:	ae5b 0020      	macw %a3u,%spl,%a3@\+&,%sp,%acc1
     8d8:	aedb 0030      	macw %a3u,%spl,%a3@\+&,%sp,%acc3
     8dc:	a61b 0000      	macw %a3u,%d3l,%a3@\+,%d3,%acc1
     8e0:	a69b 0010      	macw %a3u,%d3l,%a3@\+,%d3,%acc3
     8e4:	a65b 0000      	macw %a3u,%a3l,%a3@\+,%a3,%acc1
     8e8:	a6db 0010      	macw %a3u,%a3l,%a3@\+,%a3,%acc3
     8ec:	a61b 0000      	macw %a3u,%d3l,%a3@\+,%d3,%acc1
     8f0:	a69b 0010      	macw %a3u,%d3l,%a3@\+,%d3,%acc3
     8f4:	ae5b 0000      	macw %a3u,%spl,%a3@\+,%sp,%acc1
     8f8:	aedb 0010      	macw %a3u,%spl,%a3@\+,%sp,%acc3
     8fc:	a61b 0020      	macw %a3u,%d3l,%a3@\+&,%d3,%acc1
     900:	a69b 0030      	macw %a3u,%d3l,%a3@\+&,%d3,%acc3
     904:	a65b 0020      	macw %a3u,%a3l,%a3@\+&,%a3,%acc1
     908:	a6db 0030      	macw %a3u,%a3l,%a3@\+&,%a3,%acc3
     90c:	a61b 0020      	macw %a3u,%d3l,%a3@\+&,%d3,%acc1
     910:	a69b 0030      	macw %a3u,%d3l,%a3@\+&,%d3,%acc3
     914:	ae5b 0020      	macw %a3u,%spl,%a3@\+&,%sp,%acc1
     918:	aedb 0030      	macw %a3u,%spl,%a3@\+&,%sp,%acc3
     91c:	a62f 0000 000a 	macw %spl,%d3l,%sp@\(10\),%d3,%acc1
     922:	a6af 0010 000a 	macw %spl,%d3l,%sp@\(10\),%d3,%acc3
     928:	a66f 0000 000a 	macw %spl,%a3l,%sp@\(10\),%a3,%acc1
     92e:	a6ef 0010 000a 	macw %spl,%a3l,%sp@\(10\),%a3,%acc3
     934:	a62f 0000 000a 	macw %spl,%d3l,%sp@\(10\),%d3,%acc1
     93a:	a6af 0010 000a 	macw %spl,%d3l,%sp@\(10\),%d3,%acc3
     940:	ae6f 0000 000a 	macw %spl,%spl,%sp@\(10\),%sp,%acc1
     946:	aeef 0010 000a 	macw %spl,%spl,%sp@\(10\),%sp,%acc3
     94c:	a62f 0020 000a 	macw %spl,%d3l,%sp@\(10\)&,%d3,%acc1
     952:	a6af 0030 000a 	macw %spl,%d3l,%sp@\(10\)&,%d3,%acc3
     958:	a66f 0020 000a 	macw %spl,%a3l,%sp@\(10\)&,%a3,%acc1
     95e:	a6ef 0030 000a 	macw %spl,%a3l,%sp@\(10\)&,%a3,%acc3
     964:	a62f 0020 000a 	macw %spl,%d3l,%sp@\(10\)&,%d3,%acc1
     96a:	a6af 0030 000a 	macw %spl,%d3l,%sp@\(10\)&,%d3,%acc3
     970:	ae6f 0020 000a 	macw %spl,%spl,%sp@\(10\)&,%sp,%acc1
     976:	aeef 0030 000a 	macw %spl,%spl,%sp@\(10\)&,%sp,%acc3
     97c:	a629 0000 a6a9 	macw %a1l,%d3l,%a1@\(-22871\),%d3,%acc1
     982:	0010           	020
     984:	a669 0000 a6e9 	macw %a1l,%a3l,%a1@\(-22807\),%a3,%acc1
     98a:	0010           	020
     98c:	a629 0000 a6a9 	macw %a1l,%d3l,%a1@\(-22871\),%d3,%acc1
     992:	0010           	020
     994:	ae69 0000 aee9 	macw %a1l,%spl,%a1@\(-20759\),%sp,%acc1
     99a:	0010           	020
     99c:	a629 0020 a6a9 	macw %a1l,%d3l,%a1@\(-22871\)&,%d3,%acc1
     9a2:	0030           	060
     9a4:	a669 0020 a6e9 	macw %a1l,%a3l,%a1@\(-22807\)&,%a3,%acc1
     9aa:	0030           	060
     9ac:	a629 0020 a6a9 	macw %a1l,%d3l,%a1@\(-22871\)&,%d3,%acc1
     9b2:	0030           	060
     9b4:	ae69 0020 aee9 	macw %a1l,%spl,%a1@\(-20759\)&,%sp,%acc1
     9ba:	0030           	060
     9bc:	a61b 0200      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     9c0:	a69b 0210      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     9c4:	a65b 0200      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
     9c8:	a6db 0210      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
     9cc:	a61b 0200      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     9d0:	a69b 0210      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     9d4:	ae5b 0200      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
     9d8:	aedb 0210      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
     9dc:	a61b 0220      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     9e0:	a69b 0230      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     9e4:	a65b 0220      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
     9e8:	a6db 0230      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
     9ec:	a61b 0220      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     9f0:	a69b 0230      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     9f4:	ae5b 0220      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
     9f8:	aedb 0230      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
     9fc:	a61b 0200      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     a00:	a69b 0210      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     a04:	a65b 0200      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
     a08:	a6db 0210      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
     a0c:	a61b 0200      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     a10:	a69b 0210      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     a14:	ae5b 0200      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
     a18:	aedb 0210      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
     a1c:	a61b 0220      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     a20:	a69b 0230      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     a24:	a65b 0220      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
     a28:	a6db 0230      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
     a2c:	a61b 0220      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     a30:	a69b 0230      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     a34:	ae5b 0220      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
     a38:	aedb 0230      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
     a3c:	a62f 0200 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
     a42:	a6af 0210 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
     a48:	a66f 0200 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
     a4e:	a6ef 0210 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
     a54:	a62f 0200 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
     a5a:	a6af 0210 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
     a60:	ae6f 0200 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
     a66:	aeef 0210 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
     a6c:	a62f 0220 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
     a72:	a6af 0230 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
     a78:	a66f 0220 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
     a7e:	a6ef 0230 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
     a84:	a62f 0220 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
     a8a:	a6af 0230 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
     a90:	ae6f 0220 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
     a96:	aeef 0230 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
     a9c:	a629 0200 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\),%d3,%acc1
     aa2:	0210           	01020
     aa4:	a669 0200 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\),%a3,%acc1
     aaa:	0210           	01020
     aac:	a629 0200 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\),%d3,%acc1
     ab2:	0210           	01020
     ab4:	ae69 0200 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\),%sp,%acc1
     aba:	0210           	01020
     abc:	a629 0220 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\)&,%d3,%acc1
     ac2:	0230           	01060
     ac4:	a669 0220 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\)&,%a3,%acc1
     aca:	0230           	01060
     acc:	a629 0220 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\)&,%d3,%acc1
     ad2:	0230           	01060
     ad4:	ae69 0220 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\)&,%sp,%acc1
     ada:	0230           	01060
     adc:	a61b 0600      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     ae0:	a69b 0610      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     ae4:	a65b 0600      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
     ae8:	a6db 0610      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
     aec:	a61b 0600      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     af0:	a69b 0610      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     af4:	ae5b 0600      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
     af8:	aedb 0610      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
     afc:	a61b 0620      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     b00:	a69b 0630      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     b04:	a65b 0620      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
     b08:	a6db 0630      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
     b0c:	a61b 0620      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     b10:	a69b 0630      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     b14:	ae5b 0620      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
     b18:	aedb 0630      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
     b1c:	a61b 0600      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     b20:	a69b 0610      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     b24:	a65b 0600      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
     b28:	a6db 0610      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
     b2c:	a61b 0600      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     b30:	a69b 0610      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     b34:	ae5b 0600      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
     b38:	aedb 0610      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
     b3c:	a61b 0620      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     b40:	a69b 0630      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     b44:	a65b 0620      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
     b48:	a6db 0630      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
     b4c:	a61b 0620      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     b50:	a69b 0630      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     b54:	ae5b 0620      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
     b58:	aedb 0630      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
     b5c:	a62f 0600 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
     b62:	a6af 0610 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
     b68:	a66f 0600 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
     b6e:	a6ef 0610 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
     b74:	a62f 0600 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
     b7a:	a6af 0610 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
     b80:	ae6f 0600 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
     b86:	aeef 0610 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
     b8c:	a62f 0620 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
     b92:	a6af 0630 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
     b98:	a66f 0620 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
     b9e:	a6ef 0630 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
     ba4:	a62f 0620 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
     baa:	a6af 0630 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
     bb0:	ae6f 0620 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
     bb6:	aeef 0630 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
     bbc:	a629 0600 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\),%d3,%acc1
     bc2:	0610           	03020
     bc4:	a669 0600 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\),%a3,%acc1
     bca:	0610           	03020
     bcc:	a629 0600 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\),%d3,%acc1
     bd2:	0610           	03020
     bd4:	ae69 0600 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\),%sp,%acc1
     bda:	0610           	03020
     bdc:	a629 0620 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\)&,%d3,%acc1
     be2:	0630           	03060
     be4:	a669 0620 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\)&,%a3,%acc1
     bea:	0630           	03060
     bec:	a629 0620 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\)&,%d3,%acc1
     bf2:	0630           	03060
     bf4:	ae69 0620 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\)&,%sp,%acc1
     bfa:	0630           	03060
     bfc:	a61b 0200      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     c00:	a69b 0210      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     c04:	a65b 0200      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
     c08:	a6db 0210      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
     c0c:	a61b 0200      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     c10:	a69b 0210      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     c14:	ae5b 0200      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
     c18:	aedb 0210      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
     c1c:	a61b 0220      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     c20:	a69b 0230      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     c24:	a65b 0220      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
     c28:	a6db 0230      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
     c2c:	a61b 0220      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     c30:	a69b 0230      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     c34:	ae5b 0220      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
     c38:	aedb 0230      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
     c3c:	a61b 0200      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     c40:	a69b 0210      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     c44:	a65b 0200      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
     c48:	a6db 0210      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
     c4c:	a61b 0200      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     c50:	a69b 0210      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     c54:	ae5b 0200      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
     c58:	aedb 0210      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
     c5c:	a61b 0220      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     c60:	a69b 0230      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     c64:	a65b 0220      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
     c68:	a6db 0230      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
     c6c:	a61b 0220      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     c70:	a69b 0230      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     c74:	ae5b 0220      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
     c78:	aedb 0230      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
     c7c:	a62f 0200 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
     c82:	a6af 0210 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
     c88:	a66f 0200 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
     c8e:	a6ef 0210 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
     c94:	a62f 0200 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
     c9a:	a6af 0210 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
     ca0:	ae6f 0200 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
     ca6:	aeef 0210 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
     cac:	a62f 0220 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
     cb2:	a6af 0230 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
     cb8:	a66f 0220 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
     cbe:	a6ef 0230 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
     cc4:	a62f 0220 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
     cca:	a6af 0230 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
     cd0:	ae6f 0220 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
     cd6:	aeef 0230 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
     cdc:	a629 0200 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\),%d3,%acc1
     ce2:	0210           	01020
     ce4:	a669 0200 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\),%a3,%acc1
     cea:	0210           	01020
     cec:	a629 0200 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\),%d3,%acc1
     cf2:	0210           	01020
     cf4:	ae69 0200 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\),%sp,%acc1
     cfa:	0210           	01020
     cfc:	a629 0220 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\)&,%d3,%acc1
     d02:	0230           	01060
     d04:	a669 0220 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\)&,%a3,%acc1
     d0a:	0230           	01060
     d0c:	a629 0220 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\)&,%d3,%acc1
     d12:	0230           	01060
     d14:	ae69 0220 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\)&,%sp,%acc1
     d1a:	0230           	01060
     d1c:	a61b 0600      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     d20:	a69b 0610      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     d24:	a65b 0600      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
     d28:	a6db 0610      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
     d2c:	a61b 0600      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     d30:	a69b 0610      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     d34:	ae5b 0600      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
     d38:	aedb 0610      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
     d3c:	a61b 0620      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     d40:	a69b 0630      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     d44:	a65b 0620      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
     d48:	a6db 0630      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
     d4c:	a61b 0620      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     d50:	a69b 0630      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     d54:	ae5b 0620      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
     d58:	aedb 0630      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
     d5c:	a61b 0600      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     d60:	a69b 0610      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     d64:	a65b 0600      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
     d68:	a6db 0610      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
     d6c:	a61b 0600      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
     d70:	a69b 0610      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
     d74:	ae5b 0600      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
     d78:	aedb 0610      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
     d7c:	a61b 0620      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     d80:	a69b 0630      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     d84:	a65b 0620      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
     d88:	a6db 0630      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
     d8c:	a61b 0620      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
     d90:	a69b 0630      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
     d94:	ae5b 0620      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
     d98:	aedb 0630      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
     d9c:	a62f 0600 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
     da2:	a6af 0610 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
     da8:	a66f 0600 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
     dae:	a6ef 0610 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
     db4:	a62f 0600 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
     dba:	a6af 0610 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
     dc0:	ae6f 0600 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
     dc6:	aeef 0610 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
     dcc:	a62f 0620 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
     dd2:	a6af 0630 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
     dd8:	a66f 0620 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
     dde:	a6ef 0630 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
     de4:	a62f 0620 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
     dea:	a6af 0630 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
     df0:	ae6f 0620 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
     df6:	aeef 0630 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
     dfc:	a629 0600 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\),%d3,%acc1
     e02:	0610           	03020
     e04:	a669 0600 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\),%a3,%acc1
     e0a:	0610           	03020
     e0c:	a629 0600 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\),%d3,%acc1
     e12:	0610           	03020
     e14:	ae69 0600 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\),%sp,%acc1
     e1a:	0610           	03020
     e1c:	a629 0620 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\)&,%d3,%acc1
     e22:	0630           	03060
     e24:	a669 0620 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\)&,%a3,%acc1
     e2a:	0630           	03060
     e2c:	a629 0620 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\)&,%d3,%acc1
     e32:	0630           	03060
     e34:	ae69 0620 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\)&,%sp,%acc1
     e3a:	0630           	03060
     e3c:	ae5b 0080      	macw %a3u,%spu,%a3@\+,%sp,%acc1
     e40:	aedb 0090      	macw %a3u,%spu,%a3@\+,%sp,%acc3
     e44:	ae5b 0080      	macw %a3u,%spu,%a3@\+,%sp,%acc1
     e48:	aedb 0090      	macw %a3u,%spu,%a3@\+,%sp,%acc3
     e4c:	ae5b 0080      	macw %a3u,%spu,%a3@\+,%sp,%acc1
     e50:	aedb 0090      	macw %a3u,%spu,%a3@\+,%sp,%acc3
     e54:	ae5b 0080      	macw %a3u,%spu,%a3@\+,%sp,%acc1
     e58:	aedb 0090      	macw %a3u,%spu,%a3@\+,%sp,%acc3
     e5c:	ae5b 00a0      	macw %a3u,%spu,%a3@\+&,%sp,%acc1
     e60:	aedb 00b0      	macw %a3u,%spu,%a3@\+&,%sp,%acc3
     e64:	ae5b 00a0      	macw %a3u,%spu,%a3@\+&,%sp,%acc1
     e68:	aedb 00b0      	macw %a3u,%spu,%a3@\+&,%sp,%acc3
     e6c:	ae5b 00a0      	macw %a3u,%spu,%a3@\+&,%sp,%acc1
     e70:	aedb 00b0      	macw %a3u,%spu,%a3@\+&,%sp,%acc3
     e74:	ae5b 00a0      	macw %a3u,%spu,%a3@\+&,%sp,%acc1
     e78:	aedb 00b0      	macw %a3u,%spu,%a3@\+&,%sp,%acc3
     e7c:	ae5b 0080      	macw %a3u,%spu,%a3@\+,%sp,%acc1
     e80:	aedb 0090      	macw %a3u,%spu,%a3@\+,%sp,%acc3
     e84:	ae5b 0080      	macw %a3u,%spu,%a3@\+,%sp,%acc1
     e88:	aedb 0090      	macw %a3u,%spu,%a3@\+,%sp,%acc3
     e8c:	ae5b 0080      	macw %a3u,%spu,%a3@\+,%sp,%acc1
     e90:	aedb 0090      	macw %a3u,%spu,%a3@\+,%sp,%acc3
     e94:	ae5b 0080      	macw %a3u,%spu,%a3@\+,%sp,%acc1
     e98:	aedb 0090      	macw %a3u,%spu,%a3@\+,%sp,%acc3
     e9c:	ae5b 00a0      	macw %a3u,%spu,%a3@\+&,%sp,%acc1
     ea0:	aedb 00b0      	macw %a3u,%spu,%a3@\+&,%sp,%acc3
     ea4:	ae5b 00a0      	macw %a3u,%spu,%a3@\+&,%sp,%acc1
     ea8:	aedb 00b0      	macw %a3u,%spu,%a3@\+&,%sp,%acc3
     eac:	ae5b 00a0      	macw %a3u,%spu,%a3@\+&,%sp,%acc1
     eb0:	aedb 00b0      	macw %a3u,%spu,%a3@\+&,%sp,%acc3
     eb4:	ae5b 00a0      	macw %a3u,%spu,%a3@\+&,%sp,%acc1
     eb8:	aedb 00b0      	macw %a3u,%spu,%a3@\+&,%sp,%acc3
     ebc:	ae6f 0080 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc1
     ec2:	aeef 0090 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc3
     ec8:	ae6f 0080 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc1
     ece:	aeef 0090 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc3
     ed4:	ae6f 0080 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc1
     eda:	aeef 0090 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc3
     ee0:	ae6f 0080 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc1
     ee6:	aeef 0090 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc3
     eec:	ae6f 00a0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc1
     ef2:	aeef 00b0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc3
     ef8:	ae6f 00a0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc1
     efe:	aeef 00b0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc3
     f04:	ae6f 00a0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc1
     f0a:	aeef 00b0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc3
     f10:	ae6f 00a0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc1
     f16:	aeef 00b0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc3
     f1c:	ae69 0080 aee9 	macw %a1l,%spu,%a1@\(-20759\),%sp,%acc1
     f22:	0090 ae69 0080 	oril #-1368850304,%d0
     f28:	aee9 0090 ae69 	macw %a1l,%spu,%a1@\(-20887\),%sp,%acc3
     f2e:	0080 aee9 0090 	oril #-1360461680,%d0
     f34:	ae69 0080 aee9 	macw %a1l,%spu,%a1@\(-20759\),%sp,%acc1
     f3a:	0090 ae69 00a0 	oril #-1368850272,%d0
     f40:	aee9 00b0 ae69 	macw %a1l,%spu,%a1@\(-20887\)&,%sp,%acc3
     f46:	00a0 aee9 00b0 	oril #-1360461648,%d0
     f4c:	ae69 00a0 aee9 	macw %a1l,%spu,%a1@\(-20759\)&,%sp,%acc1
     f52:	00b0 ae69 00a0 	oril #-1368850272,%d0
     f58:	aee9 00b0 ae5b 	macw %a1l,%spu,%a1@\(-20901\)&,%sp,%acc3
     f5e:	0280 aedb 0290 	andil #-1361378672,%d0
     f64:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     f68:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     f6c:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     f70:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     f74:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     f78:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     f7c:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     f80:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     f84:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     f88:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     f8c:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     f90:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     f94:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     f98:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     f9c:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     fa0:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     fa4:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     fa8:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     fac:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     fb0:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     fb4:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
     fb8:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
     fbc:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     fc0:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     fc4:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     fc8:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     fcc:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     fd0:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     fd4:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
     fd8:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
     fdc:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
     fe2:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
     fe8:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
     fee:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
     ff4:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
     ffa:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    1000:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    1006:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    100c:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    1012:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    1018:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    101e:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    1024:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    102a:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    1030:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    1036:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    103c:	ae69 0280 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\),%sp,%acc1
    1042:	0290 ae69 0280 	andil #-1368849792,%d0
    1048:	aee9 0290 ae69 	macw %a1l,%spu,>>,%a1@\(-20887\),%sp,%acc3
    104e:	0280 aee9 0290 	andil #-1360461168,%d0
    1054:	ae69 0280 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\),%sp,%acc1
    105a:	0290 ae69 02a0 	andil #-1368849760,%d0
    1060:	aee9 02b0 ae69 	macw %a1l,%spu,>>,%a1@\(-20887\)&,%sp,%acc3
    1066:	02a0 aee9 02b0 	andil #-1360461136,%d0
    106c:	ae69 02a0 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\)&,%sp,%acc1
    1072:	02b0 ae69 02a0 	andil #-1368849760,%d0
    1078:	aee9 02b0 ae5b 	macw %a1l,%spu,>>,%a1@\(-20901\)&,%sp,%acc3
    107e:	0680 aedb 0690 	addil #-1361377648,%d0
    1084:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    1088:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    108c:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    1090:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    1094:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    1098:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    109c:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    10a0:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    10a4:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    10a8:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    10ac:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    10b0:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    10b4:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    10b8:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    10bc:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    10c0:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    10c4:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    10c8:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    10cc:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    10d0:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    10d4:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    10d8:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    10dc:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    10e0:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    10e4:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    10e8:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    10ec:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    10f0:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    10f4:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    10f8:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    10fc:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    1102:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    1108:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    110e:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    1114:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    111a:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    1120:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    1126:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    112c:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    1132:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    1138:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    113e:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    1144:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    114a:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    1150:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    1156:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    115c:	ae69 0680 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\),%sp,%acc1
    1162:	0690 ae69 0680 	addil #-1368848768,%d0
    1168:	aee9 0690 ae69 	macw %a1l,%spu,>>,%a1@\(-20887\),%sp,%acc3
    116e:	0680 aee9 0690 	addil #-1360460144,%d0
    1174:	ae69 0680 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\),%sp,%acc1
    117a:	0690 ae69 06a0 	addil #-1368848736,%d0
    1180:	aee9 06b0 ae69 	macw %a1l,%spu,>>,%a1@\(-20887\)&,%sp,%acc3
    1186:	06a0 aee9 06b0 	addil #-1360460112,%d0
    118c:	ae69 06a0 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\)&,%sp,%acc1
    1192:	06b0 ae69 06a0 	addil #-1368848736,%d0
    1198:	aee9 06b0 ae5b 	macw %a1l,%spu,>>,%a1@\(-20901\)&,%sp,%acc3
    119e:	0280 aedb 0290 	andil #-1361378672,%d0
    11a4:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    11a8:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    11ac:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    11b0:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    11b4:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    11b8:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    11bc:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    11c0:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    11c4:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    11c8:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    11cc:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    11d0:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    11d4:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    11d8:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    11dc:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    11e0:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    11e4:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    11e8:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    11ec:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    11f0:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    11f4:	ae5b 0280      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    11f8:	aedb 0290      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    11fc:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    1200:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    1204:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    1208:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    120c:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    1210:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    1214:	ae5b 02a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    1218:	aedb 02b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    121c:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    1222:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    1228:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    122e:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    1234:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    123a:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    1240:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    1246:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    124c:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    1252:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    1258:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    125e:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    1264:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    126a:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    1270:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    1276:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    127c:	ae69 0280 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\),%sp,%acc1
    1282:	0290 ae69 0280 	andil #-1368849792,%d0
    1288:	aee9 0290 ae69 	macw %a1l,%spu,>>,%a1@\(-20887\),%sp,%acc3
    128e:	0280 aee9 0290 	andil #-1360461168,%d0
    1294:	ae69 0280 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\),%sp,%acc1
    129a:	0290 ae69 02a0 	andil #-1368849760,%d0
    12a0:	aee9 02b0 ae69 	macw %a1l,%spu,>>,%a1@\(-20887\)&,%sp,%acc3
    12a6:	02a0 aee9 02b0 	andil #-1360461136,%d0
    12ac:	ae69 02a0 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\)&,%sp,%acc1
    12b2:	02b0 ae69 02a0 	andil #-1368849760,%d0
    12b8:	aee9 02b0 ae5b 	macw %a1l,%spu,>>,%a1@\(-20901\)&,%sp,%acc3
    12be:	0680 aedb 0690 	addil #-1361377648,%d0
    12c4:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    12c8:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    12cc:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    12d0:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    12d4:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    12d8:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    12dc:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    12e0:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    12e4:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    12e8:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    12ec:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    12f0:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    12f4:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    12f8:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    12fc:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    1300:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    1304:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    1308:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    130c:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    1310:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    1314:	ae5b 0680      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc1
    1318:	aedb 0690      	macw %a3u,%spu,>>,%a3@\+,%sp,%acc3
    131c:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    1320:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    1324:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    1328:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    132c:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    1330:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    1334:	ae5b 06a0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc1
    1338:	aedb 06b0      	macw %a3u,%spu,>>,%a3@\+&,%sp,%acc3
    133c:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    1342:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    1348:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    134e:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    1354:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    135a:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    1360:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    1366:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    136c:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    1372:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    1378:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    137e:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    1384:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    138a:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    1390:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    1396:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    139c:	ae69 0680 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\),%sp,%acc1
    13a2:	0690 ae69 0680 	addil #-1368848768,%d0
    13a8:	aee9 0690 ae69 	macw %a1l,%spu,>>,%a1@\(-20887\),%sp,%acc3
    13ae:	0680 aee9 0690 	addil #-1360460144,%d0
    13b4:	ae69 0680 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\),%sp,%acc1
    13ba:	0690 ae69 06a0 	addil #-1368848736,%d0
    13c0:	aee9 06b0 ae69 	macw %a1l,%spu,>>,%a1@\(-20887\)&,%sp,%acc3
    13c6:	06a0 aee9 06b0 	addil #-1360460112,%d0
    13cc:	ae69 06a0 aee9 	macw %a1l,%spu,>>,%a1@\(-20759\)&,%sp,%acc1
    13d2:	06b0 ae69 06a0 	addil #-1368848736,%d0
    13d8:	aee9 06b0 a21b 	macw %a1l,%spu,>>,%a1@\(-24037\)&,%sp,%acc3
    13de:	0000           	00
    13e0:	a29b 0010      	macw %a3u,%d1l,%a3@\+,%d1,%acc3
    13e4:	a65b 0000      	macw %a3u,%a3l,%a3@\+,%a3,%acc1
    13e8:	a6db 0010      	macw %a3u,%a3l,%a3@\+,%a3,%acc3
    13ec:	a61b 0000      	macw %a3u,%d3l,%a3@\+,%d3,%acc1
    13f0:	a69b 0010      	macw %a3u,%d3l,%a3@\+,%d3,%acc3
    13f4:	ae5b 0000      	macw %a3u,%spl,%a3@\+,%sp,%acc1
    13f8:	aedb 0010      	macw %a3u,%spl,%a3@\+,%sp,%acc3
    13fc:	a21b 0020      	macw %a3u,%d1l,%a3@\+&,%d1,%acc1
    1400:	a29b 0030      	macw %a3u,%d1l,%a3@\+&,%d1,%acc3
    1404:	a65b 0020      	macw %a3u,%a3l,%a3@\+&,%a3,%acc1
    1408:	a6db 0030      	macw %a3u,%a3l,%a3@\+&,%a3,%acc3
    140c:	a61b 0020      	macw %a3u,%d3l,%a3@\+&,%d3,%acc1
    1410:	a69b 0030      	macw %a3u,%d3l,%a3@\+&,%d3,%acc3
    1414:	ae5b 0020      	macw %a3u,%spl,%a3@\+&,%sp,%acc1
    1418:	aedb 0030      	macw %a3u,%spl,%a3@\+&,%sp,%acc3
    141c:	a21b 0000      	macw %a3u,%d1l,%a3@\+,%d1,%acc1
    1420:	a29b 0010      	macw %a3u,%d1l,%a3@\+,%d1,%acc3
    1424:	a65b 0000      	macw %a3u,%a3l,%a3@\+,%a3,%acc1
    1428:	a6db 0010      	macw %a3u,%a3l,%a3@\+,%a3,%acc3
    142c:	a61b 0000      	macw %a3u,%d3l,%a3@\+,%d3,%acc1
    1430:	a69b 0010      	macw %a3u,%d3l,%a3@\+,%d3,%acc3
    1434:	ae5b 0000      	macw %a3u,%spl,%a3@\+,%sp,%acc1
    1438:	aedb 0010      	macw %a3u,%spl,%a3@\+,%sp,%acc3
    143c:	a21b 0020      	macw %a3u,%d1l,%a3@\+&,%d1,%acc1
    1440:	a29b 0030      	macw %a3u,%d1l,%a3@\+&,%d1,%acc3
    1444:	a65b 0020      	macw %a3u,%a3l,%a3@\+&,%a3,%acc1
    1448:	a6db 0030      	macw %a3u,%a3l,%a3@\+&,%a3,%acc3
    144c:	a61b 0020      	macw %a3u,%d3l,%a3@\+&,%d3,%acc1
    1450:	a69b 0030      	macw %a3u,%d3l,%a3@\+&,%d3,%acc3
    1454:	ae5b 0020      	macw %a3u,%spl,%a3@\+&,%sp,%acc1
    1458:	aedb 0030      	macw %a3u,%spl,%a3@\+&,%sp,%acc3
    145c:	a22f 0000 000a 	macw %spl,%d1l,%sp@\(10\),%d1,%acc1
    1462:	a2af 0010 000a 	macw %spl,%d1l,%sp@\(10\),%d1,%acc3
    1468:	a66f 0000 000a 	macw %spl,%a3l,%sp@\(10\),%a3,%acc1
    146e:	a6ef 0010 000a 	macw %spl,%a3l,%sp@\(10\),%a3,%acc3
    1474:	a62f 0000 000a 	macw %spl,%d3l,%sp@\(10\),%d3,%acc1
    147a:	a6af 0010 000a 	macw %spl,%d3l,%sp@\(10\),%d3,%acc3
    1480:	ae6f 0000 000a 	macw %spl,%spl,%sp@\(10\),%sp,%acc1
    1486:	aeef 0010 000a 	macw %spl,%spl,%sp@\(10\),%sp,%acc3
    148c:	a22f 0020 000a 	macw %spl,%d1l,%sp@\(10\)&,%d1,%acc1
    1492:	a2af 0030 000a 	macw %spl,%d1l,%sp@\(10\)&,%d1,%acc3
    1498:	a66f 0020 000a 	macw %spl,%a3l,%sp@\(10\)&,%a3,%acc1
    149e:	a6ef 0030 000a 	macw %spl,%a3l,%sp@\(10\)&,%a3,%acc3
    14a4:	a62f 0020 000a 	macw %spl,%d3l,%sp@\(10\)&,%d3,%acc1
    14aa:	a6af 0030 000a 	macw %spl,%d3l,%sp@\(10\)&,%d3,%acc3
    14b0:	ae6f 0020 000a 	macw %spl,%spl,%sp@\(10\)&,%sp,%acc1
    14b6:	aeef 0030 000a 	macw %spl,%spl,%sp@\(10\)&,%sp,%acc3
    14bc:	a229 0000 a2a9 	macw %a1l,%d1l,%a1@\(-23895\),%d1,%acc1
    14c2:	0010           	020
    14c4:	a669 0000 a6e9 	macw %a1l,%a3l,%a1@\(-22807\),%a3,%acc1
    14ca:	0010           	020
    14cc:	a629 0000 a6a9 	macw %a1l,%d3l,%a1@\(-22871\),%d3,%acc1
    14d2:	0010           	020
    14d4:	ae69 0000 aee9 	macw %a1l,%spl,%a1@\(-20759\),%sp,%acc1
    14da:	0010           	020
    14dc:	a229 0020 a2a9 	macw %a1l,%d1l,%a1@\(-23895\)&,%d1,%acc1
    14e2:	0030           	060
    14e4:	a669 0020 a6e9 	macw %a1l,%a3l,%a1@\(-22807\)&,%a3,%acc1
    14ea:	0030           	060
    14ec:	a629 0020 a6a9 	macw %a1l,%d3l,%a1@\(-22871\)&,%d3,%acc1
    14f2:	0030           	060
    14f4:	ae69 0020 aee9 	macw %a1l,%spl,%a1@\(-20759\)&,%sp,%acc1
    14fa:	0030           	060
    14fc:	a21b 0200      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc1
    1500:	a29b 0210      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc3
    1504:	a65b 0200      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
    1508:	a6db 0210      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
    150c:	a61b 0200      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
    1510:	a69b 0210      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
    1514:	ae5b 0200      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
    1518:	aedb 0210      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
    151c:	a21b 0220      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc1
    1520:	a29b 0230      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc3
    1524:	a65b 0220      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
    1528:	a6db 0230      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
    152c:	a61b 0220      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
    1530:	a69b 0230      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
    1534:	ae5b 0220      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
    1538:	aedb 0230      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
    153c:	a21b 0200      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc1
    1540:	a29b 0210      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc3
    1544:	a65b 0200      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
    1548:	a6db 0210      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
    154c:	a61b 0200      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
    1550:	a69b 0210      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
    1554:	ae5b 0200      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
    1558:	aedb 0210      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
    155c:	a21b 0220      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc1
    1560:	a29b 0230      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc3
    1564:	a65b 0220      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
    1568:	a6db 0230      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
    156c:	a61b 0220      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
    1570:	a69b 0230      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
    1574:	ae5b 0220      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
    1578:	aedb 0230      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
    157c:	a22f 0200 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc1
    1582:	a2af 0210 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc3
    1588:	a66f 0200 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
    158e:	a6ef 0210 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
    1594:	a62f 0200 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    159a:	a6af 0210 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    15a0:	ae6f 0200 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
    15a6:	aeef 0210 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
    15ac:	a22f 0220 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc1
    15b2:	a2af 0230 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc3
    15b8:	a66f 0220 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
    15be:	a6ef 0230 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
    15c4:	a62f 0220 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    15ca:	a6af 0230 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    15d0:	ae6f 0220 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
    15d6:	aeef 0230 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
    15dc:	a229 0200 a2a9 	macw %a1l,%d1l,<<,%a1@\(-23895\),%d1,%acc1
    15e2:	0210           	01020
    15e4:	a669 0200 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\),%a3,%acc1
    15ea:	0210           	01020
    15ec:	a629 0200 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\),%d3,%acc1
    15f2:	0210           	01020
    15f4:	ae69 0200 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\),%sp,%acc1
    15fa:	0210           	01020
    15fc:	a229 0220 a2a9 	macw %a1l,%d1l,<<,%a1@\(-23895\)&,%d1,%acc1
    1602:	0230           	01060
    1604:	a669 0220 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\)&,%a3,%acc1
    160a:	0230           	01060
    160c:	a629 0220 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\)&,%d3,%acc1
    1612:	0230           	01060
    1614:	ae69 0220 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\)&,%sp,%acc1
    161a:	0230           	01060
    161c:	a21b 0600      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc1
    1620:	a29b 0610      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc3
    1624:	a65b 0600      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
    1628:	a6db 0610      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
    162c:	a61b 0600      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
    1630:	a69b 0610      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
    1634:	ae5b 0600      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
    1638:	aedb 0610      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
    163c:	a21b 0620      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc1
    1640:	a29b 0630      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc3
    1644:	a65b 0620      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
    1648:	a6db 0630      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
    164c:	a61b 0620      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
    1650:	a69b 0630      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
    1654:	ae5b 0620      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
    1658:	aedb 0630      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
    165c:	a21b 0600      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc1
    1660:	a29b 0610      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc3
    1664:	a65b 0600      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
    1668:	a6db 0610      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
    166c:	a61b 0600      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
    1670:	a69b 0610      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
    1674:	ae5b 0600      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
    1678:	aedb 0610      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
    167c:	a21b 0620      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc1
    1680:	a29b 0630      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc3
    1684:	a65b 0620      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
    1688:	a6db 0630      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
    168c:	a61b 0620      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
    1690:	a69b 0630      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
    1694:	ae5b 0620      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
    1698:	aedb 0630      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
    169c:	a22f 0600 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc1
    16a2:	a2af 0610 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc3
    16a8:	a66f 0600 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
    16ae:	a6ef 0610 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
    16b4:	a62f 0600 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    16ba:	a6af 0610 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    16c0:	ae6f 0600 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
    16c6:	aeef 0610 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
    16cc:	a22f 0620 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc1
    16d2:	a2af 0630 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc3
    16d8:	a66f 0620 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
    16de:	a6ef 0630 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
    16e4:	a62f 0620 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    16ea:	a6af 0630 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    16f0:	ae6f 0620 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
    16f6:	aeef 0630 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
    16fc:	a229 0600 a2a9 	macw %a1l,%d1l,<<,%a1@\(-23895\),%d1,%acc1
    1702:	0610           	03020
    1704:	a669 0600 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\),%a3,%acc1
    170a:	0610           	03020
    170c:	a629 0600 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\),%d3,%acc1
    1712:	0610           	03020
    1714:	ae69 0600 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\),%sp,%acc1
    171a:	0610           	03020
    171c:	a229 0620 a2a9 	macw %a1l,%d1l,<<,%a1@\(-23895\)&,%d1,%acc1
    1722:	0630           	03060
    1724:	a669 0620 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\)&,%a3,%acc1
    172a:	0630           	03060
    172c:	a629 0620 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\)&,%d3,%acc1
    1732:	0630           	03060
    1734:	ae69 0620 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\)&,%sp,%acc1
    173a:	0630           	03060
    173c:	a21b 0200      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc1
    1740:	a29b 0210      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc3
    1744:	a65b 0200      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
    1748:	a6db 0210      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
    174c:	a61b 0200      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
    1750:	a69b 0210      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
    1754:	ae5b 0200      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
    1758:	aedb 0210      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
    175c:	a21b 0220      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc1
    1760:	a29b 0230      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc3
    1764:	a65b 0220      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
    1768:	a6db 0230      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
    176c:	a61b 0220      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
    1770:	a69b 0230      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
    1774:	ae5b 0220      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
    1778:	aedb 0230      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
    177c:	a21b 0200      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc1
    1780:	a29b 0210      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc3
    1784:	a65b 0200      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
    1788:	a6db 0210      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
    178c:	a61b 0200      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
    1790:	a69b 0210      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
    1794:	ae5b 0200      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
    1798:	aedb 0210      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
    179c:	a21b 0220      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc1
    17a0:	a29b 0230      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc3
    17a4:	a65b 0220      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
    17a8:	a6db 0230      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
    17ac:	a61b 0220      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
    17b0:	a69b 0230      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
    17b4:	ae5b 0220      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
    17b8:	aedb 0230      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
    17bc:	a22f 0200 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc1
    17c2:	a2af 0210 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc3
    17c8:	a66f 0200 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
    17ce:	a6ef 0210 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
    17d4:	a62f 0200 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    17da:	a6af 0210 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    17e0:	ae6f 0200 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
    17e6:	aeef 0210 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
    17ec:	a22f 0220 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc1
    17f2:	a2af 0230 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc3
    17f8:	a66f 0220 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
    17fe:	a6ef 0230 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
    1804:	a62f 0220 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    180a:	a6af 0230 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    1810:	ae6f 0220 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
    1816:	aeef 0230 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
    181c:	a229 0200 a2a9 	macw %a1l,%d1l,<<,%a1@\(-23895\),%d1,%acc1
    1822:	0210           	01020
    1824:	a669 0200 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\),%a3,%acc1
    182a:	0210           	01020
    182c:	a629 0200 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\),%d3,%acc1
    1832:	0210           	01020
    1834:	ae69 0200 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\),%sp,%acc1
    183a:	0210           	01020
    183c:	a229 0220 a2a9 	macw %a1l,%d1l,<<,%a1@\(-23895\)&,%d1,%acc1
    1842:	0230           	01060
    1844:	a669 0220 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\)&,%a3,%acc1
    184a:	0230           	01060
    184c:	a629 0220 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\)&,%d3,%acc1
    1852:	0230           	01060
    1854:	ae69 0220 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\)&,%sp,%acc1
    185a:	0230           	01060
    185c:	a21b 0600      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc1
    1860:	a29b 0610      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc3
    1864:	a65b 0600      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
    1868:	a6db 0610      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
    186c:	a61b 0600      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
    1870:	a69b 0610      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
    1874:	ae5b 0600      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
    1878:	aedb 0610      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
    187c:	a21b 0620      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc1
    1880:	a29b 0630      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc3
    1884:	a65b 0620      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
    1888:	a6db 0630      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
    188c:	a61b 0620      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
    1890:	a69b 0630      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
    1894:	ae5b 0620      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
    1898:	aedb 0630      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
    189c:	a21b 0600      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc1
    18a0:	a29b 0610      	macw %a3u,%d1l,<<,%a3@\+,%d1,%acc3
    18a4:	a65b 0600      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc1
    18a8:	a6db 0610      	macw %a3u,%a3l,>>,%a3@\+,%a3,%acc3
    18ac:	a61b 0600      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc1
    18b0:	a69b 0610      	macw %a3u,%d3l,>>,%a3@\+,%d3,%acc3
    18b4:	ae5b 0600      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc1
    18b8:	aedb 0610      	macw %a3u,%spl,>>,%a3@\+,%sp,%acc3
    18bc:	a21b 0620      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc1
    18c0:	a29b 0630      	macw %a3u,%d1l,<<,%a3@\+&,%d1,%acc3
    18c4:	a65b 0620      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc1
    18c8:	a6db 0630      	macw %a3u,%a3l,>>,%a3@\+&,%a3,%acc3
    18cc:	a61b 0620      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc1
    18d0:	a69b 0630      	macw %a3u,%d3l,>>,%a3@\+&,%d3,%acc3
    18d4:	ae5b 0620      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc1
    18d8:	aedb 0630      	macw %a3u,%spl,>>,%a3@\+&,%sp,%acc3
    18dc:	a22f 0600 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc1
    18e2:	a2af 0610 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc3
    18e8:	a66f 0600 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
    18ee:	a6ef 0610 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
    18f4:	a62f 0600 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    18fa:	a6af 0610 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    1900:	ae6f 0600 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
    1906:	aeef 0610 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
    190c:	a22f 0620 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc1
    1912:	a2af 0630 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc3
    1918:	a66f 0620 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
    191e:	a6ef 0630 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
    1924:	a62f 0620 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    192a:	a6af 0630 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    1930:	ae6f 0620 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
    1936:	aeef 0630 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
    193c:	a229 0600 a2a9 	macw %a1l,%d1l,<<,%a1@\(-23895\),%d1,%acc1
    1942:	0610           	03020
    1944:	a669 0600 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\),%a3,%acc1
    194a:	0610           	03020
    194c:	a629 0600 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\),%d3,%acc1
    1952:	0610           	03020
    1954:	ae69 0600 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\),%sp,%acc1
    195a:	0610           	03020
    195c:	a229 0620 a2a9 	macw %a1l,%d1l,<<,%a1@\(-23895\)&,%d1,%acc1
    1962:	0630           	03060
    1964:	a669 0620 a6e9 	macw %a1l,%a3l,>>,%a1@\(-22807\)&,%a3,%acc1
    196a:	0630           	03060
    196c:	a629 0620 a6a9 	macw %a1l,%d3l,>>,%a1@\(-22871\)&,%d3,%acc1
    1972:	0630           	03060
    1974:	ae69 0620 aee9 	macw %a1l,%spl,>>,%a1@\(-20759\)&,%sp,%acc1
    197a:	0630           	03060
    197c:	a653 00c0      	macw %d3u,%a3u,%a3@,%a3,%acc1
    1980:	a6d3 00d0      	macw %d3u,%a3u,%a3@,%a3,%acc3
    1984:	a653 00c0      	macw %d3u,%a3u,%a3@,%a3,%acc1
    1988:	a6d3 00d0      	macw %d3u,%a3u,%a3@,%a3,%acc3
    198c:	a453 00c0      	macw %d3u,%a2u,%a3@,%a2,%acc1
    1990:	a4d3 00d0      	macw %d3u,%a2u,%a3@,%a2,%acc3
    1994:	ae53 00c0      	macw %d3u,%spu,%a3@,%sp,%acc1
    1998:	aed3 00d0      	macw %d3u,%spu,%a3@,%sp,%acc3
    199c:	a653 00e0      	macw %d3u,%a3u,%a3@&,%a3,%acc1
    19a0:	a6d3 00f0      	macw %d3u,%a3u,%a3@&,%a3,%acc3
    19a4:	a653 00e0      	macw %d3u,%a3u,%a3@&,%a3,%acc1
    19a8:	a6d3 00f0      	macw %d3u,%a3u,%a3@&,%a3,%acc3
    19ac:	a453 00e0      	macw %d3u,%a2u,%a3@&,%a2,%acc1
    19b0:	a4d3 00f0      	macw %d3u,%a2u,%a3@&,%a2,%acc3
    19b4:	ae53 00e0      	macw %d3u,%spu,%a3@&,%sp,%acc1
    19b8:	aed3 00f0      	macw %d3u,%spu,%a3@&,%sp,%acc3
    19bc:	a65a 00c0      	macw %a2u,%a3u,%a2@\+,%a3,%acc1
    19c0:	a6da 00d0      	macw %a2u,%a3u,%a2@\+,%a3,%acc3
    19c4:	a65a 00c0      	macw %a2u,%a3u,%a2@\+,%a3,%acc1
    19c8:	a6da 00d0      	macw %a2u,%a3u,%a2@\+,%a3,%acc3
    19cc:	a45a 00c0      	macw %a2u,%a2u,%a2@\+,%a2,%acc1
    19d0:	a4da 00d0      	macw %a2u,%a2u,%a2@\+,%a2,%acc3
    19d4:	ae5a 00c0      	macw %a2u,%spu,%a2@\+,%sp,%acc1
    19d8:	aeda 00d0      	macw %a2u,%spu,%a2@\+,%sp,%acc3
    19dc:	a65a 00e0      	macw %a2u,%a3u,%a2@\+&,%a3,%acc1
    19e0:	a6da 00f0      	macw %a2u,%a3u,%a2@\+&,%a3,%acc3
    19e4:	a65a 00e0      	macw %a2u,%a3u,%a2@\+&,%a3,%acc1
    19e8:	a6da 00f0      	macw %a2u,%a3u,%a2@\+&,%a3,%acc3
    19ec:	a45a 00e0      	macw %a2u,%a2u,%a2@\+&,%a2,%acc1
    19f0:	a4da 00f0      	macw %a2u,%a2u,%a2@\+&,%a2,%acc3
    19f4:	ae5a 00e0      	macw %a2u,%spu,%a2@\+&,%sp,%acc1
    19f8:	aeda 00f0      	macw %a2u,%spu,%a2@\+&,%sp,%acc3
    19fc:	a66e 00c0 000a 	macw %fpu,%a3u,%fp@\(10\),%a3,%acc1
    1a02:	a6ee 00d0 000a 	macw %fpu,%a3u,%fp@\(10\),%a3,%acc3
    1a08:	a66e 00c0 000a 	macw %fpu,%a3u,%fp@\(10\),%a3,%acc1
    1a0e:	a6ee 00d0 000a 	macw %fpu,%a3u,%fp@\(10\),%a3,%acc3
    1a14:	a46e 00c0 000a 	macw %fpu,%a2u,%fp@\(10\),%a2,%acc1
    1a1a:	a4ee 00d0 000a 	macw %fpu,%a2u,%fp@\(10\),%a2,%acc3
    1a20:	ae6e 00c0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc1
    1a26:	aeee 00d0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc3
    1a2c:	a66e 00e0 000a 	macw %fpu,%a3u,%fp@\(10\)&,%a3,%acc1
    1a32:	a6ee 00f0 000a 	macw %fpu,%a3u,%fp@\(10\)&,%a3,%acc3
    1a38:	a66e 00e0 000a 	macw %fpu,%a3u,%fp@\(10\)&,%a3,%acc1
    1a3e:	a6ee 00f0 000a 	macw %fpu,%a3u,%fp@\(10\)&,%a3,%acc3
    1a44:	a46e 00e0 000a 	macw %fpu,%a2u,%fp@\(10\)&,%a2,%acc1
    1a4a:	a4ee 00f0 000a 	macw %fpu,%a2u,%fp@\(10\)&,%a2,%acc3
    1a50:	ae6e 00e0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc1
    1a56:	aeee 00f0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc3
    1a5c:	a663 00c0      	macw %d3u,%a3u,%a3@-,%a3,%acc1
    1a60:	a6e3 00d0      	macw %d3u,%a3u,%a3@-,%a3,%acc3
    1a64:	a663 00c0      	macw %d3u,%a3u,%a3@-,%a3,%acc1
    1a68:	a6e3 00d0      	macw %d3u,%a3u,%a3@-,%a3,%acc3
    1a6c:	a463 00c0      	macw %d3u,%a2u,%a3@-,%a2,%acc1
    1a70:	a4e3 00d0      	macw %d3u,%a2u,%a3@-,%a2,%acc3
    1a74:	ae63 00c0      	macw %d3u,%spu,%a3@-,%sp,%acc1
    1a78:	aee3 00d0      	macw %d3u,%spu,%a3@-,%sp,%acc3
    1a7c:	a663 00e0      	macw %d3u,%a3u,%a3@-&,%a3,%acc1
    1a80:	a6e3 00f0      	macw %d3u,%a3u,%a3@-&,%a3,%acc3
    1a84:	a663 00e0      	macw %d3u,%a3u,%a3@-&,%a3,%acc1
    1a88:	a6e3 00f0      	macw %d3u,%a3u,%a3@-&,%a3,%acc3
    1a8c:	a463 00e0      	macw %d3u,%a2u,%a3@-&,%a2,%acc1
    1a90:	a4e3 00f0      	macw %d3u,%a2u,%a3@-&,%a2,%acc3
    1a94:	ae63 00e0      	macw %d3u,%spu,%a3@-&,%sp,%acc1
    1a98:	aee3 00f0      	macw %d3u,%spu,%a3@-&,%sp,%acc3
    1a9c:	a653 02c0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc1
    1aa0:	a6d3 02d0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc3
    1aa4:	a653 02c0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc1
    1aa8:	a6d3 02d0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc3
    1aac:	a453 02c0      	macw %d3u,%a2u,,%a3@,%a2,%acc1
    1ab0:	a4d3 02d0      	macw %d3u,%a2u,,%a3@,%a2,%acc3
    1ab4:	ae53 02c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    1ab8:	aed3 02d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    1abc:	a653 02e0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc1
    1ac0:	a6d3 02f0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc3
    1ac4:	a653 02e0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc1
    1ac8:	a6d3 02f0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc3
    1acc:	a453 02e0      	macw %d3u,%a2u,,%a3@&,%a2,%acc1
    1ad0:	a4d3 02f0      	macw %d3u,%a2u,,%a3@&,%a2,%acc3
    1ad4:	ae53 02e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    1ad8:	aed3 02f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    1adc:	a65a 02c0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc1
    1ae0:	a6da 02d0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc3
    1ae4:	a65a 02c0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc1
    1ae8:	a6da 02d0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc3
    1aec:	a45a 02c0      	macw %a2u,%a2u,,%a2@\+,%a2,%acc1
    1af0:	a4da 02d0      	macw %a2u,%a2u,,%a2@\+,%a2,%acc3
    1af4:	ae5a 02c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    1af8:	aeda 02d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    1afc:	a65a 02e0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc1
    1b00:	a6da 02f0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc3
    1b04:	a65a 02e0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc1
    1b08:	a6da 02f0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc3
    1b0c:	a45a 02e0      	macw %a2u,%a2u,,%a2@\+&,%a2,%acc1
    1b10:	a4da 02f0      	macw %a2u,%a2u,,%a2@\+&,%a2,%acc3
    1b14:	ae5a 02e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    1b18:	aeda 02f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    1b1c:	a66e 02c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    1b22:	a6ee 02d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    1b28:	a66e 02c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    1b2e:	a6ee 02d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    1b34:	a46e 02c0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc1
    1b3a:	a4ee 02d0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc3
    1b40:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    1b46:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    1b4c:	a66e 02e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    1b52:	a6ee 02f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    1b58:	a66e 02e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    1b5e:	a6ee 02f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    1b64:	a46e 02e0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc1
    1b6a:	a4ee 02f0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc3
    1b70:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    1b76:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    1b7c:	a663 02c0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc1
    1b80:	a6e3 02d0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc3
    1b84:	a663 02c0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc1
    1b88:	a6e3 02d0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc3
    1b8c:	a463 02c0      	macw %d3u,%a2u,,%a3@-,%a2,%acc1
    1b90:	a4e3 02d0      	macw %d3u,%a2u,,%a3@-,%a2,%acc3
    1b94:	ae63 02c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    1b98:	aee3 02d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    1b9c:	a663 02e0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc1
    1ba0:	a6e3 02f0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc3
    1ba4:	a663 02e0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc1
    1ba8:	a6e3 02f0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc3
    1bac:	a463 02e0      	macw %d3u,%a2u,,%a3@-&,%a2,%acc1
    1bb0:	a4e3 02f0      	macw %d3u,%a2u,,%a3@-&,%a2,%acc3
    1bb4:	ae63 02e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    1bb8:	aee3 02f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    1bbc:	a653 06c0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc1
    1bc0:	a6d3 06d0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc3
    1bc4:	a653 06c0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc1
    1bc8:	a6d3 06d0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc3
    1bcc:	a453 06c0      	macw %d3u,%a2u,,%a3@,%a2,%acc1
    1bd0:	a4d3 06d0      	macw %d3u,%a2u,,%a3@,%a2,%acc3
    1bd4:	ae53 06c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    1bd8:	aed3 06d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    1bdc:	a653 06e0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc1
    1be0:	a6d3 06f0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc3
    1be4:	a653 06e0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc1
    1be8:	a6d3 06f0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc3
    1bec:	a453 06e0      	macw %d3u,%a2u,,%a3@&,%a2,%acc1
    1bf0:	a4d3 06f0      	macw %d3u,%a2u,,%a3@&,%a2,%acc3
    1bf4:	ae53 06e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    1bf8:	aed3 06f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    1bfc:	a65a 06c0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc1
    1c00:	a6da 06d0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc3
    1c04:	a65a 06c0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc1
    1c08:	a6da 06d0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc3
    1c0c:	a45a 06c0      	macw %a2u,%a2u,,%a2@\+,%a2,%acc1
    1c10:	a4da 06d0      	macw %a2u,%a2u,,%a2@\+,%a2,%acc3
    1c14:	ae5a 06c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    1c18:	aeda 06d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    1c1c:	a65a 06e0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc1
    1c20:	a6da 06f0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc3
    1c24:	a65a 06e0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc1
    1c28:	a6da 06f0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc3
    1c2c:	a45a 06e0      	macw %a2u,%a2u,,%a2@\+&,%a2,%acc1
    1c30:	a4da 06f0      	macw %a2u,%a2u,,%a2@\+&,%a2,%acc3
    1c34:	ae5a 06e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    1c38:	aeda 06f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    1c3c:	a66e 06c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    1c42:	a6ee 06d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    1c48:	a66e 06c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    1c4e:	a6ee 06d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    1c54:	a46e 06c0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc1
    1c5a:	a4ee 06d0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc3
    1c60:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    1c66:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    1c6c:	a66e 06e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    1c72:	a6ee 06f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    1c78:	a66e 06e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    1c7e:	a6ee 06f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    1c84:	a46e 06e0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc1
    1c8a:	a4ee 06f0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc3
    1c90:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    1c96:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    1c9c:	a663 06c0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc1
    1ca0:	a6e3 06d0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc3
    1ca4:	a663 06c0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc1
    1ca8:	a6e3 06d0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc3
    1cac:	a463 06c0      	macw %d3u,%a2u,,%a3@-,%a2,%acc1
    1cb0:	a4e3 06d0      	macw %d3u,%a2u,,%a3@-,%a2,%acc3
    1cb4:	ae63 06c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    1cb8:	aee3 06d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    1cbc:	a663 06e0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc1
    1cc0:	a6e3 06f0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc3
    1cc4:	a663 06e0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc1
    1cc8:	a6e3 06f0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc3
    1ccc:	a463 06e0      	macw %d3u,%a2u,,%a3@-&,%a2,%acc1
    1cd0:	a4e3 06f0      	macw %d3u,%a2u,,%a3@-&,%a2,%acc3
    1cd4:	ae63 06e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    1cd8:	aee3 06f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    1cdc:	a653 02c0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc1
    1ce0:	a6d3 02d0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc3
    1ce4:	a653 02c0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc1
    1ce8:	a6d3 02d0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc3
    1cec:	a453 02c0      	macw %d3u,%a2u,,%a3@,%a2,%acc1
    1cf0:	a4d3 02d0      	macw %d3u,%a2u,,%a3@,%a2,%acc3
    1cf4:	ae53 02c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    1cf8:	aed3 02d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    1cfc:	a653 02e0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc1
    1d00:	a6d3 02f0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc3
    1d04:	a653 02e0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc1
    1d08:	a6d3 02f0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc3
    1d0c:	a453 02e0      	macw %d3u,%a2u,,%a3@&,%a2,%acc1
    1d10:	a4d3 02f0      	macw %d3u,%a2u,,%a3@&,%a2,%acc3
    1d14:	ae53 02e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    1d18:	aed3 02f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    1d1c:	a65a 02c0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc1
    1d20:	a6da 02d0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc3
    1d24:	a65a 02c0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc1
    1d28:	a6da 02d0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc3
    1d2c:	a45a 02c0      	macw %a2u,%a2u,,%a2@\+,%a2,%acc1
    1d30:	a4da 02d0      	macw %a2u,%a2u,,%a2@\+,%a2,%acc3
    1d34:	ae5a 02c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    1d38:	aeda 02d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    1d3c:	a65a 02e0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc1
    1d40:	a6da 02f0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc3
    1d44:	a65a 02e0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc1
    1d48:	a6da 02f0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc3
    1d4c:	a45a 02e0      	macw %a2u,%a2u,,%a2@\+&,%a2,%acc1
    1d50:	a4da 02f0      	macw %a2u,%a2u,,%a2@\+&,%a2,%acc3
    1d54:	ae5a 02e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    1d58:	aeda 02f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    1d5c:	a66e 02c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    1d62:	a6ee 02d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    1d68:	a66e 02c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    1d6e:	a6ee 02d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    1d74:	a46e 02c0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc1
    1d7a:	a4ee 02d0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc3
    1d80:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    1d86:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    1d8c:	a66e 02e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    1d92:	a6ee 02f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    1d98:	a66e 02e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    1d9e:	a6ee 02f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    1da4:	a46e 02e0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc1
    1daa:	a4ee 02f0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc3
    1db0:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    1db6:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    1dbc:	a663 02c0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc1
    1dc0:	a6e3 02d0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc3
    1dc4:	a663 02c0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc1
    1dc8:	a6e3 02d0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc3
    1dcc:	a463 02c0      	macw %d3u,%a2u,,%a3@-,%a2,%acc1
    1dd0:	a4e3 02d0      	macw %d3u,%a2u,,%a3@-,%a2,%acc3
    1dd4:	ae63 02c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    1dd8:	aee3 02d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    1ddc:	a663 02e0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc1
    1de0:	a6e3 02f0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc3
    1de4:	a663 02e0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc1
    1de8:	a6e3 02f0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc3
    1dec:	a463 02e0      	macw %d3u,%a2u,,%a3@-&,%a2,%acc1
    1df0:	a4e3 02f0      	macw %d3u,%a2u,,%a3@-&,%a2,%acc3
    1df4:	ae63 02e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    1df8:	aee3 02f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    1dfc:	a653 06c0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc1
    1e00:	a6d3 06d0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc3
    1e04:	a653 06c0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc1
    1e08:	a6d3 06d0      	macw %d3u,%a3u,>>,%a3@,%a3,%acc3
    1e0c:	a453 06c0      	macw %d3u,%a2u,,%a3@,%a2,%acc1
    1e10:	a4d3 06d0      	macw %d3u,%a2u,,%a3@,%a2,%acc3
    1e14:	ae53 06c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    1e18:	aed3 06d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    1e1c:	a653 06e0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc1
    1e20:	a6d3 06f0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc3
    1e24:	a653 06e0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc1
    1e28:	a6d3 06f0      	macw %d3u,%a3u,>>,%a3@&,%a3,%acc3
    1e2c:	a453 06e0      	macw %d3u,%a2u,,%a3@&,%a2,%acc1
    1e30:	a4d3 06f0      	macw %d3u,%a2u,,%a3@&,%a2,%acc3
    1e34:	ae53 06e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    1e38:	aed3 06f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    1e3c:	a65a 06c0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc1
    1e40:	a6da 06d0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc3
    1e44:	a65a 06c0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc1
    1e48:	a6da 06d0      	macw %a2u,%a3u,>>,%a2@\+,%a3,%acc3
    1e4c:	a45a 06c0      	macw %a2u,%a2u,,%a2@\+,%a2,%acc1
    1e50:	a4da 06d0      	macw %a2u,%a2u,,%a2@\+,%a2,%acc3
    1e54:	ae5a 06c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    1e58:	aeda 06d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    1e5c:	a65a 06e0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc1
    1e60:	a6da 06f0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc3
    1e64:	a65a 06e0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc1
    1e68:	a6da 06f0      	macw %a2u,%a3u,>>,%a2@\+&,%a3,%acc3
    1e6c:	a45a 06e0      	macw %a2u,%a2u,,%a2@\+&,%a2,%acc1
    1e70:	a4da 06f0      	macw %a2u,%a2u,,%a2@\+&,%a2,%acc3
    1e74:	ae5a 06e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    1e78:	aeda 06f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    1e7c:	a66e 06c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    1e82:	a6ee 06d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    1e88:	a66e 06c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    1e8e:	a6ee 06d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    1e94:	a46e 06c0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc1
    1e9a:	a4ee 06d0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc3
    1ea0:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    1ea6:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    1eac:	a66e 06e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    1eb2:	a6ee 06f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    1eb8:	a66e 06e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    1ebe:	a6ee 06f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    1ec4:	a46e 06e0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc1
    1eca:	a4ee 06f0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc3
    1ed0:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    1ed6:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    1edc:	a663 06c0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc1
    1ee0:	a6e3 06d0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc3
    1ee4:	a663 06c0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc1
    1ee8:	a6e3 06d0      	macw %d3u,%a3u,>>,%a3@-,%a3,%acc3
    1eec:	a463 06c0      	macw %d3u,%a2u,,%a3@-,%a2,%acc1
    1ef0:	a4e3 06d0      	macw %d3u,%a2u,,%a3@-,%a2,%acc3
    1ef4:	ae63 06c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    1ef8:	aee3 06d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    1efc:	a663 06e0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc1
    1f00:	a6e3 06f0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc3
    1f04:	a663 06e0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc1
    1f08:	a6e3 06f0      	macw %d3u,%a3u,>>,%a3@-&,%a3,%acc3
    1f0c:	a463 06e0      	macw %d3u,%a2u,,%a3@-&,%a2,%acc1
    1f10:	a4e3 06f0      	macw %d3u,%a2u,,%a3@-&,%a2,%acc3
    1f14:	ae63 06e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    1f18:	aee3 06f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    1f1c:	a613 0040      	macw %d3u,%d3l,%a3@,%d3,%acc1
    1f20:	a693 0050      	macw %d3u,%d3l,%a3@,%d3,%acc3
    1f24:	a653 0040      	macw %d3u,%a3l,%a3@,%a3,%acc1
    1f28:	a6d3 0050      	macw %d3u,%a3l,%a3@,%a3,%acc3
    1f2c:	a613 0040      	macw %d3u,%d3l,%a3@,%d3,%acc1
    1f30:	a693 0050      	macw %d3u,%d3l,%a3@,%d3,%acc3
    1f34:	ae53 0040      	macw %d3u,%spl,%a3@,%sp,%acc1
    1f38:	aed3 0050      	macw %d3u,%spl,%a3@,%sp,%acc3
    1f3c:	a613 0060      	macw %d3u,%d3l,%a3@&,%d3,%acc1
    1f40:	a693 0070      	macw %d3u,%d3l,%a3@&,%d3,%acc3
    1f44:	a653 0060      	macw %d3u,%a3l,%a3@&,%a3,%acc1
    1f48:	a6d3 0070      	macw %d3u,%a3l,%a3@&,%a3,%acc3
    1f4c:	a613 0060      	macw %d3u,%d3l,%a3@&,%d3,%acc1
    1f50:	a693 0070      	macw %d3u,%d3l,%a3@&,%d3,%acc3
    1f54:	ae53 0060      	macw %d3u,%spl,%a3@&,%sp,%acc1
    1f58:	aed3 0070      	macw %d3u,%spl,%a3@&,%sp,%acc3
    1f5c:	a61a 0040      	macw %a2u,%d3l,%a2@\+,%d3,%acc1
    1f60:	a69a 0050      	macw %a2u,%d3l,%a2@\+,%d3,%acc3
    1f64:	a65a 0040      	macw %a2u,%a3l,%a2@\+,%a3,%acc1
    1f68:	a6da 0050      	macw %a2u,%a3l,%a2@\+,%a3,%acc3
    1f6c:	a61a 0040      	macw %a2u,%d3l,%a2@\+,%d3,%acc1
    1f70:	a69a 0050      	macw %a2u,%d3l,%a2@\+,%d3,%acc3
    1f74:	ae5a 0040      	macw %a2u,%spl,%a2@\+,%sp,%acc1
    1f78:	aeda 0050      	macw %a2u,%spl,%a2@\+,%sp,%acc3
    1f7c:	a61a 0060      	macw %a2u,%d3l,%a2@\+&,%d3,%acc1
    1f80:	a69a 0070      	macw %a2u,%d3l,%a2@\+&,%d3,%acc3
    1f84:	a65a 0060      	macw %a2u,%a3l,%a2@\+&,%a3,%acc1
    1f88:	a6da 0070      	macw %a2u,%a3l,%a2@\+&,%a3,%acc3
    1f8c:	a61a 0060      	macw %a2u,%d3l,%a2@\+&,%d3,%acc1
    1f90:	a69a 0070      	macw %a2u,%d3l,%a2@\+&,%d3,%acc3
    1f94:	ae5a 0060      	macw %a2u,%spl,%a2@\+&,%sp,%acc1
    1f98:	aeda 0070      	macw %a2u,%spl,%a2@\+&,%sp,%acc3
    1f9c:	a62e 0040 000a 	macw %fpu,%d3l,%fp@\(10\),%d3,%acc1
    1fa2:	a6ae 0050 000a 	macw %fpu,%d3l,%fp@\(10\),%d3,%acc3
    1fa8:	a66e 0040 000a 	macw %fpu,%a3l,%fp@\(10\),%a3,%acc1
    1fae:	a6ee 0050 000a 	macw %fpu,%a3l,%fp@\(10\),%a3,%acc3
    1fb4:	a62e 0040 000a 	macw %fpu,%d3l,%fp@\(10\),%d3,%acc1
    1fba:	a6ae 0050 000a 	macw %fpu,%d3l,%fp@\(10\),%d3,%acc3
    1fc0:	ae6e 0040 000a 	macw %fpu,%spl,%fp@\(10\),%sp,%acc1
    1fc6:	aeee 0050 000a 	macw %fpu,%spl,%fp@\(10\),%sp,%acc3
    1fcc:	a62e 0060 000a 	macw %fpu,%d3l,%fp@\(10\)&,%d3,%acc1
    1fd2:	a6ae 0070 000a 	macw %fpu,%d3l,%fp@\(10\)&,%d3,%acc3
    1fd8:	a66e 0060 000a 	macw %fpu,%a3l,%fp@\(10\)&,%a3,%acc1
    1fde:	a6ee 0070 000a 	macw %fpu,%a3l,%fp@\(10\)&,%a3,%acc3
    1fe4:	a62e 0060 000a 	macw %fpu,%d3l,%fp@\(10\)&,%d3,%acc1
    1fea:	a6ae 0070 000a 	macw %fpu,%d3l,%fp@\(10\)&,%d3,%acc3
    1ff0:	ae6e 0060 000a 	macw %fpu,%spl,%fp@\(10\)&,%sp,%acc1
    1ff6:	aeee 0070 000a 	macw %fpu,%spl,%fp@\(10\)&,%sp,%acc3
    1ffc:	a623 0040      	macw %d3u,%d3l,%a3@-,%d3,%acc1
    2000:	a6a3 0050      	macw %d3u,%d3l,%a3@-,%d3,%acc3
    2004:	a663 0040      	macw %d3u,%a3l,%a3@-,%a3,%acc1
    2008:	a6e3 0050      	macw %d3u,%a3l,%a3@-,%a3,%acc3
    200c:	a623 0040      	macw %d3u,%d3l,%a3@-,%d3,%acc1
    2010:	a6a3 0050      	macw %d3u,%d3l,%a3@-,%d3,%acc3
    2014:	ae63 0040      	macw %d3u,%spl,%a3@-,%sp,%acc1
    2018:	aee3 0050      	macw %d3u,%spl,%a3@-,%sp,%acc3
    201c:	a623 0060      	macw %d3u,%d3l,%a3@-&,%d3,%acc1
    2020:	a6a3 0070      	macw %d3u,%d3l,%a3@-&,%d3,%acc3
    2024:	a663 0060      	macw %d3u,%a3l,%a3@-&,%a3,%acc1
    2028:	a6e3 0070      	macw %d3u,%a3l,%a3@-&,%a3,%acc3
    202c:	a623 0060      	macw %d3u,%d3l,%a3@-&,%d3,%acc1
    2030:	a6a3 0070      	macw %d3u,%d3l,%a3@-&,%d3,%acc3
    2034:	ae63 0060      	macw %d3u,%spl,%a3@-&,%sp,%acc1
    2038:	aee3 0070      	macw %d3u,%spl,%a3@-&,%sp,%acc3
    203c:	a613 0240      	macw %d3u,%d3l,>>,%a3@,%d3,%acc1
    2040:	a693 0250      	macw %d3u,%d3l,>>,%a3@,%d3,%acc3
    2044:	a653 0240      	macw %d3u,%a3l,>>,%a3@,%a3,%acc1
    2048:	a6d3 0250      	macw %d3u,%a3l,>>,%a3@,%a3,%acc3
    204c:	a613 0240      	macw %d3u,%d3l,>>,%a3@,%d3,%acc1
    2050:	a693 0250      	macw %d3u,%d3l,>>,%a3@,%d3,%acc3
    2054:	ae53 0240      	macw %d3u,%spl,>>,%a3@,%sp,%acc1
    2058:	aed3 0250      	macw %d3u,%spl,>>,%a3@,%sp,%acc3
    205c:	a613 0260      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc1
    2060:	a693 0270      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc3
    2064:	a653 0260      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc1
    2068:	a6d3 0270      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc3
    206c:	a613 0260      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc1
    2070:	a693 0270      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc3
    2074:	ae53 0260      	macw %d3u,%spl,>>,%a3@&,%sp,%acc1
    2078:	aed3 0270      	macw %d3u,%spl,>>,%a3@&,%sp,%acc3
    207c:	a61a 0240      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc1
    2080:	a69a 0250      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc3
    2084:	a65a 0240      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc1
    2088:	a6da 0250      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc3
    208c:	a61a 0240      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc1
    2090:	a69a 0250      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc3
    2094:	ae5a 0240      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc1
    2098:	aeda 0250      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc3
    209c:	a61a 0260      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc1
    20a0:	a69a 0270      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc3
    20a4:	a65a 0260      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc1
    20a8:	a6da 0270      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc3
    20ac:	a61a 0260      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc1
    20b0:	a69a 0270      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc3
    20b4:	ae5a 0260      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc1
    20b8:	aeda 0270      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc3
    20bc:	a62e 0240 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    20c2:	a6ae 0250 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    20c8:	a66e 0240 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    20ce:	a6ee 0250 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    20d4:	a62e 0240 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    20da:	a6ae 0250 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    20e0:	ae6e 0240 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    20e6:	aeee 0250 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    20ec:	a62e 0260 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    20f2:	a6ae 0270 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    20f8:	a66e 0260 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    20fe:	a6ee 0270 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    2104:	a62e 0260 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    210a:	a6ae 0270 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    2110:	ae6e 0260 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    2116:	aeee 0270 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    211c:	a623 0240      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc1
    2120:	a6a3 0250      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc3
    2124:	a663 0240      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc1
    2128:	a6e3 0250      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc3
    212c:	a623 0240      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc1
    2130:	a6a3 0250      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc3
    2134:	ae63 0240      	macw %d3u,%spl,>>,%a3@-,%sp,%acc1
    2138:	aee3 0250      	macw %d3u,%spl,>>,%a3@-,%sp,%acc3
    213c:	a623 0260      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc1
    2140:	a6a3 0270      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc3
    2144:	a663 0260      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc1
    2148:	a6e3 0270      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc3
    214c:	a623 0260      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc1
    2150:	a6a3 0270      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc3
    2154:	ae63 0260      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc1
    2158:	aee3 0270      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc3
    215c:	a613 0640      	macw %d3u,%d3l,>>,%a3@,%d3,%acc1
    2160:	a693 0650      	macw %d3u,%d3l,>>,%a3@,%d3,%acc3
    2164:	a653 0640      	macw %d3u,%a3l,>>,%a3@,%a3,%acc1
    2168:	a6d3 0650      	macw %d3u,%a3l,>>,%a3@,%a3,%acc3
    216c:	a613 0640      	macw %d3u,%d3l,>>,%a3@,%d3,%acc1
    2170:	a693 0650      	macw %d3u,%d3l,>>,%a3@,%d3,%acc3
    2174:	ae53 0640      	macw %d3u,%spl,>>,%a3@,%sp,%acc1
    2178:	aed3 0650      	macw %d3u,%spl,>>,%a3@,%sp,%acc3
    217c:	a613 0660      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc1
    2180:	a693 0670      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc3
    2184:	a653 0660      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc1
    2188:	a6d3 0670      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc3
    218c:	a613 0660      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc1
    2190:	a693 0670      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc3
    2194:	ae53 0660      	macw %d3u,%spl,>>,%a3@&,%sp,%acc1
    2198:	aed3 0670      	macw %d3u,%spl,>>,%a3@&,%sp,%acc3
    219c:	a61a 0640      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc1
    21a0:	a69a 0650      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc3
    21a4:	a65a 0640      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc1
    21a8:	a6da 0650      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc3
    21ac:	a61a 0640      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc1
    21b0:	a69a 0650      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc3
    21b4:	ae5a 0640      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc1
    21b8:	aeda 0650      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc3
    21bc:	a61a 0660      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc1
    21c0:	a69a 0670      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc3
    21c4:	a65a 0660      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc1
    21c8:	a6da 0670      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc3
    21cc:	a61a 0660      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc1
    21d0:	a69a 0670      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc3
    21d4:	ae5a 0660      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc1
    21d8:	aeda 0670      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc3
    21dc:	a62e 0640 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    21e2:	a6ae 0650 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    21e8:	a66e 0640 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    21ee:	a6ee 0650 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    21f4:	a62e 0640 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    21fa:	a6ae 0650 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    2200:	ae6e 0640 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    2206:	aeee 0650 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    220c:	a62e 0660 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    2212:	a6ae 0670 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    2218:	a66e 0660 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    221e:	a6ee 0670 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    2224:	a62e 0660 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    222a:	a6ae 0670 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    2230:	ae6e 0660 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    2236:	aeee 0670 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    223c:	a623 0640      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc1
    2240:	a6a3 0650      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc3
    2244:	a663 0640      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc1
    2248:	a6e3 0650      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc3
    224c:	a623 0640      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc1
    2250:	a6a3 0650      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc3
    2254:	ae63 0640      	macw %d3u,%spl,>>,%a3@-,%sp,%acc1
    2258:	aee3 0650      	macw %d3u,%spl,>>,%a3@-,%sp,%acc3
    225c:	a623 0660      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc1
    2260:	a6a3 0670      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc3
    2264:	a663 0660      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc1
    2268:	a6e3 0670      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc3
    226c:	a623 0660      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc1
    2270:	a6a3 0670      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc3
    2274:	ae63 0660      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc1
    2278:	aee3 0670      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc3
    227c:	a613 0240      	macw %d3u,%d3l,>>,%a3@,%d3,%acc1
    2280:	a693 0250      	macw %d3u,%d3l,>>,%a3@,%d3,%acc3
    2284:	a653 0240      	macw %d3u,%a3l,>>,%a3@,%a3,%acc1
    2288:	a6d3 0250      	macw %d3u,%a3l,>>,%a3@,%a3,%acc3
    228c:	a613 0240      	macw %d3u,%d3l,>>,%a3@,%d3,%acc1
    2290:	a693 0250      	macw %d3u,%d3l,>>,%a3@,%d3,%acc3
    2294:	ae53 0240      	macw %d3u,%spl,>>,%a3@,%sp,%acc1
    2298:	aed3 0250      	macw %d3u,%spl,>>,%a3@,%sp,%acc3
    229c:	a613 0260      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc1
    22a0:	a693 0270      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc3
    22a4:	a653 0260      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc1
    22a8:	a6d3 0270      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc3
    22ac:	a613 0260      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc1
    22b0:	a693 0270      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc3
    22b4:	ae53 0260      	macw %d3u,%spl,>>,%a3@&,%sp,%acc1
    22b8:	aed3 0270      	macw %d3u,%spl,>>,%a3@&,%sp,%acc3
    22bc:	a61a 0240      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc1
    22c0:	a69a 0250      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc3
    22c4:	a65a 0240      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc1
    22c8:	a6da 0250      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc3
    22cc:	a61a 0240      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc1
    22d0:	a69a 0250      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc3
    22d4:	ae5a 0240      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc1
    22d8:	aeda 0250      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc3
    22dc:	a61a 0260      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc1
    22e0:	a69a 0270      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc3
    22e4:	a65a 0260      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc1
    22e8:	a6da 0270      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc3
    22ec:	a61a 0260      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc1
    22f0:	a69a 0270      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc3
    22f4:	ae5a 0260      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc1
    22f8:	aeda 0270      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc3
    22fc:	a62e 0240 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    2302:	a6ae 0250 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    2308:	a66e 0240 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    230e:	a6ee 0250 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    2314:	a62e 0240 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    231a:	a6ae 0250 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    2320:	ae6e 0240 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    2326:	aeee 0250 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    232c:	a62e 0260 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    2332:	a6ae 0270 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    2338:	a66e 0260 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    233e:	a6ee 0270 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    2344:	a62e 0260 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    234a:	a6ae 0270 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    2350:	ae6e 0260 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    2356:	aeee 0270 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    235c:	a623 0240      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc1
    2360:	a6a3 0250      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc3
    2364:	a663 0240      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc1
    2368:	a6e3 0250      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc3
    236c:	a623 0240      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc1
    2370:	a6a3 0250      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc3
    2374:	ae63 0240      	macw %d3u,%spl,>>,%a3@-,%sp,%acc1
    2378:	aee3 0250      	macw %d3u,%spl,>>,%a3@-,%sp,%acc3
    237c:	a623 0260      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc1
    2380:	a6a3 0270      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc3
    2384:	a663 0260      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc1
    2388:	a6e3 0270      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc3
    238c:	a623 0260      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc1
    2390:	a6a3 0270      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc3
    2394:	ae63 0260      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc1
    2398:	aee3 0270      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc3
    239c:	a613 0640      	macw %d3u,%d3l,>>,%a3@,%d3,%acc1
    23a0:	a693 0650      	macw %d3u,%d3l,>>,%a3@,%d3,%acc3
    23a4:	a653 0640      	macw %d3u,%a3l,>>,%a3@,%a3,%acc1
    23a8:	a6d3 0650      	macw %d3u,%a3l,>>,%a3@,%a3,%acc3
    23ac:	a613 0640      	macw %d3u,%d3l,>>,%a3@,%d3,%acc1
    23b0:	a693 0650      	macw %d3u,%d3l,>>,%a3@,%d3,%acc3
    23b4:	ae53 0640      	macw %d3u,%spl,>>,%a3@,%sp,%acc1
    23b8:	aed3 0650      	macw %d3u,%spl,>>,%a3@,%sp,%acc3
    23bc:	a613 0660      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc1
    23c0:	a693 0670      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc3
    23c4:	a653 0660      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc1
    23c8:	a6d3 0670      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc3
    23cc:	a613 0660      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc1
    23d0:	a693 0670      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc3
    23d4:	ae53 0660      	macw %d3u,%spl,>>,%a3@&,%sp,%acc1
    23d8:	aed3 0670      	macw %d3u,%spl,>>,%a3@&,%sp,%acc3
    23dc:	a61a 0640      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc1
    23e0:	a69a 0650      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc3
    23e4:	a65a 0640      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc1
    23e8:	a6da 0650      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc3
    23ec:	a61a 0640      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc1
    23f0:	a69a 0650      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc3
    23f4:	ae5a 0640      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc1
    23f8:	aeda 0650      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc3
    23fc:	a61a 0660      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc1
    2400:	a69a 0670      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc3
    2404:	a65a 0660      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc1
    2408:	a6da 0670      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc3
    240c:	a61a 0660      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc1
    2410:	a69a 0670      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc3
    2414:	ae5a 0660      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc1
    2418:	aeda 0670      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc3
    241c:	a62e 0640 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    2422:	a6ae 0650 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    2428:	a66e 0640 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    242e:	a6ee 0650 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    2434:	a62e 0640 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    243a:	a6ae 0650 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    2440:	ae6e 0640 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    2446:	aeee 0650 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    244c:	a62e 0660 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    2452:	a6ae 0670 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    2458:	a66e 0660 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    245e:	a6ee 0670 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    2464:	a62e 0660 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    246a:	a6ae 0670 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    2470:	ae6e 0660 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    2476:	aeee 0670 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    247c:	a623 0640      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc1
    2480:	a6a3 0650      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc3
    2484:	a663 0640      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc1
    2488:	a6e3 0650      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc3
    248c:	a623 0640      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc1
    2490:	a6a3 0650      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc3
    2494:	ae63 0640      	macw %d3u,%spl,>>,%a3@-,%sp,%acc1
    2498:	aee3 0650      	macw %d3u,%spl,>>,%a3@-,%sp,%acc3
    249c:	a623 0660      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc1
    24a0:	a6a3 0670      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc3
    24a4:	a663 0660      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc1
    24a8:	a6e3 0670      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc3
    24ac:	a623 0660      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc1
    24b0:	a6a3 0670      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc3
    24b4:	ae63 0660      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc1
    24b8:	aee3 0670      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc3
    24bc:	ae53 00c0      	macw %d3u,%spu,%a3@,%sp,%acc1
    24c0:	aed3 00d0      	macw %d3u,%spu,%a3@,%sp,%acc3
    24c4:	ae53 00c0      	macw %d3u,%spu,%a3@,%sp,%acc1
    24c8:	aed3 00d0      	macw %d3u,%spu,%a3@,%sp,%acc3
    24cc:	ae53 00c0      	macw %d3u,%spu,%a3@,%sp,%acc1
    24d0:	aed3 00d0      	macw %d3u,%spu,%a3@,%sp,%acc3
    24d4:	ae53 00c0      	macw %d3u,%spu,%a3@,%sp,%acc1
    24d8:	aed3 00d0      	macw %d3u,%spu,%a3@,%sp,%acc3
    24dc:	ae53 00e0      	macw %d3u,%spu,%a3@&,%sp,%acc1
    24e0:	aed3 00f0      	macw %d3u,%spu,%a3@&,%sp,%acc3
    24e4:	ae53 00e0      	macw %d3u,%spu,%a3@&,%sp,%acc1
    24e8:	aed3 00f0      	macw %d3u,%spu,%a3@&,%sp,%acc3
    24ec:	ae53 00e0      	macw %d3u,%spu,%a3@&,%sp,%acc1
    24f0:	aed3 00f0      	macw %d3u,%spu,%a3@&,%sp,%acc3
    24f4:	ae53 00e0      	macw %d3u,%spu,%a3@&,%sp,%acc1
    24f8:	aed3 00f0      	macw %d3u,%spu,%a3@&,%sp,%acc3
    24fc:	ae5a 00c0      	macw %a2u,%spu,%a2@\+,%sp,%acc1
    2500:	aeda 00d0      	macw %a2u,%spu,%a2@\+,%sp,%acc3
    2504:	ae5a 00c0      	macw %a2u,%spu,%a2@\+,%sp,%acc1
    2508:	aeda 00d0      	macw %a2u,%spu,%a2@\+,%sp,%acc3
    250c:	ae5a 00c0      	macw %a2u,%spu,%a2@\+,%sp,%acc1
    2510:	aeda 00d0      	macw %a2u,%spu,%a2@\+,%sp,%acc3
    2514:	ae5a 00c0      	macw %a2u,%spu,%a2@\+,%sp,%acc1
    2518:	aeda 00d0      	macw %a2u,%spu,%a2@\+,%sp,%acc3
    251c:	ae5a 00e0      	macw %a2u,%spu,%a2@\+&,%sp,%acc1
    2520:	aeda 00f0      	macw %a2u,%spu,%a2@\+&,%sp,%acc3
    2524:	ae5a 00e0      	macw %a2u,%spu,%a2@\+&,%sp,%acc1
    2528:	aeda 00f0      	macw %a2u,%spu,%a2@\+&,%sp,%acc3
    252c:	ae5a 00e0      	macw %a2u,%spu,%a2@\+&,%sp,%acc1
    2530:	aeda 00f0      	macw %a2u,%spu,%a2@\+&,%sp,%acc3
    2534:	ae5a 00e0      	macw %a2u,%spu,%a2@\+&,%sp,%acc1
    2538:	aeda 00f0      	macw %a2u,%spu,%a2@\+&,%sp,%acc3
    253c:	ae6e 00c0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc1
    2542:	aeee 00d0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc3
    2548:	ae6e 00c0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc1
    254e:	aeee 00d0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc3
    2554:	ae6e 00c0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc1
    255a:	aeee 00d0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc3
    2560:	ae6e 00c0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc1
    2566:	aeee 00d0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc3
    256c:	ae6e 00e0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc1
    2572:	aeee 00f0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc3
    2578:	ae6e 00e0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc1
    257e:	aeee 00f0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc3
    2584:	ae6e 00e0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc1
    258a:	aeee 00f0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc3
    2590:	ae6e 00e0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc1
    2596:	aeee 00f0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc3
    259c:	ae63 00c0      	macw %d3u,%spu,%a3@-,%sp,%acc1
    25a0:	aee3 00d0      	macw %d3u,%spu,%a3@-,%sp,%acc3
    25a4:	ae63 00c0      	macw %d3u,%spu,%a3@-,%sp,%acc1
    25a8:	aee3 00d0      	macw %d3u,%spu,%a3@-,%sp,%acc3
    25ac:	ae63 00c0      	macw %d3u,%spu,%a3@-,%sp,%acc1
    25b0:	aee3 00d0      	macw %d3u,%spu,%a3@-,%sp,%acc3
    25b4:	ae63 00c0      	macw %d3u,%spu,%a3@-,%sp,%acc1
    25b8:	aee3 00d0      	macw %d3u,%spu,%a3@-,%sp,%acc3
    25bc:	ae63 00e0      	macw %d3u,%spu,%a3@-&,%sp,%acc1
    25c0:	aee3 00f0      	macw %d3u,%spu,%a3@-&,%sp,%acc3
    25c4:	ae63 00e0      	macw %d3u,%spu,%a3@-&,%sp,%acc1
    25c8:	aee3 00f0      	macw %d3u,%spu,%a3@-&,%sp,%acc3
    25cc:	ae63 00e0      	macw %d3u,%spu,%a3@-&,%sp,%acc1
    25d0:	aee3 00f0      	macw %d3u,%spu,%a3@-&,%sp,%acc3
    25d4:	ae63 00e0      	macw %d3u,%spu,%a3@-&,%sp,%acc1
    25d8:	aee3 00f0      	macw %d3u,%spu,%a3@-&,%sp,%acc3
    25dc:	ae53 02c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    25e0:	aed3 02d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    25e4:	ae53 02c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    25e8:	aed3 02d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    25ec:	ae53 02c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    25f0:	aed3 02d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    25f4:	ae53 02c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    25f8:	aed3 02d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    25fc:	ae53 02e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2600:	aed3 02f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    2604:	ae53 02e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2608:	aed3 02f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    260c:	ae53 02e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2610:	aed3 02f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    2614:	ae53 02e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2618:	aed3 02f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    261c:	ae5a 02c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2620:	aeda 02d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    2624:	ae5a 02c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2628:	aeda 02d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    262c:	ae5a 02c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2630:	aeda 02d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    2634:	ae5a 02c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2638:	aeda 02d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    263c:	ae5a 02e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    2640:	aeda 02f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    2644:	ae5a 02e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    2648:	aeda 02f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    264c:	ae5a 02e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    2650:	aeda 02f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    2654:	ae5a 02e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    2658:	aeda 02f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    265c:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    2662:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    2668:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    266e:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    2674:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    267a:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    2680:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    2686:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    268c:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    2692:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    2698:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    269e:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    26a4:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    26aa:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    26b0:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    26b6:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    26bc:	ae63 02c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    26c0:	aee3 02d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    26c4:	ae63 02c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    26c8:	aee3 02d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    26cc:	ae63 02c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    26d0:	aee3 02d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    26d4:	ae63 02c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    26d8:	aee3 02d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    26dc:	ae63 02e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    26e0:	aee3 02f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    26e4:	ae63 02e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    26e8:	aee3 02f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    26ec:	ae63 02e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    26f0:	aee3 02f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    26f4:	ae63 02e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    26f8:	aee3 02f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    26fc:	ae53 06c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    2700:	aed3 06d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    2704:	ae53 06c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    2708:	aed3 06d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    270c:	ae53 06c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    2710:	aed3 06d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    2714:	ae53 06c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    2718:	aed3 06d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    271c:	ae53 06e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2720:	aed3 06f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    2724:	ae53 06e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2728:	aed3 06f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    272c:	ae53 06e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2730:	aed3 06f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    2734:	ae53 06e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2738:	aed3 06f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    273c:	ae5a 06c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2740:	aeda 06d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    2744:	ae5a 06c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2748:	aeda 06d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    274c:	ae5a 06c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2750:	aeda 06d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    2754:	ae5a 06c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2758:	aeda 06d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    275c:	ae5a 06e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    2760:	aeda 06f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    2764:	ae5a 06e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    2768:	aeda 06f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    276c:	ae5a 06e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    2770:	aeda 06f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    2774:	ae5a 06e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    2778:	aeda 06f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    277c:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    2782:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    2788:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    278e:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    2794:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    279a:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    27a0:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    27a6:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    27ac:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    27b2:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    27b8:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    27be:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    27c4:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    27ca:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    27d0:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    27d6:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    27dc:	ae63 06c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    27e0:	aee3 06d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    27e4:	ae63 06c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    27e8:	aee3 06d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    27ec:	ae63 06c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    27f0:	aee3 06d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    27f4:	ae63 06c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    27f8:	aee3 06d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    27fc:	ae63 06e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    2800:	aee3 06f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    2804:	ae63 06e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    2808:	aee3 06f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    280c:	ae63 06e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    2810:	aee3 06f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    2814:	ae63 06e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    2818:	aee3 06f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    281c:	ae53 02c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    2820:	aed3 02d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    2824:	ae53 02c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    2828:	aed3 02d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    282c:	ae53 02c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    2830:	aed3 02d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    2834:	ae53 02c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    2838:	aed3 02d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    283c:	ae53 02e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2840:	aed3 02f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    2844:	ae53 02e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2848:	aed3 02f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    284c:	ae53 02e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2850:	aed3 02f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    2854:	ae53 02e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2858:	aed3 02f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    285c:	ae5a 02c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2860:	aeda 02d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    2864:	ae5a 02c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2868:	aeda 02d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    286c:	ae5a 02c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2870:	aeda 02d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    2874:	ae5a 02c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2878:	aeda 02d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    287c:	ae5a 02e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    2880:	aeda 02f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    2884:	ae5a 02e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    2888:	aeda 02f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    288c:	ae5a 02e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    2890:	aeda 02f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    2894:	ae5a 02e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    2898:	aeda 02f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    289c:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    28a2:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    28a8:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    28ae:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    28b4:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    28ba:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    28c0:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    28c6:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    28cc:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    28d2:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    28d8:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    28de:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    28e4:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    28ea:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    28f0:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    28f6:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    28fc:	ae63 02c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    2900:	aee3 02d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    2904:	ae63 02c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    2908:	aee3 02d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    290c:	ae63 02c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    2910:	aee3 02d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    2914:	ae63 02c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    2918:	aee3 02d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    291c:	ae63 02e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    2920:	aee3 02f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    2924:	ae63 02e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    2928:	aee3 02f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    292c:	ae63 02e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    2930:	aee3 02f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    2934:	ae63 02e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    2938:	aee3 02f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    293c:	ae53 06c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    2940:	aed3 06d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    2944:	ae53 06c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    2948:	aed3 06d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    294c:	ae53 06c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    2950:	aed3 06d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    2954:	ae53 06c0      	macw %d3u,%spu,>>,%a3@,%sp,%acc1
    2958:	aed3 06d0      	macw %d3u,%spu,>>,%a3@,%sp,%acc3
    295c:	ae53 06e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2960:	aed3 06f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    2964:	ae53 06e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2968:	aed3 06f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    296c:	ae53 06e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2970:	aed3 06f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    2974:	ae53 06e0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc1
    2978:	aed3 06f0      	macw %d3u,%spu,>>,%a3@&,%sp,%acc3
    297c:	ae5a 06c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2980:	aeda 06d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    2984:	ae5a 06c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2988:	aeda 06d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    298c:	ae5a 06c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2990:	aeda 06d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    2994:	ae5a 06c0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc1
    2998:	aeda 06d0      	macw %a2u,%spu,>>,%a2@\+,%sp,%acc3
    299c:	ae5a 06e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    29a0:	aeda 06f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    29a4:	ae5a 06e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    29a8:	aeda 06f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    29ac:	ae5a 06e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    29b0:	aeda 06f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    29b4:	ae5a 06e0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc1
    29b8:	aeda 06f0      	macw %a2u,%spu,>>,%a2@\+&,%sp,%acc3
    29bc:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    29c2:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    29c8:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    29ce:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    29d4:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    29da:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    29e0:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    29e6:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    29ec:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    29f2:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    29f8:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    29fe:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    2a04:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    2a0a:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    2a10:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    2a16:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    2a1c:	ae63 06c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    2a20:	aee3 06d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    2a24:	ae63 06c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    2a28:	aee3 06d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    2a2c:	ae63 06c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    2a30:	aee3 06d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    2a34:	ae63 06c0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc1
    2a38:	aee3 06d0      	macw %d3u,%spu,>>,%a3@-,%sp,%acc3
    2a3c:	ae63 06e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    2a40:	aee3 06f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    2a44:	ae63 06e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    2a48:	aee3 06f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    2a4c:	ae63 06e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    2a50:	aee3 06f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    2a54:	ae63 06e0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc1
    2a58:	aee3 06f0      	macw %d3u,%spu,>>,%a3@-&,%sp,%acc3
    2a5c:	a213 0040      	macw %d3u,%d1l,%a3@,%d1,%acc1
    2a60:	a293 0050      	macw %d3u,%d1l,%a3@,%d1,%acc3
    2a64:	a653 0040      	macw %d3u,%a3l,%a3@,%a3,%acc1
    2a68:	a6d3 0050      	macw %d3u,%a3l,%a3@,%a3,%acc3
    2a6c:	a613 0040      	macw %d3u,%d3l,%a3@,%d3,%acc1
    2a70:	a693 0050      	macw %d3u,%d3l,%a3@,%d3,%acc3
    2a74:	ae53 0040      	macw %d3u,%spl,%a3@,%sp,%acc1
    2a78:	aed3 0050      	macw %d3u,%spl,%a3@,%sp,%acc3
    2a7c:	a213 0060      	macw %d3u,%d1l,%a3@&,%d1,%acc1
    2a80:	a293 0070      	macw %d3u,%d1l,%a3@&,%d1,%acc3
    2a84:	a653 0060      	macw %d3u,%a3l,%a3@&,%a3,%acc1
    2a88:	a6d3 0070      	macw %d3u,%a3l,%a3@&,%a3,%acc3
    2a8c:	a613 0060      	macw %d3u,%d3l,%a3@&,%d3,%acc1
    2a90:	a693 0070      	macw %d3u,%d3l,%a3@&,%d3,%acc3
    2a94:	ae53 0060      	macw %d3u,%spl,%a3@&,%sp,%acc1
    2a98:	aed3 0070      	macw %d3u,%spl,%a3@&,%sp,%acc3
    2a9c:	a21a 0040      	macw %a2u,%d1l,%a2@\+,%d1,%acc1
    2aa0:	a29a 0050      	macw %a2u,%d1l,%a2@\+,%d1,%acc3
    2aa4:	a65a 0040      	macw %a2u,%a3l,%a2@\+,%a3,%acc1
    2aa8:	a6da 0050      	macw %a2u,%a3l,%a2@\+,%a3,%acc3
    2aac:	a61a 0040      	macw %a2u,%d3l,%a2@\+,%d3,%acc1
    2ab0:	a69a 0050      	macw %a2u,%d3l,%a2@\+,%d3,%acc3
    2ab4:	ae5a 0040      	macw %a2u,%spl,%a2@\+,%sp,%acc1
    2ab8:	aeda 0050      	macw %a2u,%spl,%a2@\+,%sp,%acc3
    2abc:	a21a 0060      	macw %a2u,%d1l,%a2@\+&,%d1,%acc1
    2ac0:	a29a 0070      	macw %a2u,%d1l,%a2@\+&,%d1,%acc3
    2ac4:	a65a 0060      	macw %a2u,%a3l,%a2@\+&,%a3,%acc1
    2ac8:	a6da 0070      	macw %a2u,%a3l,%a2@\+&,%a3,%acc3
    2acc:	a61a 0060      	macw %a2u,%d3l,%a2@\+&,%d3,%acc1
    2ad0:	a69a 0070      	macw %a2u,%d3l,%a2@\+&,%d3,%acc3
    2ad4:	ae5a 0060      	macw %a2u,%spl,%a2@\+&,%sp,%acc1
    2ad8:	aeda 0070      	macw %a2u,%spl,%a2@\+&,%sp,%acc3
    2adc:	a22e 0040 000a 	macw %fpu,%d1l,%fp@\(10\),%d1,%acc1
    2ae2:	a2ae 0050 000a 	macw %fpu,%d1l,%fp@\(10\),%d1,%acc3
    2ae8:	a66e 0040 000a 	macw %fpu,%a3l,%fp@\(10\),%a3,%acc1
    2aee:	a6ee 0050 000a 	macw %fpu,%a3l,%fp@\(10\),%a3,%acc3
    2af4:	a62e 0040 000a 	macw %fpu,%d3l,%fp@\(10\),%d3,%acc1
    2afa:	a6ae 0050 000a 	macw %fpu,%d3l,%fp@\(10\),%d3,%acc3
    2b00:	ae6e 0040 000a 	macw %fpu,%spl,%fp@\(10\),%sp,%acc1
    2b06:	aeee 0050 000a 	macw %fpu,%spl,%fp@\(10\),%sp,%acc3
    2b0c:	a22e 0060 000a 	macw %fpu,%d1l,%fp@\(10\)&,%d1,%acc1
    2b12:	a2ae 0070 000a 	macw %fpu,%d1l,%fp@\(10\)&,%d1,%acc3
    2b18:	a66e 0060 000a 	macw %fpu,%a3l,%fp@\(10\)&,%a3,%acc1
    2b1e:	a6ee 0070 000a 	macw %fpu,%a3l,%fp@\(10\)&,%a3,%acc3
    2b24:	a62e 0060 000a 	macw %fpu,%d3l,%fp@\(10\)&,%d3,%acc1
    2b2a:	a6ae 0070 000a 	macw %fpu,%d3l,%fp@\(10\)&,%d3,%acc3
    2b30:	ae6e 0060 000a 	macw %fpu,%spl,%fp@\(10\)&,%sp,%acc1
    2b36:	aeee 0070 000a 	macw %fpu,%spl,%fp@\(10\)&,%sp,%acc3
    2b3c:	a223 0040      	macw %d3u,%d1l,%a3@-,%d1,%acc1
    2b40:	a2a3 0050      	macw %d3u,%d1l,%a3@-,%d1,%acc3
    2b44:	a663 0040      	macw %d3u,%a3l,%a3@-,%a3,%acc1
    2b48:	a6e3 0050      	macw %d3u,%a3l,%a3@-,%a3,%acc3
    2b4c:	a623 0040      	macw %d3u,%d3l,%a3@-,%d3,%acc1
    2b50:	a6a3 0050      	macw %d3u,%d3l,%a3@-,%d3,%acc3
    2b54:	ae63 0040      	macw %d3u,%spl,%a3@-,%sp,%acc1
    2b58:	aee3 0050      	macw %d3u,%spl,%a3@-,%sp,%acc3
    2b5c:	a223 0060      	macw %d3u,%d1l,%a3@-&,%d1,%acc1
    2b60:	a2a3 0070      	macw %d3u,%d1l,%a3@-&,%d1,%acc3
    2b64:	a663 0060      	macw %d3u,%a3l,%a3@-&,%a3,%acc1
    2b68:	a6e3 0070      	macw %d3u,%a3l,%a3@-&,%a3,%acc3
    2b6c:	a623 0060      	macw %d3u,%d3l,%a3@-&,%d3,%acc1
    2b70:	a6a3 0070      	macw %d3u,%d3l,%a3@-&,%d3,%acc3
    2b74:	ae63 0060      	macw %d3u,%spl,%a3@-&,%sp,%acc1
    2b78:	aee3 0070      	macw %d3u,%spl,%a3@-&,%sp,%acc3
    2b7c:	a213 0240      	macw %d3u,%d1l,<<,%a3@,%d1,%acc1
    2b80:	a293 0250      	macw %d3u,%d1l,<<,%a3@,%d1,%acc3
    2b84:	a653 0240      	macw %d3u,%a3l,>>,%a3@,%a3,%acc1
    2b88:	a6d3 0250      	macw %d3u,%a3l,>>,%a3@,%a3,%acc3
    2b8c:	a613 0240      	macw %d3u,%d3l,>>,%a3@,%d3,%acc1
    2b90:	a693 0250      	macw %d3u,%d3l,>>,%a3@,%d3,%acc3
    2b94:	ae53 0240      	macw %d3u,%spl,>>,%a3@,%sp,%acc1
    2b98:	aed3 0250      	macw %d3u,%spl,>>,%a3@,%sp,%acc3
    2b9c:	a213 0260      	macw %d3u,%d1l,<<,%a3@&,%d1,%acc1
    2ba0:	a293 0270      	macw %d3u,%d1l,<<,%a3@&,%d1,%acc3
    2ba4:	a653 0260      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc1
    2ba8:	a6d3 0270      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc3
    2bac:	a613 0260      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc1
    2bb0:	a693 0270      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc3
    2bb4:	ae53 0260      	macw %d3u,%spl,>>,%a3@&,%sp,%acc1
    2bb8:	aed3 0270      	macw %d3u,%spl,>>,%a3@&,%sp,%acc3
    2bbc:	a21a 0240      	macw %a2u,%d1l,<<,%a2@\+,%d1,%acc1
    2bc0:	a29a 0250      	macw %a2u,%d1l,<<,%a2@\+,%d1,%acc3
    2bc4:	a65a 0240      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc1
    2bc8:	a6da 0250      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc3
    2bcc:	a61a 0240      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc1
    2bd0:	a69a 0250      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc3
    2bd4:	ae5a 0240      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc1
    2bd8:	aeda 0250      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc3
    2bdc:	a21a 0260      	macw %a2u,%d1l,<<,%a2@\+&,%d1,%acc1
    2be0:	a29a 0270      	macw %a2u,%d1l,<<,%a2@\+&,%d1,%acc3
    2be4:	a65a 0260      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc1
    2be8:	a6da 0270      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc3
    2bec:	a61a 0260      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc1
    2bf0:	a69a 0270      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc3
    2bf4:	ae5a 0260      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc1
    2bf8:	aeda 0270      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc3
    2bfc:	a22e 0240 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc1
    2c02:	a2ae 0250 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc3
    2c08:	a66e 0240 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    2c0e:	a6ee 0250 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    2c14:	a62e 0240 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    2c1a:	a6ae 0250 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    2c20:	ae6e 0240 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    2c26:	aeee 0250 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    2c2c:	a22e 0260 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc1
    2c32:	a2ae 0270 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc3
    2c38:	a66e 0260 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    2c3e:	a6ee 0270 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    2c44:	a62e 0260 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    2c4a:	a6ae 0270 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    2c50:	ae6e 0260 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    2c56:	aeee 0270 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    2c5c:	a223 0240      	macw %d3u,%d1l,<<,%a3@-,%d1,%acc1
    2c60:	a2a3 0250      	macw %d3u,%d1l,<<,%a3@-,%d1,%acc3
    2c64:	a663 0240      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc1
    2c68:	a6e3 0250      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc3
    2c6c:	a623 0240      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc1
    2c70:	a6a3 0250      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc3
    2c74:	ae63 0240      	macw %d3u,%spl,>>,%a3@-,%sp,%acc1
    2c78:	aee3 0250      	macw %d3u,%spl,>>,%a3@-,%sp,%acc3
    2c7c:	a223 0260      	macw %d3u,%d1l,<<,%a3@-&,%d1,%acc1
    2c80:	a2a3 0270      	macw %d3u,%d1l,<<,%a3@-&,%d1,%acc3
    2c84:	a663 0260      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc1
    2c88:	a6e3 0270      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc3
    2c8c:	a623 0260      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc1
    2c90:	a6a3 0270      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc3
    2c94:	ae63 0260      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc1
    2c98:	aee3 0270      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc3
    2c9c:	a213 0640      	macw %d3u,%d1l,<<,%a3@,%d1,%acc1
    2ca0:	a293 0650      	macw %d3u,%d1l,<<,%a3@,%d1,%acc3
    2ca4:	a653 0640      	macw %d3u,%a3l,>>,%a3@,%a3,%acc1
    2ca8:	a6d3 0650      	macw %d3u,%a3l,>>,%a3@,%a3,%acc3
    2cac:	a613 0640      	macw %d3u,%d3l,>>,%a3@,%d3,%acc1
    2cb0:	a693 0650      	macw %d3u,%d3l,>>,%a3@,%d3,%acc3
    2cb4:	ae53 0640      	macw %d3u,%spl,>>,%a3@,%sp,%acc1
    2cb8:	aed3 0650      	macw %d3u,%spl,>>,%a3@,%sp,%acc3
    2cbc:	a213 0660      	macw %d3u,%d1l,<<,%a3@&,%d1,%acc1
    2cc0:	a293 0670      	macw %d3u,%d1l,<<,%a3@&,%d1,%acc3
    2cc4:	a653 0660      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc1
    2cc8:	a6d3 0670      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc3
    2ccc:	a613 0660      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc1
    2cd0:	a693 0670      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc3
    2cd4:	ae53 0660      	macw %d3u,%spl,>>,%a3@&,%sp,%acc1
    2cd8:	aed3 0670      	macw %d3u,%spl,>>,%a3@&,%sp,%acc3
    2cdc:	a21a 0640      	macw %a2u,%d1l,<<,%a2@\+,%d1,%acc1
    2ce0:	a29a 0650      	macw %a2u,%d1l,<<,%a2@\+,%d1,%acc3
    2ce4:	a65a 0640      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc1
    2ce8:	a6da 0650      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc3
    2cec:	a61a 0640      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc1
    2cf0:	a69a 0650      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc3
    2cf4:	ae5a 0640      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc1
    2cf8:	aeda 0650      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc3
    2cfc:	a21a 0660      	macw %a2u,%d1l,<<,%a2@\+&,%d1,%acc1
    2d00:	a29a 0670      	macw %a2u,%d1l,<<,%a2@\+&,%d1,%acc3
    2d04:	a65a 0660      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc1
    2d08:	a6da 0670      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc3
    2d0c:	a61a 0660      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc1
    2d10:	a69a 0670      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc3
    2d14:	ae5a 0660      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc1
    2d18:	aeda 0670      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc3
    2d1c:	a22e 0640 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc1
    2d22:	a2ae 0650 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc3
    2d28:	a66e 0640 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    2d2e:	a6ee 0650 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    2d34:	a62e 0640 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    2d3a:	a6ae 0650 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    2d40:	ae6e 0640 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    2d46:	aeee 0650 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    2d4c:	a22e 0660 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc1
    2d52:	a2ae 0670 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc3
    2d58:	a66e 0660 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    2d5e:	a6ee 0670 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    2d64:	a62e 0660 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    2d6a:	a6ae 0670 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    2d70:	ae6e 0660 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    2d76:	aeee 0670 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    2d7c:	a223 0640      	macw %d3u,%d1l,<<,%a3@-,%d1,%acc1
    2d80:	a2a3 0650      	macw %d3u,%d1l,<<,%a3@-,%d1,%acc3
    2d84:	a663 0640      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc1
    2d88:	a6e3 0650      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc3
    2d8c:	a623 0640      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc1
    2d90:	a6a3 0650      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc3
    2d94:	ae63 0640      	macw %d3u,%spl,>>,%a3@-,%sp,%acc1
    2d98:	aee3 0650      	macw %d3u,%spl,>>,%a3@-,%sp,%acc3
    2d9c:	a223 0660      	macw %d3u,%d1l,<<,%a3@-&,%d1,%acc1
    2da0:	a2a3 0670      	macw %d3u,%d1l,<<,%a3@-&,%d1,%acc3
    2da4:	a663 0660      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc1
    2da8:	a6e3 0670      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc3
    2dac:	a623 0660      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc1
    2db0:	a6a3 0670      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc3
    2db4:	ae63 0660      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc1
    2db8:	aee3 0670      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc3
    2dbc:	a213 0240      	macw %d3u,%d1l,<<,%a3@,%d1,%acc1
    2dc0:	a293 0250      	macw %d3u,%d1l,<<,%a3@,%d1,%acc3
    2dc4:	a653 0240      	macw %d3u,%a3l,>>,%a3@,%a3,%acc1
    2dc8:	a6d3 0250      	macw %d3u,%a3l,>>,%a3@,%a3,%acc3
    2dcc:	a613 0240      	macw %d3u,%d3l,>>,%a3@,%d3,%acc1
    2dd0:	a693 0250      	macw %d3u,%d3l,>>,%a3@,%d3,%acc3
    2dd4:	ae53 0240      	macw %d3u,%spl,>>,%a3@,%sp,%acc1
    2dd8:	aed3 0250      	macw %d3u,%spl,>>,%a3@,%sp,%acc3
    2ddc:	a213 0260      	macw %d3u,%d1l,<<,%a3@&,%d1,%acc1
    2de0:	a293 0270      	macw %d3u,%d1l,<<,%a3@&,%d1,%acc3
    2de4:	a653 0260      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc1
    2de8:	a6d3 0270      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc3
    2dec:	a613 0260      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc1
    2df0:	a693 0270      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc3
    2df4:	ae53 0260      	macw %d3u,%spl,>>,%a3@&,%sp,%acc1
    2df8:	aed3 0270      	macw %d3u,%spl,>>,%a3@&,%sp,%acc3
    2dfc:	a21a 0240      	macw %a2u,%d1l,<<,%a2@\+,%d1,%acc1
    2e00:	a29a 0250      	macw %a2u,%d1l,<<,%a2@\+,%d1,%acc3
    2e04:	a65a 0240      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc1
    2e08:	a6da 0250      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc3
    2e0c:	a61a 0240      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc1
    2e10:	a69a 0250      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc3
    2e14:	ae5a 0240      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc1
    2e18:	aeda 0250      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc3
    2e1c:	a21a 0260      	macw %a2u,%d1l,<<,%a2@\+&,%d1,%acc1
    2e20:	a29a 0270      	macw %a2u,%d1l,<<,%a2@\+&,%d1,%acc3
    2e24:	a65a 0260      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc1
    2e28:	a6da 0270      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc3
    2e2c:	a61a 0260      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc1
    2e30:	a69a 0270      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc3
    2e34:	ae5a 0260      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc1
    2e38:	aeda 0270      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc3
    2e3c:	a22e 0240 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc1
    2e42:	a2ae 0250 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc3
    2e48:	a66e 0240 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    2e4e:	a6ee 0250 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    2e54:	a62e 0240 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    2e5a:	a6ae 0250 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    2e60:	ae6e 0240 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    2e66:	aeee 0250 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    2e6c:	a22e 0260 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc1
    2e72:	a2ae 0270 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc3
    2e78:	a66e 0260 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    2e7e:	a6ee 0270 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    2e84:	a62e 0260 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    2e8a:	a6ae 0270 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    2e90:	ae6e 0260 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    2e96:	aeee 0270 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    2e9c:	a223 0240      	macw %d3u,%d1l,<<,%a3@-,%d1,%acc1
    2ea0:	a2a3 0250      	macw %d3u,%d1l,<<,%a3@-,%d1,%acc3
    2ea4:	a663 0240      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc1
    2ea8:	a6e3 0250      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc3
    2eac:	a623 0240      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc1
    2eb0:	a6a3 0250      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc3
    2eb4:	ae63 0240      	macw %d3u,%spl,>>,%a3@-,%sp,%acc1
    2eb8:	aee3 0250      	macw %d3u,%spl,>>,%a3@-,%sp,%acc3
    2ebc:	a223 0260      	macw %d3u,%d1l,<<,%a3@-&,%d1,%acc1
    2ec0:	a2a3 0270      	macw %d3u,%d1l,<<,%a3@-&,%d1,%acc3
    2ec4:	a663 0260      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc1
    2ec8:	a6e3 0270      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc3
    2ecc:	a623 0260      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc1
    2ed0:	a6a3 0270      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc3
    2ed4:	ae63 0260      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc1
    2ed8:	aee3 0270      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc3
    2edc:	a213 0640      	macw %d3u,%d1l,<<,%a3@,%d1,%acc1
    2ee0:	a293 0650      	macw %d3u,%d1l,<<,%a3@,%d1,%acc3
    2ee4:	a653 0640      	macw %d3u,%a3l,>>,%a3@,%a3,%acc1
    2ee8:	a6d3 0650      	macw %d3u,%a3l,>>,%a3@,%a3,%acc3
    2eec:	a613 0640      	macw %d3u,%d3l,>>,%a3@,%d3,%acc1
    2ef0:	a693 0650      	macw %d3u,%d3l,>>,%a3@,%d3,%acc3
    2ef4:	ae53 0640      	macw %d3u,%spl,>>,%a3@,%sp,%acc1
    2ef8:	aed3 0650      	macw %d3u,%spl,>>,%a3@,%sp,%acc3
    2efc:	a213 0660      	macw %d3u,%d1l,<<,%a3@&,%d1,%acc1
    2f00:	a293 0670      	macw %d3u,%d1l,<<,%a3@&,%d1,%acc3
    2f04:	a653 0660      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc1
    2f08:	a6d3 0670      	macw %d3u,%a3l,>>,%a3@&,%a3,%acc3
    2f0c:	a613 0660      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc1
    2f10:	a693 0670      	macw %d3u,%d3l,>>,%a3@&,%d3,%acc3
    2f14:	ae53 0660      	macw %d3u,%spl,>>,%a3@&,%sp,%acc1
    2f18:	aed3 0670      	macw %d3u,%spl,>>,%a3@&,%sp,%acc3
    2f1c:	a21a 0640      	macw %a2u,%d1l,<<,%a2@\+,%d1,%acc1
    2f20:	a29a 0650      	macw %a2u,%d1l,<<,%a2@\+,%d1,%acc3
    2f24:	a65a 0640      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc1
    2f28:	a6da 0650      	macw %a2u,%a3l,>>,%a2@\+,%a3,%acc3
    2f2c:	a61a 0640      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc1
    2f30:	a69a 0650      	macw %a2u,%d3l,>>,%a2@\+,%d3,%acc3
    2f34:	ae5a 0640      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc1
    2f38:	aeda 0650      	macw %a2u,%spl,>>,%a2@\+,%sp,%acc3
    2f3c:	a21a 0660      	macw %a2u,%d1l,<<,%a2@\+&,%d1,%acc1
    2f40:	a29a 0670      	macw %a2u,%d1l,<<,%a2@\+&,%d1,%acc3
    2f44:	a65a 0660      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc1
    2f48:	a6da 0670      	macw %a2u,%a3l,>>,%a2@\+&,%a3,%acc3
    2f4c:	a61a 0660      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc1
    2f50:	a69a 0670      	macw %a2u,%d3l,>>,%a2@\+&,%d3,%acc3
    2f54:	ae5a 0660      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc1
    2f58:	aeda 0670      	macw %a2u,%spl,>>,%a2@\+&,%sp,%acc3
    2f5c:	a22e 0640 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc1
    2f62:	a2ae 0650 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc3
    2f68:	a66e 0640 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    2f6e:	a6ee 0650 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    2f74:	a62e 0640 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    2f7a:	a6ae 0650 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    2f80:	ae6e 0640 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    2f86:	aeee 0650 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    2f8c:	a22e 0660 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc1
    2f92:	a2ae 0670 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc3
    2f98:	a66e 0660 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    2f9e:	a6ee 0670 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    2fa4:	a62e 0660 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    2faa:	a6ae 0670 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    2fb0:	ae6e 0660 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    2fb6:	aeee 0670 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    2fbc:	a223 0640      	macw %d3u,%d1l,<<,%a3@-,%d1,%acc1
    2fc0:	a2a3 0650      	macw %d3u,%d1l,<<,%a3@-,%d1,%acc3
    2fc4:	a663 0640      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc1
    2fc8:	a6e3 0650      	macw %d3u,%a3l,>>,%a3@-,%a3,%acc3
    2fcc:	a623 0640      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc1
    2fd0:	a6a3 0650      	macw %d3u,%d3l,>>,%a3@-,%d3,%acc3
    2fd4:	ae63 0640      	macw %d3u,%spl,>>,%a3@-,%sp,%acc1
    2fd8:	aee3 0650      	macw %d3u,%spl,>>,%a3@-,%sp,%acc3
    2fdc:	a223 0660      	macw %d3u,%d1l,<<,%a3@-&,%d1,%acc1
    2fe0:	a2a3 0670      	macw %d3u,%d1l,<<,%a3@-&,%d1,%acc3
    2fe4:	a663 0660      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc1
    2fe8:	a6e3 0670      	macw %d3u,%a3l,>>,%a3@-&,%a3,%acc3
    2fec:	a623 0660      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc1
    2ff0:	a6a3 0670      	macw %d3u,%d3l,>>,%a3@-&,%d3,%acc3
    2ff4:	ae63 0660      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc1
    2ff8:	aee3 0670      	macw %d3u,%spl,>>,%a3@-&,%sp,%acc3
    2ffc:	a65f 0080      	macw %spu,%a3u,%sp@\+,%a3,%acc1
    3000:	a6df 0090      	macw %spu,%a3u,%sp@\+,%a3,%acc3
    3004:	a65f 0080      	macw %spu,%a3u,%sp@\+,%a3,%acc1
    3008:	a6df 0090      	macw %spu,%a3u,%sp@\+,%a3,%acc3
    300c:	a45f 0080      	macw %spu,%a2u,%sp@\+,%a2,%acc1
    3010:	a4df 0090      	macw %spu,%a2u,%sp@\+,%a2,%acc3
    3014:	ae5f 0080      	macw %spu,%spu,%sp@\+,%sp,%acc1
    3018:	aedf 0090      	macw %spu,%spu,%sp@\+,%sp,%acc3
    301c:	a65f 00a0      	macw %spu,%a3u,%sp@\+&,%a3,%acc1
    3020:	a6df 00b0      	macw %spu,%a3u,%sp@\+&,%a3,%acc3
    3024:	a65f 00a0      	macw %spu,%a3u,%sp@\+&,%a3,%acc1
    3028:	a6df 00b0      	macw %spu,%a3u,%sp@\+&,%a3,%acc3
    302c:	a45f 00a0      	macw %spu,%a2u,%sp@\+&,%a2,%acc1
    3030:	a4df 00b0      	macw %spu,%a2u,%sp@\+&,%a2,%acc3
    3034:	ae5f 00a0      	macw %spu,%spu,%sp@\+&,%sp,%acc1
    3038:	aedf 00b0      	macw %spu,%spu,%sp@\+&,%sp,%acc3
    303c:	a65f 0080      	macw %spu,%a3u,%sp@\+,%a3,%acc1
    3040:	a6df 0090      	macw %spu,%a3u,%sp@\+,%a3,%acc3
    3044:	a65f 0080      	macw %spu,%a3u,%sp@\+,%a3,%acc1
    3048:	a6df 0090      	macw %spu,%a3u,%sp@\+,%a3,%acc3
    304c:	a45f 0080      	macw %spu,%a2u,%sp@\+,%a2,%acc1
    3050:	a4df 0090      	macw %spu,%a2u,%sp@\+,%a2,%acc3
    3054:	ae5f 0080      	macw %spu,%spu,%sp@\+,%sp,%acc1
    3058:	aedf 0090      	macw %spu,%spu,%sp@\+,%sp,%acc3
    305c:	a65f 00a0      	macw %spu,%a3u,%sp@\+&,%a3,%acc1
    3060:	a6df 00b0      	macw %spu,%a3u,%sp@\+&,%a3,%acc3
    3064:	a65f 00a0      	macw %spu,%a3u,%sp@\+&,%a3,%acc1
    3068:	a6df 00b0      	macw %spu,%a3u,%sp@\+&,%a3,%acc3
    306c:	a45f 00a0      	macw %spu,%a2u,%sp@\+&,%a2,%acc1
    3070:	a4df 00b0      	macw %spu,%a2u,%sp@\+&,%a2,%acc3
    3074:	ae5f 00a0      	macw %spu,%spu,%sp@\+&,%sp,%acc1
    3078:	aedf 00b0      	macw %spu,%spu,%sp@\+&,%sp,%acc3
    307c:	a66f 0080 000a 	macw %spl,%a3u,%sp@\(10\),%a3,%acc1
    3082:	a6ef 0090 000a 	macw %spl,%a3u,%sp@\(10\),%a3,%acc3
    3088:	a66f 0080 000a 	macw %spl,%a3u,%sp@\(10\),%a3,%acc1
    308e:	a6ef 0090 000a 	macw %spl,%a3u,%sp@\(10\),%a3,%acc3
    3094:	a46f 0080 000a 	macw %spl,%a2u,%sp@\(10\),%a2,%acc1
    309a:	a4ef 0090 000a 	macw %spl,%a2u,%sp@\(10\),%a2,%acc3
    30a0:	ae6f 0080 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc1
    30a6:	aeef 0090 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc3
    30ac:	a66f 00a0 000a 	macw %spl,%a3u,%sp@\(10\)&,%a3,%acc1
    30b2:	a6ef 00b0 000a 	macw %spl,%a3u,%sp@\(10\)&,%a3,%acc3
    30b8:	a66f 00a0 000a 	macw %spl,%a3u,%sp@\(10\)&,%a3,%acc1
    30be:	a6ef 00b0 000a 	macw %spl,%a3u,%sp@\(10\)&,%a3,%acc3
    30c4:	a46f 00a0 000a 	macw %spl,%a2u,%sp@\(10\)&,%a2,%acc1
    30ca:	a4ef 00b0 000a 	macw %spl,%a2u,%sp@\(10\)&,%a2,%acc3
    30d0:	ae6f 00a0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc1
    30d6:	aeef 00b0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc3
    30dc:	a66d 0080 a6ed 	macw %a5l,%a3u,%a5@\(-22803\),%a3,%acc1
    30e2:	0090 a66d 0080 	oril #-1502805888,%d0
    30e8:	a6ed 0090 a46d 	macw %a5l,%a3u,%a5@\(-23443\),%a3,%acc3
    30ee:	0080 a4ed 0090 	oril #-1527971696,%d0
    30f4:	ae6d 0080 aeed 	macw %a5l,%spu,%a5@\(-20755\),%sp,%acc1
    30fa:	0090 a66d 00a0 	oril #-1502805856,%d0
    3100:	a6ed 00b0 a66d 	macw %a5l,%a3u,%a5@\(-22931\)&,%a3,%acc3
    3106:	00a0 a6ed 00b0 	oril #-1494417232,%d0
    310c:	a46d 00a0 a4ed 	macw %a5l,%a2u,%a5@\(-23315\)&,%a2,%acc1
    3112:	00b0 ae6d 00a0 	oril #-1368588128,%d0
    3118:	aeed 00b0 a65f 	macw %a5l,%spu,%a5@\(-22945\)&,%sp,%acc3
    311e:	0280 a6df 0290 	andil #-1495334256,%d0
    3124:	a65f 0280      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc1
    3128:	a6df 0290      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc3
    312c:	a45f 0280      	macw %spu,%a2u,,%sp@\+,%a2,%acc1
    3130:	a4df 0290      	macw %spu,%a2u,,%sp@\+,%a2,%acc3
    3134:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3138:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    313c:	a65f 02a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    3140:	a6df 02b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    3144:	a65f 02a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    3148:	a6df 02b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    314c:	a45f 02a0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc1
    3150:	a4df 02b0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc3
    3154:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3158:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    315c:	a65f 0280      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc1
    3160:	a6df 0290      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc3
    3164:	a65f 0280      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc1
    3168:	a6df 0290      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc3
    316c:	a45f 0280      	macw %spu,%a2u,,%sp@\+,%a2,%acc1
    3170:	a4df 0290      	macw %spu,%a2u,,%sp@\+,%a2,%acc3
    3174:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3178:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    317c:	a65f 02a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    3180:	a6df 02b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    3184:	a65f 02a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    3188:	a6df 02b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    318c:	a45f 02a0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc1
    3190:	a4df 02b0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc3
    3194:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3198:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    319c:	a66f 0280 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
    31a2:	a6ef 0290 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
    31a8:	a66f 0280 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
    31ae:	a6ef 0290 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
    31b4:	a46f 0280 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc1
    31ba:	a4ef 0290 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc3
    31c0:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    31c6:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    31cc:	a66f 02a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
    31d2:	a6ef 02b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
    31d8:	a66f 02a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
    31de:	a6ef 02b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
    31e4:	a46f 02a0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc1
    31ea:	a4ef 02b0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc3
    31f0:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    31f6:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    31fc:	a66d 0280 a6ed 	macw %a5l,%a3u,>>,%a5@\(-22803\),%a3,%acc1
    3202:	0290 a66d 0280 	andil #-1502805376,%d0
    3208:	a6ed 0290 a46d 	macw %a5l,%a3u,>>,%a5@\(-23443\),%a3,%acc3
    320e:	0280 a4ed 0290 	andil #-1527971184,%d0
    3214:	ae6d 0280 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\),%sp,%acc1
    321a:	0290 a66d 02a0 	andil #-1502805344,%d0
    3220:	a6ed 02b0 a66d 	macw %a5l,%a3u,>>,%a5@\(-22931\)&,%a3,%acc3
    3226:	02a0 a6ed 02b0 	andil #-1494416720,%d0
    322c:	a46d 02a0 a4ed 	macw %a5l,%a2u,,%a5@\(-23315\)&,%a2,%acc1
    3232:	02b0 ae6d 02a0 	andil #-1368587616,%d0
    3238:	aeed 02b0 a65f 	macw %a5l,%spu,>>,%a5@\(-22945\)&,%sp,%acc3
    323e:	0680 a6df 0690 	addil #-1495333232,%d0
    3244:	a65f 0680      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc1
    3248:	a6df 0690      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc3
    324c:	a45f 0680      	macw %spu,%a2u,,%sp@\+,%a2,%acc1
    3250:	a4df 0690      	macw %spu,%a2u,,%sp@\+,%a2,%acc3
    3254:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3258:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    325c:	a65f 06a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    3260:	a6df 06b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    3264:	a65f 06a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    3268:	a6df 06b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    326c:	a45f 06a0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc1
    3270:	a4df 06b0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc3
    3274:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3278:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    327c:	a65f 0680      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc1
    3280:	a6df 0690      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc3
    3284:	a65f 0680      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc1
    3288:	a6df 0690      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc3
    328c:	a45f 0680      	macw %spu,%a2u,,%sp@\+,%a2,%acc1
    3290:	a4df 0690      	macw %spu,%a2u,,%sp@\+,%a2,%acc3
    3294:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3298:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    329c:	a65f 06a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    32a0:	a6df 06b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    32a4:	a65f 06a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    32a8:	a6df 06b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    32ac:	a45f 06a0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc1
    32b0:	a4df 06b0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc3
    32b4:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    32b8:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    32bc:	a66f 0680 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
    32c2:	a6ef 0690 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
    32c8:	a66f 0680 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
    32ce:	a6ef 0690 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
    32d4:	a46f 0680 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc1
    32da:	a4ef 0690 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc3
    32e0:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    32e6:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    32ec:	a66f 06a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
    32f2:	a6ef 06b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
    32f8:	a66f 06a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
    32fe:	a6ef 06b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
    3304:	a46f 06a0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc1
    330a:	a4ef 06b0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc3
    3310:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3316:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    331c:	a66d 0680 a6ed 	macw %a5l,%a3u,>>,%a5@\(-22803\),%a3,%acc1
    3322:	0690 a66d 0680 	addil #-1502804352,%d0
    3328:	a6ed 0690 a46d 	macw %a5l,%a3u,>>,%a5@\(-23443\),%a3,%acc3
    332e:	0680 a4ed 0690 	addil #-1527970160,%d0
    3334:	ae6d 0680 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\),%sp,%acc1
    333a:	0690 a66d 06a0 	addil #-1502804320,%d0
    3340:	a6ed 06b0 a66d 	macw %a5l,%a3u,>>,%a5@\(-22931\)&,%a3,%acc3
    3346:	06a0 a6ed 06b0 	addil #-1494415696,%d0
    334c:	a46d 06a0 a4ed 	macw %a5l,%a2u,,%a5@\(-23315\)&,%a2,%acc1
    3352:	06b0 ae6d 06a0 	addil #-1368586592,%d0
    3358:	aeed 06b0 a65f 	macw %a5l,%spu,>>,%a5@\(-22945\)&,%sp,%acc3
    335e:	0280 a6df 0290 	andil #-1495334256,%d0
    3364:	a65f 0280      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc1
    3368:	a6df 0290      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc3
    336c:	a45f 0280      	macw %spu,%a2u,,%sp@\+,%a2,%acc1
    3370:	a4df 0290      	macw %spu,%a2u,,%sp@\+,%a2,%acc3
    3374:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3378:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    337c:	a65f 02a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    3380:	a6df 02b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    3384:	a65f 02a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    3388:	a6df 02b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    338c:	a45f 02a0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc1
    3390:	a4df 02b0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc3
    3394:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3398:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    339c:	a65f 0280      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc1
    33a0:	a6df 0290      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc3
    33a4:	a65f 0280      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc1
    33a8:	a6df 0290      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc3
    33ac:	a45f 0280      	macw %spu,%a2u,,%sp@\+,%a2,%acc1
    33b0:	a4df 0290      	macw %spu,%a2u,,%sp@\+,%a2,%acc3
    33b4:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    33b8:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    33bc:	a65f 02a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    33c0:	a6df 02b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    33c4:	a65f 02a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    33c8:	a6df 02b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    33cc:	a45f 02a0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc1
    33d0:	a4df 02b0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc3
    33d4:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    33d8:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    33dc:	a66f 0280 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
    33e2:	a6ef 0290 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
    33e8:	a66f 0280 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
    33ee:	a6ef 0290 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
    33f4:	a46f 0280 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc1
    33fa:	a4ef 0290 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc3
    3400:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3406:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    340c:	a66f 02a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
    3412:	a6ef 02b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
    3418:	a66f 02a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
    341e:	a6ef 02b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
    3424:	a46f 02a0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc1
    342a:	a4ef 02b0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc3
    3430:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3436:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    343c:	a66d 0280 a6ed 	macw %a5l,%a3u,>>,%a5@\(-22803\),%a3,%acc1
    3442:	0290 a66d 0280 	andil #-1502805376,%d0
    3448:	a6ed 0290 a46d 	macw %a5l,%a3u,>>,%a5@\(-23443\),%a3,%acc3
    344e:	0280 a4ed 0290 	andil #-1527971184,%d0
    3454:	ae6d 0280 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\),%sp,%acc1
    345a:	0290 a66d 02a0 	andil #-1502805344,%d0
    3460:	a6ed 02b0 a66d 	macw %a5l,%a3u,>>,%a5@\(-22931\)&,%a3,%acc3
    3466:	02a0 a6ed 02b0 	andil #-1494416720,%d0
    346c:	a46d 02a0 a4ed 	macw %a5l,%a2u,,%a5@\(-23315\)&,%a2,%acc1
    3472:	02b0 ae6d 02a0 	andil #-1368587616,%d0
    3478:	aeed 02b0 a65f 	macw %a5l,%spu,>>,%a5@\(-22945\)&,%sp,%acc3
    347e:	0680 a6df 0690 	addil #-1495333232,%d0
    3484:	a65f 0680      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc1
    3488:	a6df 0690      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc3
    348c:	a45f 0680      	macw %spu,%a2u,,%sp@\+,%a2,%acc1
    3490:	a4df 0690      	macw %spu,%a2u,,%sp@\+,%a2,%acc3
    3494:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3498:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    349c:	a65f 06a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    34a0:	a6df 06b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    34a4:	a65f 06a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    34a8:	a6df 06b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    34ac:	a45f 06a0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc1
    34b0:	a4df 06b0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc3
    34b4:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    34b8:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    34bc:	a65f 0680      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc1
    34c0:	a6df 0690      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc3
    34c4:	a65f 0680      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc1
    34c8:	a6df 0690      	macw %spu,%a3u,>>,%sp@\+,%a3,%acc3
    34cc:	a45f 0680      	macw %spu,%a2u,,%sp@\+,%a2,%acc1
    34d0:	a4df 0690      	macw %spu,%a2u,,%sp@\+,%a2,%acc3
    34d4:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    34d8:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    34dc:	a65f 06a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    34e0:	a6df 06b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    34e4:	a65f 06a0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc1
    34e8:	a6df 06b0      	macw %spu,%a3u,>>,%sp@\+&,%a3,%acc3
    34ec:	a45f 06a0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc1
    34f0:	a4df 06b0      	macw %spu,%a2u,,%sp@\+&,%a2,%acc3
    34f4:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    34f8:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    34fc:	a66f 0680 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
    3502:	a6ef 0690 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
    3508:	a66f 0680 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc1
    350e:	a6ef 0690 000a 	macw %spl,%a3u,>>,%sp@\(10\),%a3,%acc3
    3514:	a46f 0680 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc1
    351a:	a4ef 0690 000a 	macw %spl,%a2u,,%sp@\(10\),%a2,%acc3
    3520:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3526:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    352c:	a66f 06a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
    3532:	a6ef 06b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
    3538:	a66f 06a0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc1
    353e:	a6ef 06b0 000a 	macw %spl,%a3u,>>,%sp@\(10\)&,%a3,%acc3
    3544:	a46f 06a0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc1
    354a:	a4ef 06b0 000a 	macw %spl,%a2u,,%sp@\(10\)&,%a2,%acc3
    3550:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3556:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    355c:	a66d 0680 a6ed 	macw %a5l,%a3u,>>,%a5@\(-22803\),%a3,%acc1
    3562:	0690 a66d 0680 	addil #-1502804352,%d0
    3568:	a6ed 0690 a46d 	macw %a5l,%a3u,>>,%a5@\(-23443\),%a3,%acc3
    356e:	0680 a4ed 0690 	addil #-1527970160,%d0
    3574:	ae6d 0680 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\),%sp,%acc1
    357a:	0690 a66d 06a0 	addil #-1502804320,%d0
    3580:	a6ed 06b0 a66d 	macw %a5l,%a3u,>>,%a5@\(-22931\)&,%a3,%acc3
    3586:	06a0 a6ed 06b0 	addil #-1494415696,%d0
    358c:	a46d 06a0 a4ed 	macw %a5l,%a2u,,%a5@\(-23315\)&,%a2,%acc1
    3592:	06b0 ae6d 06a0 	addil #-1368586592,%d0
    3598:	aeed 06b0 a61f 	macw %a5l,%spu,>>,%a5@\(-23009\)&,%sp,%acc3
    359e:	0000           	00
    35a0:	a69f 0010      	macw %spu,%d3l,%sp@\+,%d3,%acc3
    35a4:	a65f 0000      	macw %spu,%a3l,%sp@\+,%a3,%acc1
    35a8:	a6df 0010      	macw %spu,%a3l,%sp@\+,%a3,%acc3
    35ac:	a61f 0000      	macw %spu,%d3l,%sp@\+,%d3,%acc1
    35b0:	a69f 0010      	macw %spu,%d3l,%sp@\+,%d3,%acc3
    35b4:	ae5f 0000      	macw %spu,%spl,%sp@\+,%sp,%acc1
    35b8:	aedf 0010      	macw %spu,%spl,%sp@\+,%sp,%acc3
    35bc:	a61f 0020      	macw %spu,%d3l,%sp@\+&,%d3,%acc1
    35c0:	a69f 0030      	macw %spu,%d3l,%sp@\+&,%d3,%acc3
    35c4:	a65f 0020      	macw %spu,%a3l,%sp@\+&,%a3,%acc1
    35c8:	a6df 0030      	macw %spu,%a3l,%sp@\+&,%a3,%acc3
    35cc:	a61f 0020      	macw %spu,%d3l,%sp@\+&,%d3,%acc1
    35d0:	a69f 0030      	macw %spu,%d3l,%sp@\+&,%d3,%acc3
    35d4:	ae5f 0020      	macw %spu,%spl,%sp@\+&,%sp,%acc1
    35d8:	aedf 0030      	macw %spu,%spl,%sp@\+&,%sp,%acc3
    35dc:	a61f 0000      	macw %spu,%d3l,%sp@\+,%d3,%acc1
    35e0:	a69f 0010      	macw %spu,%d3l,%sp@\+,%d3,%acc3
    35e4:	a65f 0000      	macw %spu,%a3l,%sp@\+,%a3,%acc1
    35e8:	a6df 0010      	macw %spu,%a3l,%sp@\+,%a3,%acc3
    35ec:	a61f 0000      	macw %spu,%d3l,%sp@\+,%d3,%acc1
    35f0:	a69f 0010      	macw %spu,%d3l,%sp@\+,%d3,%acc3
    35f4:	ae5f 0000      	macw %spu,%spl,%sp@\+,%sp,%acc1
    35f8:	aedf 0010      	macw %spu,%spl,%sp@\+,%sp,%acc3
    35fc:	a61f 0020      	macw %spu,%d3l,%sp@\+&,%d3,%acc1
    3600:	a69f 0030      	macw %spu,%d3l,%sp@\+&,%d3,%acc3
    3604:	a65f 0020      	macw %spu,%a3l,%sp@\+&,%a3,%acc1
    3608:	a6df 0030      	macw %spu,%a3l,%sp@\+&,%a3,%acc3
    360c:	a61f 0020      	macw %spu,%d3l,%sp@\+&,%d3,%acc1
    3610:	a69f 0030      	macw %spu,%d3l,%sp@\+&,%d3,%acc3
    3614:	ae5f 0020      	macw %spu,%spl,%sp@\+&,%sp,%acc1
    3618:	aedf 0030      	macw %spu,%spl,%sp@\+&,%sp,%acc3
    361c:	a62f 0000 000a 	macw %spl,%d3l,%sp@\(10\),%d3,%acc1
    3622:	a6af 0010 000a 	macw %spl,%d3l,%sp@\(10\),%d3,%acc3
    3628:	a66f 0000 000a 	macw %spl,%a3l,%sp@\(10\),%a3,%acc1
    362e:	a6ef 0010 000a 	macw %spl,%a3l,%sp@\(10\),%a3,%acc3
    3634:	a62f 0000 000a 	macw %spl,%d3l,%sp@\(10\),%d3,%acc1
    363a:	a6af 0010 000a 	macw %spl,%d3l,%sp@\(10\),%d3,%acc3
    3640:	ae6f 0000 000a 	macw %spl,%spl,%sp@\(10\),%sp,%acc1
    3646:	aeef 0010 000a 	macw %spl,%spl,%sp@\(10\),%sp,%acc3
    364c:	a62f 0020 000a 	macw %spl,%d3l,%sp@\(10\)&,%d3,%acc1
    3652:	a6af 0030 000a 	macw %spl,%d3l,%sp@\(10\)&,%d3,%acc3
    3658:	a66f 0020 000a 	macw %spl,%a3l,%sp@\(10\)&,%a3,%acc1
    365e:	a6ef 0030 000a 	macw %spl,%a3l,%sp@\(10\)&,%a3,%acc3
    3664:	a62f 0020 000a 	macw %spl,%d3l,%sp@\(10\)&,%d3,%acc1
    366a:	a6af 0030 000a 	macw %spl,%d3l,%sp@\(10\)&,%d3,%acc3
    3670:	ae6f 0020 000a 	macw %spl,%spl,%sp@\(10\)&,%sp,%acc1
    3676:	aeef 0030 000a 	macw %spl,%spl,%sp@\(10\)&,%sp,%acc3
    367c:	a62d 0000 a6ad 	macw %a5l,%d3l,%a5@\(-22867\),%d3,%acc1
    3682:	0010           	020
    3684:	a66d 0000 a6ed 	macw %a5l,%a3l,%a5@\(-22803\),%a3,%acc1
    368a:	0010           	020
    368c:	a62d 0000 a6ad 	macw %a5l,%d3l,%a5@\(-22867\),%d3,%acc1
    3692:	0010           	020
    3694:	ae6d 0000 aeed 	macw %a5l,%spl,%a5@\(-20755\),%sp,%acc1
    369a:	0010           	020
    369c:	a62d 0020 a6ad 	macw %a5l,%d3l,%a5@\(-22867\)&,%d3,%acc1
    36a2:	0030           	060
    36a4:	a66d 0020 a6ed 	macw %a5l,%a3l,%a5@\(-22803\)&,%a3,%acc1
    36aa:	0030           	060
    36ac:	a62d 0020 a6ad 	macw %a5l,%d3l,%a5@\(-22867\)&,%d3,%acc1
    36b2:	0030           	060
    36b4:	ae6d 0020 aeed 	macw %a5l,%spl,%a5@\(-20755\)&,%sp,%acc1
    36ba:	0030           	060
    36bc:	a61f 0200      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    36c0:	a69f 0210      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    36c4:	a65f 0200      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    36c8:	a6df 0210      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    36cc:	a61f 0200      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    36d0:	a69f 0210      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    36d4:	ae5f 0200      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    36d8:	aedf 0210      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    36dc:	a61f 0220      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    36e0:	a69f 0230      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    36e4:	a65f 0220      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    36e8:	a6df 0230      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    36ec:	a61f 0220      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    36f0:	a69f 0230      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    36f4:	ae5f 0220      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    36f8:	aedf 0230      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    36fc:	a61f 0200      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    3700:	a69f 0210      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    3704:	a65f 0200      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    3708:	a6df 0210      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    370c:	a61f 0200      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    3710:	a69f 0210      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    3714:	ae5f 0200      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    3718:	aedf 0210      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    371c:	a61f 0220      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3720:	a69f 0230      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3724:	a65f 0220      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    3728:	a6df 0230      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    372c:	a61f 0220      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3730:	a69f 0230      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3734:	ae5f 0220      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    3738:	aedf 0230      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    373c:	a62f 0200 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    3742:	a6af 0210 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    3748:	a66f 0200 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
    374e:	a6ef 0210 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
    3754:	a62f 0200 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    375a:	a6af 0210 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    3760:	ae6f 0200 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
    3766:	aeef 0210 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
    376c:	a62f 0220 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    3772:	a6af 0230 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    3778:	a66f 0220 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
    377e:	a6ef 0230 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
    3784:	a62f 0220 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    378a:	a6af 0230 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    3790:	ae6f 0220 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
    3796:	aeef 0230 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
    379c:	a62d 0200 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\),%d3,%acc1
    37a2:	0210           	01020
    37a4:	a66d 0200 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\),%a3,%acc1
    37aa:	0210           	01020
    37ac:	a62d 0200 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\),%d3,%acc1
    37b2:	0210           	01020
    37b4:	ae6d 0200 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\),%sp,%acc1
    37ba:	0210           	01020
    37bc:	a62d 0220 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\)&,%d3,%acc1
    37c2:	0230           	01060
    37c4:	a66d 0220 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\)&,%a3,%acc1
    37ca:	0230           	01060
    37cc:	a62d 0220 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\)&,%d3,%acc1
    37d2:	0230           	01060
    37d4:	ae6d 0220 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\)&,%sp,%acc1
    37da:	0230           	01060
    37dc:	a61f 0600      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    37e0:	a69f 0610      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    37e4:	a65f 0600      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    37e8:	a6df 0610      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    37ec:	a61f 0600      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    37f0:	a69f 0610      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    37f4:	ae5f 0600      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    37f8:	aedf 0610      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    37fc:	a61f 0620      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3800:	a69f 0630      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3804:	a65f 0620      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    3808:	a6df 0630      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    380c:	a61f 0620      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3810:	a69f 0630      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3814:	ae5f 0620      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    3818:	aedf 0630      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    381c:	a61f 0600      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    3820:	a69f 0610      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    3824:	a65f 0600      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    3828:	a6df 0610      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    382c:	a61f 0600      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    3830:	a69f 0610      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    3834:	ae5f 0600      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    3838:	aedf 0610      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    383c:	a61f 0620      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3840:	a69f 0630      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3844:	a65f 0620      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    3848:	a6df 0630      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    384c:	a61f 0620      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3850:	a69f 0630      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3854:	ae5f 0620      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    3858:	aedf 0630      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    385c:	a62f 0600 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    3862:	a6af 0610 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    3868:	a66f 0600 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
    386e:	a6ef 0610 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
    3874:	a62f 0600 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    387a:	a6af 0610 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    3880:	ae6f 0600 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
    3886:	aeef 0610 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
    388c:	a62f 0620 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    3892:	a6af 0630 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    3898:	a66f 0620 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
    389e:	a6ef 0630 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
    38a4:	a62f 0620 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    38aa:	a6af 0630 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    38b0:	ae6f 0620 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
    38b6:	aeef 0630 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
    38bc:	a62d 0600 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\),%d3,%acc1
    38c2:	0610           	03020
    38c4:	a66d 0600 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\),%a3,%acc1
    38ca:	0610           	03020
    38cc:	a62d 0600 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\),%d3,%acc1
    38d2:	0610           	03020
    38d4:	ae6d 0600 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\),%sp,%acc1
    38da:	0610           	03020
    38dc:	a62d 0620 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\)&,%d3,%acc1
    38e2:	0630           	03060
    38e4:	a66d 0620 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\)&,%a3,%acc1
    38ea:	0630           	03060
    38ec:	a62d 0620 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\)&,%d3,%acc1
    38f2:	0630           	03060
    38f4:	ae6d 0620 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\)&,%sp,%acc1
    38fa:	0630           	03060
    38fc:	a61f 0200      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    3900:	a69f 0210      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    3904:	a65f 0200      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    3908:	a6df 0210      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    390c:	a61f 0200      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    3910:	a69f 0210      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    3914:	ae5f 0200      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    3918:	aedf 0210      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    391c:	a61f 0220      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3920:	a69f 0230      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3924:	a65f 0220      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    3928:	a6df 0230      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    392c:	a61f 0220      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3930:	a69f 0230      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3934:	ae5f 0220      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    3938:	aedf 0230      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    393c:	a61f 0200      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    3940:	a69f 0210      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    3944:	a65f 0200      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    3948:	a6df 0210      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    394c:	a61f 0200      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    3950:	a69f 0210      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    3954:	ae5f 0200      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    3958:	aedf 0210      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    395c:	a61f 0220      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3960:	a69f 0230      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3964:	a65f 0220      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    3968:	a6df 0230      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    396c:	a61f 0220      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3970:	a69f 0230      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3974:	ae5f 0220      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    3978:	aedf 0230      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    397c:	a62f 0200 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    3982:	a6af 0210 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    3988:	a66f 0200 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
    398e:	a6ef 0210 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
    3994:	a62f 0200 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    399a:	a6af 0210 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    39a0:	ae6f 0200 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
    39a6:	aeef 0210 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
    39ac:	a62f 0220 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    39b2:	a6af 0230 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    39b8:	a66f 0220 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
    39be:	a6ef 0230 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
    39c4:	a62f 0220 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    39ca:	a6af 0230 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    39d0:	ae6f 0220 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
    39d6:	aeef 0230 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
    39dc:	a62d 0200 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\),%d3,%acc1
    39e2:	0210           	01020
    39e4:	a66d 0200 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\),%a3,%acc1
    39ea:	0210           	01020
    39ec:	a62d 0200 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\),%d3,%acc1
    39f2:	0210           	01020
    39f4:	ae6d 0200 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\),%sp,%acc1
    39fa:	0210           	01020
    39fc:	a62d 0220 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\)&,%d3,%acc1
    3a02:	0230           	01060
    3a04:	a66d 0220 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\)&,%a3,%acc1
    3a0a:	0230           	01060
    3a0c:	a62d 0220 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\)&,%d3,%acc1
    3a12:	0230           	01060
    3a14:	ae6d 0220 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\)&,%sp,%acc1
    3a1a:	0230           	01060
    3a1c:	a61f 0600      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    3a20:	a69f 0610      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    3a24:	a65f 0600      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    3a28:	a6df 0610      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    3a2c:	a61f 0600      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    3a30:	a69f 0610      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    3a34:	ae5f 0600      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    3a38:	aedf 0610      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    3a3c:	a61f 0620      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3a40:	a69f 0630      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3a44:	a65f 0620      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    3a48:	a6df 0630      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    3a4c:	a61f 0620      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3a50:	a69f 0630      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3a54:	ae5f 0620      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    3a58:	aedf 0630      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    3a5c:	a61f 0600      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    3a60:	a69f 0610      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    3a64:	a65f 0600      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    3a68:	a6df 0610      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    3a6c:	a61f 0600      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    3a70:	a69f 0610      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    3a74:	ae5f 0600      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    3a78:	aedf 0610      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    3a7c:	a61f 0620      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3a80:	a69f 0630      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3a84:	a65f 0620      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    3a88:	a6df 0630      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    3a8c:	a61f 0620      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    3a90:	a69f 0630      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    3a94:	ae5f 0620      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    3a98:	aedf 0630      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    3a9c:	a62f 0600 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    3aa2:	a6af 0610 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    3aa8:	a66f 0600 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
    3aae:	a6ef 0610 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
    3ab4:	a62f 0600 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    3aba:	a6af 0610 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    3ac0:	ae6f 0600 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
    3ac6:	aeef 0610 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
    3acc:	a62f 0620 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    3ad2:	a6af 0630 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    3ad8:	a66f 0620 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
    3ade:	a6ef 0630 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
    3ae4:	a62f 0620 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    3aea:	a6af 0630 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    3af0:	ae6f 0620 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
    3af6:	aeef 0630 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
    3afc:	a62d 0600 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\),%d3,%acc1
    3b02:	0610           	03020
    3b04:	a66d 0600 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\),%a3,%acc1
    3b0a:	0610           	03020
    3b0c:	a62d 0600 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\),%d3,%acc1
    3b12:	0610           	03020
    3b14:	ae6d 0600 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\),%sp,%acc1
    3b1a:	0610           	03020
    3b1c:	a62d 0620 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\)&,%d3,%acc1
    3b22:	0630           	03060
    3b24:	a66d 0620 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\)&,%a3,%acc1
    3b2a:	0630           	03060
    3b2c:	a62d 0620 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\)&,%d3,%acc1
    3b32:	0630           	03060
    3b34:	ae6d 0620 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\)&,%sp,%acc1
    3b3a:	0630           	03060
    3b3c:	ae5f 0080      	macw %spu,%spu,%sp@\+,%sp,%acc1
    3b40:	aedf 0090      	macw %spu,%spu,%sp@\+,%sp,%acc3
    3b44:	ae5f 0080      	macw %spu,%spu,%sp@\+,%sp,%acc1
    3b48:	aedf 0090      	macw %spu,%spu,%sp@\+,%sp,%acc3
    3b4c:	ae5f 0080      	macw %spu,%spu,%sp@\+,%sp,%acc1
    3b50:	aedf 0090      	macw %spu,%spu,%sp@\+,%sp,%acc3
    3b54:	ae5f 0080      	macw %spu,%spu,%sp@\+,%sp,%acc1
    3b58:	aedf 0090      	macw %spu,%spu,%sp@\+,%sp,%acc3
    3b5c:	ae5f 00a0      	macw %spu,%spu,%sp@\+&,%sp,%acc1
    3b60:	aedf 00b0      	macw %spu,%spu,%sp@\+&,%sp,%acc3
    3b64:	ae5f 00a0      	macw %spu,%spu,%sp@\+&,%sp,%acc1
    3b68:	aedf 00b0      	macw %spu,%spu,%sp@\+&,%sp,%acc3
    3b6c:	ae5f 00a0      	macw %spu,%spu,%sp@\+&,%sp,%acc1
    3b70:	aedf 00b0      	macw %spu,%spu,%sp@\+&,%sp,%acc3
    3b74:	ae5f 00a0      	macw %spu,%spu,%sp@\+&,%sp,%acc1
    3b78:	aedf 00b0      	macw %spu,%spu,%sp@\+&,%sp,%acc3
    3b7c:	ae5f 0080      	macw %spu,%spu,%sp@\+,%sp,%acc1
    3b80:	aedf 0090      	macw %spu,%spu,%sp@\+,%sp,%acc3
    3b84:	ae5f 0080      	macw %spu,%spu,%sp@\+,%sp,%acc1
    3b88:	aedf 0090      	macw %spu,%spu,%sp@\+,%sp,%acc3
    3b8c:	ae5f 0080      	macw %spu,%spu,%sp@\+,%sp,%acc1
    3b90:	aedf 0090      	macw %spu,%spu,%sp@\+,%sp,%acc3
    3b94:	ae5f 0080      	macw %spu,%spu,%sp@\+,%sp,%acc1
    3b98:	aedf 0090      	macw %spu,%spu,%sp@\+,%sp,%acc3
    3b9c:	ae5f 00a0      	macw %spu,%spu,%sp@\+&,%sp,%acc1
    3ba0:	aedf 00b0      	macw %spu,%spu,%sp@\+&,%sp,%acc3
    3ba4:	ae5f 00a0      	macw %spu,%spu,%sp@\+&,%sp,%acc1
    3ba8:	aedf 00b0      	macw %spu,%spu,%sp@\+&,%sp,%acc3
    3bac:	ae5f 00a0      	macw %spu,%spu,%sp@\+&,%sp,%acc1
    3bb0:	aedf 00b0      	macw %spu,%spu,%sp@\+&,%sp,%acc3
    3bb4:	ae5f 00a0      	macw %spu,%spu,%sp@\+&,%sp,%acc1
    3bb8:	aedf 00b0      	macw %spu,%spu,%sp@\+&,%sp,%acc3
    3bbc:	ae6f 0080 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc1
    3bc2:	aeef 0090 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc3
    3bc8:	ae6f 0080 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc1
    3bce:	aeef 0090 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc3
    3bd4:	ae6f 0080 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc1
    3bda:	aeef 0090 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc3
    3be0:	ae6f 0080 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc1
    3be6:	aeef 0090 000a 	macw %spl,%spu,%sp@\(10\),%sp,%acc3
    3bec:	ae6f 00a0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc1
    3bf2:	aeef 00b0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc3
    3bf8:	ae6f 00a0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc1
    3bfe:	aeef 00b0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc3
    3c04:	ae6f 00a0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc1
    3c0a:	aeef 00b0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc3
    3c10:	ae6f 00a0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc1
    3c16:	aeef 00b0 000a 	macw %spl,%spu,%sp@\(10\)&,%sp,%acc3
    3c1c:	ae6d 0080 aeed 	macw %a5l,%spu,%a5@\(-20755\),%sp,%acc1
    3c22:	0090 ae6d 0080 	oril #-1368588160,%d0
    3c28:	aeed 0090 ae6d 	macw %a5l,%spu,%a5@\(-20883\),%sp,%acc3
    3c2e:	0080 aeed 0090 	oril #-1360199536,%d0
    3c34:	ae6d 0080 aeed 	macw %a5l,%spu,%a5@\(-20755\),%sp,%acc1
    3c3a:	0090 ae6d 00a0 	oril #-1368588128,%d0
    3c40:	aeed 00b0 ae6d 	macw %a5l,%spu,%a5@\(-20883\)&,%sp,%acc3
    3c46:	00a0 aeed 00b0 	oril #-1360199504,%d0
    3c4c:	ae6d 00a0 aeed 	macw %a5l,%spu,%a5@\(-20755\)&,%sp,%acc1
    3c52:	00b0 ae6d 00a0 	oril #-1368588128,%d0
    3c58:	aeed 00b0 ae5f 	macw %a5l,%spu,%a5@\(-20897\)&,%sp,%acc3
    3c5e:	0280 aedf 0290 	andil #-1361116528,%d0
    3c64:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3c68:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3c6c:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3c70:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3c74:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3c78:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3c7c:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3c80:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3c84:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3c88:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3c8c:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3c90:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3c94:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3c98:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3c9c:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3ca0:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3ca4:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3ca8:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3cac:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3cb0:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3cb4:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3cb8:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3cbc:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3cc0:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3cc4:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3cc8:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3ccc:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3cd0:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3cd4:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3cd8:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3cdc:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3ce2:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    3ce8:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3cee:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    3cf4:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3cfa:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    3d00:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3d06:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    3d0c:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3d12:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    3d18:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3d1e:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    3d24:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3d2a:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    3d30:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3d36:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    3d3c:	ae6d 0280 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\),%sp,%acc1
    3d42:	0290 ae6d 0280 	andil #-1368587648,%d0
    3d48:	aeed 0290 ae6d 	macw %a5l,%spu,>>,%a5@\(-20883\),%sp,%acc3
    3d4e:	0280 aeed 0290 	andil #-1360199024,%d0
    3d54:	ae6d 0280 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\),%sp,%acc1
    3d5a:	0290 ae6d 02a0 	andil #-1368587616,%d0
    3d60:	aeed 02b0 ae6d 	macw %a5l,%spu,>>,%a5@\(-20883\)&,%sp,%acc3
    3d66:	02a0 aeed 02b0 	andil #-1360198992,%d0
    3d6c:	ae6d 02a0 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\)&,%sp,%acc1
    3d72:	02b0 ae6d 02a0 	andil #-1368587616,%d0
    3d78:	aeed 02b0 ae5f 	macw %a5l,%spu,>>,%a5@\(-20897\)&,%sp,%acc3
    3d7e:	0680 aedf 0690 	addil #-1361115504,%d0
    3d84:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3d88:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3d8c:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3d90:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3d94:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3d98:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3d9c:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3da0:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3da4:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3da8:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3dac:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3db0:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3db4:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3db8:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3dbc:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3dc0:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3dc4:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3dc8:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3dcc:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3dd0:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3dd4:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3dd8:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3ddc:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3de0:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3de4:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3de8:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3dec:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3df0:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3df4:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3df8:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3dfc:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3e02:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    3e08:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3e0e:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    3e14:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3e1a:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    3e20:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3e26:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    3e2c:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3e32:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    3e38:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3e3e:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    3e44:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3e4a:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    3e50:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3e56:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    3e5c:	ae6d 0680 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\),%sp,%acc1
    3e62:	0690 ae6d 0680 	addil #-1368586624,%d0
    3e68:	aeed 0690 ae6d 	macw %a5l,%spu,>>,%a5@\(-20883\),%sp,%acc3
    3e6e:	0680 aeed 0690 	addil #-1360198000,%d0
    3e74:	ae6d 0680 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\),%sp,%acc1
    3e7a:	0690 ae6d 06a0 	addil #-1368586592,%d0
    3e80:	aeed 06b0 ae6d 	macw %a5l,%spu,>>,%a5@\(-20883\)&,%sp,%acc3
    3e86:	06a0 aeed 06b0 	addil #-1360197968,%d0
    3e8c:	ae6d 06a0 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\)&,%sp,%acc1
    3e92:	06b0 ae6d 06a0 	addil #-1368586592,%d0
    3e98:	aeed 06b0 ae5f 	macw %a5l,%spu,>>,%a5@\(-20897\)&,%sp,%acc3
    3e9e:	0280 aedf 0290 	andil #-1361116528,%d0
    3ea4:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3ea8:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3eac:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3eb0:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3eb4:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3eb8:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3ebc:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3ec0:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3ec4:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3ec8:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3ecc:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3ed0:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3ed4:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3ed8:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3edc:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3ee0:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3ee4:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3ee8:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3eec:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3ef0:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3ef4:	ae5f 0280      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3ef8:	aedf 0290      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3efc:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3f00:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3f04:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3f08:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3f0c:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3f10:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3f14:	ae5f 02a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3f18:	aedf 02b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3f1c:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3f22:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    3f28:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3f2e:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    3f34:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3f3a:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    3f40:	ae6f 0280 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    3f46:	aeef 0290 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    3f4c:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3f52:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    3f58:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3f5e:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    3f64:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3f6a:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    3f70:	ae6f 02a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    3f76:	aeef 02b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    3f7c:	ae6d 0280 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\),%sp,%acc1
    3f82:	0290 ae6d 0280 	andil #-1368587648,%d0
    3f88:	aeed 0290 ae6d 	macw %a5l,%spu,>>,%a5@\(-20883\),%sp,%acc3
    3f8e:	0280 aeed 0290 	andil #-1360199024,%d0
    3f94:	ae6d 0280 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\),%sp,%acc1
    3f9a:	0290 ae6d 02a0 	andil #-1368587616,%d0
    3fa0:	aeed 02b0 ae6d 	macw %a5l,%spu,>>,%a5@\(-20883\)&,%sp,%acc3
    3fa6:	02a0 aeed 02b0 	andil #-1360198992,%d0
    3fac:	ae6d 02a0 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\)&,%sp,%acc1
    3fb2:	02b0 ae6d 02a0 	andil #-1368587616,%d0
    3fb8:	aeed 02b0 ae5f 	macw %a5l,%spu,>>,%a5@\(-20897\)&,%sp,%acc3
    3fbe:	0680 aedf 0690 	addil #-1361115504,%d0
    3fc4:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3fc8:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3fcc:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3fd0:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3fd4:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    3fd8:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    3fdc:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3fe0:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3fe4:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3fe8:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3fec:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3ff0:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3ff4:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    3ff8:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    3ffc:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    4000:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    4004:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    4008:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    400c:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    4010:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    4014:	ae5f 0680      	macw %spu,%spu,>>,%sp@\+,%sp,%acc1
    4018:	aedf 0690      	macw %spu,%spu,>>,%sp@\+,%sp,%acc3
    401c:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    4020:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    4024:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    4028:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    402c:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    4030:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    4034:	ae5f 06a0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc1
    4038:	aedf 06b0      	macw %spu,%spu,>>,%sp@\+&,%sp,%acc3
    403c:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    4042:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    4048:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    404e:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    4054:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    405a:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    4060:	ae6f 0680 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc1
    4066:	aeef 0690 000a 	macw %spl,%spu,>>,%sp@\(10\),%sp,%acc3
    406c:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    4072:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    4078:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    407e:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    4084:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    408a:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    4090:	ae6f 06a0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc1
    4096:	aeef 06b0 000a 	macw %spl,%spu,>>,%sp@\(10\)&,%sp,%acc3
    409c:	ae6d 0680 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\),%sp,%acc1
    40a2:	0690 ae6d 0680 	addil #-1368586624,%d0
    40a8:	aeed 0690 ae6d 	macw %a5l,%spu,>>,%a5@\(-20883\),%sp,%acc3
    40ae:	0680 aeed 0690 	addil #-1360198000,%d0
    40b4:	ae6d 0680 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\),%sp,%acc1
    40ba:	0690 ae6d 06a0 	addil #-1368586592,%d0
    40c0:	aeed 06b0 ae6d 	macw %a5l,%spu,>>,%a5@\(-20883\)&,%sp,%acc3
    40c6:	06a0 aeed 06b0 	addil #-1360197968,%d0
    40cc:	ae6d 06a0 aeed 	macw %a5l,%spu,>>,%a5@\(-20755\)&,%sp,%acc1
    40d2:	06b0 ae6d 06a0 	addil #-1368586592,%d0
    40d8:	aeed 06b0 a21f 	macw %a5l,%spu,>>,%a5@\(-24033\)&,%sp,%acc3
    40de:	0000           	00
    40e0:	a29f 0010      	macw %spu,%d1l,%sp@\+,%d1,%acc3
    40e4:	a65f 0000      	macw %spu,%a3l,%sp@\+,%a3,%acc1
    40e8:	a6df 0010      	macw %spu,%a3l,%sp@\+,%a3,%acc3
    40ec:	a61f 0000      	macw %spu,%d3l,%sp@\+,%d3,%acc1
    40f0:	a69f 0010      	macw %spu,%d3l,%sp@\+,%d3,%acc3
    40f4:	ae5f 0000      	macw %spu,%spl,%sp@\+,%sp,%acc1
    40f8:	aedf 0010      	macw %spu,%spl,%sp@\+,%sp,%acc3
    40fc:	a21f 0020      	macw %spu,%d1l,%sp@\+&,%d1,%acc1
    4100:	a29f 0030      	macw %spu,%d1l,%sp@\+&,%d1,%acc3
    4104:	a65f 0020      	macw %spu,%a3l,%sp@\+&,%a3,%acc1
    4108:	a6df 0030      	macw %spu,%a3l,%sp@\+&,%a3,%acc3
    410c:	a61f 0020      	macw %spu,%d3l,%sp@\+&,%d3,%acc1
    4110:	a69f 0030      	macw %spu,%d3l,%sp@\+&,%d3,%acc3
    4114:	ae5f 0020      	macw %spu,%spl,%sp@\+&,%sp,%acc1
    4118:	aedf 0030      	macw %spu,%spl,%sp@\+&,%sp,%acc3
    411c:	a21f 0000      	macw %spu,%d1l,%sp@\+,%d1,%acc1
    4120:	a29f 0010      	macw %spu,%d1l,%sp@\+,%d1,%acc3
    4124:	a65f 0000      	macw %spu,%a3l,%sp@\+,%a3,%acc1
    4128:	a6df 0010      	macw %spu,%a3l,%sp@\+,%a3,%acc3
    412c:	a61f 0000      	macw %spu,%d3l,%sp@\+,%d3,%acc1
    4130:	a69f 0010      	macw %spu,%d3l,%sp@\+,%d3,%acc3
    4134:	ae5f 0000      	macw %spu,%spl,%sp@\+,%sp,%acc1
    4138:	aedf 0010      	macw %spu,%spl,%sp@\+,%sp,%acc3
    413c:	a21f 0020      	macw %spu,%d1l,%sp@\+&,%d1,%acc1
    4140:	a29f 0030      	macw %spu,%d1l,%sp@\+&,%d1,%acc3
    4144:	a65f 0020      	macw %spu,%a3l,%sp@\+&,%a3,%acc1
    4148:	a6df 0030      	macw %spu,%a3l,%sp@\+&,%a3,%acc3
    414c:	a61f 0020      	macw %spu,%d3l,%sp@\+&,%d3,%acc1
    4150:	a69f 0030      	macw %spu,%d3l,%sp@\+&,%d3,%acc3
    4154:	ae5f 0020      	macw %spu,%spl,%sp@\+&,%sp,%acc1
    4158:	aedf 0030      	macw %spu,%spl,%sp@\+&,%sp,%acc3
    415c:	a22f 0000 000a 	macw %spl,%d1l,%sp@\(10\),%d1,%acc1
    4162:	a2af 0010 000a 	macw %spl,%d1l,%sp@\(10\),%d1,%acc3
    4168:	a66f 0000 000a 	macw %spl,%a3l,%sp@\(10\),%a3,%acc1
    416e:	a6ef 0010 000a 	macw %spl,%a3l,%sp@\(10\),%a3,%acc3
    4174:	a62f 0000 000a 	macw %spl,%d3l,%sp@\(10\),%d3,%acc1
    417a:	a6af 0010 000a 	macw %spl,%d3l,%sp@\(10\),%d3,%acc3
    4180:	ae6f 0000 000a 	macw %spl,%spl,%sp@\(10\),%sp,%acc1
    4186:	aeef 0010 000a 	macw %spl,%spl,%sp@\(10\),%sp,%acc3
    418c:	a22f 0020 000a 	macw %spl,%d1l,%sp@\(10\)&,%d1,%acc1
    4192:	a2af 0030 000a 	macw %spl,%d1l,%sp@\(10\)&,%d1,%acc3
    4198:	a66f 0020 000a 	macw %spl,%a3l,%sp@\(10\)&,%a3,%acc1
    419e:	a6ef 0030 000a 	macw %spl,%a3l,%sp@\(10\)&,%a3,%acc3
    41a4:	a62f 0020 000a 	macw %spl,%d3l,%sp@\(10\)&,%d3,%acc1
    41aa:	a6af 0030 000a 	macw %spl,%d3l,%sp@\(10\)&,%d3,%acc3
    41b0:	ae6f 0020 000a 	macw %spl,%spl,%sp@\(10\)&,%sp,%acc1
    41b6:	aeef 0030 000a 	macw %spl,%spl,%sp@\(10\)&,%sp,%acc3
    41bc:	a22d 0000 a2ad 	macw %a5l,%d1l,%a5@\(-23891\),%d1,%acc1
    41c2:	0010           	020
    41c4:	a66d 0000 a6ed 	macw %a5l,%a3l,%a5@\(-22803\),%a3,%acc1
    41ca:	0010           	020
    41cc:	a62d 0000 a6ad 	macw %a5l,%d3l,%a5@\(-22867\),%d3,%acc1
    41d2:	0010           	020
    41d4:	ae6d 0000 aeed 	macw %a5l,%spl,%a5@\(-20755\),%sp,%acc1
    41da:	0010           	020
    41dc:	a22d 0020 a2ad 	macw %a5l,%d1l,%a5@\(-23891\)&,%d1,%acc1
    41e2:	0030           	060
    41e4:	a66d 0020 a6ed 	macw %a5l,%a3l,%a5@\(-22803\)&,%a3,%acc1
    41ea:	0030           	060
    41ec:	a62d 0020 a6ad 	macw %a5l,%d3l,%a5@\(-22867\)&,%d3,%acc1
    41f2:	0030           	060
    41f4:	ae6d 0020 aeed 	macw %a5l,%spl,%a5@\(-20755\)&,%sp,%acc1
    41fa:	0030           	060
    41fc:	a21f 0200      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc1
    4200:	a29f 0210      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc3
    4204:	a65f 0200      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    4208:	a6df 0210      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    420c:	a61f 0200      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    4210:	a69f 0210      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    4214:	ae5f 0200      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    4218:	aedf 0210      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    421c:	a21f 0220      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc1
    4220:	a29f 0230      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc3
    4224:	a65f 0220      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    4228:	a6df 0230      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    422c:	a61f 0220      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    4230:	a69f 0230      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    4234:	ae5f 0220      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    4238:	aedf 0230      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    423c:	a21f 0200      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc1
    4240:	a29f 0210      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc3
    4244:	a65f 0200      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    4248:	a6df 0210      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    424c:	a61f 0200      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    4250:	a69f 0210      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    4254:	ae5f 0200      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    4258:	aedf 0210      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    425c:	a21f 0220      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc1
    4260:	a29f 0230      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc3
    4264:	a65f 0220      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    4268:	a6df 0230      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    426c:	a61f 0220      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    4270:	a69f 0230      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    4274:	ae5f 0220      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    4278:	aedf 0230      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    427c:	a22f 0200 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc1
    4282:	a2af 0210 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc3
    4288:	a66f 0200 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
    428e:	a6ef 0210 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
    4294:	a62f 0200 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    429a:	a6af 0210 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    42a0:	ae6f 0200 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
    42a6:	aeef 0210 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
    42ac:	a22f 0220 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc1
    42b2:	a2af 0230 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc3
    42b8:	a66f 0220 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
    42be:	a6ef 0230 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
    42c4:	a62f 0220 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    42ca:	a6af 0230 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    42d0:	ae6f 0220 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
    42d6:	aeef 0230 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
    42dc:	a22d 0200 a2ad 	macw %a5l,%d1l,<<,%a5@\(-23891\),%d1,%acc1
    42e2:	0210           	01020
    42e4:	a66d 0200 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\),%a3,%acc1
    42ea:	0210           	01020
    42ec:	a62d 0200 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\),%d3,%acc1
    42f2:	0210           	01020
    42f4:	ae6d 0200 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\),%sp,%acc1
    42fa:	0210           	01020
    42fc:	a22d 0220 a2ad 	macw %a5l,%d1l,<<,%a5@\(-23891\)&,%d1,%acc1
    4302:	0230           	01060
    4304:	a66d 0220 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\)&,%a3,%acc1
    430a:	0230           	01060
    430c:	a62d 0220 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\)&,%d3,%acc1
    4312:	0230           	01060
    4314:	ae6d 0220 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\)&,%sp,%acc1
    431a:	0230           	01060
    431c:	a21f 0600      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc1
    4320:	a29f 0610      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc3
    4324:	a65f 0600      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    4328:	a6df 0610      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    432c:	a61f 0600      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    4330:	a69f 0610      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    4334:	ae5f 0600      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    4338:	aedf 0610      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    433c:	a21f 0620      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc1
    4340:	a29f 0630      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc3
    4344:	a65f 0620      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    4348:	a6df 0630      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    434c:	a61f 0620      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    4350:	a69f 0630      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    4354:	ae5f 0620      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    4358:	aedf 0630      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    435c:	a21f 0600      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc1
    4360:	a29f 0610      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc3
    4364:	a65f 0600      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    4368:	a6df 0610      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    436c:	a61f 0600      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    4370:	a69f 0610      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    4374:	ae5f 0600      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    4378:	aedf 0610      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    437c:	a21f 0620      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc1
    4380:	a29f 0630      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc3
    4384:	a65f 0620      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    4388:	a6df 0630      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    438c:	a61f 0620      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    4390:	a69f 0630      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    4394:	ae5f 0620      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    4398:	aedf 0630      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    439c:	a22f 0600 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc1
    43a2:	a2af 0610 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc3
    43a8:	a66f 0600 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
    43ae:	a6ef 0610 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
    43b4:	a62f 0600 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    43ba:	a6af 0610 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    43c0:	ae6f 0600 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
    43c6:	aeef 0610 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
    43cc:	a22f 0620 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc1
    43d2:	a2af 0630 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc3
    43d8:	a66f 0620 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
    43de:	a6ef 0630 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
    43e4:	a62f 0620 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    43ea:	a6af 0630 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    43f0:	ae6f 0620 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
    43f6:	aeef 0630 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
    43fc:	a22d 0600 a2ad 	macw %a5l,%d1l,<<,%a5@\(-23891\),%d1,%acc1
    4402:	0610           	03020
    4404:	a66d 0600 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\),%a3,%acc1
    440a:	0610           	03020
    440c:	a62d 0600 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\),%d3,%acc1
    4412:	0610           	03020
    4414:	ae6d 0600 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\),%sp,%acc1
    441a:	0610           	03020
    441c:	a22d 0620 a2ad 	macw %a5l,%d1l,<<,%a5@\(-23891\)&,%d1,%acc1
    4422:	0630           	03060
    4424:	a66d 0620 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\)&,%a3,%acc1
    442a:	0630           	03060
    442c:	a62d 0620 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\)&,%d3,%acc1
    4432:	0630           	03060
    4434:	ae6d 0620 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\)&,%sp,%acc1
    443a:	0630           	03060
    443c:	a21f 0200      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc1
    4440:	a29f 0210      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc3
    4444:	a65f 0200      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    4448:	a6df 0210      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    444c:	a61f 0200      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    4450:	a69f 0210      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    4454:	ae5f 0200      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    4458:	aedf 0210      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    445c:	a21f 0220      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc1
    4460:	a29f 0230      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc3
    4464:	a65f 0220      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    4468:	a6df 0230      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    446c:	a61f 0220      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    4470:	a69f 0230      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    4474:	ae5f 0220      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    4478:	aedf 0230      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    447c:	a21f 0200      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc1
    4480:	a29f 0210      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc3
    4484:	a65f 0200      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    4488:	a6df 0210      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    448c:	a61f 0200      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    4490:	a69f 0210      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    4494:	ae5f 0200      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    4498:	aedf 0210      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    449c:	a21f 0220      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc1
    44a0:	a29f 0230      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc3
    44a4:	a65f 0220      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    44a8:	a6df 0230      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    44ac:	a61f 0220      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    44b0:	a69f 0230      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    44b4:	ae5f 0220      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    44b8:	aedf 0230      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    44bc:	a22f 0200 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc1
    44c2:	a2af 0210 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc3
    44c8:	a66f 0200 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
    44ce:	a6ef 0210 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
    44d4:	a62f 0200 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    44da:	a6af 0210 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    44e0:	ae6f 0200 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
    44e6:	aeef 0210 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
    44ec:	a22f 0220 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc1
    44f2:	a2af 0230 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc3
    44f8:	a66f 0220 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
    44fe:	a6ef 0230 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
    4504:	a62f 0220 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    450a:	a6af 0230 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    4510:	ae6f 0220 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
    4516:	aeef 0230 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
    451c:	a22d 0200 a2ad 	macw %a5l,%d1l,<<,%a5@\(-23891\),%d1,%acc1
    4522:	0210           	01020
    4524:	a66d 0200 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\),%a3,%acc1
    452a:	0210           	01020
    452c:	a62d 0200 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\),%d3,%acc1
    4532:	0210           	01020
    4534:	ae6d 0200 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\),%sp,%acc1
    453a:	0210           	01020
    453c:	a22d 0220 a2ad 	macw %a5l,%d1l,<<,%a5@\(-23891\)&,%d1,%acc1
    4542:	0230           	01060
    4544:	a66d 0220 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\)&,%a3,%acc1
    454a:	0230           	01060
    454c:	a62d 0220 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\)&,%d3,%acc1
    4552:	0230           	01060
    4554:	ae6d 0220 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\)&,%sp,%acc1
    455a:	0230           	01060
    455c:	a21f 0600      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc1
    4560:	a29f 0610      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc3
    4564:	a65f 0600      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    4568:	a6df 0610      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    456c:	a61f 0600      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    4570:	a69f 0610      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    4574:	ae5f 0600      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    4578:	aedf 0610      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    457c:	a21f 0620      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc1
    4580:	a29f 0630      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc3
    4584:	a65f 0620      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    4588:	a6df 0630      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    458c:	a61f 0620      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    4590:	a69f 0630      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    4594:	ae5f 0620      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    4598:	aedf 0630      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    459c:	a21f 0600      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc1
    45a0:	a29f 0610      	macw %spu,%d1l,<<,%sp@\+,%d1,%acc3
    45a4:	a65f 0600      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc1
    45a8:	a6df 0610      	macw %spu,%a3l,>>,%sp@\+,%a3,%acc3
    45ac:	a61f 0600      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc1
    45b0:	a69f 0610      	macw %spu,%d3l,>>,%sp@\+,%d3,%acc3
    45b4:	ae5f 0600      	macw %spu,%spl,>>,%sp@\+,%sp,%acc1
    45b8:	aedf 0610      	macw %spu,%spl,>>,%sp@\+,%sp,%acc3
    45bc:	a21f 0620      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc1
    45c0:	a29f 0630      	macw %spu,%d1l,<<,%sp@\+&,%d1,%acc3
    45c4:	a65f 0620      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc1
    45c8:	a6df 0630      	macw %spu,%a3l,>>,%sp@\+&,%a3,%acc3
    45cc:	a61f 0620      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc1
    45d0:	a69f 0630      	macw %spu,%d3l,>>,%sp@\+&,%d3,%acc3
    45d4:	ae5f 0620      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc1
    45d8:	aedf 0630      	macw %spu,%spl,>>,%sp@\+&,%sp,%acc3
    45dc:	a22f 0600 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc1
    45e2:	a2af 0610 000a 	macw %spl,%d1l,<<,%sp@\(10\),%d1,%acc3
    45e8:	a66f 0600 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc1
    45ee:	a6ef 0610 000a 	macw %spl,%a3l,>>,%sp@\(10\),%a3,%acc3
    45f4:	a62f 0600 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc1
    45fa:	a6af 0610 000a 	macw %spl,%d3l,>>,%sp@\(10\),%d3,%acc3
    4600:	ae6f 0600 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc1
    4606:	aeef 0610 000a 	macw %spl,%spl,>>,%sp@\(10\),%sp,%acc3
    460c:	a22f 0620 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc1
    4612:	a2af 0630 000a 	macw %spl,%d1l,<<,%sp@\(10\)&,%d1,%acc3
    4618:	a66f 0620 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc1
    461e:	a6ef 0630 000a 	macw %spl,%a3l,>>,%sp@\(10\)&,%a3,%acc3
    4624:	a62f 0620 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc1
    462a:	a6af 0630 000a 	macw %spl,%d3l,>>,%sp@\(10\)&,%d3,%acc3
    4630:	ae6f 0620 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc1
    4636:	aeef 0630 000a 	macw %spl,%spl,>>,%sp@\(10\)&,%sp,%acc3
    463c:	a22d 0600 a2ad 	macw %a5l,%d1l,<<,%a5@\(-23891\),%d1,%acc1
    4642:	0610           	03020
    4644:	a66d 0600 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\),%a3,%acc1
    464a:	0610           	03020
    464c:	a62d 0600 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\),%d3,%acc1
    4652:	0610           	03020
    4654:	ae6d 0600 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\),%sp,%acc1
    465a:	0610           	03020
    465c:	a22d 0620 a2ad 	macw %a5l,%d1l,<<,%a5@\(-23891\)&,%d1,%acc1
    4662:	0630           	03060
    4664:	a66d 0620 a6ed 	macw %a5l,%a3l,>>,%a5@\(-22803\)&,%a3,%acc1
    466a:	0630           	03060
    466c:	a62d 0620 a6ad 	macw %a5l,%d3l,>>,%a5@\(-22867\)&,%d3,%acc1
    4672:	0630           	03060
    4674:	ae6d 0620 aeed 	macw %a5l,%spl,>>,%a5@\(-20755\)&,%sp,%acc1
    467a:	0630           	03060
    467c:	a657 00c0      	macw %d7u,%a3u,%sp@,%a3,%acc1
    4680:	a6d7 00d0      	macw %d7u,%a3u,%sp@,%a3,%acc3
    4684:	a657 00c0      	macw %d7u,%a3u,%sp@,%a3,%acc1
    4688:	a6d7 00d0      	macw %d7u,%a3u,%sp@,%a3,%acc3
    468c:	a457 00c0      	macw %d7u,%a2u,%sp@,%a2,%acc1
    4690:	a4d7 00d0      	macw %d7u,%a2u,%sp@,%a2,%acc3
    4694:	ae57 00c0      	macw %d7u,%spu,%sp@,%sp,%acc1
    4698:	aed7 00d0      	macw %d7u,%spu,%sp@,%sp,%acc3
    469c:	a657 00e0      	macw %d7u,%a3u,%sp@&,%a3,%acc1
    46a0:	a6d7 00f0      	macw %d7u,%a3u,%sp@&,%a3,%acc3
    46a4:	a657 00e0      	macw %d7u,%a3u,%sp@&,%a3,%acc1
    46a8:	a6d7 00f0      	macw %d7u,%a3u,%sp@&,%a3,%acc3
    46ac:	a457 00e0      	macw %d7u,%a2u,%sp@&,%a2,%acc1
    46b0:	a4d7 00f0      	macw %d7u,%a2u,%sp@&,%a2,%acc3
    46b4:	ae57 00e0      	macw %d7u,%spu,%sp@&,%sp,%acc1
    46b8:	aed7 00f0      	macw %d7u,%spu,%sp@&,%sp,%acc3
    46bc:	a65e 00c0      	macw %fpu,%a3u,%fp@\+,%a3,%acc1
    46c0:	a6de 00d0      	macw %fpu,%a3u,%fp@\+,%a3,%acc3
    46c4:	a65e 00c0      	macw %fpu,%a3u,%fp@\+,%a3,%acc1
    46c8:	a6de 00d0      	macw %fpu,%a3u,%fp@\+,%a3,%acc3
    46cc:	a45e 00c0      	macw %fpu,%a2u,%fp@\+,%a2,%acc1
    46d0:	a4de 00d0      	macw %fpu,%a2u,%fp@\+,%a2,%acc3
    46d4:	ae5e 00c0      	macw %fpu,%spu,%fp@\+,%sp,%acc1
    46d8:	aede 00d0      	macw %fpu,%spu,%fp@\+,%sp,%acc3
    46dc:	a65e 00e0      	macw %fpu,%a3u,%fp@\+&,%a3,%acc1
    46e0:	a6de 00f0      	macw %fpu,%a3u,%fp@\+&,%a3,%acc3
    46e4:	a65e 00e0      	macw %fpu,%a3u,%fp@\+&,%a3,%acc1
    46e8:	a6de 00f0      	macw %fpu,%a3u,%fp@\+&,%a3,%acc3
    46ec:	a45e 00e0      	macw %fpu,%a2u,%fp@\+&,%a2,%acc1
    46f0:	a4de 00f0      	macw %fpu,%a2u,%fp@\+&,%a2,%acc3
    46f4:	ae5e 00e0      	macw %fpu,%spu,%fp@\+&,%sp,%acc1
    46f8:	aede 00f0      	macw %fpu,%spu,%fp@\+&,%sp,%acc3
    46fc:	a66e 00c0 000a 	macw %fpu,%a3u,%fp@\(10\),%a3,%acc1
    4702:	a6ee 00d0 000a 	macw %fpu,%a3u,%fp@\(10\),%a3,%acc3
    4708:	a66e 00c0 000a 	macw %fpu,%a3u,%fp@\(10\),%a3,%acc1
    470e:	a6ee 00d0 000a 	macw %fpu,%a3u,%fp@\(10\),%a3,%acc3
    4714:	a46e 00c0 000a 	macw %fpu,%a2u,%fp@\(10\),%a2,%acc1
    471a:	a4ee 00d0 000a 	macw %fpu,%a2u,%fp@\(10\),%a2,%acc3
    4720:	ae6e 00c0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc1
    4726:	aeee 00d0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc3
    472c:	a66e 00e0 000a 	macw %fpu,%a3u,%fp@\(10\)&,%a3,%acc1
    4732:	a6ee 00f0 000a 	macw %fpu,%a3u,%fp@\(10\)&,%a3,%acc3
    4738:	a66e 00e0 000a 	macw %fpu,%a3u,%fp@\(10\)&,%a3,%acc1
    473e:	a6ee 00f0 000a 	macw %fpu,%a3u,%fp@\(10\)&,%a3,%acc3
    4744:	a46e 00e0 000a 	macw %fpu,%a2u,%fp@\(10\)&,%a2,%acc1
    474a:	a4ee 00f0 000a 	macw %fpu,%a2u,%fp@\(10\)&,%a2,%acc3
    4750:	ae6e 00e0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc1
    4756:	aeee 00f0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc3
    475c:	a667 00c0      	macw %d7u,%a3u,%sp@-,%a3,%acc1
    4760:	a6e7 00d0      	macw %d7u,%a3u,%sp@-,%a3,%acc3
    4764:	a667 00c0      	macw %d7u,%a3u,%sp@-,%a3,%acc1
    4768:	a6e7 00d0      	macw %d7u,%a3u,%sp@-,%a3,%acc3
    476c:	a467 00c0      	macw %d7u,%a2u,%sp@-,%a2,%acc1
    4770:	a4e7 00d0      	macw %d7u,%a2u,%sp@-,%a2,%acc3
    4774:	ae67 00c0      	macw %d7u,%spu,%sp@-,%sp,%acc1
    4778:	aee7 00d0      	macw %d7u,%spu,%sp@-,%sp,%acc3
    477c:	a667 00e0      	macw %d7u,%a3u,%sp@-&,%a3,%acc1
    4780:	a6e7 00f0      	macw %d7u,%a3u,%sp@-&,%a3,%acc3
    4784:	a667 00e0      	macw %d7u,%a3u,%sp@-&,%a3,%acc1
    4788:	a6e7 00f0      	macw %d7u,%a3u,%sp@-&,%a3,%acc3
    478c:	a467 00e0      	macw %d7u,%a2u,%sp@-&,%a2,%acc1
    4790:	a4e7 00f0      	macw %d7u,%a2u,%sp@-&,%a2,%acc3
    4794:	ae67 00e0      	macw %d7u,%spu,%sp@-&,%sp,%acc1
    4798:	aee7 00f0      	macw %d7u,%spu,%sp@-&,%sp,%acc3
    479c:	a657 02c0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc1
    47a0:	a6d7 02d0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc3
    47a4:	a657 02c0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc1
    47a8:	a6d7 02d0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc3
    47ac:	a457 02c0      	macw %d7u,%a2u,,%sp@,%a2,%acc1
    47b0:	a4d7 02d0      	macw %d7u,%a2u,,%sp@,%a2,%acc3
    47b4:	ae57 02c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    47b8:	aed7 02d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    47bc:	a657 02e0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc1
    47c0:	a6d7 02f0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc3
    47c4:	a657 02e0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc1
    47c8:	a6d7 02f0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc3
    47cc:	a457 02e0      	macw %d7u,%a2u,,%sp@&,%a2,%acc1
    47d0:	a4d7 02f0      	macw %d7u,%a2u,,%sp@&,%a2,%acc3
    47d4:	ae57 02e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    47d8:	aed7 02f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    47dc:	a65e 02c0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc1
    47e0:	a6de 02d0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc3
    47e4:	a65e 02c0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc1
    47e8:	a6de 02d0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc3
    47ec:	a45e 02c0      	macw %fpu,%a2u,,%fp@\+,%a2,%acc1
    47f0:	a4de 02d0      	macw %fpu,%a2u,,%fp@\+,%a2,%acc3
    47f4:	ae5e 02c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    47f8:	aede 02d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    47fc:	a65e 02e0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc1
    4800:	a6de 02f0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc3
    4804:	a65e 02e0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc1
    4808:	a6de 02f0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc3
    480c:	a45e 02e0      	macw %fpu,%a2u,,%fp@\+&,%a2,%acc1
    4810:	a4de 02f0      	macw %fpu,%a2u,,%fp@\+&,%a2,%acc3
    4814:	ae5e 02e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    4818:	aede 02f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    481c:	a66e 02c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    4822:	a6ee 02d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    4828:	a66e 02c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    482e:	a6ee 02d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    4834:	a46e 02c0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc1
    483a:	a4ee 02d0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc3
    4840:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    4846:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    484c:	a66e 02e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    4852:	a6ee 02f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    4858:	a66e 02e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    485e:	a6ee 02f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    4864:	a46e 02e0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc1
    486a:	a4ee 02f0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc3
    4870:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    4876:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    487c:	a667 02c0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc1
    4880:	a6e7 02d0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc3
    4884:	a667 02c0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc1
    4888:	a6e7 02d0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc3
    488c:	a467 02c0      	macw %d7u,%a2u,,%sp@-,%a2,%acc1
    4890:	a4e7 02d0      	macw %d7u,%a2u,,%sp@-,%a2,%acc3
    4894:	ae67 02c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    4898:	aee7 02d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    489c:	a667 02e0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc1
    48a0:	a6e7 02f0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc3
    48a4:	a667 02e0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc1
    48a8:	a6e7 02f0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc3
    48ac:	a467 02e0      	macw %d7u,%a2u,,%sp@-&,%a2,%acc1
    48b0:	a4e7 02f0      	macw %d7u,%a2u,,%sp@-&,%a2,%acc3
    48b4:	ae67 02e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    48b8:	aee7 02f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    48bc:	a657 06c0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc1
    48c0:	a6d7 06d0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc3
    48c4:	a657 06c0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc1
    48c8:	a6d7 06d0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc3
    48cc:	a457 06c0      	macw %d7u,%a2u,,%sp@,%a2,%acc1
    48d0:	a4d7 06d0      	macw %d7u,%a2u,,%sp@,%a2,%acc3
    48d4:	ae57 06c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    48d8:	aed7 06d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    48dc:	a657 06e0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc1
    48e0:	a6d7 06f0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc3
    48e4:	a657 06e0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc1
    48e8:	a6d7 06f0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc3
    48ec:	a457 06e0      	macw %d7u,%a2u,,%sp@&,%a2,%acc1
    48f0:	a4d7 06f0      	macw %d7u,%a2u,,%sp@&,%a2,%acc3
    48f4:	ae57 06e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    48f8:	aed7 06f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    48fc:	a65e 06c0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc1
    4900:	a6de 06d0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc3
    4904:	a65e 06c0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc1
    4908:	a6de 06d0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc3
    490c:	a45e 06c0      	macw %fpu,%a2u,,%fp@\+,%a2,%acc1
    4910:	a4de 06d0      	macw %fpu,%a2u,,%fp@\+,%a2,%acc3
    4914:	ae5e 06c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    4918:	aede 06d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    491c:	a65e 06e0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc1
    4920:	a6de 06f0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc3
    4924:	a65e 06e0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc1
    4928:	a6de 06f0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc3
    492c:	a45e 06e0      	macw %fpu,%a2u,,%fp@\+&,%a2,%acc1
    4930:	a4de 06f0      	macw %fpu,%a2u,,%fp@\+&,%a2,%acc3
    4934:	ae5e 06e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    4938:	aede 06f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    493c:	a66e 06c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    4942:	a6ee 06d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    4948:	a66e 06c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    494e:	a6ee 06d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    4954:	a46e 06c0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc1
    495a:	a4ee 06d0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc3
    4960:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    4966:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    496c:	a66e 06e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    4972:	a6ee 06f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    4978:	a66e 06e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    497e:	a6ee 06f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    4984:	a46e 06e0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc1
    498a:	a4ee 06f0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc3
    4990:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    4996:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    499c:	a667 06c0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc1
    49a0:	a6e7 06d0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc3
    49a4:	a667 06c0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc1
    49a8:	a6e7 06d0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc3
    49ac:	a467 06c0      	macw %d7u,%a2u,,%sp@-,%a2,%acc1
    49b0:	a4e7 06d0      	macw %d7u,%a2u,,%sp@-,%a2,%acc3
    49b4:	ae67 06c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    49b8:	aee7 06d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    49bc:	a667 06e0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc1
    49c0:	a6e7 06f0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc3
    49c4:	a667 06e0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc1
    49c8:	a6e7 06f0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc3
    49cc:	a467 06e0      	macw %d7u,%a2u,,%sp@-&,%a2,%acc1
    49d0:	a4e7 06f0      	macw %d7u,%a2u,,%sp@-&,%a2,%acc3
    49d4:	ae67 06e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    49d8:	aee7 06f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    49dc:	a657 02c0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc1
    49e0:	a6d7 02d0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc3
    49e4:	a657 02c0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc1
    49e8:	a6d7 02d0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc3
    49ec:	a457 02c0      	macw %d7u,%a2u,,%sp@,%a2,%acc1
    49f0:	a4d7 02d0      	macw %d7u,%a2u,,%sp@,%a2,%acc3
    49f4:	ae57 02c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    49f8:	aed7 02d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    49fc:	a657 02e0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc1
    4a00:	a6d7 02f0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc3
    4a04:	a657 02e0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc1
    4a08:	a6d7 02f0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc3
    4a0c:	a457 02e0      	macw %d7u,%a2u,,%sp@&,%a2,%acc1
    4a10:	a4d7 02f0      	macw %d7u,%a2u,,%sp@&,%a2,%acc3
    4a14:	ae57 02e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    4a18:	aed7 02f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    4a1c:	a65e 02c0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc1
    4a20:	a6de 02d0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc3
    4a24:	a65e 02c0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc1
    4a28:	a6de 02d0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc3
    4a2c:	a45e 02c0      	macw %fpu,%a2u,,%fp@\+,%a2,%acc1
    4a30:	a4de 02d0      	macw %fpu,%a2u,,%fp@\+,%a2,%acc3
    4a34:	ae5e 02c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    4a38:	aede 02d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    4a3c:	a65e 02e0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc1
    4a40:	a6de 02f0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc3
    4a44:	a65e 02e0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc1
    4a48:	a6de 02f0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc3
    4a4c:	a45e 02e0      	macw %fpu,%a2u,,%fp@\+&,%a2,%acc1
    4a50:	a4de 02f0      	macw %fpu,%a2u,,%fp@\+&,%a2,%acc3
    4a54:	ae5e 02e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    4a58:	aede 02f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    4a5c:	a66e 02c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    4a62:	a6ee 02d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    4a68:	a66e 02c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    4a6e:	a6ee 02d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    4a74:	a46e 02c0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc1
    4a7a:	a4ee 02d0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc3
    4a80:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    4a86:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    4a8c:	a66e 02e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    4a92:	a6ee 02f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    4a98:	a66e 02e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    4a9e:	a6ee 02f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    4aa4:	a46e 02e0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc1
    4aaa:	a4ee 02f0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc3
    4ab0:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    4ab6:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    4abc:	a667 02c0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc1
    4ac0:	a6e7 02d0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc3
    4ac4:	a667 02c0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc1
    4ac8:	a6e7 02d0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc3
    4acc:	a467 02c0      	macw %d7u,%a2u,,%sp@-,%a2,%acc1
    4ad0:	a4e7 02d0      	macw %d7u,%a2u,,%sp@-,%a2,%acc3
    4ad4:	ae67 02c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    4ad8:	aee7 02d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    4adc:	a667 02e0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc1
    4ae0:	a6e7 02f0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc3
    4ae4:	a667 02e0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc1
    4ae8:	a6e7 02f0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc3
    4aec:	a467 02e0      	macw %d7u,%a2u,,%sp@-&,%a2,%acc1
    4af0:	a4e7 02f0      	macw %d7u,%a2u,,%sp@-&,%a2,%acc3
    4af4:	ae67 02e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    4af8:	aee7 02f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    4afc:	a657 06c0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc1
    4b00:	a6d7 06d0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc3
    4b04:	a657 06c0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc1
    4b08:	a6d7 06d0      	macw %d7u,%a3u,>>,%sp@,%a3,%acc3
    4b0c:	a457 06c0      	macw %d7u,%a2u,,%sp@,%a2,%acc1
    4b10:	a4d7 06d0      	macw %d7u,%a2u,,%sp@,%a2,%acc3
    4b14:	ae57 06c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    4b18:	aed7 06d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    4b1c:	a657 06e0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc1
    4b20:	a6d7 06f0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc3
    4b24:	a657 06e0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc1
    4b28:	a6d7 06f0      	macw %d7u,%a3u,>>,%sp@&,%a3,%acc3
    4b2c:	a457 06e0      	macw %d7u,%a2u,,%sp@&,%a2,%acc1
    4b30:	a4d7 06f0      	macw %d7u,%a2u,,%sp@&,%a2,%acc3
    4b34:	ae57 06e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    4b38:	aed7 06f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    4b3c:	a65e 06c0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc1
    4b40:	a6de 06d0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc3
    4b44:	a65e 06c0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc1
    4b48:	a6de 06d0      	macw %fpu,%a3u,>>,%fp@\+,%a3,%acc3
    4b4c:	a45e 06c0      	macw %fpu,%a2u,,%fp@\+,%a2,%acc1
    4b50:	a4de 06d0      	macw %fpu,%a2u,,%fp@\+,%a2,%acc3
    4b54:	ae5e 06c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    4b58:	aede 06d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    4b5c:	a65e 06e0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc1
    4b60:	a6de 06f0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc3
    4b64:	a65e 06e0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc1
    4b68:	a6de 06f0      	macw %fpu,%a3u,>>,%fp@\+&,%a3,%acc3
    4b6c:	a45e 06e0      	macw %fpu,%a2u,,%fp@\+&,%a2,%acc1
    4b70:	a4de 06f0      	macw %fpu,%a2u,,%fp@\+&,%a2,%acc3
    4b74:	ae5e 06e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    4b78:	aede 06f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    4b7c:	a66e 06c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    4b82:	a6ee 06d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    4b88:	a66e 06c0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc1
    4b8e:	a6ee 06d0 000a 	macw %fpu,%a3u,>>,%fp@\(10\),%a3,%acc3
    4b94:	a46e 06c0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc1
    4b9a:	a4ee 06d0 000a 	macw %fpu,%a2u,,%fp@\(10\),%a2,%acc3
    4ba0:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    4ba6:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    4bac:	a66e 06e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    4bb2:	a6ee 06f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    4bb8:	a66e 06e0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc1
    4bbe:	a6ee 06f0 000a 	macw %fpu,%a3u,>>,%fp@\(10\)&,%a3,%acc3
    4bc4:	a46e 06e0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc1
    4bca:	a4ee 06f0 000a 	macw %fpu,%a2u,,%fp@\(10\)&,%a2,%acc3
    4bd0:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    4bd6:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    4bdc:	a667 06c0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc1
    4be0:	a6e7 06d0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc3
    4be4:	a667 06c0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc1
    4be8:	a6e7 06d0      	macw %d7u,%a3u,>>,%sp@-,%a3,%acc3
    4bec:	a467 06c0      	macw %d7u,%a2u,,%sp@-,%a2,%acc1
    4bf0:	a4e7 06d0      	macw %d7u,%a2u,,%sp@-,%a2,%acc3
    4bf4:	ae67 06c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    4bf8:	aee7 06d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    4bfc:	a667 06e0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc1
    4c00:	a6e7 06f0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc3
    4c04:	a667 06e0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc1
    4c08:	a6e7 06f0      	macw %d7u,%a3u,>>,%sp@-&,%a3,%acc3
    4c0c:	a467 06e0      	macw %d7u,%a2u,,%sp@-&,%a2,%acc1
    4c10:	a4e7 06f0      	macw %d7u,%a2u,,%sp@-&,%a2,%acc3
    4c14:	ae67 06e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    4c18:	aee7 06f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    4c1c:	a617 0040      	macw %d7u,%d3l,%sp@,%d3,%acc1
    4c20:	a697 0050      	macw %d7u,%d3l,%sp@,%d3,%acc3
    4c24:	a657 0040      	macw %d7u,%a3l,%sp@,%a3,%acc1
    4c28:	a6d7 0050      	macw %d7u,%a3l,%sp@,%a3,%acc3
    4c2c:	a617 0040      	macw %d7u,%d3l,%sp@,%d3,%acc1
    4c30:	a697 0050      	macw %d7u,%d3l,%sp@,%d3,%acc3
    4c34:	ae57 0040      	macw %d7u,%spl,%sp@,%sp,%acc1
    4c38:	aed7 0050      	macw %d7u,%spl,%sp@,%sp,%acc3
    4c3c:	a617 0060      	macw %d7u,%d3l,%sp@&,%d3,%acc1
    4c40:	a697 0070      	macw %d7u,%d3l,%sp@&,%d3,%acc3
    4c44:	a657 0060      	macw %d7u,%a3l,%sp@&,%a3,%acc1
    4c48:	a6d7 0070      	macw %d7u,%a3l,%sp@&,%a3,%acc3
    4c4c:	a617 0060      	macw %d7u,%d3l,%sp@&,%d3,%acc1
    4c50:	a697 0070      	macw %d7u,%d3l,%sp@&,%d3,%acc3
    4c54:	ae57 0060      	macw %d7u,%spl,%sp@&,%sp,%acc1
    4c58:	aed7 0070      	macw %d7u,%spl,%sp@&,%sp,%acc3
    4c5c:	a61e 0040      	macw %fpu,%d3l,%fp@\+,%d3,%acc1
    4c60:	a69e 0050      	macw %fpu,%d3l,%fp@\+,%d3,%acc3
    4c64:	a65e 0040      	macw %fpu,%a3l,%fp@\+,%a3,%acc1
    4c68:	a6de 0050      	macw %fpu,%a3l,%fp@\+,%a3,%acc3
    4c6c:	a61e 0040      	macw %fpu,%d3l,%fp@\+,%d3,%acc1
    4c70:	a69e 0050      	macw %fpu,%d3l,%fp@\+,%d3,%acc3
    4c74:	ae5e 0040      	macw %fpu,%spl,%fp@\+,%sp,%acc1
    4c78:	aede 0050      	macw %fpu,%spl,%fp@\+,%sp,%acc3
    4c7c:	a61e 0060      	macw %fpu,%d3l,%fp@\+&,%d3,%acc1
    4c80:	a69e 0070      	macw %fpu,%d3l,%fp@\+&,%d3,%acc3
    4c84:	a65e 0060      	macw %fpu,%a3l,%fp@\+&,%a3,%acc1
    4c88:	a6de 0070      	macw %fpu,%a3l,%fp@\+&,%a3,%acc3
    4c8c:	a61e 0060      	macw %fpu,%d3l,%fp@\+&,%d3,%acc1
    4c90:	a69e 0070      	macw %fpu,%d3l,%fp@\+&,%d3,%acc3
    4c94:	ae5e 0060      	macw %fpu,%spl,%fp@\+&,%sp,%acc1
    4c98:	aede 0070      	macw %fpu,%spl,%fp@\+&,%sp,%acc3
    4c9c:	a62e 0040 000a 	macw %fpu,%d3l,%fp@\(10\),%d3,%acc1
    4ca2:	a6ae 0050 000a 	macw %fpu,%d3l,%fp@\(10\),%d3,%acc3
    4ca8:	a66e 0040 000a 	macw %fpu,%a3l,%fp@\(10\),%a3,%acc1
    4cae:	a6ee 0050 000a 	macw %fpu,%a3l,%fp@\(10\),%a3,%acc3
    4cb4:	a62e 0040 000a 	macw %fpu,%d3l,%fp@\(10\),%d3,%acc1
    4cba:	a6ae 0050 000a 	macw %fpu,%d3l,%fp@\(10\),%d3,%acc3
    4cc0:	ae6e 0040 000a 	macw %fpu,%spl,%fp@\(10\),%sp,%acc1
    4cc6:	aeee 0050 000a 	macw %fpu,%spl,%fp@\(10\),%sp,%acc3
    4ccc:	a62e 0060 000a 	macw %fpu,%d3l,%fp@\(10\)&,%d3,%acc1
    4cd2:	a6ae 0070 000a 	macw %fpu,%d3l,%fp@\(10\)&,%d3,%acc3
    4cd8:	a66e 0060 000a 	macw %fpu,%a3l,%fp@\(10\)&,%a3,%acc1
    4cde:	a6ee 0070 000a 	macw %fpu,%a3l,%fp@\(10\)&,%a3,%acc3
    4ce4:	a62e 0060 000a 	macw %fpu,%d3l,%fp@\(10\)&,%d3,%acc1
    4cea:	a6ae 0070 000a 	macw %fpu,%d3l,%fp@\(10\)&,%d3,%acc3
    4cf0:	ae6e 0060 000a 	macw %fpu,%spl,%fp@\(10\)&,%sp,%acc1
    4cf6:	aeee 0070 000a 	macw %fpu,%spl,%fp@\(10\)&,%sp,%acc3
    4cfc:	a627 0040      	macw %d7u,%d3l,%sp@-,%d3,%acc1
    4d00:	a6a7 0050      	macw %d7u,%d3l,%sp@-,%d3,%acc3
    4d04:	a667 0040      	macw %d7u,%a3l,%sp@-,%a3,%acc1
    4d08:	a6e7 0050      	macw %d7u,%a3l,%sp@-,%a3,%acc3
    4d0c:	a627 0040      	macw %d7u,%d3l,%sp@-,%d3,%acc1
    4d10:	a6a7 0050      	macw %d7u,%d3l,%sp@-,%d3,%acc3
    4d14:	ae67 0040      	macw %d7u,%spl,%sp@-,%sp,%acc1
    4d18:	aee7 0050      	macw %d7u,%spl,%sp@-,%sp,%acc3
    4d1c:	a627 0060      	macw %d7u,%d3l,%sp@-&,%d3,%acc1
    4d20:	a6a7 0070      	macw %d7u,%d3l,%sp@-&,%d3,%acc3
    4d24:	a667 0060      	macw %d7u,%a3l,%sp@-&,%a3,%acc1
    4d28:	a6e7 0070      	macw %d7u,%a3l,%sp@-&,%a3,%acc3
    4d2c:	a627 0060      	macw %d7u,%d3l,%sp@-&,%d3,%acc1
    4d30:	a6a7 0070      	macw %d7u,%d3l,%sp@-&,%d3,%acc3
    4d34:	ae67 0060      	macw %d7u,%spl,%sp@-&,%sp,%acc1
    4d38:	aee7 0070      	macw %d7u,%spl,%sp@-&,%sp,%acc3
    4d3c:	a617 0240      	macw %d7u,%d3l,>>,%sp@,%d3,%acc1
    4d40:	a697 0250      	macw %d7u,%d3l,>>,%sp@,%d3,%acc3
    4d44:	a657 0240      	macw %d7u,%a3l,>>,%sp@,%a3,%acc1
    4d48:	a6d7 0250      	macw %d7u,%a3l,>>,%sp@,%a3,%acc3
    4d4c:	a617 0240      	macw %d7u,%d3l,>>,%sp@,%d3,%acc1
    4d50:	a697 0250      	macw %d7u,%d3l,>>,%sp@,%d3,%acc3
    4d54:	ae57 0240      	macw %d7u,%spl,>>,%sp@,%sp,%acc1
    4d58:	aed7 0250      	macw %d7u,%spl,>>,%sp@,%sp,%acc3
    4d5c:	a617 0260      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc1
    4d60:	a697 0270      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc3
    4d64:	a657 0260      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc1
    4d68:	a6d7 0270      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc3
    4d6c:	a617 0260      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc1
    4d70:	a697 0270      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc3
    4d74:	ae57 0260      	macw %d7u,%spl,>>,%sp@&,%sp,%acc1
    4d78:	aed7 0270      	macw %d7u,%spl,>>,%sp@&,%sp,%acc3
    4d7c:	a61e 0240      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc1
    4d80:	a69e 0250      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc3
    4d84:	a65e 0240      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc1
    4d88:	a6de 0250      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc3
    4d8c:	a61e 0240      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc1
    4d90:	a69e 0250      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc3
    4d94:	ae5e 0240      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc1
    4d98:	aede 0250      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc3
    4d9c:	a61e 0260      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc1
    4da0:	a69e 0270      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc3
    4da4:	a65e 0260      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc1
    4da8:	a6de 0270      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc3
    4dac:	a61e 0260      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc1
    4db0:	a69e 0270      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc3
    4db4:	ae5e 0260      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc1
    4db8:	aede 0270      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc3
    4dbc:	a62e 0240 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    4dc2:	a6ae 0250 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    4dc8:	a66e 0240 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    4dce:	a6ee 0250 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    4dd4:	a62e 0240 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    4dda:	a6ae 0250 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    4de0:	ae6e 0240 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    4de6:	aeee 0250 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    4dec:	a62e 0260 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    4df2:	a6ae 0270 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    4df8:	a66e 0260 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    4dfe:	a6ee 0270 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    4e04:	a62e 0260 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    4e0a:	a6ae 0270 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    4e10:	ae6e 0260 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    4e16:	aeee 0270 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    4e1c:	a627 0240      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc1
    4e20:	a6a7 0250      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc3
    4e24:	a667 0240      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc1
    4e28:	a6e7 0250      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc3
    4e2c:	a627 0240      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc1
    4e30:	a6a7 0250      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc3
    4e34:	ae67 0240      	macw %d7u,%spl,>>,%sp@-,%sp,%acc1
    4e38:	aee7 0250      	macw %d7u,%spl,>>,%sp@-,%sp,%acc3
    4e3c:	a627 0260      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc1
    4e40:	a6a7 0270      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc3
    4e44:	a667 0260      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc1
    4e48:	a6e7 0270      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc3
    4e4c:	a627 0260      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc1
    4e50:	a6a7 0270      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc3
    4e54:	ae67 0260      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc1
    4e58:	aee7 0270      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc3
    4e5c:	a617 0640      	macw %d7u,%d3l,>>,%sp@,%d3,%acc1
    4e60:	a697 0650      	macw %d7u,%d3l,>>,%sp@,%d3,%acc3
    4e64:	a657 0640      	macw %d7u,%a3l,>>,%sp@,%a3,%acc1
    4e68:	a6d7 0650      	macw %d7u,%a3l,>>,%sp@,%a3,%acc3
    4e6c:	a617 0640      	macw %d7u,%d3l,>>,%sp@,%d3,%acc1
    4e70:	a697 0650      	macw %d7u,%d3l,>>,%sp@,%d3,%acc3
    4e74:	ae57 0640      	macw %d7u,%spl,>>,%sp@,%sp,%acc1
    4e78:	aed7 0650      	macw %d7u,%spl,>>,%sp@,%sp,%acc3
    4e7c:	a617 0660      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc1
    4e80:	a697 0670      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc3
    4e84:	a657 0660      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc1
    4e88:	a6d7 0670      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc3
    4e8c:	a617 0660      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc1
    4e90:	a697 0670      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc3
    4e94:	ae57 0660      	macw %d7u,%spl,>>,%sp@&,%sp,%acc1
    4e98:	aed7 0670      	macw %d7u,%spl,>>,%sp@&,%sp,%acc3
    4e9c:	a61e 0640      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc1
    4ea0:	a69e 0650      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc3
    4ea4:	a65e 0640      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc1
    4ea8:	a6de 0650      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc3
    4eac:	a61e 0640      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc1
    4eb0:	a69e 0650      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc3
    4eb4:	ae5e 0640      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc1
    4eb8:	aede 0650      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc3
    4ebc:	a61e 0660      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc1
    4ec0:	a69e 0670      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc3
    4ec4:	a65e 0660      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc1
    4ec8:	a6de 0670      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc3
    4ecc:	a61e 0660      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc1
    4ed0:	a69e 0670      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc3
    4ed4:	ae5e 0660      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc1
    4ed8:	aede 0670      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc3
    4edc:	a62e 0640 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    4ee2:	a6ae 0650 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    4ee8:	a66e 0640 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    4eee:	a6ee 0650 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    4ef4:	a62e 0640 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    4efa:	a6ae 0650 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    4f00:	ae6e 0640 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    4f06:	aeee 0650 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    4f0c:	a62e 0660 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    4f12:	a6ae 0670 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    4f18:	a66e 0660 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    4f1e:	a6ee 0670 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    4f24:	a62e 0660 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    4f2a:	a6ae 0670 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    4f30:	ae6e 0660 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    4f36:	aeee 0670 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    4f3c:	a627 0640      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc1
    4f40:	a6a7 0650      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc3
    4f44:	a667 0640      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc1
    4f48:	a6e7 0650      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc3
    4f4c:	a627 0640      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc1
    4f50:	a6a7 0650      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc3
    4f54:	ae67 0640      	macw %d7u,%spl,>>,%sp@-,%sp,%acc1
    4f58:	aee7 0650      	macw %d7u,%spl,>>,%sp@-,%sp,%acc3
    4f5c:	a627 0660      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc1
    4f60:	a6a7 0670      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc3
    4f64:	a667 0660      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc1
    4f68:	a6e7 0670      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc3
    4f6c:	a627 0660      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc1
    4f70:	a6a7 0670      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc3
    4f74:	ae67 0660      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc1
    4f78:	aee7 0670      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc3
    4f7c:	a617 0240      	macw %d7u,%d3l,>>,%sp@,%d3,%acc1
    4f80:	a697 0250      	macw %d7u,%d3l,>>,%sp@,%d3,%acc3
    4f84:	a657 0240      	macw %d7u,%a3l,>>,%sp@,%a3,%acc1
    4f88:	a6d7 0250      	macw %d7u,%a3l,>>,%sp@,%a3,%acc3
    4f8c:	a617 0240      	macw %d7u,%d3l,>>,%sp@,%d3,%acc1
    4f90:	a697 0250      	macw %d7u,%d3l,>>,%sp@,%d3,%acc3
    4f94:	ae57 0240      	macw %d7u,%spl,>>,%sp@,%sp,%acc1
    4f98:	aed7 0250      	macw %d7u,%spl,>>,%sp@,%sp,%acc3
    4f9c:	a617 0260      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc1
    4fa0:	a697 0270      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc3
    4fa4:	a657 0260      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc1
    4fa8:	a6d7 0270      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc3
    4fac:	a617 0260      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc1
    4fb0:	a697 0270      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc3
    4fb4:	ae57 0260      	macw %d7u,%spl,>>,%sp@&,%sp,%acc1
    4fb8:	aed7 0270      	macw %d7u,%spl,>>,%sp@&,%sp,%acc3
    4fbc:	a61e 0240      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc1
    4fc0:	a69e 0250      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc3
    4fc4:	a65e 0240      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc1
    4fc8:	a6de 0250      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc3
    4fcc:	a61e 0240      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc1
    4fd0:	a69e 0250      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc3
    4fd4:	ae5e 0240      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc1
    4fd8:	aede 0250      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc3
    4fdc:	a61e 0260      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc1
    4fe0:	a69e 0270      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc3
    4fe4:	a65e 0260      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc1
    4fe8:	a6de 0270      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc3
    4fec:	a61e 0260      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc1
    4ff0:	a69e 0270      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc3
    4ff4:	ae5e 0260      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc1
    4ff8:	aede 0270      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc3
    4ffc:	a62e 0240 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    5002:	a6ae 0250 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    5008:	a66e 0240 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    500e:	a6ee 0250 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    5014:	a62e 0240 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    501a:	a6ae 0250 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    5020:	ae6e 0240 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    5026:	aeee 0250 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    502c:	a62e 0260 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    5032:	a6ae 0270 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    5038:	a66e 0260 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    503e:	a6ee 0270 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    5044:	a62e 0260 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    504a:	a6ae 0270 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    5050:	ae6e 0260 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    5056:	aeee 0270 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    505c:	a627 0240      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc1
    5060:	a6a7 0250      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc3
    5064:	a667 0240      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc1
    5068:	a6e7 0250      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc3
    506c:	a627 0240      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc1
    5070:	a6a7 0250      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc3
    5074:	ae67 0240      	macw %d7u,%spl,>>,%sp@-,%sp,%acc1
    5078:	aee7 0250      	macw %d7u,%spl,>>,%sp@-,%sp,%acc3
    507c:	a627 0260      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc1
    5080:	a6a7 0270      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc3
    5084:	a667 0260      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc1
    5088:	a6e7 0270      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc3
    508c:	a627 0260      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc1
    5090:	a6a7 0270      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc3
    5094:	ae67 0260      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc1
    5098:	aee7 0270      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc3
    509c:	a617 0640      	macw %d7u,%d3l,>>,%sp@,%d3,%acc1
    50a0:	a697 0650      	macw %d7u,%d3l,>>,%sp@,%d3,%acc3
    50a4:	a657 0640      	macw %d7u,%a3l,>>,%sp@,%a3,%acc1
    50a8:	a6d7 0650      	macw %d7u,%a3l,>>,%sp@,%a3,%acc3
    50ac:	a617 0640      	macw %d7u,%d3l,>>,%sp@,%d3,%acc1
    50b0:	a697 0650      	macw %d7u,%d3l,>>,%sp@,%d3,%acc3
    50b4:	ae57 0640      	macw %d7u,%spl,>>,%sp@,%sp,%acc1
    50b8:	aed7 0650      	macw %d7u,%spl,>>,%sp@,%sp,%acc3
    50bc:	a617 0660      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc1
    50c0:	a697 0670      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc3
    50c4:	a657 0660      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc1
    50c8:	a6d7 0670      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc3
    50cc:	a617 0660      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc1
    50d0:	a697 0670      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc3
    50d4:	ae57 0660      	macw %d7u,%spl,>>,%sp@&,%sp,%acc1
    50d8:	aed7 0670      	macw %d7u,%spl,>>,%sp@&,%sp,%acc3
    50dc:	a61e 0640      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc1
    50e0:	a69e 0650      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc3
    50e4:	a65e 0640      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc1
    50e8:	a6de 0650      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc3
    50ec:	a61e 0640      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc1
    50f0:	a69e 0650      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc3
    50f4:	ae5e 0640      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc1
    50f8:	aede 0650      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc3
    50fc:	a61e 0660      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc1
    5100:	a69e 0670      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc3
    5104:	a65e 0660      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc1
    5108:	a6de 0670      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc3
    510c:	a61e 0660      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc1
    5110:	a69e 0670      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc3
    5114:	ae5e 0660      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc1
    5118:	aede 0670      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc3
    511c:	a62e 0640 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    5122:	a6ae 0650 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    5128:	a66e 0640 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    512e:	a6ee 0650 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    5134:	a62e 0640 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    513a:	a6ae 0650 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    5140:	ae6e 0640 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    5146:	aeee 0650 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    514c:	a62e 0660 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    5152:	a6ae 0670 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    5158:	a66e 0660 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    515e:	a6ee 0670 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    5164:	a62e 0660 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    516a:	a6ae 0670 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    5170:	ae6e 0660 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    5176:	aeee 0670 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    517c:	a627 0640      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc1
    5180:	a6a7 0650      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc3
    5184:	a667 0640      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc1
    5188:	a6e7 0650      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc3
    518c:	a627 0640      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc1
    5190:	a6a7 0650      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc3
    5194:	ae67 0640      	macw %d7u,%spl,>>,%sp@-,%sp,%acc1
    5198:	aee7 0650      	macw %d7u,%spl,>>,%sp@-,%sp,%acc3
    519c:	a627 0660      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc1
    51a0:	a6a7 0670      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc3
    51a4:	a667 0660      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc1
    51a8:	a6e7 0670      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc3
    51ac:	a627 0660      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc1
    51b0:	a6a7 0670      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc3
    51b4:	ae67 0660      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc1
    51b8:	aee7 0670      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc3
    51bc:	ae57 00c0      	macw %d7u,%spu,%sp@,%sp,%acc1
    51c0:	aed7 00d0      	macw %d7u,%spu,%sp@,%sp,%acc3
    51c4:	ae57 00c0      	macw %d7u,%spu,%sp@,%sp,%acc1
    51c8:	aed7 00d0      	macw %d7u,%spu,%sp@,%sp,%acc3
    51cc:	ae57 00c0      	macw %d7u,%spu,%sp@,%sp,%acc1
    51d0:	aed7 00d0      	macw %d7u,%spu,%sp@,%sp,%acc3
    51d4:	ae57 00c0      	macw %d7u,%spu,%sp@,%sp,%acc1
    51d8:	aed7 00d0      	macw %d7u,%spu,%sp@,%sp,%acc3
    51dc:	ae57 00e0      	macw %d7u,%spu,%sp@&,%sp,%acc1
    51e0:	aed7 00f0      	macw %d7u,%spu,%sp@&,%sp,%acc3
    51e4:	ae57 00e0      	macw %d7u,%spu,%sp@&,%sp,%acc1
    51e8:	aed7 00f0      	macw %d7u,%spu,%sp@&,%sp,%acc3
    51ec:	ae57 00e0      	macw %d7u,%spu,%sp@&,%sp,%acc1
    51f0:	aed7 00f0      	macw %d7u,%spu,%sp@&,%sp,%acc3
    51f4:	ae57 00e0      	macw %d7u,%spu,%sp@&,%sp,%acc1
    51f8:	aed7 00f0      	macw %d7u,%spu,%sp@&,%sp,%acc3
    51fc:	ae5e 00c0      	macw %fpu,%spu,%fp@\+,%sp,%acc1
    5200:	aede 00d0      	macw %fpu,%spu,%fp@\+,%sp,%acc3
    5204:	ae5e 00c0      	macw %fpu,%spu,%fp@\+,%sp,%acc1
    5208:	aede 00d0      	macw %fpu,%spu,%fp@\+,%sp,%acc3
    520c:	ae5e 00c0      	macw %fpu,%spu,%fp@\+,%sp,%acc1
    5210:	aede 00d0      	macw %fpu,%spu,%fp@\+,%sp,%acc3
    5214:	ae5e 00c0      	macw %fpu,%spu,%fp@\+,%sp,%acc1
    5218:	aede 00d0      	macw %fpu,%spu,%fp@\+,%sp,%acc3
    521c:	ae5e 00e0      	macw %fpu,%spu,%fp@\+&,%sp,%acc1
    5220:	aede 00f0      	macw %fpu,%spu,%fp@\+&,%sp,%acc3
    5224:	ae5e 00e0      	macw %fpu,%spu,%fp@\+&,%sp,%acc1
    5228:	aede 00f0      	macw %fpu,%spu,%fp@\+&,%sp,%acc3
    522c:	ae5e 00e0      	macw %fpu,%spu,%fp@\+&,%sp,%acc1
    5230:	aede 00f0      	macw %fpu,%spu,%fp@\+&,%sp,%acc3
    5234:	ae5e 00e0      	macw %fpu,%spu,%fp@\+&,%sp,%acc1
    5238:	aede 00f0      	macw %fpu,%spu,%fp@\+&,%sp,%acc3
    523c:	ae6e 00c0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc1
    5242:	aeee 00d0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc3
    5248:	ae6e 00c0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc1
    524e:	aeee 00d0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc3
    5254:	ae6e 00c0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc1
    525a:	aeee 00d0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc3
    5260:	ae6e 00c0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc1
    5266:	aeee 00d0 000a 	macw %fpu,%spu,%fp@\(10\),%sp,%acc3
    526c:	ae6e 00e0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc1
    5272:	aeee 00f0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc3
    5278:	ae6e 00e0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc1
    527e:	aeee 00f0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc3
    5284:	ae6e 00e0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc1
    528a:	aeee 00f0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc3
    5290:	ae6e 00e0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc1
    5296:	aeee 00f0 000a 	macw %fpu,%spu,%fp@\(10\)&,%sp,%acc3
    529c:	ae67 00c0      	macw %d7u,%spu,%sp@-,%sp,%acc1
    52a0:	aee7 00d0      	macw %d7u,%spu,%sp@-,%sp,%acc3
    52a4:	ae67 00c0      	macw %d7u,%spu,%sp@-,%sp,%acc1
    52a8:	aee7 00d0      	macw %d7u,%spu,%sp@-,%sp,%acc3
    52ac:	ae67 00c0      	macw %d7u,%spu,%sp@-,%sp,%acc1
    52b0:	aee7 00d0      	macw %d7u,%spu,%sp@-,%sp,%acc3
    52b4:	ae67 00c0      	macw %d7u,%spu,%sp@-,%sp,%acc1
    52b8:	aee7 00d0      	macw %d7u,%spu,%sp@-,%sp,%acc3
    52bc:	ae67 00e0      	macw %d7u,%spu,%sp@-&,%sp,%acc1
    52c0:	aee7 00f0      	macw %d7u,%spu,%sp@-&,%sp,%acc3
    52c4:	ae67 00e0      	macw %d7u,%spu,%sp@-&,%sp,%acc1
    52c8:	aee7 00f0      	macw %d7u,%spu,%sp@-&,%sp,%acc3
    52cc:	ae67 00e0      	macw %d7u,%spu,%sp@-&,%sp,%acc1
    52d0:	aee7 00f0      	macw %d7u,%spu,%sp@-&,%sp,%acc3
    52d4:	ae67 00e0      	macw %d7u,%spu,%sp@-&,%sp,%acc1
    52d8:	aee7 00f0      	macw %d7u,%spu,%sp@-&,%sp,%acc3
    52dc:	ae57 02c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    52e0:	aed7 02d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    52e4:	ae57 02c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    52e8:	aed7 02d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    52ec:	ae57 02c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    52f0:	aed7 02d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    52f4:	ae57 02c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    52f8:	aed7 02d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    52fc:	ae57 02e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5300:	aed7 02f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    5304:	ae57 02e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5308:	aed7 02f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    530c:	ae57 02e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5310:	aed7 02f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    5314:	ae57 02e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5318:	aed7 02f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    531c:	ae5e 02c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5320:	aede 02d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    5324:	ae5e 02c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5328:	aede 02d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    532c:	ae5e 02c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5330:	aede 02d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    5334:	ae5e 02c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5338:	aede 02d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    533c:	ae5e 02e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    5340:	aede 02f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    5344:	ae5e 02e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    5348:	aede 02f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    534c:	ae5e 02e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    5350:	aede 02f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    5354:	ae5e 02e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    5358:	aede 02f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    535c:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    5362:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    5368:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    536e:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    5374:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    537a:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    5380:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    5386:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    538c:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    5392:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    5398:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    539e:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    53a4:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    53aa:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    53b0:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    53b6:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    53bc:	ae67 02c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    53c0:	aee7 02d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    53c4:	ae67 02c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    53c8:	aee7 02d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    53cc:	ae67 02c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    53d0:	aee7 02d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    53d4:	ae67 02c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    53d8:	aee7 02d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    53dc:	ae67 02e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    53e0:	aee7 02f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    53e4:	ae67 02e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    53e8:	aee7 02f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    53ec:	ae67 02e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    53f0:	aee7 02f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    53f4:	ae67 02e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    53f8:	aee7 02f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    53fc:	ae57 06c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    5400:	aed7 06d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    5404:	ae57 06c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    5408:	aed7 06d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    540c:	ae57 06c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    5410:	aed7 06d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    5414:	ae57 06c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    5418:	aed7 06d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    541c:	ae57 06e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5420:	aed7 06f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    5424:	ae57 06e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5428:	aed7 06f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    542c:	ae57 06e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5430:	aed7 06f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    5434:	ae57 06e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5438:	aed7 06f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    543c:	ae5e 06c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5440:	aede 06d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    5444:	ae5e 06c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5448:	aede 06d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    544c:	ae5e 06c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5450:	aede 06d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    5454:	ae5e 06c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5458:	aede 06d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    545c:	ae5e 06e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    5460:	aede 06f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    5464:	ae5e 06e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    5468:	aede 06f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    546c:	ae5e 06e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    5470:	aede 06f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    5474:	ae5e 06e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    5478:	aede 06f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    547c:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    5482:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    5488:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    548e:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    5494:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    549a:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    54a0:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    54a6:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    54ac:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    54b2:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    54b8:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    54be:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    54c4:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    54ca:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    54d0:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    54d6:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    54dc:	ae67 06c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    54e0:	aee7 06d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    54e4:	ae67 06c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    54e8:	aee7 06d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    54ec:	ae67 06c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    54f0:	aee7 06d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    54f4:	ae67 06c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    54f8:	aee7 06d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    54fc:	ae67 06e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    5500:	aee7 06f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    5504:	ae67 06e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    5508:	aee7 06f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    550c:	ae67 06e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    5510:	aee7 06f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    5514:	ae67 06e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    5518:	aee7 06f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    551c:	ae57 02c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    5520:	aed7 02d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    5524:	ae57 02c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    5528:	aed7 02d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    552c:	ae57 02c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    5530:	aed7 02d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    5534:	ae57 02c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    5538:	aed7 02d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    553c:	ae57 02e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5540:	aed7 02f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    5544:	ae57 02e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5548:	aed7 02f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    554c:	ae57 02e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5550:	aed7 02f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    5554:	ae57 02e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5558:	aed7 02f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    555c:	ae5e 02c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5560:	aede 02d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    5564:	ae5e 02c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5568:	aede 02d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    556c:	ae5e 02c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5570:	aede 02d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    5574:	ae5e 02c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5578:	aede 02d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    557c:	ae5e 02e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    5580:	aede 02f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    5584:	ae5e 02e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    5588:	aede 02f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    558c:	ae5e 02e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    5590:	aede 02f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    5594:	ae5e 02e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    5598:	aede 02f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    559c:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    55a2:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    55a8:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    55ae:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    55b4:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    55ba:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    55c0:	ae6e 02c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    55c6:	aeee 02d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    55cc:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    55d2:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    55d8:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    55de:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    55e4:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    55ea:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    55f0:	ae6e 02e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    55f6:	aeee 02f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    55fc:	ae67 02c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    5600:	aee7 02d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    5604:	ae67 02c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    5608:	aee7 02d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    560c:	ae67 02c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    5610:	aee7 02d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    5614:	ae67 02c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    5618:	aee7 02d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    561c:	ae67 02e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    5620:	aee7 02f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    5624:	ae67 02e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    5628:	aee7 02f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    562c:	ae67 02e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    5630:	aee7 02f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    5634:	ae67 02e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    5638:	aee7 02f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    563c:	ae57 06c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    5640:	aed7 06d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    5644:	ae57 06c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    5648:	aed7 06d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    564c:	ae57 06c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    5650:	aed7 06d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    5654:	ae57 06c0      	macw %d7u,%spu,>>,%sp@,%sp,%acc1
    5658:	aed7 06d0      	macw %d7u,%spu,>>,%sp@,%sp,%acc3
    565c:	ae57 06e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5660:	aed7 06f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    5664:	ae57 06e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5668:	aed7 06f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    566c:	ae57 06e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5670:	aed7 06f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    5674:	ae57 06e0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc1
    5678:	aed7 06f0      	macw %d7u,%spu,>>,%sp@&,%sp,%acc3
    567c:	ae5e 06c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5680:	aede 06d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    5684:	ae5e 06c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5688:	aede 06d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    568c:	ae5e 06c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5690:	aede 06d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    5694:	ae5e 06c0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc1
    5698:	aede 06d0      	macw %fpu,%spu,>>,%fp@\+,%sp,%acc3
    569c:	ae5e 06e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    56a0:	aede 06f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    56a4:	ae5e 06e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    56a8:	aede 06f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    56ac:	ae5e 06e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    56b0:	aede 06f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    56b4:	ae5e 06e0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc1
    56b8:	aede 06f0      	macw %fpu,%spu,>>,%fp@\+&,%sp,%acc3
    56bc:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    56c2:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    56c8:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    56ce:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    56d4:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    56da:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    56e0:	ae6e 06c0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc1
    56e6:	aeee 06d0 000a 	macw %fpu,%spu,>>,%fp@\(10\),%sp,%acc3
    56ec:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    56f2:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    56f8:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    56fe:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    5704:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    570a:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    5710:	ae6e 06e0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc1
    5716:	aeee 06f0 000a 	macw %fpu,%spu,>>,%fp@\(10\)&,%sp,%acc3
    571c:	ae67 06c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    5720:	aee7 06d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    5724:	ae67 06c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    5728:	aee7 06d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    572c:	ae67 06c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    5730:	aee7 06d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    5734:	ae67 06c0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc1
    5738:	aee7 06d0      	macw %d7u,%spu,>>,%sp@-,%sp,%acc3
    573c:	ae67 06e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    5740:	aee7 06f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    5744:	ae67 06e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    5748:	aee7 06f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    574c:	ae67 06e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    5750:	aee7 06f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    5754:	ae67 06e0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc1
    5758:	aee7 06f0      	macw %d7u,%spu,>>,%sp@-&,%sp,%acc3
    575c:	a217 0040      	macw %d7u,%d1l,%sp@,%d1,%acc1
    5760:	a297 0050      	macw %d7u,%d1l,%sp@,%d1,%acc3
    5764:	a657 0040      	macw %d7u,%a3l,%sp@,%a3,%acc1
    5768:	a6d7 0050      	macw %d7u,%a3l,%sp@,%a3,%acc3
    576c:	a617 0040      	macw %d7u,%d3l,%sp@,%d3,%acc1
    5770:	a697 0050      	macw %d7u,%d3l,%sp@,%d3,%acc3
    5774:	ae57 0040      	macw %d7u,%spl,%sp@,%sp,%acc1
    5778:	aed7 0050      	macw %d7u,%spl,%sp@,%sp,%acc3
    577c:	a217 0060      	macw %d7u,%d1l,%sp@&,%d1,%acc1
    5780:	a297 0070      	macw %d7u,%d1l,%sp@&,%d1,%acc3
    5784:	a657 0060      	macw %d7u,%a3l,%sp@&,%a3,%acc1
    5788:	a6d7 0070      	macw %d7u,%a3l,%sp@&,%a3,%acc3
    578c:	a617 0060      	macw %d7u,%d3l,%sp@&,%d3,%acc1
    5790:	a697 0070      	macw %d7u,%d3l,%sp@&,%d3,%acc3
    5794:	ae57 0060      	macw %d7u,%spl,%sp@&,%sp,%acc1
    5798:	aed7 0070      	macw %d7u,%spl,%sp@&,%sp,%acc3
    579c:	a21e 0040      	macw %fpu,%d1l,%fp@\+,%d1,%acc1
    57a0:	a29e 0050      	macw %fpu,%d1l,%fp@\+,%d1,%acc3
    57a4:	a65e 0040      	macw %fpu,%a3l,%fp@\+,%a3,%acc1
    57a8:	a6de 0050      	macw %fpu,%a3l,%fp@\+,%a3,%acc3
    57ac:	a61e 0040      	macw %fpu,%d3l,%fp@\+,%d3,%acc1
    57b0:	a69e 0050      	macw %fpu,%d3l,%fp@\+,%d3,%acc3
    57b4:	ae5e 0040      	macw %fpu,%spl,%fp@\+,%sp,%acc1
    57b8:	aede 0050      	macw %fpu,%spl,%fp@\+,%sp,%acc3
    57bc:	a21e 0060      	macw %fpu,%d1l,%fp@\+&,%d1,%acc1
    57c0:	a29e 0070      	macw %fpu,%d1l,%fp@\+&,%d1,%acc3
    57c4:	a65e 0060      	macw %fpu,%a3l,%fp@\+&,%a3,%acc1
    57c8:	a6de 0070      	macw %fpu,%a3l,%fp@\+&,%a3,%acc3
    57cc:	a61e 0060      	macw %fpu,%d3l,%fp@\+&,%d3,%acc1
    57d0:	a69e 0070      	macw %fpu,%d3l,%fp@\+&,%d3,%acc3
    57d4:	ae5e 0060      	macw %fpu,%spl,%fp@\+&,%sp,%acc1
    57d8:	aede 0070      	macw %fpu,%spl,%fp@\+&,%sp,%acc3
    57dc:	a22e 0040 000a 	macw %fpu,%d1l,%fp@\(10\),%d1,%acc1
    57e2:	a2ae 0050 000a 	macw %fpu,%d1l,%fp@\(10\),%d1,%acc3
    57e8:	a66e 0040 000a 	macw %fpu,%a3l,%fp@\(10\),%a3,%acc1
    57ee:	a6ee 0050 000a 	macw %fpu,%a3l,%fp@\(10\),%a3,%acc3
    57f4:	a62e 0040 000a 	macw %fpu,%d3l,%fp@\(10\),%d3,%acc1
    57fa:	a6ae 0050 000a 	macw %fpu,%d3l,%fp@\(10\),%d3,%acc3
    5800:	ae6e 0040 000a 	macw %fpu,%spl,%fp@\(10\),%sp,%acc1
    5806:	aeee 0050 000a 	macw %fpu,%spl,%fp@\(10\),%sp,%acc3
    580c:	a22e 0060 000a 	macw %fpu,%d1l,%fp@\(10\)&,%d1,%acc1
    5812:	a2ae 0070 000a 	macw %fpu,%d1l,%fp@\(10\)&,%d1,%acc3
    5818:	a66e 0060 000a 	macw %fpu,%a3l,%fp@\(10\)&,%a3,%acc1
    581e:	a6ee 0070 000a 	macw %fpu,%a3l,%fp@\(10\)&,%a3,%acc3
    5824:	a62e 0060 000a 	macw %fpu,%d3l,%fp@\(10\)&,%d3,%acc1
    582a:	a6ae 0070 000a 	macw %fpu,%d3l,%fp@\(10\)&,%d3,%acc3
    5830:	ae6e 0060 000a 	macw %fpu,%spl,%fp@\(10\)&,%sp,%acc1
    5836:	aeee 0070 000a 	macw %fpu,%spl,%fp@\(10\)&,%sp,%acc3
    583c:	a227 0040      	macw %d7u,%d1l,%sp@-,%d1,%acc1
    5840:	a2a7 0050      	macw %d7u,%d1l,%sp@-,%d1,%acc3
    5844:	a667 0040      	macw %d7u,%a3l,%sp@-,%a3,%acc1
    5848:	a6e7 0050      	macw %d7u,%a3l,%sp@-,%a3,%acc3
    584c:	a627 0040      	macw %d7u,%d3l,%sp@-,%d3,%acc1
    5850:	a6a7 0050      	macw %d7u,%d3l,%sp@-,%d3,%acc3
    5854:	ae67 0040      	macw %d7u,%spl,%sp@-,%sp,%acc1
    5858:	aee7 0050      	macw %d7u,%spl,%sp@-,%sp,%acc3
    585c:	a227 0060      	macw %d7u,%d1l,%sp@-&,%d1,%acc1
    5860:	a2a7 0070      	macw %d7u,%d1l,%sp@-&,%d1,%acc3
    5864:	a667 0060      	macw %d7u,%a3l,%sp@-&,%a3,%acc1
    5868:	a6e7 0070      	macw %d7u,%a3l,%sp@-&,%a3,%acc3
    586c:	a627 0060      	macw %d7u,%d3l,%sp@-&,%d3,%acc1
    5870:	a6a7 0070      	macw %d7u,%d3l,%sp@-&,%d3,%acc3
    5874:	ae67 0060      	macw %d7u,%spl,%sp@-&,%sp,%acc1
    5878:	aee7 0070      	macw %d7u,%spl,%sp@-&,%sp,%acc3
    587c:	a217 0240      	macw %d7u,%d1l,<<,%sp@,%d1,%acc1
    5880:	a297 0250      	macw %d7u,%d1l,<<,%sp@,%d1,%acc3
    5884:	a657 0240      	macw %d7u,%a3l,>>,%sp@,%a3,%acc1
    5888:	a6d7 0250      	macw %d7u,%a3l,>>,%sp@,%a3,%acc3
    588c:	a617 0240      	macw %d7u,%d3l,>>,%sp@,%d3,%acc1
    5890:	a697 0250      	macw %d7u,%d3l,>>,%sp@,%d3,%acc3
    5894:	ae57 0240      	macw %d7u,%spl,>>,%sp@,%sp,%acc1
    5898:	aed7 0250      	macw %d7u,%spl,>>,%sp@,%sp,%acc3
    589c:	a217 0260      	macw %d7u,%d1l,<<,%sp@&,%d1,%acc1
    58a0:	a297 0270      	macw %d7u,%d1l,<<,%sp@&,%d1,%acc3
    58a4:	a657 0260      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc1
    58a8:	a6d7 0270      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc3
    58ac:	a617 0260      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc1
    58b0:	a697 0270      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc3
    58b4:	ae57 0260      	macw %d7u,%spl,>>,%sp@&,%sp,%acc1
    58b8:	aed7 0270      	macw %d7u,%spl,>>,%sp@&,%sp,%acc3
    58bc:	a21e 0240      	macw %fpu,%d1l,<<,%fp@\+,%d1,%acc1
    58c0:	a29e 0250      	macw %fpu,%d1l,<<,%fp@\+,%d1,%acc3
    58c4:	a65e 0240      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc1
    58c8:	a6de 0250      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc3
    58cc:	a61e 0240      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc1
    58d0:	a69e 0250      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc3
    58d4:	ae5e 0240      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc1
    58d8:	aede 0250      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc3
    58dc:	a21e 0260      	macw %fpu,%d1l,<<,%fp@\+&,%d1,%acc1
    58e0:	a29e 0270      	macw %fpu,%d1l,<<,%fp@\+&,%d1,%acc3
    58e4:	a65e 0260      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc1
    58e8:	a6de 0270      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc3
    58ec:	a61e 0260      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc1
    58f0:	a69e 0270      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc3
    58f4:	ae5e 0260      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc1
    58f8:	aede 0270      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc3
    58fc:	a22e 0240 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc1
    5902:	a2ae 0250 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc3
    5908:	a66e 0240 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    590e:	a6ee 0250 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    5914:	a62e 0240 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    591a:	a6ae 0250 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    5920:	ae6e 0240 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    5926:	aeee 0250 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    592c:	a22e 0260 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc1
    5932:	a2ae 0270 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc3
    5938:	a66e 0260 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    593e:	a6ee 0270 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    5944:	a62e 0260 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    594a:	a6ae 0270 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    5950:	ae6e 0260 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    5956:	aeee 0270 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    595c:	a227 0240      	macw %d7u,%d1l,<<,%sp@-,%d1,%acc1
    5960:	a2a7 0250      	macw %d7u,%d1l,<<,%sp@-,%d1,%acc3
    5964:	a667 0240      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc1
    5968:	a6e7 0250      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc3
    596c:	a627 0240      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc1
    5970:	a6a7 0250      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc3
    5974:	ae67 0240      	macw %d7u,%spl,>>,%sp@-,%sp,%acc1
    5978:	aee7 0250      	macw %d7u,%spl,>>,%sp@-,%sp,%acc3
    597c:	a227 0260      	macw %d7u,%d1l,<<,%sp@-&,%d1,%acc1
    5980:	a2a7 0270      	macw %d7u,%d1l,<<,%sp@-&,%d1,%acc3
    5984:	a667 0260      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc1
    5988:	a6e7 0270      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc3
    598c:	a627 0260      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc1
    5990:	a6a7 0270      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc3
    5994:	ae67 0260      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc1
    5998:	aee7 0270      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc3
    599c:	a217 0640      	macw %d7u,%d1l,<<,%sp@,%d1,%acc1
    59a0:	a297 0650      	macw %d7u,%d1l,<<,%sp@,%d1,%acc3
    59a4:	a657 0640      	macw %d7u,%a3l,>>,%sp@,%a3,%acc1
    59a8:	a6d7 0650      	macw %d7u,%a3l,>>,%sp@,%a3,%acc3
    59ac:	a617 0640      	macw %d7u,%d3l,>>,%sp@,%d3,%acc1
    59b0:	a697 0650      	macw %d7u,%d3l,>>,%sp@,%d3,%acc3
    59b4:	ae57 0640      	macw %d7u,%spl,>>,%sp@,%sp,%acc1
    59b8:	aed7 0650      	macw %d7u,%spl,>>,%sp@,%sp,%acc3
    59bc:	a217 0660      	macw %d7u,%d1l,<<,%sp@&,%d1,%acc1
    59c0:	a297 0670      	macw %d7u,%d1l,<<,%sp@&,%d1,%acc3
    59c4:	a657 0660      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc1
    59c8:	a6d7 0670      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc3
    59cc:	a617 0660      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc1
    59d0:	a697 0670      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc3
    59d4:	ae57 0660      	macw %d7u,%spl,>>,%sp@&,%sp,%acc1
    59d8:	aed7 0670      	macw %d7u,%spl,>>,%sp@&,%sp,%acc3
    59dc:	a21e 0640      	macw %fpu,%d1l,<<,%fp@\+,%d1,%acc1
    59e0:	a29e 0650      	macw %fpu,%d1l,<<,%fp@\+,%d1,%acc3
    59e4:	a65e 0640      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc1
    59e8:	a6de 0650      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc3
    59ec:	a61e 0640      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc1
    59f0:	a69e 0650      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc3
    59f4:	ae5e 0640      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc1
    59f8:	aede 0650      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc3
    59fc:	a21e 0660      	macw %fpu,%d1l,<<,%fp@\+&,%d1,%acc1
    5a00:	a29e 0670      	macw %fpu,%d1l,<<,%fp@\+&,%d1,%acc3
    5a04:	a65e 0660      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc1
    5a08:	a6de 0670      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc3
    5a0c:	a61e 0660      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc1
    5a10:	a69e 0670      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc3
    5a14:	ae5e 0660      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc1
    5a18:	aede 0670      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc3
    5a1c:	a22e 0640 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc1
    5a22:	a2ae 0650 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc3
    5a28:	a66e 0640 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    5a2e:	a6ee 0650 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    5a34:	a62e 0640 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    5a3a:	a6ae 0650 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    5a40:	ae6e 0640 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    5a46:	aeee 0650 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    5a4c:	a22e 0660 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc1
    5a52:	a2ae 0670 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc3
    5a58:	a66e 0660 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    5a5e:	a6ee 0670 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    5a64:	a62e 0660 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    5a6a:	a6ae 0670 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    5a70:	ae6e 0660 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    5a76:	aeee 0670 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    5a7c:	a227 0640      	macw %d7u,%d1l,<<,%sp@-,%d1,%acc1
    5a80:	a2a7 0650      	macw %d7u,%d1l,<<,%sp@-,%d1,%acc3
    5a84:	a667 0640      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc1
    5a88:	a6e7 0650      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc3
    5a8c:	a627 0640      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc1
    5a90:	a6a7 0650      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc3
    5a94:	ae67 0640      	macw %d7u,%spl,>>,%sp@-,%sp,%acc1
    5a98:	aee7 0650      	macw %d7u,%spl,>>,%sp@-,%sp,%acc3
    5a9c:	a227 0660      	macw %d7u,%d1l,<<,%sp@-&,%d1,%acc1
    5aa0:	a2a7 0670      	macw %d7u,%d1l,<<,%sp@-&,%d1,%acc3
    5aa4:	a667 0660      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc1
    5aa8:	a6e7 0670      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc3
    5aac:	a627 0660      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc1
    5ab0:	a6a7 0670      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc3
    5ab4:	ae67 0660      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc1
    5ab8:	aee7 0670      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc3
    5abc:	a217 0240      	macw %d7u,%d1l,<<,%sp@,%d1,%acc1
    5ac0:	a297 0250      	macw %d7u,%d1l,<<,%sp@,%d1,%acc3
    5ac4:	a657 0240      	macw %d7u,%a3l,>>,%sp@,%a3,%acc1
    5ac8:	a6d7 0250      	macw %d7u,%a3l,>>,%sp@,%a3,%acc3
    5acc:	a617 0240      	macw %d7u,%d3l,>>,%sp@,%d3,%acc1
    5ad0:	a697 0250      	macw %d7u,%d3l,>>,%sp@,%d3,%acc3
    5ad4:	ae57 0240      	macw %d7u,%spl,>>,%sp@,%sp,%acc1
    5ad8:	aed7 0250      	macw %d7u,%spl,>>,%sp@,%sp,%acc3
    5adc:	a217 0260      	macw %d7u,%d1l,<<,%sp@&,%d1,%acc1
    5ae0:	a297 0270      	macw %d7u,%d1l,<<,%sp@&,%d1,%acc3
    5ae4:	a657 0260      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc1
    5ae8:	a6d7 0270      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc3
    5aec:	a617 0260      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc1
    5af0:	a697 0270      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc3
    5af4:	ae57 0260      	macw %d7u,%spl,>>,%sp@&,%sp,%acc1
    5af8:	aed7 0270      	macw %d7u,%spl,>>,%sp@&,%sp,%acc3
    5afc:	a21e 0240      	macw %fpu,%d1l,<<,%fp@\+,%d1,%acc1
    5b00:	a29e 0250      	macw %fpu,%d1l,<<,%fp@\+,%d1,%acc3
    5b04:	a65e 0240      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc1
    5b08:	a6de 0250      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc3
    5b0c:	a61e 0240      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc1
    5b10:	a69e 0250      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc3
    5b14:	ae5e 0240      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc1
    5b18:	aede 0250      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc3
    5b1c:	a21e 0260      	macw %fpu,%d1l,<<,%fp@\+&,%d1,%acc1
    5b20:	a29e 0270      	macw %fpu,%d1l,<<,%fp@\+&,%d1,%acc3
    5b24:	a65e 0260      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc1
    5b28:	a6de 0270      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc3
    5b2c:	a61e 0260      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc1
    5b30:	a69e 0270      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc3
    5b34:	ae5e 0260      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc1
    5b38:	aede 0270      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc3
    5b3c:	a22e 0240 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc1
    5b42:	a2ae 0250 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc3
    5b48:	a66e 0240 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    5b4e:	a6ee 0250 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    5b54:	a62e 0240 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    5b5a:	a6ae 0250 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    5b60:	ae6e 0240 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    5b66:	aeee 0250 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    5b6c:	a22e 0260 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc1
    5b72:	a2ae 0270 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc3
    5b78:	a66e 0260 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    5b7e:	a6ee 0270 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    5b84:	a62e 0260 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    5b8a:	a6ae 0270 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    5b90:	ae6e 0260 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    5b96:	aeee 0270 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    5b9c:	a227 0240      	macw %d7u,%d1l,<<,%sp@-,%d1,%acc1
    5ba0:	a2a7 0250      	macw %d7u,%d1l,<<,%sp@-,%d1,%acc3
    5ba4:	a667 0240      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc1
    5ba8:	a6e7 0250      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc3
    5bac:	a627 0240      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc1
    5bb0:	a6a7 0250      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc3
    5bb4:	ae67 0240      	macw %d7u,%spl,>>,%sp@-,%sp,%acc1
    5bb8:	aee7 0250      	macw %d7u,%spl,>>,%sp@-,%sp,%acc3
    5bbc:	a227 0260      	macw %d7u,%d1l,<<,%sp@-&,%d1,%acc1
    5bc0:	a2a7 0270      	macw %d7u,%d1l,<<,%sp@-&,%d1,%acc3
    5bc4:	a667 0260      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc1
    5bc8:	a6e7 0270      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc3
    5bcc:	a627 0260      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc1
    5bd0:	a6a7 0270      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc3
    5bd4:	ae67 0260      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc1
    5bd8:	aee7 0270      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc3
    5bdc:	a217 0640      	macw %d7u,%d1l,<<,%sp@,%d1,%acc1
    5be0:	a297 0650      	macw %d7u,%d1l,<<,%sp@,%d1,%acc3
    5be4:	a657 0640      	macw %d7u,%a3l,>>,%sp@,%a3,%acc1
    5be8:	a6d7 0650      	macw %d7u,%a3l,>>,%sp@,%a3,%acc3
    5bec:	a617 0640      	macw %d7u,%d3l,>>,%sp@,%d3,%acc1
    5bf0:	a697 0650      	macw %d7u,%d3l,>>,%sp@,%d3,%acc3
    5bf4:	ae57 0640      	macw %d7u,%spl,>>,%sp@,%sp,%acc1
    5bf8:	aed7 0650      	macw %d7u,%spl,>>,%sp@,%sp,%acc3
    5bfc:	a217 0660      	macw %d7u,%d1l,<<,%sp@&,%d1,%acc1
    5c00:	a297 0670      	macw %d7u,%d1l,<<,%sp@&,%d1,%acc3
    5c04:	a657 0660      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc1
    5c08:	a6d7 0670      	macw %d7u,%a3l,>>,%sp@&,%a3,%acc3
    5c0c:	a617 0660      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc1
    5c10:	a697 0670      	macw %d7u,%d3l,>>,%sp@&,%d3,%acc3
    5c14:	ae57 0660      	macw %d7u,%spl,>>,%sp@&,%sp,%acc1
    5c18:	aed7 0670      	macw %d7u,%spl,>>,%sp@&,%sp,%acc3
    5c1c:	a21e 0640      	macw %fpu,%d1l,<<,%fp@\+,%d1,%acc1
    5c20:	a29e 0650      	macw %fpu,%d1l,<<,%fp@\+,%d1,%acc3
    5c24:	a65e 0640      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc1
    5c28:	a6de 0650      	macw %fpu,%a3l,>>,%fp@\+,%a3,%acc3
    5c2c:	a61e 0640      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc1
    5c30:	a69e 0650      	macw %fpu,%d3l,>>,%fp@\+,%d3,%acc3
    5c34:	ae5e 0640      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc1
    5c38:	aede 0650      	macw %fpu,%spl,>>,%fp@\+,%sp,%acc3
    5c3c:	a21e 0660      	macw %fpu,%d1l,<<,%fp@\+&,%d1,%acc1
    5c40:	a29e 0670      	macw %fpu,%d1l,<<,%fp@\+&,%d1,%acc3
    5c44:	a65e 0660      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc1
    5c48:	a6de 0670      	macw %fpu,%a3l,>>,%fp@\+&,%a3,%acc3
    5c4c:	a61e 0660      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc1
    5c50:	a69e 0670      	macw %fpu,%d3l,>>,%fp@\+&,%d3,%acc3
    5c54:	ae5e 0660      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc1
    5c58:	aede 0670      	macw %fpu,%spl,>>,%fp@\+&,%sp,%acc3
    5c5c:	a22e 0640 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc1
    5c62:	a2ae 0650 000a 	macw %fpu,%d1l,<<,%fp@\(10\),%d1,%acc3
    5c68:	a66e 0640 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc1
    5c6e:	a6ee 0650 000a 	macw %fpu,%a3l,>>,%fp@\(10\),%a3,%acc3
    5c74:	a62e 0640 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc1
    5c7a:	a6ae 0650 000a 	macw %fpu,%d3l,>>,%fp@\(10\),%d3,%acc3
    5c80:	ae6e 0640 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc1
    5c86:	aeee 0650 000a 	macw %fpu,%spl,>>,%fp@\(10\),%sp,%acc3
    5c8c:	a22e 0660 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc1
    5c92:	a2ae 0670 000a 	macw %fpu,%d1l,<<,%fp@\(10\)&,%d1,%acc3
    5c98:	a66e 0660 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc1
    5c9e:	a6ee 0670 000a 	macw %fpu,%a3l,>>,%fp@\(10\)&,%a3,%acc3
    5ca4:	a62e 0660 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc1
    5caa:	a6ae 0670 000a 	macw %fpu,%d3l,>>,%fp@\(10\)&,%d3,%acc3
    5cb0:	ae6e 0660 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc1
    5cb6:	aeee 0670 000a 	macw %fpu,%spl,>>,%fp@\(10\)&,%sp,%acc3
    5cbc:	a227 0640      	macw %d7u,%d1l,<<,%sp@-,%d1,%acc1
    5cc0:	a2a7 0650      	macw %d7u,%d1l,<<,%sp@-,%d1,%acc3
    5cc4:	a667 0640      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc1
    5cc8:	a6e7 0650      	macw %d7u,%a3l,>>,%sp@-,%a3,%acc3
    5ccc:	a627 0640      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc1
    5cd0:	a6a7 0650      	macw %d7u,%d3l,>>,%sp@-,%d3,%acc3
    5cd4:	ae67 0640      	macw %d7u,%spl,>>,%sp@-,%sp,%acc1
    5cd8:	aee7 0650      	macw %d7u,%spl,>>,%sp@-,%sp,%acc3
    5cdc:	a227 0660      	macw %d7u,%d1l,<<,%sp@-&,%d1,%acc1
    5ce0:	a2a7 0670      	macw %d7u,%d1l,<<,%sp@-&,%d1,%acc3
    5ce4:	a667 0660      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc1
    5ce8:	a6e7 0670      	macw %d7u,%a3l,>>,%sp@-&,%a3,%acc3
    5cec:	a627 0660      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc1
    5cf0:	a6a7 0670      	macw %d7u,%d3l,>>,%sp@-&,%d3,%acc3
    5cf4:	ae67 0660      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc1
    5cf8:	aee7 0670      	macw %d7u,%spl,>>,%sp@-&,%sp,%acc3
    5cfc:	a6c9 0800      	macl %a1,%a3,%acc0
    5d00:	a649 0810      	macl %a1,%a3,%acc2
    5d04:	a6c9 0a00      	macl %a1,%a3,>>,%acc0
    5d08:	a649 0a10      	macl %a1,%a3,>>,%acc2
    5d0c:	a6c9 0e00      	macl %a1,%a3,>>,%acc0
    5d10:	a649 0e10      	macl %a1,%a3,>>,%acc2
    5d14:	a6c9 0a00      	macl %a1,%a3,>>,%acc0
    5d18:	a649 0a10      	macl %a1,%a3,>>,%acc2
    5d1c:	a6c9 0e00      	macl %a1,%a3,>>,%acc0
    5d20:	a649 0e10      	macl %a1,%a3,>>,%acc2
    5d24:	a889 0800      	macl %a1,%d4,%acc0
    5d28:	a809 0810      	macl %a1,%d4,%acc2
    5d2c:	a889 0a00      	macl %a1,%d4,,%acc0
    5d30:	a809 0a10      	macl %a1,%d4,,%acc2
    5d34:	a889 0e00      	macl %a1,%d4,,%acc0
    5d38:	a809 0e10      	macl %a1,%d4,,%acc2
    5d3c:	a889 0a00      	macl %a1,%d4,,%acc0
    5d40:	a809 0a10      	macl %a1,%d4,,%acc2
    5d44:	a889 0e00      	macl %a1,%d4,,%acc0
    5d48:	a809 0e10      	macl %a1,%d4,,%acc2
    5d4c:	a6c6 0800      	macl %d6,%a3,%acc0
    5d50:	a646 0810      	macl %d6,%a3,%acc2
    5d54:	a6c6 0a00      	macl %d6,%a3,>>,%acc0
    5d58:	a646 0a10      	macl %d6,%a3,>>,%acc2
    5d5c:	a6c6 0e00      	macl %d6,%a3,>>,%acc0
    5d60:	a646 0e10      	macl %d6,%a3,>>,%acc2
    5d64:	a6c6 0a00      	macl %d6,%a3,>>,%acc0
    5d68:	a646 0a10      	macl %d6,%a3,>>,%acc2
    5d6c:	a6c6 0e00      	macl %d6,%a3,>>,%acc0
    5d70:	a646 0e10      	macl %d6,%a3,>>,%acc2
    5d74:	a886 0800      	macl %d6,%d4,%acc0
    5d78:	a806 0810      	macl %d6,%d4,%acc2
    5d7c:	a886 0a00      	macl %d6,%d4,,%acc0
    5d80:	a806 0a10      	macl %d6,%d4,,%acc2
    5d84:	a886 0e00      	macl %d6,%d4,,%acc0
    5d88:	a806 0e10      	macl %d6,%d4,,%acc2
    5d8c:	a886 0a00      	macl %d6,%d4,,%acc0
    5d90:	a806 0a10      	macl %d6,%d4,,%acc2
    5d94:	a886 0e00      	macl %d6,%d4,,%acc0
    5d98:	a806 0e10      	macl %d6,%d4,,%acc2
    5d9c:	a213 b809      	macl %a1,%a3,%a3@,%d1,%acc0
    5da0:	a293 b819      	macl %a1,%a3,%a3@,%d1,%acc2
    5da4:	a653 b809      	macl %a1,%a3,%a3@,%a3,%acc0
    5da8:	a6d3 b819      	macl %a1,%a3,%a3@,%a3,%acc2
    5dac:	a413 b809      	macl %a1,%a3,%a3@,%d2,%acc0
    5db0:	a493 b819      	macl %a1,%a3,%a3@,%d2,%acc2
    5db4:	ae53 b809      	macl %a1,%a3,%a3@,%sp,%acc0
    5db8:	aed3 b819      	macl %a1,%a3,%a3@,%sp,%acc2
    5dbc:	a213 b829      	macl %a1,%a3,%a3@&,%d1,%acc0
    5dc0:	a293 b839      	macl %a1,%a3,%a3@&,%d1,%acc2
    5dc4:	a653 b829      	macl %a1,%a3,%a3@&,%a3,%acc0
    5dc8:	a6d3 b839      	macl %a1,%a3,%a3@&,%a3,%acc2
    5dcc:	a413 b829      	macl %a1,%a3,%a3@&,%d2,%acc0
    5dd0:	a493 b839      	macl %a1,%a3,%a3@&,%d2,%acc2
    5dd4:	ae53 b829      	macl %a1,%a3,%a3@&,%sp,%acc0
    5dd8:	aed3 b839      	macl %a1,%a3,%a3@&,%sp,%acc2
    5ddc:	a21a b809      	macl %a1,%a3,%a2@\+,%d1,%acc0
    5de0:	a29a b819      	macl %a1,%a3,%a2@\+,%d1,%acc2
    5de4:	a65a b809      	macl %a1,%a3,%a2@\+,%a3,%acc0
    5de8:	a6da b819      	macl %a1,%a3,%a2@\+,%a3,%acc2
    5dec:	a41a b809      	macl %a1,%a3,%a2@\+,%d2,%acc0
    5df0:	a49a b819      	macl %a1,%a3,%a2@\+,%d2,%acc2
    5df4:	ae5a b809      	macl %a1,%a3,%a2@\+,%sp,%acc0
    5df8:	aeda b819      	macl %a1,%a3,%a2@\+,%sp,%acc2
    5dfc:	a21a b829      	macl %a1,%a3,%a2@\+&,%d1,%acc0
    5e00:	a29a b839      	macl %a1,%a3,%a2@\+&,%d1,%acc2
    5e04:	a65a b829      	macl %a1,%a3,%a2@\+&,%a3,%acc0
    5e08:	a6da b839      	macl %a1,%a3,%a2@\+&,%a3,%acc2
    5e0c:	a41a b829      	macl %a1,%a3,%a2@\+&,%d2,%acc0
    5e10:	a49a b839      	macl %a1,%a3,%a2@\+&,%d2,%acc2
    5e14:	ae5a b829      	macl %a1,%a3,%a2@\+&,%sp,%acc0
    5e18:	aeda b839      	macl %a1,%a3,%a2@\+&,%sp,%acc2
    5e1c:	a22e b809 000a 	macl %a1,%a3,%fp@\(10\),%d1,%acc0
    5e22:	a2ae b819 000a 	macl %a1,%a3,%fp@\(10\),%d1,%acc2
    5e28:	a66e b809 000a 	macl %a1,%a3,%fp@\(10\),%a3,%acc0
    5e2e:	a6ee b819 000a 	macl %a1,%a3,%fp@\(10\),%a3,%acc2
    5e34:	a42e b809 000a 	macl %a1,%a3,%fp@\(10\),%d2,%acc0
    5e3a:	a4ae b819 000a 	macl %a1,%a3,%fp@\(10\),%d2,%acc2
    5e40:	ae6e b809 000a 	macl %a1,%a3,%fp@\(10\),%sp,%acc0
    5e46:	aeee b819 000a 	macl %a1,%a3,%fp@\(10\),%sp,%acc2
    5e4c:	a22e b829 000a 	macl %a1,%a3,%fp@\(10\)&,%d1,%acc0
    5e52:	a2ae b839 000a 	macl %a1,%a3,%fp@\(10\)&,%d1,%acc2
    5e58:	a66e b829 000a 	macl %a1,%a3,%fp@\(10\)&,%a3,%acc0
    5e5e:	a6ee b839 000a 	macl %a1,%a3,%fp@\(10\)&,%a3,%acc2
    5e64:	a42e b829 000a 	macl %a1,%a3,%fp@\(10\)&,%d2,%acc0
    5e6a:	a4ae b839 000a 	macl %a1,%a3,%fp@\(10\)&,%d2,%acc2
    5e70:	ae6e b829 000a 	macl %a1,%a3,%fp@\(10\)&,%sp,%acc0
    5e76:	aeee b839 000a 	macl %a1,%a3,%fp@\(10\)&,%sp,%acc2
    5e7c:	a221 b809      	macl %a1,%a3,%a1@-,%d1,%acc0
    5e80:	a2a1 b819      	macl %a1,%a3,%a1@-,%d1,%acc2
    5e84:	a661 b809      	macl %a1,%a3,%a1@-,%a3,%acc0
    5e88:	a6e1 b819      	macl %a1,%a3,%a1@-,%a3,%acc2
    5e8c:	a421 b809      	macl %a1,%a3,%a1@-,%d2,%acc0
    5e90:	a4a1 b819      	macl %a1,%a3,%a1@-,%d2,%acc2
    5e94:	ae61 b809      	macl %a1,%a3,%a1@-,%sp,%acc0
    5e98:	aee1 b819      	macl %a1,%a3,%a1@-,%sp,%acc2
    5e9c:	a221 b829      	macl %a1,%a3,%a1@-&,%d1,%acc0
    5ea0:	a2a1 b839      	macl %a1,%a3,%a1@-&,%d1,%acc2
    5ea4:	a661 b829      	macl %a1,%a3,%a1@-&,%a3,%acc0
    5ea8:	a6e1 b839      	macl %a1,%a3,%a1@-&,%a3,%acc2
    5eac:	a421 b829      	macl %a1,%a3,%a1@-&,%d2,%acc0
    5eb0:	a4a1 b839      	macl %a1,%a3,%a1@-&,%d2,%acc2
    5eb4:	ae61 b829      	macl %a1,%a3,%a1@-&,%sp,%acc0
    5eb8:	aee1 b839      	macl %a1,%a3,%a1@-&,%sp,%acc2
    5ebc:	a213 ba09      	macl %a1,%a3,<<,%a3@,%d1,%acc0
    5ec0:	a293 ba19      	macl %a1,%a3,<<,%a3@,%d1,%acc2
    5ec4:	a653 ba09      	macl %a1,%a3,>>,%a3@,%a3,%acc0
    5ec8:	a6d3 ba19      	macl %a1,%a3,>>,%a3@,%a3,%acc2
    5ecc:	a413 ba09      	macl %a1,%a3,,%a3@,%d2,%acc0
    5ed0:	a493 ba19      	macl %a1,%a3,,%a3@,%d2,%acc2
    5ed4:	ae53 ba09      	macl %a1,%a3,>>,%a3@,%sp,%acc0
    5ed8:	aed3 ba19      	macl %a1,%a3,>>,%a3@,%sp,%acc2
    5edc:	a213 ba29      	macl %a1,%a3,<<,%a3@&,%d1,%acc0
    5ee0:	a293 ba39      	macl %a1,%a3,<<,%a3@&,%d1,%acc2
    5ee4:	a653 ba29      	macl %a1,%a3,>>,%a3@&,%a3,%acc0
    5ee8:	a6d3 ba39      	macl %a1,%a3,>>,%a3@&,%a3,%acc2
    5eec:	a413 ba29      	macl %a1,%a3,,%a3@&,%d2,%acc0
    5ef0:	a493 ba39      	macl %a1,%a3,,%a3@&,%d2,%acc2
    5ef4:	ae53 ba29      	macl %a1,%a3,>>,%a3@&,%sp,%acc0
    5ef8:	aed3 ba39      	macl %a1,%a3,>>,%a3@&,%sp,%acc2
    5efc:	a21a ba09      	macl %a1,%a3,<<,%a2@\+,%d1,%acc0
    5f00:	a29a ba19      	macl %a1,%a3,<<,%a2@\+,%d1,%acc2
    5f04:	a65a ba09      	macl %a1,%a3,>>,%a2@\+,%a3,%acc0
    5f08:	a6da ba19      	macl %a1,%a3,>>,%a2@\+,%a3,%acc2
    5f0c:	a41a ba09      	macl %a1,%a3,,%a2@\+,%d2,%acc0
    5f10:	a49a ba19      	macl %a1,%a3,,%a2@\+,%d2,%acc2
    5f14:	ae5a ba09      	macl %a1,%a3,>>,%a2@\+,%sp,%acc0
    5f18:	aeda ba19      	macl %a1,%a3,>>,%a2@\+,%sp,%acc2
    5f1c:	a21a ba29      	macl %a1,%a3,<<,%a2@\+&,%d1,%acc0
    5f20:	a29a ba39      	macl %a1,%a3,<<,%a2@\+&,%d1,%acc2
    5f24:	a65a ba29      	macl %a1,%a3,>>,%a2@\+&,%a3,%acc0
    5f28:	a6da ba39      	macl %a1,%a3,>>,%a2@\+&,%a3,%acc2
    5f2c:	a41a ba29      	macl %a1,%a3,,%a2@\+&,%d2,%acc0
    5f30:	a49a ba39      	macl %a1,%a3,,%a2@\+&,%d2,%acc2
    5f34:	ae5a ba29      	macl %a1,%a3,>>,%a2@\+&,%sp,%acc0
    5f38:	aeda ba39      	macl %a1,%a3,>>,%a2@\+&,%sp,%acc2
    5f3c:	a22e ba09 000a 	macl %a1,%a3,<<,%fp@\(10\),%d1,%acc0
    5f42:	a2ae ba19 000a 	macl %a1,%a3,<<,%fp@\(10\),%d1,%acc2
    5f48:	a66e ba09 000a 	macl %a1,%a3,>>,%fp@\(10\),%a3,%acc0
    5f4e:	a6ee ba19 000a 	macl %a1,%a3,>>,%fp@\(10\),%a3,%acc2
    5f54:	a42e ba09 000a 	macl %a1,%a3,,%fp@\(10\),%d2,%acc0
    5f5a:	a4ae ba19 000a 	macl %a1,%a3,,%fp@\(10\),%d2,%acc2
    5f60:	ae6e ba09 000a 	macl %a1,%a3,>>,%fp@\(10\),%sp,%acc0
    5f66:	aeee ba19 000a 	macl %a1,%a3,>>,%fp@\(10\),%sp,%acc2
    5f6c:	a22e ba29 000a 	macl %a1,%a3,<<,%fp@\(10\)&,%d1,%acc0
    5f72:	a2ae ba39 000a 	macl %a1,%a3,<<,%fp@\(10\)&,%d1,%acc2
    5f78:	a66e ba29 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%a3,%acc0
    5f7e:	a6ee ba39 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%a3,%acc2
    5f84:	a42e ba29 000a 	macl %a1,%a3,,%fp@\(10\)&,%d2,%acc0
    5f8a:	a4ae ba39 000a 	macl %a1,%a3,,%fp@\(10\)&,%d2,%acc2
    5f90:	ae6e ba29 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%sp,%acc0
    5f96:	aeee ba39 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%sp,%acc2
    5f9c:	a221 ba09      	macl %a1,%a3,<<,%a1@-,%d1,%acc0
    5fa0:	a2a1 ba19      	macl %a1,%a3,<<,%a1@-,%d1,%acc2
    5fa4:	a661 ba09      	macl %a1,%a3,>>,%a1@-,%a3,%acc0
    5fa8:	a6e1 ba19      	macl %a1,%a3,>>,%a1@-,%a3,%acc2
    5fac:	a421 ba09      	macl %a1,%a3,,%a1@-,%d2,%acc0
    5fb0:	a4a1 ba19      	macl %a1,%a3,,%a1@-,%d2,%acc2
    5fb4:	ae61 ba09      	macl %a1,%a3,>>,%a1@-,%sp,%acc0
    5fb8:	aee1 ba19      	macl %a1,%a3,>>,%a1@-,%sp,%acc2
    5fbc:	a221 ba29      	macl %a1,%a3,<<,%a1@-&,%d1,%acc0
    5fc0:	a2a1 ba39      	macl %a1,%a3,<<,%a1@-&,%d1,%acc2
    5fc4:	a661 ba29      	macl %a1,%a3,>>,%a1@-&,%a3,%acc0
    5fc8:	a6e1 ba39      	macl %a1,%a3,>>,%a1@-&,%a3,%acc2
    5fcc:	a421 ba29      	macl %a1,%a3,,%a1@-&,%d2,%acc0
    5fd0:	a4a1 ba39      	macl %a1,%a3,,%a1@-&,%d2,%acc2
    5fd4:	ae61 ba29      	macl %a1,%a3,>>,%a1@-&,%sp,%acc0
    5fd8:	aee1 ba39      	macl %a1,%a3,>>,%a1@-&,%sp,%acc2
    5fdc:	a213 be09      	macl %a1,%a3,<<,%a3@,%d1,%acc0
    5fe0:	a293 be19      	macl %a1,%a3,<<,%a3@,%d1,%acc2
    5fe4:	a653 be09      	macl %a1,%a3,>>,%a3@,%a3,%acc0
    5fe8:	a6d3 be19      	macl %a1,%a3,>>,%a3@,%a3,%acc2
    5fec:	a413 be09      	macl %a1,%a3,,%a3@,%d2,%acc0
    5ff0:	a493 be19      	macl %a1,%a3,,%a3@,%d2,%acc2
    5ff4:	ae53 be09      	macl %a1,%a3,>>,%a3@,%sp,%acc0
    5ff8:	aed3 be19      	macl %a1,%a3,>>,%a3@,%sp,%acc2
    5ffc:	a213 be29      	macl %a1,%a3,<<,%a3@&,%d1,%acc0
    6000:	a293 be39      	macl %a1,%a3,<<,%a3@&,%d1,%acc2
    6004:	a653 be29      	macl %a1,%a3,>>,%a3@&,%a3,%acc0
    6008:	a6d3 be39      	macl %a1,%a3,>>,%a3@&,%a3,%acc2
    600c:	a413 be29      	macl %a1,%a3,,%a3@&,%d2,%acc0
    6010:	a493 be39      	macl %a1,%a3,,%a3@&,%d2,%acc2
    6014:	ae53 be29      	macl %a1,%a3,>>,%a3@&,%sp,%acc0
    6018:	aed3 be39      	macl %a1,%a3,>>,%a3@&,%sp,%acc2
    601c:	a21a be09      	macl %a1,%a3,<<,%a2@\+,%d1,%acc0
    6020:	a29a be19      	macl %a1,%a3,<<,%a2@\+,%d1,%acc2
    6024:	a65a be09      	macl %a1,%a3,>>,%a2@\+,%a3,%acc0
    6028:	a6da be19      	macl %a1,%a3,>>,%a2@\+,%a3,%acc2
    602c:	a41a be09      	macl %a1,%a3,,%a2@\+,%d2,%acc0
    6030:	a49a be19      	macl %a1,%a3,,%a2@\+,%d2,%acc2
    6034:	ae5a be09      	macl %a1,%a3,>>,%a2@\+,%sp,%acc0
    6038:	aeda be19      	macl %a1,%a3,>>,%a2@\+,%sp,%acc2
    603c:	a21a be29      	macl %a1,%a3,<<,%a2@\+&,%d1,%acc0
    6040:	a29a be39      	macl %a1,%a3,<<,%a2@\+&,%d1,%acc2
    6044:	a65a be29      	macl %a1,%a3,>>,%a2@\+&,%a3,%acc0
    6048:	a6da be39      	macl %a1,%a3,>>,%a2@\+&,%a3,%acc2
    604c:	a41a be29      	macl %a1,%a3,,%a2@\+&,%d2,%acc0
    6050:	a49a be39      	macl %a1,%a3,,%a2@\+&,%d2,%acc2
    6054:	ae5a be29      	macl %a1,%a3,>>,%a2@\+&,%sp,%acc0
    6058:	aeda be39      	macl %a1,%a3,>>,%a2@\+&,%sp,%acc2
    605c:	a22e be09 000a 	macl %a1,%a3,<<,%fp@\(10\),%d1,%acc0
    6062:	a2ae be19 000a 	macl %a1,%a3,<<,%fp@\(10\),%d1,%acc2
    6068:	a66e be09 000a 	macl %a1,%a3,>>,%fp@\(10\),%a3,%acc0
    606e:	a6ee be19 000a 	macl %a1,%a3,>>,%fp@\(10\),%a3,%acc2
    6074:	a42e be09 000a 	macl %a1,%a3,,%fp@\(10\),%d2,%acc0
    607a:	a4ae be19 000a 	macl %a1,%a3,,%fp@\(10\),%d2,%acc2
    6080:	ae6e be09 000a 	macl %a1,%a3,>>,%fp@\(10\),%sp,%acc0
    6086:	aeee be19 000a 	macl %a1,%a3,>>,%fp@\(10\),%sp,%acc2
    608c:	a22e be29 000a 	macl %a1,%a3,<<,%fp@\(10\)&,%d1,%acc0
    6092:	a2ae be39 000a 	macl %a1,%a3,<<,%fp@\(10\)&,%d1,%acc2
    6098:	a66e be29 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%a3,%acc0
    609e:	a6ee be39 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%a3,%acc2
    60a4:	a42e be29 000a 	macl %a1,%a3,,%fp@\(10\)&,%d2,%acc0
    60aa:	a4ae be39 000a 	macl %a1,%a3,,%fp@\(10\)&,%d2,%acc2
    60b0:	ae6e be29 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%sp,%acc0
    60b6:	aeee be39 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%sp,%acc2
    60bc:	a221 be09      	macl %a1,%a3,<<,%a1@-,%d1,%acc0
    60c0:	a2a1 be19      	macl %a1,%a3,<<,%a1@-,%d1,%acc2
    60c4:	a661 be09      	macl %a1,%a3,>>,%a1@-,%a3,%acc0
    60c8:	a6e1 be19      	macl %a1,%a3,>>,%a1@-,%a3,%acc2
    60cc:	a421 be09      	macl %a1,%a3,,%a1@-,%d2,%acc0
    60d0:	a4a1 be19      	macl %a1,%a3,,%a1@-,%d2,%acc2
    60d4:	ae61 be09      	macl %a1,%a3,>>,%a1@-,%sp,%acc0
    60d8:	aee1 be19      	macl %a1,%a3,>>,%a1@-,%sp,%acc2
    60dc:	a221 be29      	macl %a1,%a3,<<,%a1@-&,%d1,%acc0
    60e0:	a2a1 be39      	macl %a1,%a3,<<,%a1@-&,%d1,%acc2
    60e4:	a661 be29      	macl %a1,%a3,>>,%a1@-&,%a3,%acc0
    60e8:	a6e1 be39      	macl %a1,%a3,>>,%a1@-&,%a3,%acc2
    60ec:	a421 be29      	macl %a1,%a3,,%a1@-&,%d2,%acc0
    60f0:	a4a1 be39      	macl %a1,%a3,,%a1@-&,%d2,%acc2
    60f4:	ae61 be29      	macl %a1,%a3,>>,%a1@-&,%sp,%acc0
    60f8:	aee1 be39      	macl %a1,%a3,>>,%a1@-&,%sp,%acc2
    60fc:	a213 ba09      	macl %a1,%a3,<<,%a3@,%d1,%acc0
    6100:	a293 ba19      	macl %a1,%a3,<<,%a3@,%d1,%acc2
    6104:	a653 ba09      	macl %a1,%a3,>>,%a3@,%a3,%acc0
    6108:	a6d3 ba19      	macl %a1,%a3,>>,%a3@,%a3,%acc2
    610c:	a413 ba09      	macl %a1,%a3,,%a3@,%d2,%acc0
    6110:	a493 ba19      	macl %a1,%a3,,%a3@,%d2,%acc2
    6114:	ae53 ba09      	macl %a1,%a3,>>,%a3@,%sp,%acc0
    6118:	aed3 ba19      	macl %a1,%a3,>>,%a3@,%sp,%acc2
    611c:	a213 ba29      	macl %a1,%a3,<<,%a3@&,%d1,%acc0
    6120:	a293 ba39      	macl %a1,%a3,<<,%a3@&,%d1,%acc2
    6124:	a653 ba29      	macl %a1,%a3,>>,%a3@&,%a3,%acc0
    6128:	a6d3 ba39      	macl %a1,%a3,>>,%a3@&,%a3,%acc2
    612c:	a413 ba29      	macl %a1,%a3,,%a3@&,%d2,%acc0
    6130:	a493 ba39      	macl %a1,%a3,,%a3@&,%d2,%acc2
    6134:	ae53 ba29      	macl %a1,%a3,>>,%a3@&,%sp,%acc0
    6138:	aed3 ba39      	macl %a1,%a3,>>,%a3@&,%sp,%acc2
    613c:	a21a ba09      	macl %a1,%a3,<<,%a2@\+,%d1,%acc0
    6140:	a29a ba19      	macl %a1,%a3,<<,%a2@\+,%d1,%acc2
    6144:	a65a ba09      	macl %a1,%a3,>>,%a2@\+,%a3,%acc0
    6148:	a6da ba19      	macl %a1,%a3,>>,%a2@\+,%a3,%acc2
    614c:	a41a ba09      	macl %a1,%a3,,%a2@\+,%d2,%acc0
    6150:	a49a ba19      	macl %a1,%a3,,%a2@\+,%d2,%acc2
    6154:	ae5a ba09      	macl %a1,%a3,>>,%a2@\+,%sp,%acc0
    6158:	aeda ba19      	macl %a1,%a3,>>,%a2@\+,%sp,%acc2
    615c:	a21a ba29      	macl %a1,%a3,<<,%a2@\+&,%d1,%acc0
    6160:	a29a ba39      	macl %a1,%a3,<<,%a2@\+&,%d1,%acc2
    6164:	a65a ba29      	macl %a1,%a3,>>,%a2@\+&,%a3,%acc0
    6168:	a6da ba39      	macl %a1,%a3,>>,%a2@\+&,%a3,%acc2
    616c:	a41a ba29      	macl %a1,%a3,,%a2@\+&,%d2,%acc0
    6170:	a49a ba39      	macl %a1,%a3,,%a2@\+&,%d2,%acc2
    6174:	ae5a ba29      	macl %a1,%a3,>>,%a2@\+&,%sp,%acc0
    6178:	aeda ba39      	macl %a1,%a3,>>,%a2@\+&,%sp,%acc2
    617c:	a22e ba09 000a 	macl %a1,%a3,<<,%fp@\(10\),%d1,%acc0
    6182:	a2ae ba19 000a 	macl %a1,%a3,<<,%fp@\(10\),%d1,%acc2
    6188:	a66e ba09 000a 	macl %a1,%a3,>>,%fp@\(10\),%a3,%acc0
    618e:	a6ee ba19 000a 	macl %a1,%a3,>>,%fp@\(10\),%a3,%acc2
    6194:	a42e ba09 000a 	macl %a1,%a3,,%fp@\(10\),%d2,%acc0
    619a:	a4ae ba19 000a 	macl %a1,%a3,,%fp@\(10\),%d2,%acc2
    61a0:	ae6e ba09 000a 	macl %a1,%a3,>>,%fp@\(10\),%sp,%acc0
    61a6:	aeee ba19 000a 	macl %a1,%a3,>>,%fp@\(10\),%sp,%acc2
    61ac:	a22e ba29 000a 	macl %a1,%a3,<<,%fp@\(10\)&,%d1,%acc0
    61b2:	a2ae ba39 000a 	macl %a1,%a3,<<,%fp@\(10\)&,%d1,%acc2
    61b8:	a66e ba29 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%a3,%acc0
    61be:	a6ee ba39 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%a3,%acc2
    61c4:	a42e ba29 000a 	macl %a1,%a3,,%fp@\(10\)&,%d2,%acc0
    61ca:	a4ae ba39 000a 	macl %a1,%a3,,%fp@\(10\)&,%d2,%acc2
    61d0:	ae6e ba29 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%sp,%acc0
    61d6:	aeee ba39 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%sp,%acc2
    61dc:	a221 ba09      	macl %a1,%a3,<<,%a1@-,%d1,%acc0
    61e0:	a2a1 ba19      	macl %a1,%a3,<<,%a1@-,%d1,%acc2
    61e4:	a661 ba09      	macl %a1,%a3,>>,%a1@-,%a3,%acc0
    61e8:	a6e1 ba19      	macl %a1,%a3,>>,%a1@-,%a3,%acc2
    61ec:	a421 ba09      	macl %a1,%a3,,%a1@-,%d2,%acc0
    61f0:	a4a1 ba19      	macl %a1,%a3,,%a1@-,%d2,%acc2
    61f4:	ae61 ba09      	macl %a1,%a3,>>,%a1@-,%sp,%acc0
    61f8:	aee1 ba19      	macl %a1,%a3,>>,%a1@-,%sp,%acc2
    61fc:	a221 ba29      	macl %a1,%a3,<<,%a1@-&,%d1,%acc0
    6200:	a2a1 ba39      	macl %a1,%a3,<<,%a1@-&,%d1,%acc2
    6204:	a661 ba29      	macl %a1,%a3,>>,%a1@-&,%a3,%acc0
    6208:	a6e1 ba39      	macl %a1,%a3,>>,%a1@-&,%a3,%acc2
    620c:	a421 ba29      	macl %a1,%a3,,%a1@-&,%d2,%acc0
    6210:	a4a1 ba39      	macl %a1,%a3,,%a1@-&,%d2,%acc2
    6214:	ae61 ba29      	macl %a1,%a3,>>,%a1@-&,%sp,%acc0
    6218:	aee1 ba39      	macl %a1,%a3,>>,%a1@-&,%sp,%acc2
    621c:	a213 be09      	macl %a1,%a3,<<,%a3@,%d1,%acc0
    6220:	a293 be19      	macl %a1,%a3,<<,%a3@,%d1,%acc2
    6224:	a653 be09      	macl %a1,%a3,>>,%a3@,%a3,%acc0
    6228:	a6d3 be19      	macl %a1,%a3,>>,%a3@,%a3,%acc2
    622c:	a413 be09      	macl %a1,%a3,,%a3@,%d2,%acc0
    6230:	a493 be19      	macl %a1,%a3,,%a3@,%d2,%acc2
    6234:	ae53 be09      	macl %a1,%a3,>>,%a3@,%sp,%acc0
    6238:	aed3 be19      	macl %a1,%a3,>>,%a3@,%sp,%acc2
    623c:	a213 be29      	macl %a1,%a3,<<,%a3@&,%d1,%acc0
    6240:	a293 be39      	macl %a1,%a3,<<,%a3@&,%d1,%acc2
    6244:	a653 be29      	macl %a1,%a3,>>,%a3@&,%a3,%acc0
    6248:	a6d3 be39      	macl %a1,%a3,>>,%a3@&,%a3,%acc2
    624c:	a413 be29      	macl %a1,%a3,,%a3@&,%d2,%acc0
    6250:	a493 be39      	macl %a1,%a3,,%a3@&,%d2,%acc2
    6254:	ae53 be29      	macl %a1,%a3,>>,%a3@&,%sp,%acc0
    6258:	aed3 be39      	macl %a1,%a3,>>,%a3@&,%sp,%acc2
    625c:	a21a be09      	macl %a1,%a3,<<,%a2@\+,%d1,%acc0
    6260:	a29a be19      	macl %a1,%a3,<<,%a2@\+,%d1,%acc2
    6264:	a65a be09      	macl %a1,%a3,>>,%a2@\+,%a3,%acc0
    6268:	a6da be19      	macl %a1,%a3,>>,%a2@\+,%a3,%acc2
    626c:	a41a be09      	macl %a1,%a3,,%a2@\+,%d2,%acc0
    6270:	a49a be19      	macl %a1,%a3,,%a2@\+,%d2,%acc2
    6274:	ae5a be09      	macl %a1,%a3,>>,%a2@\+,%sp,%acc0
    6278:	aeda be19      	macl %a1,%a3,>>,%a2@\+,%sp,%acc2
    627c:	a21a be29      	macl %a1,%a3,<<,%a2@\+&,%d1,%acc0
    6280:	a29a be39      	macl %a1,%a3,<<,%a2@\+&,%d1,%acc2
    6284:	a65a be29      	macl %a1,%a3,>>,%a2@\+&,%a3,%acc0
    6288:	a6da be39      	macl %a1,%a3,>>,%a2@\+&,%a3,%acc2
    628c:	a41a be29      	macl %a1,%a3,,%a2@\+&,%d2,%acc0
    6290:	a49a be39      	macl %a1,%a3,,%a2@\+&,%d2,%acc2
    6294:	ae5a be29      	macl %a1,%a3,>>,%a2@\+&,%sp,%acc0
    6298:	aeda be39      	macl %a1,%a3,>>,%a2@\+&,%sp,%acc2
    629c:	a22e be09 000a 	macl %a1,%a3,<<,%fp@\(10\),%d1,%acc0
    62a2:	a2ae be19 000a 	macl %a1,%a3,<<,%fp@\(10\),%d1,%acc2
    62a8:	a66e be09 000a 	macl %a1,%a3,>>,%fp@\(10\),%a3,%acc0
    62ae:	a6ee be19 000a 	macl %a1,%a3,>>,%fp@\(10\),%a3,%acc2
    62b4:	a42e be09 000a 	macl %a1,%a3,,%fp@\(10\),%d2,%acc0
    62ba:	a4ae be19 000a 	macl %a1,%a3,,%fp@\(10\),%d2,%acc2
    62c0:	ae6e be09 000a 	macl %a1,%a3,>>,%fp@\(10\),%sp,%acc0
    62c6:	aeee be19 000a 	macl %a1,%a3,>>,%fp@\(10\),%sp,%acc2
    62cc:	a22e be29 000a 	macl %a1,%a3,<<,%fp@\(10\)&,%d1,%acc0
    62d2:	a2ae be39 000a 	macl %a1,%a3,<<,%fp@\(10\)&,%d1,%acc2
    62d8:	a66e be29 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%a3,%acc0
    62de:	a6ee be39 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%a3,%acc2
    62e4:	a42e be29 000a 	macl %a1,%a3,,%fp@\(10\)&,%d2,%acc0
    62ea:	a4ae be39 000a 	macl %a1,%a3,,%fp@\(10\)&,%d2,%acc2
    62f0:	ae6e be29 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%sp,%acc0
    62f6:	aeee be39 000a 	macl %a1,%a3,>>,%fp@\(10\)&,%sp,%acc2
    62fc:	a221 be09      	macl %a1,%a3,<<,%a1@-,%d1,%acc0
    6300:	a2a1 be19      	macl %a1,%a3,<<,%a1@-,%d1,%acc2
    6304:	a661 be09      	macl %a1,%a3,>>,%a1@-,%a3,%acc0
    6308:	a6e1 be19      	macl %a1,%a3,>>,%a1@-,%a3,%acc2
    630c:	a421 be09      	macl %a1,%a3,,%a1@-,%d2,%acc0
    6310:	a4a1 be19      	macl %a1,%a3,,%a1@-,%d2,%acc2
    6314:	ae61 be09      	macl %a1,%a3,>>,%a1@-,%sp,%acc0
    6318:	aee1 be19      	macl %a1,%a3,>>,%a1@-,%sp,%acc2
    631c:	a221 be29      	macl %a1,%a3,<<,%a1@-&,%d1,%acc0
    6320:	a2a1 be39      	macl %a1,%a3,<<,%a1@-&,%d1,%acc2
    6324:	a661 be29      	macl %a1,%a3,>>,%a1@-&,%a3,%acc0
    6328:	a6e1 be39      	macl %a1,%a3,>>,%a1@-&,%a3,%acc2
    632c:	a421 be29      	macl %a1,%a3,,%a1@-&,%d2,%acc0
    6330:	a4a1 be39      	macl %a1,%a3,,%a1@-&,%d2,%acc2
    6334:	ae61 be29      	macl %a1,%a3,>>,%a1@-&,%sp,%acc0
    6338:	aee1 be39      	macl %a1,%a3,>>,%a1@-&,%sp,%acc2
    633c:	a213 4809      	macl %a1,%d4,%a3@,%d1,%acc1
    6340:	a293 4819      	macl %a1,%d4,%a3@,%d1,%acc3
    6344:	a653 4809      	macl %a1,%d4,%a3@,%a3,%acc1
    6348:	a6d3 4819      	macl %a1,%d4,%a3@,%a3,%acc3
    634c:	a413 4809      	macl %a1,%d4,%a3@,%d2,%acc1
    6350:	a493 4819      	macl %a1,%d4,%a3@,%d2,%acc3
    6354:	ae53 4809      	macl %a1,%d4,%a3@,%sp,%acc1
    6358:	aed3 4819      	macl %a1,%d4,%a3@,%sp,%acc3
    635c:	a213 4829      	macl %a1,%d4,%a3@&,%d1,%acc1
    6360:	a293 4839      	macl %a1,%d4,%a3@&,%d1,%acc3
    6364:	a653 4829      	macl %a1,%d4,%a3@&,%a3,%acc1
    6368:	a6d3 4839      	macl %a1,%d4,%a3@&,%a3,%acc3
    636c:	a413 4829      	macl %a1,%d4,%a3@&,%d2,%acc1
    6370:	a493 4839      	macl %a1,%d4,%a3@&,%d2,%acc3
    6374:	ae53 4829      	macl %a1,%d4,%a3@&,%sp,%acc1
    6378:	aed3 4839      	macl %a1,%d4,%a3@&,%sp,%acc3
    637c:	a21a 4809      	macl %a1,%d4,%a2@\+,%d1,%acc1
    6380:	a29a 4819      	macl %a1,%d4,%a2@\+,%d1,%acc3
    6384:	a65a 4809      	macl %a1,%d4,%a2@\+,%a3,%acc1
    6388:	a6da 4819      	macl %a1,%d4,%a2@\+,%a3,%acc3
    638c:	a41a 4809      	macl %a1,%d4,%a2@\+,%d2,%acc1
    6390:	a49a 4819      	macl %a1,%d4,%a2@\+,%d2,%acc3
    6394:	ae5a 4809      	macl %a1,%d4,%a2@\+,%sp,%acc1
    6398:	aeda 4819      	macl %a1,%d4,%a2@\+,%sp,%acc3
    639c:	a21a 4829      	macl %a1,%d4,%a2@\+&,%d1,%acc1
    63a0:	a29a 4839      	macl %a1,%d4,%a2@\+&,%d1,%acc3
    63a4:	a65a 4829      	macl %a1,%d4,%a2@\+&,%a3,%acc1
    63a8:	a6da 4839      	macl %a1,%d4,%a2@\+&,%a3,%acc3
    63ac:	a41a 4829      	macl %a1,%d4,%a2@\+&,%d2,%acc1
    63b0:	a49a 4839      	macl %a1,%d4,%a2@\+&,%d2,%acc3
    63b4:	ae5a 4829      	macl %a1,%d4,%a2@\+&,%sp,%acc1
    63b8:	aeda 4839      	macl %a1,%d4,%a2@\+&,%sp,%acc3
    63bc:	a22e 4809 000a 	macl %a1,%d4,%fp@\(10\),%d1,%acc1
    63c2:	a2ae 4819 000a 	macl %a1,%d4,%fp@\(10\),%d1,%acc3
    63c8:	a66e 4809 000a 	macl %a1,%d4,%fp@\(10\),%a3,%acc1
    63ce:	a6ee 4819 000a 	macl %a1,%d4,%fp@\(10\),%a3,%acc3
    63d4:	a42e 4809 000a 	macl %a1,%d4,%fp@\(10\),%d2,%acc1
    63da:	a4ae 4819 000a 	macl %a1,%d4,%fp@\(10\),%d2,%acc3
    63e0:	ae6e 4809 000a 	macl %a1,%d4,%fp@\(10\),%sp,%acc1
    63e6:	aeee 4819 000a 	macl %a1,%d4,%fp@\(10\),%sp,%acc3
    63ec:	a22e 4829 000a 	macl %a1,%d4,%fp@\(10\)&,%d1,%acc1
    63f2:	a2ae 4839 000a 	macl %a1,%d4,%fp@\(10\)&,%d1,%acc3
    63f8:	a66e 4829 000a 	macl %a1,%d4,%fp@\(10\)&,%a3,%acc1
    63fe:	a6ee 4839 000a 	macl %a1,%d4,%fp@\(10\)&,%a3,%acc3
    6404:	a42e 4829 000a 	macl %a1,%d4,%fp@\(10\)&,%d2,%acc1
    640a:	a4ae 4839 000a 	macl %a1,%d4,%fp@\(10\)&,%d2,%acc3
    6410:	ae6e 4829 000a 	macl %a1,%d4,%fp@\(10\)&,%sp,%acc1
    6416:	aeee 4839 000a 	macl %a1,%d4,%fp@\(10\)&,%sp,%acc3
    641c:	a221 4809      	macl %a1,%d4,%a1@-,%d1,%acc1
    6420:	a2a1 4819      	macl %a1,%d4,%a1@-,%d1,%acc3
    6424:	a661 4809      	macl %a1,%d4,%a1@-,%a3,%acc1
    6428:	a6e1 4819      	macl %a1,%d4,%a1@-,%a3,%acc3
    642c:	a421 4809      	macl %a1,%d4,%a1@-,%d2,%acc1
    6430:	a4a1 4819      	macl %a1,%d4,%a1@-,%d2,%acc3
    6434:	ae61 4809      	macl %a1,%d4,%a1@-,%sp,%acc1
    6438:	aee1 4819      	macl %a1,%d4,%a1@-,%sp,%acc3
    643c:	a221 4829      	macl %a1,%d4,%a1@-&,%d1,%acc1
    6440:	a2a1 4839      	macl %a1,%d4,%a1@-&,%d1,%acc3
    6444:	a661 4829      	macl %a1,%d4,%a1@-&,%a3,%acc1
    6448:	a6e1 4839      	macl %a1,%d4,%a1@-&,%a3,%acc3
    644c:	a421 4829      	macl %a1,%d4,%a1@-&,%d2,%acc1
    6450:	a4a1 4839      	macl %a1,%d4,%a1@-&,%d2,%acc3
    6454:	ae61 4829      	macl %a1,%d4,%a1@-&,%sp,%acc1
    6458:	aee1 4839      	macl %a1,%d4,%a1@-&,%sp,%acc3
    645c:	a213 4a09      	macl %a1,%d4,<<,%a3@,%d1,%acc1
    6460:	a293 4a19      	macl %a1,%d4,<<,%a3@,%d1,%acc3
    6464:	a653 4a09      	macl %a1,%d4,>>,%a3@,%a3,%acc1
    6468:	a6d3 4a19      	macl %a1,%d4,>>,%a3@,%a3,%acc3
    646c:	a413 4a09      	macl %a1,%d4,,%a3@,%d2,%acc1
    6470:	a493 4a19      	macl %a1,%d4,,%a3@,%d2,%acc3
    6474:	ae53 4a09      	macl %a1,%d4,>>,%a3@,%sp,%acc1
    6478:	aed3 4a19      	macl %a1,%d4,>>,%a3@,%sp,%acc3
    647c:	a213 4a29      	macl %a1,%d4,<<,%a3@&,%d1,%acc1
    6480:	a293 4a39      	macl %a1,%d4,<<,%a3@&,%d1,%acc3
    6484:	a653 4a29      	macl %a1,%d4,>>,%a3@&,%a3,%acc1
    6488:	a6d3 4a39      	macl %a1,%d4,>>,%a3@&,%a3,%acc3
    648c:	a413 4a29      	macl %a1,%d4,,%a3@&,%d2,%acc1
    6490:	a493 4a39      	macl %a1,%d4,,%a3@&,%d2,%acc3
    6494:	ae53 4a29      	macl %a1,%d4,>>,%a3@&,%sp,%acc1
    6498:	aed3 4a39      	macl %a1,%d4,>>,%a3@&,%sp,%acc3
    649c:	a21a 4a09      	macl %a1,%d4,<<,%a2@\+,%d1,%acc1
    64a0:	a29a 4a19      	macl %a1,%d4,<<,%a2@\+,%d1,%acc3
    64a4:	a65a 4a09      	macl %a1,%d4,>>,%a2@\+,%a3,%acc1
    64a8:	a6da 4a19      	macl %a1,%d4,>>,%a2@\+,%a3,%acc3
    64ac:	a41a 4a09      	macl %a1,%d4,,%a2@\+,%d2,%acc1
    64b0:	a49a 4a19      	macl %a1,%d4,,%a2@\+,%d2,%acc3
    64b4:	ae5a 4a09      	macl %a1,%d4,>>,%a2@\+,%sp,%acc1
    64b8:	aeda 4a19      	macl %a1,%d4,>>,%a2@\+,%sp,%acc3
    64bc:	a21a 4a29      	macl %a1,%d4,<<,%a2@\+&,%d1,%acc1
    64c0:	a29a 4a39      	macl %a1,%d4,<<,%a2@\+&,%d1,%acc3
    64c4:	a65a 4a29      	macl %a1,%d4,>>,%a2@\+&,%a3,%acc1
    64c8:	a6da 4a39      	macl %a1,%d4,>>,%a2@\+&,%a3,%acc3
    64cc:	a41a 4a29      	macl %a1,%d4,,%a2@\+&,%d2,%acc1
    64d0:	a49a 4a39      	macl %a1,%d4,,%a2@\+&,%d2,%acc3
    64d4:	ae5a 4a29      	macl %a1,%d4,>>,%a2@\+&,%sp,%acc1
    64d8:	aeda 4a39      	macl %a1,%d4,>>,%a2@\+&,%sp,%acc3
    64dc:	a22e 4a09 000a 	macl %a1,%d4,<<,%fp@\(10\),%d1,%acc1
    64e2:	a2ae 4a19 000a 	macl %a1,%d4,<<,%fp@\(10\),%d1,%acc3
    64e8:	a66e 4a09 000a 	macl %a1,%d4,>>,%fp@\(10\),%a3,%acc1
    64ee:	a6ee 4a19 000a 	macl %a1,%d4,>>,%fp@\(10\),%a3,%acc3
    64f4:	a42e 4a09 000a 	macl %a1,%d4,,%fp@\(10\),%d2,%acc1
    64fa:	a4ae 4a19 000a 	macl %a1,%d4,,%fp@\(10\),%d2,%acc3
    6500:	ae6e 4a09 000a 	macl %a1,%d4,>>,%fp@\(10\),%sp,%acc1
    6506:	aeee 4a19 000a 	macl %a1,%d4,>>,%fp@\(10\),%sp,%acc3
    650c:	a22e 4a29 000a 	macl %a1,%d4,<<,%fp@\(10\)&,%d1,%acc1
    6512:	a2ae 4a39 000a 	macl %a1,%d4,<<,%fp@\(10\)&,%d1,%acc3
    6518:	a66e 4a29 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%a3,%acc1
    651e:	a6ee 4a39 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%a3,%acc3
    6524:	a42e 4a29 000a 	macl %a1,%d4,,%fp@\(10\)&,%d2,%acc1
    652a:	a4ae 4a39 000a 	macl %a1,%d4,,%fp@\(10\)&,%d2,%acc3
    6530:	ae6e 4a29 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%sp,%acc1
    6536:	aeee 4a39 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%sp,%acc3
    653c:	a221 4a09      	macl %a1,%d4,<<,%a1@-,%d1,%acc1
    6540:	a2a1 4a19      	macl %a1,%d4,<<,%a1@-,%d1,%acc3
    6544:	a661 4a09      	macl %a1,%d4,>>,%a1@-,%a3,%acc1
    6548:	a6e1 4a19      	macl %a1,%d4,>>,%a1@-,%a3,%acc3
    654c:	a421 4a09      	macl %a1,%d4,,%a1@-,%d2,%acc1
    6550:	a4a1 4a19      	macl %a1,%d4,,%a1@-,%d2,%acc3
    6554:	ae61 4a09      	macl %a1,%d4,>>,%a1@-,%sp,%acc1
    6558:	aee1 4a19      	macl %a1,%d4,>>,%a1@-,%sp,%acc3
    655c:	a221 4a29      	macl %a1,%d4,<<,%a1@-&,%d1,%acc1
    6560:	a2a1 4a39      	macl %a1,%d4,<<,%a1@-&,%d1,%acc3
    6564:	a661 4a29      	macl %a1,%d4,>>,%a1@-&,%a3,%acc1
    6568:	a6e1 4a39      	macl %a1,%d4,>>,%a1@-&,%a3,%acc3
    656c:	a421 4a29      	macl %a1,%d4,,%a1@-&,%d2,%acc1
    6570:	a4a1 4a39      	macl %a1,%d4,,%a1@-&,%d2,%acc3
    6574:	ae61 4a29      	macl %a1,%d4,>>,%a1@-&,%sp,%acc1
    6578:	aee1 4a39      	macl %a1,%d4,>>,%a1@-&,%sp,%acc3
    657c:	a213 4e09      	macl %a1,%d4,<<,%a3@,%d1,%acc1
    6580:	a293 4e19      	macl %a1,%d4,<<,%a3@,%d1,%acc3
    6584:	a653 4e09      	macl %a1,%d4,>>,%a3@,%a3,%acc1
    6588:	a6d3 4e19      	macl %a1,%d4,>>,%a3@,%a3,%acc3
    658c:	a413 4e09      	macl %a1,%d4,,%a3@,%d2,%acc1
    6590:	a493 4e19      	macl %a1,%d4,,%a3@,%d2,%acc3
    6594:	ae53 4e09      	macl %a1,%d4,>>,%a3@,%sp,%acc1
    6598:	aed3 4e19      	macl %a1,%d4,>>,%a3@,%sp,%acc3
    659c:	a213 4e29      	macl %a1,%d4,<<,%a3@&,%d1,%acc1
    65a0:	a293 4e39      	macl %a1,%d4,<<,%a3@&,%d1,%acc3
    65a4:	a653 4e29      	macl %a1,%d4,>>,%a3@&,%a3,%acc1
    65a8:	a6d3 4e39      	macl %a1,%d4,>>,%a3@&,%a3,%acc3
    65ac:	a413 4e29      	macl %a1,%d4,,%a3@&,%d2,%acc1
    65b0:	a493 4e39      	macl %a1,%d4,,%a3@&,%d2,%acc3
    65b4:	ae53 4e29      	macl %a1,%d4,>>,%a3@&,%sp,%acc1
    65b8:	aed3 4e39      	macl %a1,%d4,>>,%a3@&,%sp,%acc3
    65bc:	a21a 4e09      	macl %a1,%d4,<<,%a2@\+,%d1,%acc1
    65c0:	a29a 4e19      	macl %a1,%d4,<<,%a2@\+,%d1,%acc3
    65c4:	a65a 4e09      	macl %a1,%d4,>>,%a2@\+,%a3,%acc1
    65c8:	a6da 4e19      	macl %a1,%d4,>>,%a2@\+,%a3,%acc3
    65cc:	a41a 4e09      	macl %a1,%d4,,%a2@\+,%d2,%acc1
    65d0:	a49a 4e19      	macl %a1,%d4,,%a2@\+,%d2,%acc3
    65d4:	ae5a 4e09      	macl %a1,%d4,>>,%a2@\+,%sp,%acc1
    65d8:	aeda 4e19      	macl %a1,%d4,>>,%a2@\+,%sp,%acc3
    65dc:	a21a 4e29      	macl %a1,%d4,<<,%a2@\+&,%d1,%acc1
    65e0:	a29a 4e39      	macl %a1,%d4,<<,%a2@\+&,%d1,%acc3
    65e4:	a65a 4e29      	macl %a1,%d4,>>,%a2@\+&,%a3,%acc1
    65e8:	a6da 4e39      	macl %a1,%d4,>>,%a2@\+&,%a3,%acc3
    65ec:	a41a 4e29      	macl %a1,%d4,,%a2@\+&,%d2,%acc1
    65f0:	a49a 4e39      	macl %a1,%d4,,%a2@\+&,%d2,%acc3
    65f4:	ae5a 4e29      	macl %a1,%d4,>>,%a2@\+&,%sp,%acc1
    65f8:	aeda 4e39      	macl %a1,%d4,>>,%a2@\+&,%sp,%acc3
    65fc:	a22e 4e09 000a 	macl %a1,%d4,<<,%fp@\(10\),%d1,%acc1
    6602:	a2ae 4e19 000a 	macl %a1,%d4,<<,%fp@\(10\),%d1,%acc3
    6608:	a66e 4e09 000a 	macl %a1,%d4,>>,%fp@\(10\),%a3,%acc1
    660e:	a6ee 4e19 000a 	macl %a1,%d4,>>,%fp@\(10\),%a3,%acc3
    6614:	a42e 4e09 000a 	macl %a1,%d4,,%fp@\(10\),%d2,%acc1
    661a:	a4ae 4e19 000a 	macl %a1,%d4,,%fp@\(10\),%d2,%acc3
    6620:	ae6e 4e09 000a 	macl %a1,%d4,>>,%fp@\(10\),%sp,%acc1
    6626:	aeee 4e19 000a 	macl %a1,%d4,>>,%fp@\(10\),%sp,%acc3
    662c:	a22e 4e29 000a 	macl %a1,%d4,<<,%fp@\(10\)&,%d1,%acc1
    6632:	a2ae 4e39 000a 	macl %a1,%d4,<<,%fp@\(10\)&,%d1,%acc3
    6638:	a66e 4e29 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%a3,%acc1
    663e:	a6ee 4e39 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%a3,%acc3
    6644:	a42e 4e29 000a 	macl %a1,%d4,,%fp@\(10\)&,%d2,%acc1
    664a:	a4ae 4e39 000a 	macl %a1,%d4,,%fp@\(10\)&,%d2,%acc3
    6650:	ae6e 4e29 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%sp,%acc1
    6656:	aeee 4e39 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%sp,%acc3
    665c:	a221 4e09      	macl %a1,%d4,<<,%a1@-,%d1,%acc1
    6660:	a2a1 4e19      	macl %a1,%d4,<<,%a1@-,%d1,%acc3
    6664:	a661 4e09      	macl %a1,%d4,>>,%a1@-,%a3,%acc1
    6668:	a6e1 4e19      	macl %a1,%d4,>>,%a1@-,%a3,%acc3
    666c:	a421 4e09      	macl %a1,%d4,,%a1@-,%d2,%acc1
    6670:	a4a1 4e19      	macl %a1,%d4,,%a1@-,%d2,%acc3
    6674:	ae61 4e09      	macl %a1,%d4,>>,%a1@-,%sp,%acc1
    6678:	aee1 4e19      	macl %a1,%d4,>>,%a1@-,%sp,%acc3
    667c:	a221 4e29      	macl %a1,%d4,<<,%a1@-&,%d1,%acc1
    6680:	a2a1 4e39      	macl %a1,%d4,<<,%a1@-&,%d1,%acc3
    6684:	a661 4e29      	macl %a1,%d4,>>,%a1@-&,%a3,%acc1
    6688:	a6e1 4e39      	macl %a1,%d4,>>,%a1@-&,%a3,%acc3
    668c:	a421 4e29      	macl %a1,%d4,,%a1@-&,%d2,%acc1
    6690:	a4a1 4e39      	macl %a1,%d4,,%a1@-&,%d2,%acc3
    6694:	ae61 4e29      	macl %a1,%d4,>>,%a1@-&,%sp,%acc1
    6698:	aee1 4e39      	macl %a1,%d4,>>,%a1@-&,%sp,%acc3
    669c:	a213 4a09      	macl %a1,%d4,<<,%a3@,%d1,%acc1
    66a0:	a293 4a19      	macl %a1,%d4,<<,%a3@,%d1,%acc3
    66a4:	a653 4a09      	macl %a1,%d4,>>,%a3@,%a3,%acc1
    66a8:	a6d3 4a19      	macl %a1,%d4,>>,%a3@,%a3,%acc3
    66ac:	a413 4a09      	macl %a1,%d4,,%a3@,%d2,%acc1
    66b0:	a493 4a19      	macl %a1,%d4,,%a3@,%d2,%acc3
    66b4:	ae53 4a09      	macl %a1,%d4,>>,%a3@,%sp,%acc1
    66b8:	aed3 4a19      	macl %a1,%d4,>>,%a3@,%sp,%acc3
    66bc:	a213 4a29      	macl %a1,%d4,<<,%a3@&,%d1,%acc1
    66c0:	a293 4a39      	macl %a1,%d4,<<,%a3@&,%d1,%acc3
    66c4:	a653 4a29      	macl %a1,%d4,>>,%a3@&,%a3,%acc1
    66c8:	a6d3 4a39      	macl %a1,%d4,>>,%a3@&,%a3,%acc3
    66cc:	a413 4a29      	macl %a1,%d4,,%a3@&,%d2,%acc1
    66d0:	a493 4a39      	macl %a1,%d4,,%a3@&,%d2,%acc3
    66d4:	ae53 4a29      	macl %a1,%d4,>>,%a3@&,%sp,%acc1
    66d8:	aed3 4a39      	macl %a1,%d4,>>,%a3@&,%sp,%acc3
    66dc:	a21a 4a09      	macl %a1,%d4,<<,%a2@\+,%d1,%acc1
    66e0:	a29a 4a19      	macl %a1,%d4,<<,%a2@\+,%d1,%acc3
    66e4:	a65a 4a09      	macl %a1,%d4,>>,%a2@\+,%a3,%acc1
    66e8:	a6da 4a19      	macl %a1,%d4,>>,%a2@\+,%a3,%acc3
    66ec:	a41a 4a09      	macl %a1,%d4,,%a2@\+,%d2,%acc1
    66f0:	a49a 4a19      	macl %a1,%d4,,%a2@\+,%d2,%acc3
    66f4:	ae5a 4a09      	macl %a1,%d4,>>,%a2@\+,%sp,%acc1
    66f8:	aeda 4a19      	macl %a1,%d4,>>,%a2@\+,%sp,%acc3
    66fc:	a21a 4a29      	macl %a1,%d4,<<,%a2@\+&,%d1,%acc1
    6700:	a29a 4a39      	macl %a1,%d4,<<,%a2@\+&,%d1,%acc3
    6704:	a65a 4a29      	macl %a1,%d4,>>,%a2@\+&,%a3,%acc1
    6708:	a6da 4a39      	macl %a1,%d4,>>,%a2@\+&,%a3,%acc3
    670c:	a41a 4a29      	macl %a1,%d4,,%a2@\+&,%d2,%acc1
    6710:	a49a 4a39      	macl %a1,%d4,,%a2@\+&,%d2,%acc3
    6714:	ae5a 4a29      	macl %a1,%d4,>>,%a2@\+&,%sp,%acc1
    6718:	aeda 4a39      	macl %a1,%d4,>>,%a2@\+&,%sp,%acc3
    671c:	a22e 4a09 000a 	macl %a1,%d4,<<,%fp@\(10\),%d1,%acc1
    6722:	a2ae 4a19 000a 	macl %a1,%d4,<<,%fp@\(10\),%d1,%acc3
    6728:	a66e 4a09 000a 	macl %a1,%d4,>>,%fp@\(10\),%a3,%acc1
    672e:	a6ee 4a19 000a 	macl %a1,%d4,>>,%fp@\(10\),%a3,%acc3
    6734:	a42e 4a09 000a 	macl %a1,%d4,,%fp@\(10\),%d2,%acc1
    673a:	a4ae 4a19 000a 	macl %a1,%d4,,%fp@\(10\),%d2,%acc3
    6740:	ae6e 4a09 000a 	macl %a1,%d4,>>,%fp@\(10\),%sp,%acc1
    6746:	aeee 4a19 000a 	macl %a1,%d4,>>,%fp@\(10\),%sp,%acc3
    674c:	a22e 4a29 000a 	macl %a1,%d4,<<,%fp@\(10\)&,%d1,%acc1
    6752:	a2ae 4a39 000a 	macl %a1,%d4,<<,%fp@\(10\)&,%d1,%acc3
    6758:	a66e 4a29 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%a3,%acc1
    675e:	a6ee 4a39 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%a3,%acc3
    6764:	a42e 4a29 000a 	macl %a1,%d4,,%fp@\(10\)&,%d2,%acc1
    676a:	a4ae 4a39 000a 	macl %a1,%d4,,%fp@\(10\)&,%d2,%acc3
    6770:	ae6e 4a29 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%sp,%acc1
    6776:	aeee 4a39 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%sp,%acc3
    677c:	a221 4a09      	macl %a1,%d4,<<,%a1@-,%d1,%acc1
    6780:	a2a1 4a19      	macl %a1,%d4,<<,%a1@-,%d1,%acc3
    6784:	a661 4a09      	macl %a1,%d4,>>,%a1@-,%a3,%acc1
    6788:	a6e1 4a19      	macl %a1,%d4,>>,%a1@-,%a3,%acc3
    678c:	a421 4a09      	macl %a1,%d4,,%a1@-,%d2,%acc1
    6790:	a4a1 4a19      	macl %a1,%d4,,%a1@-,%d2,%acc3
    6794:	ae61 4a09      	macl %a1,%d4,>>,%a1@-,%sp,%acc1
    6798:	aee1 4a19      	macl %a1,%d4,>>,%a1@-,%sp,%acc3
    679c:	a221 4a29      	macl %a1,%d4,<<,%a1@-&,%d1,%acc1
    67a0:	a2a1 4a39      	macl %a1,%d4,<<,%a1@-&,%d1,%acc3
    67a4:	a661 4a29      	macl %a1,%d4,>>,%a1@-&,%a3,%acc1
    67a8:	a6e1 4a39      	macl %a1,%d4,>>,%a1@-&,%a3,%acc3
    67ac:	a421 4a29      	macl %a1,%d4,,%a1@-&,%d2,%acc1
    67b0:	a4a1 4a39      	macl %a1,%d4,,%a1@-&,%d2,%acc3
    67b4:	ae61 4a29      	macl %a1,%d4,>>,%a1@-&,%sp,%acc1
    67b8:	aee1 4a39      	macl %a1,%d4,>>,%a1@-&,%sp,%acc3
    67bc:	a213 4e09      	macl %a1,%d4,<<,%a3@,%d1,%acc1
    67c0:	a293 4e19      	macl %a1,%d4,<<,%a3@,%d1,%acc3
    67c4:	a653 4e09      	macl %a1,%d4,>>,%a3@,%a3,%acc1
    67c8:	a6d3 4e19      	macl %a1,%d4,>>,%a3@,%a3,%acc3
    67cc:	a413 4e09      	macl %a1,%d4,,%a3@,%d2,%acc1
    67d0:	a493 4e19      	macl %a1,%d4,,%a3@,%d2,%acc3
    67d4:	ae53 4e09      	macl %a1,%d4,>>,%a3@,%sp,%acc1
    67d8:	aed3 4e19      	macl %a1,%d4,>>,%a3@,%sp,%acc3
    67dc:	a213 4e29      	macl %a1,%d4,<<,%a3@&,%d1,%acc1
    67e0:	a293 4e39      	macl %a1,%d4,<<,%a3@&,%d1,%acc3
    67e4:	a653 4e29      	macl %a1,%d4,>>,%a3@&,%a3,%acc1
    67e8:	a6d3 4e39      	macl %a1,%d4,>>,%a3@&,%a3,%acc3
    67ec:	a413 4e29      	macl %a1,%d4,,%a3@&,%d2,%acc1
    67f0:	a493 4e39      	macl %a1,%d4,,%a3@&,%d2,%acc3
    67f4:	ae53 4e29      	macl %a1,%d4,>>,%a3@&,%sp,%acc1
    67f8:	aed3 4e39      	macl %a1,%d4,>>,%a3@&,%sp,%acc3
    67fc:	a21a 4e09      	macl %a1,%d4,<<,%a2@\+,%d1,%acc1
    6800:	a29a 4e19      	macl %a1,%d4,<<,%a2@\+,%d1,%acc3
    6804:	a65a 4e09      	macl %a1,%d4,>>,%a2@\+,%a3,%acc1
    6808:	a6da 4e19      	macl %a1,%d4,>>,%a2@\+,%a3,%acc3
    680c:	a41a 4e09      	macl %a1,%d4,,%a2@\+,%d2,%acc1
    6810:	a49a 4e19      	macl %a1,%d4,,%a2@\+,%d2,%acc3
    6814:	ae5a 4e09      	macl %a1,%d4,>>,%a2@\+,%sp,%acc1
    6818:	aeda 4e19      	macl %a1,%d4,>>,%a2@\+,%sp,%acc3
    681c:	a21a 4e29      	macl %a1,%d4,<<,%a2@\+&,%d1,%acc1
    6820:	a29a 4e39      	macl %a1,%d4,<<,%a2@\+&,%d1,%acc3
    6824:	a65a 4e29      	macl %a1,%d4,>>,%a2@\+&,%a3,%acc1
    6828:	a6da 4e39      	macl %a1,%d4,>>,%a2@\+&,%a3,%acc3
    682c:	a41a 4e29      	macl %a1,%d4,,%a2@\+&,%d2,%acc1
    6830:	a49a 4e39      	macl %a1,%d4,,%a2@\+&,%d2,%acc3
    6834:	ae5a 4e29      	macl %a1,%d4,>>,%a2@\+&,%sp,%acc1
    6838:	aeda 4e39      	macl %a1,%d4,>>,%a2@\+&,%sp,%acc3
    683c:	a22e 4e09 000a 	macl %a1,%d4,<<,%fp@\(10\),%d1,%acc1
    6842:	a2ae 4e19 000a 	macl %a1,%d4,<<,%fp@\(10\),%d1,%acc3
    6848:	a66e 4e09 000a 	macl %a1,%d4,>>,%fp@\(10\),%a3,%acc1
    684e:	a6ee 4e19 000a 	macl %a1,%d4,>>,%fp@\(10\),%a3,%acc3
    6854:	a42e 4e09 000a 	macl %a1,%d4,,%fp@\(10\),%d2,%acc1
    685a:	a4ae 4e19 000a 	macl %a1,%d4,,%fp@\(10\),%d2,%acc3
    6860:	ae6e 4e09 000a 	macl %a1,%d4,>>,%fp@\(10\),%sp,%acc1
    6866:	aeee 4e19 000a 	macl %a1,%d4,>>,%fp@\(10\),%sp,%acc3
    686c:	a22e 4e29 000a 	macl %a1,%d4,<<,%fp@\(10\)&,%d1,%acc1
    6872:	a2ae 4e39 000a 	macl %a1,%d4,<<,%fp@\(10\)&,%d1,%acc3
    6878:	a66e 4e29 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%a3,%acc1
    687e:	a6ee 4e39 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%a3,%acc3
    6884:	a42e 4e29 000a 	macl %a1,%d4,,%fp@\(10\)&,%d2,%acc1
    688a:	a4ae 4e39 000a 	macl %a1,%d4,,%fp@\(10\)&,%d2,%acc3
    6890:	ae6e 4e29 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%sp,%acc1
    6896:	aeee 4e39 000a 	macl %a1,%d4,>>,%fp@\(10\)&,%sp,%acc3
    689c:	a221 4e09      	macl %a1,%d4,<<,%a1@-,%d1,%acc1
    68a0:	a2a1 4e19      	macl %a1,%d4,<<,%a1@-,%d1,%acc3
    68a4:	a661 4e09      	macl %a1,%d4,>>,%a1@-,%a3,%acc1
    68a8:	a6e1 4e19      	macl %a1,%d4,>>,%a1@-,%a3,%acc3
    68ac:	a421 4e09      	macl %a1,%d4,,%a1@-,%d2,%acc1
    68b0:	a4a1 4e19      	macl %a1,%d4,,%a1@-,%d2,%acc3
    68b4:	ae61 4e09      	macl %a1,%d4,>>,%a1@-,%sp,%acc1
    68b8:	aee1 4e19      	macl %a1,%d4,>>,%a1@-,%sp,%acc3
    68bc:	a221 4e29      	macl %a1,%d4,<<,%a1@-&,%d1,%acc1
    68c0:	a2a1 4e39      	macl %a1,%d4,<<,%a1@-&,%d1,%acc3
    68c4:	a661 4e29      	macl %a1,%d4,>>,%a1@-&,%a3,%acc1
    68c8:	a6e1 4e39      	macl %a1,%d4,>>,%a1@-&,%a3,%acc3
    68cc:	a421 4e29      	macl %a1,%d4,,%a1@-&,%d2,%acc1
    68d0:	a4a1 4e39      	macl %a1,%d4,,%a1@-&,%d2,%acc3
    68d4:	ae61 4e29      	macl %a1,%d4,>>,%a1@-&,%sp,%acc1
    68d8:	aee1 4e39      	macl %a1,%d4,>>,%a1@-&,%sp,%acc3
    68dc:	a213 b806      	macl %d6,%a3,%a3@,%d1,%acc0
    68e0:	a293 b816      	macl %d6,%a3,%a3@,%d1,%acc2
    68e4:	a653 b806      	macl %d6,%a3,%a3@,%a3,%acc0
    68e8:	a6d3 b816      	macl %d6,%a3,%a3@,%a3,%acc2
    68ec:	a413 b806      	macl %d6,%a3,%a3@,%d2,%acc0
    68f0:	a493 b816      	macl %d6,%a3,%a3@,%d2,%acc2
    68f4:	ae53 b806      	macl %d6,%a3,%a3@,%sp,%acc0
    68f8:	aed3 b816      	macl %d6,%a3,%a3@,%sp,%acc2
    68fc:	a213 b826      	macl %d6,%a3,%a3@&,%d1,%acc0
    6900:	a293 b836      	macl %d6,%a3,%a3@&,%d1,%acc2
    6904:	a653 b826      	macl %d6,%a3,%a3@&,%a3,%acc0
    6908:	a6d3 b836      	macl %d6,%a3,%a3@&,%a3,%acc2
    690c:	a413 b826      	macl %d6,%a3,%a3@&,%d2,%acc0
    6910:	a493 b836      	macl %d6,%a3,%a3@&,%d2,%acc2
    6914:	ae53 b826      	macl %d6,%a3,%a3@&,%sp,%acc0
    6918:	aed3 b836      	macl %d6,%a3,%a3@&,%sp,%acc2
    691c:	a21a b806      	macl %d6,%a3,%a2@\+,%d1,%acc0
    6920:	a29a b816      	macl %d6,%a3,%a2@\+,%d1,%acc2
    6924:	a65a b806      	macl %d6,%a3,%a2@\+,%a3,%acc0
    6928:	a6da b816      	macl %d6,%a3,%a2@\+,%a3,%acc2
    692c:	a41a b806      	macl %d6,%a3,%a2@\+,%d2,%acc0
    6930:	a49a b816      	macl %d6,%a3,%a2@\+,%d2,%acc2
    6934:	ae5a b806      	macl %d6,%a3,%a2@\+,%sp,%acc0
    6938:	aeda b816      	macl %d6,%a3,%a2@\+,%sp,%acc2
    693c:	a21a b826      	macl %d6,%a3,%a2@\+&,%d1,%acc0
    6940:	a29a b836      	macl %d6,%a3,%a2@\+&,%d1,%acc2
    6944:	a65a b826      	macl %d6,%a3,%a2@\+&,%a3,%acc0
    6948:	a6da b836      	macl %d6,%a3,%a2@\+&,%a3,%acc2
    694c:	a41a b826      	macl %d6,%a3,%a2@\+&,%d2,%acc0
    6950:	a49a b836      	macl %d6,%a3,%a2@\+&,%d2,%acc2
    6954:	ae5a b826      	macl %d6,%a3,%a2@\+&,%sp,%acc0
    6958:	aeda b836      	macl %d6,%a3,%a2@\+&,%sp,%acc2
    695c:	a22e b806 000a 	macl %d6,%a3,%fp@\(10\),%d1,%acc0
    6962:	a2ae b816 000a 	macl %d6,%a3,%fp@\(10\),%d1,%acc2
    6968:	a66e b806 000a 	macl %d6,%a3,%fp@\(10\),%a3,%acc0
    696e:	a6ee b816 000a 	macl %d6,%a3,%fp@\(10\),%a3,%acc2
    6974:	a42e b806 000a 	macl %d6,%a3,%fp@\(10\),%d2,%acc0
    697a:	a4ae b816 000a 	macl %d6,%a3,%fp@\(10\),%d2,%acc2
    6980:	ae6e b806 000a 	macl %d6,%a3,%fp@\(10\),%sp,%acc0
    6986:	aeee b816 000a 	macl %d6,%a3,%fp@\(10\),%sp,%acc2
    698c:	a22e b826 000a 	macl %d6,%a3,%fp@\(10\)&,%d1,%acc0
    6992:	a2ae b836 000a 	macl %d6,%a3,%fp@\(10\)&,%d1,%acc2
    6998:	a66e b826 000a 	macl %d6,%a3,%fp@\(10\)&,%a3,%acc0
    699e:	a6ee b836 000a 	macl %d6,%a3,%fp@\(10\)&,%a3,%acc2
    69a4:	a42e b826 000a 	macl %d6,%a3,%fp@\(10\)&,%d2,%acc0
    69aa:	a4ae b836 000a 	macl %d6,%a3,%fp@\(10\)&,%d2,%acc2
    69b0:	ae6e b826 000a 	macl %d6,%a3,%fp@\(10\)&,%sp,%acc0
    69b6:	aeee b836 000a 	macl %d6,%a3,%fp@\(10\)&,%sp,%acc2
    69bc:	a221 b806      	macl %d6,%a3,%a1@-,%d1,%acc0
    69c0:	a2a1 b816      	macl %d6,%a3,%a1@-,%d1,%acc2
    69c4:	a661 b806      	macl %d6,%a3,%a1@-,%a3,%acc0
    69c8:	a6e1 b816      	macl %d6,%a3,%a1@-,%a3,%acc2
    69cc:	a421 b806      	macl %d6,%a3,%a1@-,%d2,%acc0
    69d0:	a4a1 b816      	macl %d6,%a3,%a1@-,%d2,%acc2
    69d4:	ae61 b806      	macl %d6,%a3,%a1@-,%sp,%acc0
    69d8:	aee1 b816      	macl %d6,%a3,%a1@-,%sp,%acc2
    69dc:	a221 b826      	macl %d6,%a3,%a1@-&,%d1,%acc0
    69e0:	a2a1 b836      	macl %d6,%a3,%a1@-&,%d1,%acc2
    69e4:	a661 b826      	macl %d6,%a3,%a1@-&,%a3,%acc0
    69e8:	a6e1 b836      	macl %d6,%a3,%a1@-&,%a3,%acc2
    69ec:	a421 b826      	macl %d6,%a3,%a1@-&,%d2,%acc0
    69f0:	a4a1 b836      	macl %d6,%a3,%a1@-&,%d2,%acc2
    69f4:	ae61 b826      	macl %d6,%a3,%a1@-&,%sp,%acc0
    69f8:	aee1 b836      	macl %d6,%a3,%a1@-&,%sp,%acc2
    69fc:	a213 ba06      	macl %d6,%a3,<<,%a3@,%d1,%acc0
    6a00:	a293 ba16      	macl %d6,%a3,<<,%a3@,%d1,%acc2
    6a04:	a653 ba06      	macl %d6,%a3,>>,%a3@,%a3,%acc0
    6a08:	a6d3 ba16      	macl %d6,%a3,>>,%a3@,%a3,%acc2
    6a0c:	a413 ba06      	macl %d6,%a3,,%a3@,%d2,%acc0
    6a10:	a493 ba16      	macl %d6,%a3,,%a3@,%d2,%acc2
    6a14:	ae53 ba06      	macl %d6,%a3,>>,%a3@,%sp,%acc0
    6a18:	aed3 ba16      	macl %d6,%a3,>>,%a3@,%sp,%acc2
    6a1c:	a213 ba26      	macl %d6,%a3,<<,%a3@&,%d1,%acc0
    6a20:	a293 ba36      	macl %d6,%a3,<<,%a3@&,%d1,%acc2
    6a24:	a653 ba26      	macl %d6,%a3,>>,%a3@&,%a3,%acc0
    6a28:	a6d3 ba36      	macl %d6,%a3,>>,%a3@&,%a3,%acc2
    6a2c:	a413 ba26      	macl %d6,%a3,,%a3@&,%d2,%acc0
    6a30:	a493 ba36      	macl %d6,%a3,,%a3@&,%d2,%acc2
    6a34:	ae53 ba26      	macl %d6,%a3,>>,%a3@&,%sp,%acc0
    6a38:	aed3 ba36      	macl %d6,%a3,>>,%a3@&,%sp,%acc2
    6a3c:	a21a ba06      	macl %d6,%a3,<<,%a2@\+,%d1,%acc0
    6a40:	a29a ba16      	macl %d6,%a3,<<,%a2@\+,%d1,%acc2
    6a44:	a65a ba06      	macl %d6,%a3,>>,%a2@\+,%a3,%acc0
    6a48:	a6da ba16      	macl %d6,%a3,>>,%a2@\+,%a3,%acc2
    6a4c:	a41a ba06      	macl %d6,%a3,,%a2@\+,%d2,%acc0
    6a50:	a49a ba16      	macl %d6,%a3,,%a2@\+,%d2,%acc2
    6a54:	ae5a ba06      	macl %d6,%a3,>>,%a2@\+,%sp,%acc0
    6a58:	aeda ba16      	macl %d6,%a3,>>,%a2@\+,%sp,%acc2
    6a5c:	a21a ba26      	macl %d6,%a3,<<,%a2@\+&,%d1,%acc0
    6a60:	a29a ba36      	macl %d6,%a3,<<,%a2@\+&,%d1,%acc2
    6a64:	a65a ba26      	macl %d6,%a3,>>,%a2@\+&,%a3,%acc0
    6a68:	a6da ba36      	macl %d6,%a3,>>,%a2@\+&,%a3,%acc2
    6a6c:	a41a ba26      	macl %d6,%a3,,%a2@\+&,%d2,%acc0
    6a70:	a49a ba36      	macl %d6,%a3,,%a2@\+&,%d2,%acc2
    6a74:	ae5a ba26      	macl %d6,%a3,>>,%a2@\+&,%sp,%acc0
    6a78:	aeda ba36      	macl %d6,%a3,>>,%a2@\+&,%sp,%acc2
    6a7c:	a22e ba06 000a 	macl %d6,%a3,<<,%fp@\(10\),%d1,%acc0
    6a82:	a2ae ba16 000a 	macl %d6,%a3,<<,%fp@\(10\),%d1,%acc2
    6a88:	a66e ba06 000a 	macl %d6,%a3,>>,%fp@\(10\),%a3,%acc0
    6a8e:	a6ee ba16 000a 	macl %d6,%a3,>>,%fp@\(10\),%a3,%acc2
    6a94:	a42e ba06 000a 	macl %d6,%a3,,%fp@\(10\),%d2,%acc0
    6a9a:	a4ae ba16 000a 	macl %d6,%a3,,%fp@\(10\),%d2,%acc2
    6aa0:	ae6e ba06 000a 	macl %d6,%a3,>>,%fp@\(10\),%sp,%acc0
    6aa6:	aeee ba16 000a 	macl %d6,%a3,>>,%fp@\(10\),%sp,%acc2
    6aac:	a22e ba26 000a 	macl %d6,%a3,<<,%fp@\(10\)&,%d1,%acc0
    6ab2:	a2ae ba36 000a 	macl %d6,%a3,<<,%fp@\(10\)&,%d1,%acc2
    6ab8:	a66e ba26 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%a3,%acc0
    6abe:	a6ee ba36 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%a3,%acc2
    6ac4:	a42e ba26 000a 	macl %d6,%a3,,%fp@\(10\)&,%d2,%acc0
    6aca:	a4ae ba36 000a 	macl %d6,%a3,,%fp@\(10\)&,%d2,%acc2
    6ad0:	ae6e ba26 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%sp,%acc0
    6ad6:	aeee ba36 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%sp,%acc2
    6adc:	a221 ba06      	macl %d6,%a3,<<,%a1@-,%d1,%acc0
    6ae0:	a2a1 ba16      	macl %d6,%a3,<<,%a1@-,%d1,%acc2
    6ae4:	a661 ba06      	macl %d6,%a3,>>,%a1@-,%a3,%acc0
    6ae8:	a6e1 ba16      	macl %d6,%a3,>>,%a1@-,%a3,%acc2
    6aec:	a421 ba06      	macl %d6,%a3,,%a1@-,%d2,%acc0
    6af0:	a4a1 ba16      	macl %d6,%a3,,%a1@-,%d2,%acc2
    6af4:	ae61 ba06      	macl %d6,%a3,>>,%a1@-,%sp,%acc0
    6af8:	aee1 ba16      	macl %d6,%a3,>>,%a1@-,%sp,%acc2
    6afc:	a221 ba26      	macl %d6,%a3,<<,%a1@-&,%d1,%acc0
    6b00:	a2a1 ba36      	macl %d6,%a3,<<,%a1@-&,%d1,%acc2
    6b04:	a661 ba26      	macl %d6,%a3,>>,%a1@-&,%a3,%acc0
    6b08:	a6e1 ba36      	macl %d6,%a3,>>,%a1@-&,%a3,%acc2
    6b0c:	a421 ba26      	macl %d6,%a3,,%a1@-&,%d2,%acc0
    6b10:	a4a1 ba36      	macl %d6,%a3,,%a1@-&,%d2,%acc2
    6b14:	ae61 ba26      	macl %d6,%a3,>>,%a1@-&,%sp,%acc0
    6b18:	aee1 ba36      	macl %d6,%a3,>>,%a1@-&,%sp,%acc2
    6b1c:	a213 be06      	macl %d6,%a3,<<,%a3@,%d1,%acc0
    6b20:	a293 be16      	macl %d6,%a3,<<,%a3@,%d1,%acc2
    6b24:	a653 be06      	macl %d6,%a3,>>,%a3@,%a3,%acc0
    6b28:	a6d3 be16      	macl %d6,%a3,>>,%a3@,%a3,%acc2
    6b2c:	a413 be06      	macl %d6,%a3,,%a3@,%d2,%acc0
    6b30:	a493 be16      	macl %d6,%a3,,%a3@,%d2,%acc2
    6b34:	ae53 be06      	macl %d6,%a3,>>,%a3@,%sp,%acc0
    6b38:	aed3 be16      	macl %d6,%a3,>>,%a3@,%sp,%acc2
    6b3c:	a213 be26      	macl %d6,%a3,<<,%a3@&,%d1,%acc0
    6b40:	a293 be36      	macl %d6,%a3,<<,%a3@&,%d1,%acc2
    6b44:	a653 be26      	macl %d6,%a3,>>,%a3@&,%a3,%acc0
    6b48:	a6d3 be36      	macl %d6,%a3,>>,%a3@&,%a3,%acc2
    6b4c:	a413 be26      	macl %d6,%a3,,%a3@&,%d2,%acc0
    6b50:	a493 be36      	macl %d6,%a3,,%a3@&,%d2,%acc2
    6b54:	ae53 be26      	macl %d6,%a3,>>,%a3@&,%sp,%acc0
    6b58:	aed3 be36      	macl %d6,%a3,>>,%a3@&,%sp,%acc2
    6b5c:	a21a be06      	macl %d6,%a3,<<,%a2@\+,%d1,%acc0
    6b60:	a29a be16      	macl %d6,%a3,<<,%a2@\+,%d1,%acc2
    6b64:	a65a be06      	macl %d6,%a3,>>,%a2@\+,%a3,%acc0
    6b68:	a6da be16      	macl %d6,%a3,>>,%a2@\+,%a3,%acc2
    6b6c:	a41a be06      	macl %d6,%a3,,%a2@\+,%d2,%acc0
    6b70:	a49a be16      	macl %d6,%a3,,%a2@\+,%d2,%acc2
    6b74:	ae5a be06      	macl %d6,%a3,>>,%a2@\+,%sp,%acc0
    6b78:	aeda be16      	macl %d6,%a3,>>,%a2@\+,%sp,%acc2
    6b7c:	a21a be26      	macl %d6,%a3,<<,%a2@\+&,%d1,%acc0
    6b80:	a29a be36      	macl %d6,%a3,<<,%a2@\+&,%d1,%acc2
    6b84:	a65a be26      	macl %d6,%a3,>>,%a2@\+&,%a3,%acc0
    6b88:	a6da be36      	macl %d6,%a3,>>,%a2@\+&,%a3,%acc2
    6b8c:	a41a be26      	macl %d6,%a3,,%a2@\+&,%d2,%acc0
    6b90:	a49a be36      	macl %d6,%a3,,%a2@\+&,%d2,%acc2
    6b94:	ae5a be26      	macl %d6,%a3,>>,%a2@\+&,%sp,%acc0
    6b98:	aeda be36      	macl %d6,%a3,>>,%a2@\+&,%sp,%acc2
    6b9c:	a22e be06 000a 	macl %d6,%a3,<<,%fp@\(10\),%d1,%acc0
    6ba2:	a2ae be16 000a 	macl %d6,%a3,<<,%fp@\(10\),%d1,%acc2
    6ba8:	a66e be06 000a 	macl %d6,%a3,>>,%fp@\(10\),%a3,%acc0
    6bae:	a6ee be16 000a 	macl %d6,%a3,>>,%fp@\(10\),%a3,%acc2
    6bb4:	a42e be06 000a 	macl %d6,%a3,,%fp@\(10\),%d2,%acc0
    6bba:	a4ae be16 000a 	macl %d6,%a3,,%fp@\(10\),%d2,%acc2
    6bc0:	ae6e be06 000a 	macl %d6,%a3,>>,%fp@\(10\),%sp,%acc0
    6bc6:	aeee be16 000a 	macl %d6,%a3,>>,%fp@\(10\),%sp,%acc2
    6bcc:	a22e be26 000a 	macl %d6,%a3,<<,%fp@\(10\)&,%d1,%acc0
    6bd2:	a2ae be36 000a 	macl %d6,%a3,<<,%fp@\(10\)&,%d1,%acc2
    6bd8:	a66e be26 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%a3,%acc0
    6bde:	a6ee be36 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%a3,%acc2
    6be4:	a42e be26 000a 	macl %d6,%a3,,%fp@\(10\)&,%d2,%acc0
    6bea:	a4ae be36 000a 	macl %d6,%a3,,%fp@\(10\)&,%d2,%acc2
    6bf0:	ae6e be26 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%sp,%acc0
    6bf6:	aeee be36 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%sp,%acc2
    6bfc:	a221 be06      	macl %d6,%a3,<<,%a1@-,%d1,%acc0
    6c00:	a2a1 be16      	macl %d6,%a3,<<,%a1@-,%d1,%acc2
    6c04:	a661 be06      	macl %d6,%a3,>>,%a1@-,%a3,%acc0
    6c08:	a6e1 be16      	macl %d6,%a3,>>,%a1@-,%a3,%acc2
    6c0c:	a421 be06      	macl %d6,%a3,,%a1@-,%d2,%acc0
    6c10:	a4a1 be16      	macl %d6,%a3,,%a1@-,%d2,%acc2
    6c14:	ae61 be06      	macl %d6,%a3,>>,%a1@-,%sp,%acc0
    6c18:	aee1 be16      	macl %d6,%a3,>>,%a1@-,%sp,%acc2
    6c1c:	a221 be26      	macl %d6,%a3,<<,%a1@-&,%d1,%acc0
    6c20:	a2a1 be36      	macl %d6,%a3,<<,%a1@-&,%d1,%acc2
    6c24:	a661 be26      	macl %d6,%a3,>>,%a1@-&,%a3,%acc0
    6c28:	a6e1 be36      	macl %d6,%a3,>>,%a1@-&,%a3,%acc2
    6c2c:	a421 be26      	macl %d6,%a3,,%a1@-&,%d2,%acc0
    6c30:	a4a1 be36      	macl %d6,%a3,,%a1@-&,%d2,%acc2
    6c34:	ae61 be26      	macl %d6,%a3,>>,%a1@-&,%sp,%acc0
    6c38:	aee1 be36      	macl %d6,%a3,>>,%a1@-&,%sp,%acc2
    6c3c:	a213 ba06      	macl %d6,%a3,<<,%a3@,%d1,%acc0
    6c40:	a293 ba16      	macl %d6,%a3,<<,%a3@,%d1,%acc2
    6c44:	a653 ba06      	macl %d6,%a3,>>,%a3@,%a3,%acc0
    6c48:	a6d3 ba16      	macl %d6,%a3,>>,%a3@,%a3,%acc2
    6c4c:	a413 ba06      	macl %d6,%a3,,%a3@,%d2,%acc0
    6c50:	a493 ba16      	macl %d6,%a3,,%a3@,%d2,%acc2
    6c54:	ae53 ba06      	macl %d6,%a3,>>,%a3@,%sp,%acc0
    6c58:	aed3 ba16      	macl %d6,%a3,>>,%a3@,%sp,%acc2
    6c5c:	a213 ba26      	macl %d6,%a3,<<,%a3@&,%d1,%acc0
    6c60:	a293 ba36      	macl %d6,%a3,<<,%a3@&,%d1,%acc2
    6c64:	a653 ba26      	macl %d6,%a3,>>,%a3@&,%a3,%acc0
    6c68:	a6d3 ba36      	macl %d6,%a3,>>,%a3@&,%a3,%acc2
    6c6c:	a413 ba26      	macl %d6,%a3,,%a3@&,%d2,%acc0
    6c70:	a493 ba36      	macl %d6,%a3,,%a3@&,%d2,%acc2
    6c74:	ae53 ba26      	macl %d6,%a3,>>,%a3@&,%sp,%acc0
    6c78:	aed3 ba36      	macl %d6,%a3,>>,%a3@&,%sp,%acc2
    6c7c:	a21a ba06      	macl %d6,%a3,<<,%a2@\+,%d1,%acc0
    6c80:	a29a ba16      	macl %d6,%a3,<<,%a2@\+,%d1,%acc2
    6c84:	a65a ba06      	macl %d6,%a3,>>,%a2@\+,%a3,%acc0
    6c88:	a6da ba16      	macl %d6,%a3,>>,%a2@\+,%a3,%acc2
    6c8c:	a41a ba06      	macl %d6,%a3,,%a2@\+,%d2,%acc0
    6c90:	a49a ba16      	macl %d6,%a3,,%a2@\+,%d2,%acc2
    6c94:	ae5a ba06      	macl %d6,%a3,>>,%a2@\+,%sp,%acc0
    6c98:	aeda ba16      	macl %d6,%a3,>>,%a2@\+,%sp,%acc2
    6c9c:	a21a ba26      	macl %d6,%a3,<<,%a2@\+&,%d1,%acc0
    6ca0:	a29a ba36      	macl %d6,%a3,<<,%a2@\+&,%d1,%acc2
    6ca4:	a65a ba26      	macl %d6,%a3,>>,%a2@\+&,%a3,%acc0
    6ca8:	a6da ba36      	macl %d6,%a3,>>,%a2@\+&,%a3,%acc2
    6cac:	a41a ba26      	macl %d6,%a3,,%a2@\+&,%d2,%acc0
    6cb0:	a49a ba36      	macl %d6,%a3,,%a2@\+&,%d2,%acc2
    6cb4:	ae5a ba26      	macl %d6,%a3,>>,%a2@\+&,%sp,%acc0
    6cb8:	aeda ba36      	macl %d6,%a3,>>,%a2@\+&,%sp,%acc2
    6cbc:	a22e ba06 000a 	macl %d6,%a3,<<,%fp@\(10\),%d1,%acc0
    6cc2:	a2ae ba16 000a 	macl %d6,%a3,<<,%fp@\(10\),%d1,%acc2
    6cc8:	a66e ba06 000a 	macl %d6,%a3,>>,%fp@\(10\),%a3,%acc0
    6cce:	a6ee ba16 000a 	macl %d6,%a3,>>,%fp@\(10\),%a3,%acc2
    6cd4:	a42e ba06 000a 	macl %d6,%a3,,%fp@\(10\),%d2,%acc0
    6cda:	a4ae ba16 000a 	macl %d6,%a3,,%fp@\(10\),%d2,%acc2
    6ce0:	ae6e ba06 000a 	macl %d6,%a3,>>,%fp@\(10\),%sp,%acc0
    6ce6:	aeee ba16 000a 	macl %d6,%a3,>>,%fp@\(10\),%sp,%acc2
    6cec:	a22e ba26 000a 	macl %d6,%a3,<<,%fp@\(10\)&,%d1,%acc0
    6cf2:	a2ae ba36 000a 	macl %d6,%a3,<<,%fp@\(10\)&,%d1,%acc2
    6cf8:	a66e ba26 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%a3,%acc0
    6cfe:	a6ee ba36 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%a3,%acc2
    6d04:	a42e ba26 000a 	macl %d6,%a3,,%fp@\(10\)&,%d2,%acc0
    6d0a:	a4ae ba36 000a 	macl %d6,%a3,,%fp@\(10\)&,%d2,%acc2
    6d10:	ae6e ba26 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%sp,%acc0
    6d16:	aeee ba36 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%sp,%acc2
    6d1c:	a221 ba06      	macl %d6,%a3,<<,%a1@-,%d1,%acc0
    6d20:	a2a1 ba16      	macl %d6,%a3,<<,%a1@-,%d1,%acc2
    6d24:	a661 ba06      	macl %d6,%a3,>>,%a1@-,%a3,%acc0
    6d28:	a6e1 ba16      	macl %d6,%a3,>>,%a1@-,%a3,%acc2
    6d2c:	a421 ba06      	macl %d6,%a3,,%a1@-,%d2,%acc0
    6d30:	a4a1 ba16      	macl %d6,%a3,,%a1@-,%d2,%acc2
    6d34:	ae61 ba06      	macl %d6,%a3,>>,%a1@-,%sp,%acc0
    6d38:	aee1 ba16      	macl %d6,%a3,>>,%a1@-,%sp,%acc2
    6d3c:	a221 ba26      	macl %d6,%a3,<<,%a1@-&,%d1,%acc0
    6d40:	a2a1 ba36      	macl %d6,%a3,<<,%a1@-&,%d1,%acc2
    6d44:	a661 ba26      	macl %d6,%a3,>>,%a1@-&,%a3,%acc0
    6d48:	a6e1 ba36      	macl %d6,%a3,>>,%a1@-&,%a3,%acc2
    6d4c:	a421 ba26      	macl %d6,%a3,,%a1@-&,%d2,%acc0
    6d50:	a4a1 ba36      	macl %d6,%a3,,%a1@-&,%d2,%acc2
    6d54:	ae61 ba26      	macl %d6,%a3,>>,%a1@-&,%sp,%acc0
    6d58:	aee1 ba36      	macl %d6,%a3,>>,%a1@-&,%sp,%acc2
    6d5c:	a213 be06      	macl %d6,%a3,<<,%a3@,%d1,%acc0
    6d60:	a293 be16      	macl %d6,%a3,<<,%a3@,%d1,%acc2
    6d64:	a653 be06      	macl %d6,%a3,>>,%a3@,%a3,%acc0
    6d68:	a6d3 be16      	macl %d6,%a3,>>,%a3@,%a3,%acc2
    6d6c:	a413 be06      	macl %d6,%a3,,%a3@,%d2,%acc0
    6d70:	a493 be16      	macl %d6,%a3,,%a3@,%d2,%acc2
    6d74:	ae53 be06      	macl %d6,%a3,>>,%a3@,%sp,%acc0
    6d78:	aed3 be16      	macl %d6,%a3,>>,%a3@,%sp,%acc2
    6d7c:	a213 be26      	macl %d6,%a3,<<,%a3@&,%d1,%acc0
    6d80:	a293 be36      	macl %d6,%a3,<<,%a3@&,%d1,%acc2
    6d84:	a653 be26      	macl %d6,%a3,>>,%a3@&,%a3,%acc0
    6d88:	a6d3 be36      	macl %d6,%a3,>>,%a3@&,%a3,%acc2
    6d8c:	a413 be26      	macl %d6,%a3,,%a3@&,%d2,%acc0
    6d90:	a493 be36      	macl %d6,%a3,,%a3@&,%d2,%acc2
    6d94:	ae53 be26      	macl %d6,%a3,>>,%a3@&,%sp,%acc0
    6d98:	aed3 be36      	macl %d6,%a3,>>,%a3@&,%sp,%acc2
    6d9c:	a21a be06      	macl %d6,%a3,<<,%a2@\+,%d1,%acc0
    6da0:	a29a be16      	macl %d6,%a3,<<,%a2@\+,%d1,%acc2
    6da4:	a65a be06      	macl %d6,%a3,>>,%a2@\+,%a3,%acc0
    6da8:	a6da be16      	macl %d6,%a3,>>,%a2@\+,%a3,%acc2
    6dac:	a41a be06      	macl %d6,%a3,,%a2@\+,%d2,%acc0
    6db0:	a49a be16      	macl %d6,%a3,,%a2@\+,%d2,%acc2
    6db4:	ae5a be06      	macl %d6,%a3,>>,%a2@\+,%sp,%acc0
    6db8:	aeda be16      	macl %d6,%a3,>>,%a2@\+,%sp,%acc2
    6dbc:	a21a be26      	macl %d6,%a3,<<,%a2@\+&,%d1,%acc0
    6dc0:	a29a be36      	macl %d6,%a3,<<,%a2@\+&,%d1,%acc2
    6dc4:	a65a be26      	macl %d6,%a3,>>,%a2@\+&,%a3,%acc0
    6dc8:	a6da be36      	macl %d6,%a3,>>,%a2@\+&,%a3,%acc2
    6dcc:	a41a be26      	macl %d6,%a3,,%a2@\+&,%d2,%acc0
    6dd0:	a49a be36      	macl %d6,%a3,,%a2@\+&,%d2,%acc2
    6dd4:	ae5a be26      	macl %d6,%a3,>>,%a2@\+&,%sp,%acc0
    6dd8:	aeda be36      	macl %d6,%a3,>>,%a2@\+&,%sp,%acc2
    6ddc:	a22e be06 000a 	macl %d6,%a3,<<,%fp@\(10\),%d1,%acc0
    6de2:	a2ae be16 000a 	macl %d6,%a3,<<,%fp@\(10\),%d1,%acc2
    6de8:	a66e be06 000a 	macl %d6,%a3,>>,%fp@\(10\),%a3,%acc0
    6dee:	a6ee be16 000a 	macl %d6,%a3,>>,%fp@\(10\),%a3,%acc2
    6df4:	a42e be06 000a 	macl %d6,%a3,,%fp@\(10\),%d2,%acc0
    6dfa:	a4ae be16 000a 	macl %d6,%a3,,%fp@\(10\),%d2,%acc2
    6e00:	ae6e be06 000a 	macl %d6,%a3,>>,%fp@\(10\),%sp,%acc0
    6e06:	aeee be16 000a 	macl %d6,%a3,>>,%fp@\(10\),%sp,%acc2
    6e0c:	a22e be26 000a 	macl %d6,%a3,<<,%fp@\(10\)&,%d1,%acc0
    6e12:	a2ae be36 000a 	macl %d6,%a3,<<,%fp@\(10\)&,%d1,%acc2
    6e18:	a66e be26 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%a3,%acc0
    6e1e:	a6ee be36 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%a3,%acc2
    6e24:	a42e be26 000a 	macl %d6,%a3,,%fp@\(10\)&,%d2,%acc0
    6e2a:	a4ae be36 000a 	macl %d6,%a3,,%fp@\(10\)&,%d2,%acc2
    6e30:	ae6e be26 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%sp,%acc0
    6e36:	aeee be36 000a 	macl %d6,%a3,>>,%fp@\(10\)&,%sp,%acc2
    6e3c:	a221 be06      	macl %d6,%a3,<<,%a1@-,%d1,%acc0
    6e40:	a2a1 be16      	macl %d6,%a3,<<,%a1@-,%d1,%acc2
    6e44:	a661 be06      	macl %d6,%a3,>>,%a1@-,%a3,%acc0
    6e48:	a6e1 be16      	macl %d6,%a3,>>,%a1@-,%a3,%acc2
    6e4c:	a421 be06      	macl %d6,%a3,,%a1@-,%d2,%acc0
    6e50:	a4a1 be16      	macl %d6,%a3,,%a1@-,%d2,%acc2
    6e54:	ae61 be06      	macl %d6,%a3,>>,%a1@-,%sp,%acc0
    6e58:	aee1 be16      	macl %d6,%a3,>>,%a1@-,%sp,%acc2
    6e5c:	a221 be26      	macl %d6,%a3,<<,%a1@-&,%d1,%acc0
    6e60:	a2a1 be36      	macl %d6,%a3,<<,%a1@-&,%d1,%acc2
    6e64:	a661 be26      	macl %d6,%a3,>>,%a1@-&,%a3,%acc0
    6e68:	a6e1 be36      	macl %d6,%a3,>>,%a1@-&,%a3,%acc2
    6e6c:	a421 be26      	macl %d6,%a3,,%a1@-&,%d2,%acc0
    6e70:	a4a1 be36      	macl %d6,%a3,,%a1@-&,%d2,%acc2
    6e74:	ae61 be26      	macl %d6,%a3,>>,%a1@-&,%sp,%acc0
    6e78:	aee1 be36      	macl %d6,%a3,>>,%a1@-&,%sp,%acc2
    6e7c:	a213 4806      	macl %d6,%d4,%a3@,%d1,%acc1
    6e80:	a293 4816      	macl %d6,%d4,%a3@,%d1,%acc3
    6e84:	a653 4806      	macl %d6,%d4,%a3@,%a3,%acc1
    6e88:	a6d3 4816      	macl %d6,%d4,%a3@,%a3,%acc3
    6e8c:	a413 4806      	macl %d6,%d4,%a3@,%d2,%acc1
    6e90:	a493 4816      	macl %d6,%d4,%a3@,%d2,%acc3
    6e94:	ae53 4806      	macl %d6,%d4,%a3@,%sp,%acc1
    6e98:	aed3 4816      	macl %d6,%d4,%a3@,%sp,%acc3
    6e9c:	a213 4826      	macl %d6,%d4,%a3@&,%d1,%acc1
    6ea0:	a293 4836      	macl %d6,%d4,%a3@&,%d1,%acc3
    6ea4:	a653 4826      	macl %d6,%d4,%a3@&,%a3,%acc1
    6ea8:	a6d3 4836      	macl %d6,%d4,%a3@&,%a3,%acc3
    6eac:	a413 4826      	macl %d6,%d4,%a3@&,%d2,%acc1
    6eb0:	a493 4836      	macl %d6,%d4,%a3@&,%d2,%acc3
    6eb4:	ae53 4826      	macl %d6,%d4,%a3@&,%sp,%acc1
    6eb8:	aed3 4836      	macl %d6,%d4,%a3@&,%sp,%acc3
    6ebc:	a21a 4806      	macl %d6,%d4,%a2@\+,%d1,%acc1
    6ec0:	a29a 4816      	macl %d6,%d4,%a2@\+,%d1,%acc3
    6ec4:	a65a 4806      	macl %d6,%d4,%a2@\+,%a3,%acc1
    6ec8:	a6da 4816      	macl %d6,%d4,%a2@\+,%a3,%acc3
    6ecc:	a41a 4806      	macl %d6,%d4,%a2@\+,%d2,%acc1
    6ed0:	a49a 4816      	macl %d6,%d4,%a2@\+,%d2,%acc3
    6ed4:	ae5a 4806      	macl %d6,%d4,%a2@\+,%sp,%acc1
    6ed8:	aeda 4816      	macl %d6,%d4,%a2@\+,%sp,%acc3
    6edc:	a21a 4826      	macl %d6,%d4,%a2@\+&,%d1,%acc1
    6ee0:	a29a 4836      	macl %d6,%d4,%a2@\+&,%d1,%acc3
    6ee4:	a65a 4826      	macl %d6,%d4,%a2@\+&,%a3,%acc1
    6ee8:	a6da 4836      	macl %d6,%d4,%a2@\+&,%a3,%acc3
    6eec:	a41a 4826      	macl %d6,%d4,%a2@\+&,%d2,%acc1
    6ef0:	a49a 4836      	macl %d6,%d4,%a2@\+&,%d2,%acc3
    6ef4:	ae5a 4826      	macl %d6,%d4,%a2@\+&,%sp,%acc1
    6ef8:	aeda 4836      	macl %d6,%d4,%a2@\+&,%sp,%acc3
    6efc:	a22e 4806 000a 	macl %d6,%d4,%fp@\(10\),%d1,%acc1
    6f02:	a2ae 4816 000a 	macl %d6,%d4,%fp@\(10\),%d1,%acc3
    6f08:	a66e 4806 000a 	macl %d6,%d4,%fp@\(10\),%a3,%acc1
    6f0e:	a6ee 4816 000a 	macl %d6,%d4,%fp@\(10\),%a3,%acc3
    6f14:	a42e 4806 000a 	macl %d6,%d4,%fp@\(10\),%d2,%acc1
    6f1a:	a4ae 4816 000a 	macl %d6,%d4,%fp@\(10\),%d2,%acc3
    6f20:	ae6e 4806 000a 	macl %d6,%d4,%fp@\(10\),%sp,%acc1
    6f26:	aeee 4816 000a 	macl %d6,%d4,%fp@\(10\),%sp,%acc3
    6f2c:	a22e 4826 000a 	macl %d6,%d4,%fp@\(10\)&,%d1,%acc1
    6f32:	a2ae 4836 000a 	macl %d6,%d4,%fp@\(10\)&,%d1,%acc3
    6f38:	a66e 4826 000a 	macl %d6,%d4,%fp@\(10\)&,%a3,%acc1
    6f3e:	a6ee 4836 000a 	macl %d6,%d4,%fp@\(10\)&,%a3,%acc3
    6f44:	a42e 4826 000a 	macl %d6,%d4,%fp@\(10\)&,%d2,%acc1
    6f4a:	a4ae 4836 000a 	macl %d6,%d4,%fp@\(10\)&,%d2,%acc3
    6f50:	ae6e 4826 000a 	macl %d6,%d4,%fp@\(10\)&,%sp,%acc1
    6f56:	aeee 4836 000a 	macl %d6,%d4,%fp@\(10\)&,%sp,%acc3
    6f5c:	a221 4806      	macl %d6,%d4,%a1@-,%d1,%acc1
    6f60:	a2a1 4816      	macl %d6,%d4,%a1@-,%d1,%acc3
    6f64:	a661 4806      	macl %d6,%d4,%a1@-,%a3,%acc1
    6f68:	a6e1 4816      	macl %d6,%d4,%a1@-,%a3,%acc3
    6f6c:	a421 4806      	macl %d6,%d4,%a1@-,%d2,%acc1
    6f70:	a4a1 4816      	macl %d6,%d4,%a1@-,%d2,%acc3
    6f74:	ae61 4806      	macl %d6,%d4,%a1@-,%sp,%acc1
    6f78:	aee1 4816      	macl %d6,%d4,%a1@-,%sp,%acc3
    6f7c:	a221 4826      	macl %d6,%d4,%a1@-&,%d1,%acc1
    6f80:	a2a1 4836      	macl %d6,%d4,%a1@-&,%d1,%acc3
    6f84:	a661 4826      	macl %d6,%d4,%a1@-&,%a3,%acc1
    6f88:	a6e1 4836      	macl %d6,%d4,%a1@-&,%a3,%acc3
    6f8c:	a421 4826      	macl %d6,%d4,%a1@-&,%d2,%acc1
    6f90:	a4a1 4836      	macl %d6,%d4,%a1@-&,%d2,%acc3
    6f94:	ae61 4826      	macl %d6,%d4,%a1@-&,%sp,%acc1
    6f98:	aee1 4836      	macl %d6,%d4,%a1@-&,%sp,%acc3
    6f9c:	a213 4a06      	macl %d6,%d4,<<,%a3@,%d1,%acc1
    6fa0:	a293 4a16      	macl %d6,%d4,<<,%a3@,%d1,%acc3
    6fa4:	a653 4a06      	macl %d6,%d4,>>,%a3@,%a3,%acc1
    6fa8:	a6d3 4a16      	macl %d6,%d4,>>,%a3@,%a3,%acc3
    6fac:	a413 4a06      	macl %d6,%d4,,%a3@,%d2,%acc1
    6fb0:	a493 4a16      	macl %d6,%d4,,%a3@,%d2,%acc3
    6fb4:	ae53 4a06      	macl %d6,%d4,>>,%a3@,%sp,%acc1
    6fb8:	aed3 4a16      	macl %d6,%d4,>>,%a3@,%sp,%acc3
    6fbc:	a213 4a26      	macl %d6,%d4,<<,%a3@&,%d1,%acc1
    6fc0:	a293 4a36      	macl %d6,%d4,<<,%a3@&,%d1,%acc3
    6fc4:	a653 4a26      	macl %d6,%d4,>>,%a3@&,%a3,%acc1
    6fc8:	a6d3 4a36      	macl %d6,%d4,>>,%a3@&,%a3,%acc3
    6fcc:	a413 4a26      	macl %d6,%d4,,%a3@&,%d2,%acc1
    6fd0:	a493 4a36      	macl %d6,%d4,,%a3@&,%d2,%acc3
    6fd4:	ae53 4a26      	macl %d6,%d4,>>,%a3@&,%sp,%acc1
    6fd8:	aed3 4a36      	macl %d6,%d4,>>,%a3@&,%sp,%acc3
    6fdc:	a21a 4a06      	macl %d6,%d4,<<,%a2@\+,%d1,%acc1
    6fe0:	a29a 4a16      	macl %d6,%d4,<<,%a2@\+,%d1,%acc3
    6fe4:	a65a 4a06      	macl %d6,%d4,>>,%a2@\+,%a3,%acc1
    6fe8:	a6da 4a16      	macl %d6,%d4,>>,%a2@\+,%a3,%acc3
    6fec:	a41a 4a06      	macl %d6,%d4,,%a2@\+,%d2,%acc1
    6ff0:	a49a 4a16      	macl %d6,%d4,,%a2@\+,%d2,%acc3
    6ff4:	ae5a 4a06      	macl %d6,%d4,>>,%a2@\+,%sp,%acc1
    6ff8:	aeda 4a16      	macl %d6,%d4,>>,%a2@\+,%sp,%acc3
    6ffc:	a21a 4a26      	macl %d6,%d4,<<,%a2@\+&,%d1,%acc1
    7000:	a29a 4a36      	macl %d6,%d4,<<,%a2@\+&,%d1,%acc3
    7004:	a65a 4a26      	macl %d6,%d4,>>,%a2@\+&,%a3,%acc1
    7008:	a6da 4a36      	macl %d6,%d4,>>,%a2@\+&,%a3,%acc3
    700c:	a41a 4a26      	macl %d6,%d4,,%a2@\+&,%d2,%acc1
    7010:	a49a 4a36      	macl %d6,%d4,,%a2@\+&,%d2,%acc3
    7014:	ae5a 4a26      	macl %d6,%d4,>>,%a2@\+&,%sp,%acc1
    7018:	aeda 4a36      	macl %d6,%d4,>>,%a2@\+&,%sp,%acc3
    701c:	a22e 4a06 000a 	macl %d6,%d4,<<,%fp@\(10\),%d1,%acc1
    7022:	a2ae 4a16 000a 	macl %d6,%d4,<<,%fp@\(10\),%d1,%acc3
    7028:	a66e 4a06 000a 	macl %d6,%d4,>>,%fp@\(10\),%a3,%acc1
    702e:	a6ee 4a16 000a 	macl %d6,%d4,>>,%fp@\(10\),%a3,%acc3
    7034:	a42e 4a06 000a 	macl %d6,%d4,,%fp@\(10\),%d2,%acc1
    703a:	a4ae 4a16 000a 	macl %d6,%d4,,%fp@\(10\),%d2,%acc3
    7040:	ae6e 4a06 000a 	macl %d6,%d4,>>,%fp@\(10\),%sp,%acc1
    7046:	aeee 4a16 000a 	macl %d6,%d4,>>,%fp@\(10\),%sp,%acc3
    704c:	a22e 4a26 000a 	macl %d6,%d4,<<,%fp@\(10\)&,%d1,%acc1
    7052:	a2ae 4a36 000a 	macl %d6,%d4,<<,%fp@\(10\)&,%d1,%acc3
    7058:	a66e 4a26 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%a3,%acc1
    705e:	a6ee 4a36 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%a3,%acc3
    7064:	a42e 4a26 000a 	macl %d6,%d4,,%fp@\(10\)&,%d2,%acc1
    706a:	a4ae 4a36 000a 	macl %d6,%d4,,%fp@\(10\)&,%d2,%acc3
    7070:	ae6e 4a26 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%sp,%acc1
    7076:	aeee 4a36 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%sp,%acc3
    707c:	a221 4a06      	macl %d6,%d4,<<,%a1@-,%d1,%acc1
    7080:	a2a1 4a16      	macl %d6,%d4,<<,%a1@-,%d1,%acc3
    7084:	a661 4a06      	macl %d6,%d4,>>,%a1@-,%a3,%acc1
    7088:	a6e1 4a16      	macl %d6,%d4,>>,%a1@-,%a3,%acc3
    708c:	a421 4a06      	macl %d6,%d4,,%a1@-,%d2,%acc1
    7090:	a4a1 4a16      	macl %d6,%d4,,%a1@-,%d2,%acc3
    7094:	ae61 4a06      	macl %d6,%d4,>>,%a1@-,%sp,%acc1
    7098:	aee1 4a16      	macl %d6,%d4,>>,%a1@-,%sp,%acc3
    709c:	a221 4a26      	macl %d6,%d4,<<,%a1@-&,%d1,%acc1
    70a0:	a2a1 4a36      	macl %d6,%d4,<<,%a1@-&,%d1,%acc3
    70a4:	a661 4a26      	macl %d6,%d4,>>,%a1@-&,%a3,%acc1
    70a8:	a6e1 4a36      	macl %d6,%d4,>>,%a1@-&,%a3,%acc3
    70ac:	a421 4a26      	macl %d6,%d4,,%a1@-&,%d2,%acc1
    70b0:	a4a1 4a36      	macl %d6,%d4,,%a1@-&,%d2,%acc3
    70b4:	ae61 4a26      	macl %d6,%d4,>>,%a1@-&,%sp,%acc1
    70b8:	aee1 4a36      	macl %d6,%d4,>>,%a1@-&,%sp,%acc3
    70bc:	a213 4e06      	macl %d6,%d4,<<,%a3@,%d1,%acc1
    70c0:	a293 4e16      	macl %d6,%d4,<<,%a3@,%d1,%acc3
    70c4:	a653 4e06      	macl %d6,%d4,>>,%a3@,%a3,%acc1
    70c8:	a6d3 4e16      	macl %d6,%d4,>>,%a3@,%a3,%acc3
    70cc:	a413 4e06      	macl %d6,%d4,,%a3@,%d2,%acc1
    70d0:	a493 4e16      	macl %d6,%d4,,%a3@,%d2,%acc3
    70d4:	ae53 4e06      	macl %d6,%d4,>>,%a3@,%sp,%acc1
    70d8:	aed3 4e16      	macl %d6,%d4,>>,%a3@,%sp,%acc3
    70dc:	a213 4e26      	macl %d6,%d4,<<,%a3@&,%d1,%acc1
    70e0:	a293 4e36      	macl %d6,%d4,<<,%a3@&,%d1,%acc3
    70e4:	a653 4e26      	macl %d6,%d4,>>,%a3@&,%a3,%acc1
    70e8:	a6d3 4e36      	macl %d6,%d4,>>,%a3@&,%a3,%acc3
    70ec:	a413 4e26      	macl %d6,%d4,,%a3@&,%d2,%acc1
    70f0:	a493 4e36      	macl %d6,%d4,,%a3@&,%d2,%acc3
    70f4:	ae53 4e26      	macl %d6,%d4,>>,%a3@&,%sp,%acc1
    70f8:	aed3 4e36      	macl %d6,%d4,>>,%a3@&,%sp,%acc3
    70fc:	a21a 4e06      	macl %d6,%d4,<<,%a2@\+,%d1,%acc1
    7100:	a29a 4e16      	macl %d6,%d4,<<,%a2@\+,%d1,%acc3
    7104:	a65a 4e06      	macl %d6,%d4,>>,%a2@\+,%a3,%acc1
    7108:	a6da 4e16      	macl %d6,%d4,>>,%a2@\+,%a3,%acc3
    710c:	a41a 4e06      	macl %d6,%d4,,%a2@\+,%d2,%acc1
    7110:	a49a 4e16      	macl %d6,%d4,,%a2@\+,%d2,%acc3
    7114:	ae5a 4e06      	macl %d6,%d4,>>,%a2@\+,%sp,%acc1
    7118:	aeda 4e16      	macl %d6,%d4,>>,%a2@\+,%sp,%acc3
    711c:	a21a 4e26      	macl %d6,%d4,<<,%a2@\+&,%d1,%acc1
    7120:	a29a 4e36      	macl %d6,%d4,<<,%a2@\+&,%d1,%acc3
    7124:	a65a 4e26      	macl %d6,%d4,>>,%a2@\+&,%a3,%acc1
    7128:	a6da 4e36      	macl %d6,%d4,>>,%a2@\+&,%a3,%acc3
    712c:	a41a 4e26      	macl %d6,%d4,,%a2@\+&,%d2,%acc1
    7130:	a49a 4e36      	macl %d6,%d4,,%a2@\+&,%d2,%acc3
    7134:	ae5a 4e26      	macl %d6,%d4,>>,%a2@\+&,%sp,%acc1
    7138:	aeda 4e36      	macl %d6,%d4,>>,%a2@\+&,%sp,%acc3
    713c:	a22e 4e06 000a 	macl %d6,%d4,<<,%fp@\(10\),%d1,%acc1
    7142:	a2ae 4e16 000a 	macl %d6,%d4,<<,%fp@\(10\),%d1,%acc3
    7148:	a66e 4e06 000a 	macl %d6,%d4,>>,%fp@\(10\),%a3,%acc1
    714e:	a6ee 4e16 000a 	macl %d6,%d4,>>,%fp@\(10\),%a3,%acc3
    7154:	a42e 4e06 000a 	macl %d6,%d4,,%fp@\(10\),%d2,%acc1
    715a:	a4ae 4e16 000a 	macl %d6,%d4,,%fp@\(10\),%d2,%acc3
    7160:	ae6e 4e06 000a 	macl %d6,%d4,>>,%fp@\(10\),%sp,%acc1
    7166:	aeee 4e16 000a 	macl %d6,%d4,>>,%fp@\(10\),%sp,%acc3
    716c:	a22e 4e26 000a 	macl %d6,%d4,<<,%fp@\(10\)&,%d1,%acc1
    7172:	a2ae 4e36 000a 	macl %d6,%d4,<<,%fp@\(10\)&,%d1,%acc3
    7178:	a66e 4e26 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%a3,%acc1
    717e:	a6ee 4e36 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%a3,%acc3
    7184:	a42e 4e26 000a 	macl %d6,%d4,,%fp@\(10\)&,%d2,%acc1
    718a:	a4ae 4e36 000a 	macl %d6,%d4,,%fp@\(10\)&,%d2,%acc3
    7190:	ae6e 4e26 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%sp,%acc1
    7196:	aeee 4e36 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%sp,%acc3
    719c:	a221 4e06      	macl %d6,%d4,<<,%a1@-,%d1,%acc1
    71a0:	a2a1 4e16      	macl %d6,%d4,<<,%a1@-,%d1,%acc3
    71a4:	a661 4e06      	macl %d6,%d4,>>,%a1@-,%a3,%acc1
    71a8:	a6e1 4e16      	macl %d6,%d4,>>,%a1@-,%a3,%acc3
    71ac:	a421 4e06      	macl %d6,%d4,,%a1@-,%d2,%acc1
    71b0:	a4a1 4e16      	macl %d6,%d4,,%a1@-,%d2,%acc3
    71b4:	ae61 4e06      	macl %d6,%d4,>>,%a1@-,%sp,%acc1
    71b8:	aee1 4e16      	macl %d6,%d4,>>,%a1@-,%sp,%acc3
    71bc:	a221 4e26      	macl %d6,%d4,<<,%a1@-&,%d1,%acc1
    71c0:	a2a1 4e36      	macl %d6,%d4,<<,%a1@-&,%d1,%acc3
    71c4:	a661 4e26      	macl %d6,%d4,>>,%a1@-&,%a3,%acc1
    71c8:	a6e1 4e36      	macl %d6,%d4,>>,%a1@-&,%a3,%acc3
    71cc:	a421 4e26      	macl %d6,%d4,,%a1@-&,%d2,%acc1
    71d0:	a4a1 4e36      	macl %d6,%d4,,%a1@-&,%d2,%acc3
    71d4:	ae61 4e26      	macl %d6,%d4,>>,%a1@-&,%sp,%acc1
    71d8:	aee1 4e36      	macl %d6,%d4,>>,%a1@-&,%sp,%acc3
    71dc:	a213 4a06      	macl %d6,%d4,<<,%a3@,%d1,%acc1
    71e0:	a293 4a16      	macl %d6,%d4,<<,%a3@,%d1,%acc3
    71e4:	a653 4a06      	macl %d6,%d4,>>,%a3@,%a3,%acc1
    71e8:	a6d3 4a16      	macl %d6,%d4,>>,%a3@,%a3,%acc3
    71ec:	a413 4a06      	macl %d6,%d4,,%a3@,%d2,%acc1
    71f0:	a493 4a16      	macl %d6,%d4,,%a3@,%d2,%acc3
    71f4:	ae53 4a06      	macl %d6,%d4,>>,%a3@,%sp,%acc1
    71f8:	aed3 4a16      	macl %d6,%d4,>>,%a3@,%sp,%acc3
    71fc:	a213 4a26      	macl %d6,%d4,<<,%a3@&,%d1,%acc1
    7200:	a293 4a36      	macl %d6,%d4,<<,%a3@&,%d1,%acc3
    7204:	a653 4a26      	macl %d6,%d4,>>,%a3@&,%a3,%acc1
    7208:	a6d3 4a36      	macl %d6,%d4,>>,%a3@&,%a3,%acc3
    720c:	a413 4a26      	macl %d6,%d4,,%a3@&,%d2,%acc1
    7210:	a493 4a36      	macl %d6,%d4,,%a3@&,%d2,%acc3
    7214:	ae53 4a26      	macl %d6,%d4,>>,%a3@&,%sp,%acc1
    7218:	aed3 4a36      	macl %d6,%d4,>>,%a3@&,%sp,%acc3
    721c:	a21a 4a06      	macl %d6,%d4,<<,%a2@\+,%d1,%acc1
    7220:	a29a 4a16      	macl %d6,%d4,<<,%a2@\+,%d1,%acc3
    7224:	a65a 4a06      	macl %d6,%d4,>>,%a2@\+,%a3,%acc1
    7228:	a6da 4a16      	macl %d6,%d4,>>,%a2@\+,%a3,%acc3
    722c:	a41a 4a06      	macl %d6,%d4,,%a2@\+,%d2,%acc1
    7230:	a49a 4a16      	macl %d6,%d4,,%a2@\+,%d2,%acc3
    7234:	ae5a 4a06      	macl %d6,%d4,>>,%a2@\+,%sp,%acc1
    7238:	aeda 4a16      	macl %d6,%d4,>>,%a2@\+,%sp,%acc3
    723c:	a21a 4a26      	macl %d6,%d4,<<,%a2@\+&,%d1,%acc1
    7240:	a29a 4a36      	macl %d6,%d4,<<,%a2@\+&,%d1,%acc3
    7244:	a65a 4a26      	macl %d6,%d4,>>,%a2@\+&,%a3,%acc1
    7248:	a6da 4a36      	macl %d6,%d4,>>,%a2@\+&,%a3,%acc3
    724c:	a41a 4a26      	macl %d6,%d4,,%a2@\+&,%d2,%acc1
    7250:	a49a 4a36      	macl %d6,%d4,,%a2@\+&,%d2,%acc3
    7254:	ae5a 4a26      	macl %d6,%d4,>>,%a2@\+&,%sp,%acc1
    7258:	aeda 4a36      	macl %d6,%d4,>>,%a2@\+&,%sp,%acc3
    725c:	a22e 4a06 000a 	macl %d6,%d4,<<,%fp@\(10\),%d1,%acc1
    7262:	a2ae 4a16 000a 	macl %d6,%d4,<<,%fp@\(10\),%d1,%acc3
    7268:	a66e 4a06 000a 	macl %d6,%d4,>>,%fp@\(10\),%a3,%acc1
    726e:	a6ee 4a16 000a 	macl %d6,%d4,>>,%fp@\(10\),%a3,%acc3
    7274:	a42e 4a06 000a 	macl %d6,%d4,,%fp@\(10\),%d2,%acc1
    727a:	a4ae 4a16 000a 	macl %d6,%d4,,%fp@\(10\),%d2,%acc3
    7280:	ae6e 4a06 000a 	macl %d6,%d4,>>,%fp@\(10\),%sp,%acc1
    7286:	aeee 4a16 000a 	macl %d6,%d4,>>,%fp@\(10\),%sp,%acc3
    728c:	a22e 4a26 000a 	macl %d6,%d4,<<,%fp@\(10\)&,%d1,%acc1
    7292:	a2ae 4a36 000a 	macl %d6,%d4,<<,%fp@\(10\)&,%d1,%acc3
    7298:	a66e 4a26 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%a3,%acc1
    729e:	a6ee 4a36 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%a3,%acc3
    72a4:	a42e 4a26 000a 	macl %d6,%d4,,%fp@\(10\)&,%d2,%acc1
    72aa:	a4ae 4a36 000a 	macl %d6,%d4,,%fp@\(10\)&,%d2,%acc3
    72b0:	ae6e 4a26 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%sp,%acc1
    72b6:	aeee 4a36 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%sp,%acc3
    72bc:	a221 4a06      	macl %d6,%d4,<<,%a1@-,%d1,%acc1
    72c0:	a2a1 4a16      	macl %d6,%d4,<<,%a1@-,%d1,%acc3
    72c4:	a661 4a06      	macl %d6,%d4,>>,%a1@-,%a3,%acc1
    72c8:	a6e1 4a16      	macl %d6,%d4,>>,%a1@-,%a3,%acc3
    72cc:	a421 4a06      	macl %d6,%d4,,%a1@-,%d2,%acc1
    72d0:	a4a1 4a16      	macl %d6,%d4,,%a1@-,%d2,%acc3
    72d4:	ae61 4a06      	macl %d6,%d4,>>,%a1@-,%sp,%acc1
    72d8:	aee1 4a16      	macl %d6,%d4,>>,%a1@-,%sp,%acc3
    72dc:	a221 4a26      	macl %d6,%d4,<<,%a1@-&,%d1,%acc1
    72e0:	a2a1 4a36      	macl %d6,%d4,<<,%a1@-&,%d1,%acc3
    72e4:	a661 4a26      	macl %d6,%d4,>>,%a1@-&,%a3,%acc1
    72e8:	a6e1 4a36      	macl %d6,%d4,>>,%a1@-&,%a3,%acc3
    72ec:	a421 4a26      	macl %d6,%d4,,%a1@-&,%d2,%acc1
    72f0:	a4a1 4a36      	macl %d6,%d4,,%a1@-&,%d2,%acc3
    72f4:	ae61 4a26      	macl %d6,%d4,>>,%a1@-&,%sp,%acc1
    72f8:	aee1 4a36      	macl %d6,%d4,>>,%a1@-&,%sp,%acc3
    72fc:	a213 4e06      	macl %d6,%d4,<<,%a3@,%d1,%acc1
    7300:	a293 4e16      	macl %d6,%d4,<<,%a3@,%d1,%acc3
    7304:	a653 4e06      	macl %d6,%d4,>>,%a3@,%a3,%acc1
    7308:	a6d3 4e16      	macl %d6,%d4,>>,%a3@,%a3,%acc3
    730c:	a413 4e06      	macl %d6,%d4,,%a3@,%d2,%acc1
    7310:	a493 4e16      	macl %d6,%d4,,%a3@,%d2,%acc3
    7314:	ae53 4e06      	macl %d6,%d4,>>,%a3@,%sp,%acc1
    7318:	aed3 4e16      	macl %d6,%d4,>>,%a3@,%sp,%acc3
    731c:	a213 4e26      	macl %d6,%d4,<<,%a3@&,%d1,%acc1
    7320:	a293 4e36      	macl %d6,%d4,<<,%a3@&,%d1,%acc3
    7324:	a653 4e26      	macl %d6,%d4,>>,%a3@&,%a3,%acc1
    7328:	a6d3 4e36      	macl %d6,%d4,>>,%a3@&,%a3,%acc3
    732c:	a413 4e26      	macl %d6,%d4,,%a3@&,%d2,%acc1
    7330:	a493 4e36      	macl %d6,%d4,,%a3@&,%d2,%acc3
    7334:	ae53 4e26      	macl %d6,%d4,>>,%a3@&,%sp,%acc1
    7338:	aed3 4e36      	macl %d6,%d4,>>,%a3@&,%sp,%acc3
    733c:	a21a 4e06      	macl %d6,%d4,<<,%a2@\+,%d1,%acc1
    7340:	a29a 4e16      	macl %d6,%d4,<<,%a2@\+,%d1,%acc3
    7344:	a65a 4e06      	macl %d6,%d4,>>,%a2@\+,%a3,%acc1
    7348:	a6da 4e16      	macl %d6,%d4,>>,%a2@\+,%a3,%acc3
    734c:	a41a 4e06      	macl %d6,%d4,,%a2@\+,%d2,%acc1
    7350:	a49a 4e16      	macl %d6,%d4,,%a2@\+,%d2,%acc3
    7354:	ae5a 4e06      	macl %d6,%d4,>>,%a2@\+,%sp,%acc1
    7358:	aeda 4e16      	macl %d6,%d4,>>,%a2@\+,%sp,%acc3
    735c:	a21a 4e26      	macl %d6,%d4,<<,%a2@\+&,%d1,%acc1
    7360:	a29a 4e36      	macl %d6,%d4,<<,%a2@\+&,%d1,%acc3
    7364:	a65a 4e26      	macl %d6,%d4,>>,%a2@\+&,%a3,%acc1
    7368:	a6da 4e36      	macl %d6,%d4,>>,%a2@\+&,%a3,%acc3
    736c:	a41a 4e26      	macl %d6,%d4,,%a2@\+&,%d2,%acc1
    7370:	a49a 4e36      	macl %d6,%d4,,%a2@\+&,%d2,%acc3
    7374:	ae5a 4e26      	macl %d6,%d4,>>,%a2@\+&,%sp,%acc1
    7378:	aeda 4e36      	macl %d6,%d4,>>,%a2@\+&,%sp,%acc3
    737c:	a22e 4e06 000a 	macl %d6,%d4,<<,%fp@\(10\),%d1,%acc1
    7382:	a2ae 4e16 000a 	macl %d6,%d4,<<,%fp@\(10\),%d1,%acc3
    7388:	a66e 4e06 000a 	macl %d6,%d4,>>,%fp@\(10\),%a3,%acc1
    738e:	a6ee 4e16 000a 	macl %d6,%d4,>>,%fp@\(10\),%a3,%acc3
    7394:	a42e 4e06 000a 	macl %d6,%d4,,%fp@\(10\),%d2,%acc1
    739a:	a4ae 4e16 000a 	macl %d6,%d4,,%fp@\(10\),%d2,%acc3
    73a0:	ae6e 4e06 000a 	macl %d6,%d4,>>,%fp@\(10\),%sp,%acc1
    73a6:	aeee 4e16 000a 	macl %d6,%d4,>>,%fp@\(10\),%sp,%acc3
    73ac:	a22e 4e26 000a 	macl %d6,%d4,<<,%fp@\(10\)&,%d1,%acc1
    73b2:	a2ae 4e36 000a 	macl %d6,%d4,<<,%fp@\(10\)&,%d1,%acc3
    73b8:	a66e 4e26 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%a3,%acc1
    73be:	a6ee 4e36 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%a3,%acc3
    73c4:	a42e 4e26 000a 	macl %d6,%d4,,%fp@\(10\)&,%d2,%acc1
    73ca:	a4ae 4e36 000a 	macl %d6,%d4,,%fp@\(10\)&,%d2,%acc3
    73d0:	ae6e 4e26 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%sp,%acc1
    73d6:	aeee 4e36 000a 	macl %d6,%d4,>>,%fp@\(10\)&,%sp,%acc3
    73dc:	a221 4e06      	macl %d6,%d4,<<,%a1@-,%d1,%acc1
    73e0:	a2a1 4e16      	macl %d6,%d4,<<,%a1@-,%d1,%acc3
    73e4:	a661 4e06      	macl %d6,%d4,>>,%a1@-,%a3,%acc1
    73e8:	a6e1 4e16      	macl %d6,%d4,>>,%a1@-,%a3,%acc3
    73ec:	a421 4e06      	macl %d6,%d4,,%a1@-,%d2,%acc1
    73f0:	a4a1 4e16      	macl %d6,%d4,,%a1@-,%d2,%acc3
    73f4:	ae61 4e06      	macl %d6,%d4,>>,%a1@-,%sp,%acc1
    73f8:	aee1 4e16      	macl %d6,%d4,>>,%a1@-,%sp,%acc3
    73fc:	a221 4e26      	macl %d6,%d4,<<,%a1@-&,%d1,%acc1
    7400:	a2a1 4e36      	macl %d6,%d4,<<,%a1@-&,%d1,%acc3
    7404:	a661 4e26      	macl %d6,%d4,>>,%a1@-&,%a3,%acc1
    7408:	a6e1 4e36      	macl %d6,%d4,>>,%a1@-&,%a3,%acc3
    740c:	a421 4e26      	macl %d6,%d4,,%a1@-&,%d2,%acc1
    7410:	a4a1 4e36      	macl %d6,%d4,,%a1@-&,%d2,%acc3
    7414:	ae61 4e26      	macl %d6,%d4,>>,%a1@-&,%sp,%acc1
    7418:	aee1 4e36      	macl %d6,%d4,>>,%a1@-&,%sp,%acc3
