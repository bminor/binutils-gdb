EMUL=i386linux
OTHER_EMULATIONS= ei386linux.o
ei386linux.c: $(srcdir)/emulparams/i386linux.sh \
  $(srcdir)/emultempl/generic.em $(srcdir)/scripttempl/aout.sc ${GEN_DEPENDS}
	${GENSCRIPTS} i386linux
