# This configuration is for a gcc that uses mips-tfile. If your gcc
# uses gas, you should configure gdb --with-gnu-as.
#
# The alpha as doesn't grok #line directives, suppress them via -P during
# preprocessing.
# The sed script removes blanks that mips-tfile doesn't like and
# embedds stabs in comments.
#
WEIRDSTABS_S=weird-ecoff.S

weird.o: ${srcdir}/${WEIRDSTABS_S} ${srcdir}/weird.def ${srcdir}/weird-ecoff.sed
	cp ${srcdir}/${WEIRDSTABS_S} tmp.c
	$(CC) -I${srcdir} -E -P -DTARGET_IS_ALPHA tmp.c >tmp.s
	sed -f ${srcdir}/weird-ecoff.sed <tmp.s >weird.s
	$(CC) -c weird.s
