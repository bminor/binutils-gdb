# The mips as doesn't grok #line directives, suppress them via -P during
# preprocessing.
# The sed script removes blanks that mips-tfile doesn't like,
# embedds stabs in comments and changes .long to .word
#
WIERDSTABS_S=wierd-ecoff.S
# Only gcc knows about stabs-in-ecoff
STABSCC=gcc

wierd.o: ${srcdir}/${WIERDSTABS_S} ${srcdir}/wierd.def ${srcdir}/wierd-ecoff.sed
	cp ${srcdir}/${WIERDSTABS_S} tmp.c
	$(STABSCC) -I${srcdir} -E -P tmp.c >tmp.s
	sed -f ${srcdir}/wierd-ecoff.sed <tmp.s >wierd.s
	$(STABSCC) -c wierd.s
