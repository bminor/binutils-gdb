weird.o: $(srcdir)/weird.def $(srcdir)/weird-xcoff.sed
	sed -f $(srcdir)/weird-xcoff.sed <$(srcdir)/weird.def >weird.s
	$(CC) -c weird.s
