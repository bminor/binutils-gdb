weird.o: $(srcdir)/weird.def $(srcdir)/hppa.sed
	sed -f $(srcdir)/hppa.sed <$(srcdir)/weird.def >weird.s
	$(AS) weird.s -o weird.o
