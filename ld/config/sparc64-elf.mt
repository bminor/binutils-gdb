EMUL=elf64_sparc
OTHER_EMULATIONS= em_elf64_sparc.o
em_elf64_sparc.c: $(srcdir)/emulparams/elf64-sparc.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/elf.sc ${GEN_DEPENDS}
	${GENSCRIPTS} elf64-sparc.sh
