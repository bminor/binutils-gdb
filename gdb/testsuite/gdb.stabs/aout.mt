weird.o: $(srcdir)/weird.def $(srcdir)/weird-aout.sed
	sed -f $(srcdir)/weird-aout.sed <$(srcdir)/weird.def >weird.s
	$(CC) -c weird.s
