WEIRDSTABS_S=weird-aout.S

weird.o: ${srcdir}/${WEIRDSTABS_S} ${srcdir}/weird.def
	cp ${srcdir}/${WEIRDSTABS_S} tmp.c
	$(CC) -I${srcdir} -E tmp.c >weird.s
	$(CC) -c weird.s
