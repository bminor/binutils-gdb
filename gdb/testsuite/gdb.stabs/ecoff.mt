# The mips as doesn't grok #line directives, suppress them via -P during
# preprocessing.
# The sed script removes blanks that mips-tfile doesn't like,
# embedds stabs in comments and changes .long to .word
#
WEIRDSTABS_S=weird-ecoff.S
# Only gcc knows about stabs-in-ecoff
STABSCC=gcc

weird.o: ${srcdir}/${WEIRDSTABS_S} ${srcdir}/weird.def ${srcdir}/weird-ecoff.sed
	cp ${srcdir}/${WEIRDSTABS_S} tmp.c
	$(STABSCC) -I${srcdir} -E -P tmp.c >tmp.s
	sed -f ${srcdir}/weird-ecoff.sed <tmp.s >weird.s
	$(STABSCC) -c weird.s
