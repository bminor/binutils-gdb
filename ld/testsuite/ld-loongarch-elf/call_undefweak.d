#...
Disassembly of section \.plt:
#...
0+1200004d0 <fn2@plt>:
   1200004d0:	1c00010f 	pcaddu12i   	\$t3, 8
   1200004d4:	28ed01ef 	ld.d        	\$t3, \$t3, -1216
   1200004d8:	4c0001ed 	jirl        	\$t1, \$t3, 0
   1200004dc:	03400000 	nop

Disassembly of section \.text:
#...
0+120000668 <main>:
   120000668:	4c000000 	jr          	\$zero
   12000066c:	53fe67ff 	b           	-412	# 1200004d0 <fn2@plt>
   120000670:	4c000001 	jirl        	\$ra, \$zero, 0
   120000674:	57fe5fff 	bl          	-420	# 1200004d0 <fn2@plt>

0+120000678 <medium_call_nop>:
   120000678:	1e000001 	pcaddu18i   	\$ra, 0
   12000067c:	4c000001 	jirl        	\$ra, \$zero, 0
   120000680:	1e000001 	pcaddu18i   	\$ra, 0
   120000684:	4ffe5021 	jirl        	\$ra, \$ra, -432
   120000688:	1e00000c 	pcaddu18i   	\$t0, 0
   12000068c:	4c000000 	jr          	\$zero
   120000690:	1e00000c 	pcaddu18i   	\$t0, 0
   120000694:	4ffe4180 	jirl        	\$zero, \$t0, -448
