EMUL=elf64_sparc
OTHER_EMULATIONS= em_elf64_sparc.o
em_elf64_sparc.c: $(srcdir)/emulparams/elf64_sparc.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/elf.sc ${GEN_DEPENDS}
	${GENSCRIPTS} elf64_sparc
