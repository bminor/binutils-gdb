EMUL=i386linux
OTHER_EMULATIONS= em_i386linux.o
em_i386linux.c: $(srcdir)/emulparams/i386linux.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/aout.sc ${GEN_DEPENDS}
	${GENSCRIPTS} i386linux.sh
