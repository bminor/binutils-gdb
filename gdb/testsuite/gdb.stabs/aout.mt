weird.o: $(srcdir)/weird.def $(srcdir)/weird-aout.S
	sed -f $(srcdir)/weird-aout.S <$(srcdir)/weird.def >weird.s
	$(CC) -c weird.s
