WIERDSTABS_S=wierd-xcoff.S

wierd.o: ${srcdir}/${WIERDSTABS_S} ${srcdir}/wierd.def
	cp ${srcdir}/${WIERDSTABS_S} tmp.c
	$(CC) -I${srcdir} -E tmp.c >wierd.s
	$(CC) -c wierd.s
