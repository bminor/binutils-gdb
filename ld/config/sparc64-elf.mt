EMUL=elf64_sparc
OTHER_EMULATIONS= eelf64_sparc.o
eelf64_sparc.c: $(srcdir)/emulparams/elf64_sparc.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/elf.sc ${GEN_DEPENDS}
	${GENSCRIPTS} elf64_sparc
