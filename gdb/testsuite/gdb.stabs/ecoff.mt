# This configuration is for a gcc that uses mips-tfile. If your gcc
# uses gas, you should configure gdb --with-gnu-as.
#
weird.o: $(srcdir)/weird.def $(srcdir)/weird-ecoff.sed
	sed -e '/.long/.word/' <$(srcdir)/weird.def | \
	sed -f $(srcdir)/weird-ecoff.sed >weird.s
	$(CC) -c weird.s
