EMUL=elf32_sparc
OTHER_EMULATIONS= em_elf32_sparc.o
em_elf32_sparc.c: $(srcdir)/emulparams/elf32-sparc.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/elf.sc ${GEN_DEPENDS}
	${GENSCRIPTS} elf32-sparc.sh
