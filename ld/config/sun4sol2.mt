EMUL=elf32_sparc
OTHER_EMULATIONS= eelf32_sparc.o
eelf32_sparc.c: $(srcdir)/emulparams/elf32_sparc.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/elf.sc ${GEN_DEPENDS}
	${GENSCRIPTS} elf32_sparc
