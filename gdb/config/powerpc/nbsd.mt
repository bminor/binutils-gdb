# Target: PowerPC, running NetBSD
TDEPFILES= rs6000-tdep.o ppc-linux-tdep.o solib.o solib-svr4.o
TM_FILE= tm-nbsd.h

GDBSERVER_DEPFILES= low-nbsd.o
