# Target: Motorola m68k running NetBSD
TDEPFILES= m68k-tdep.o m68knbsd-tdep.o solib.o solib-svr4.o
TM_FILE= tm-nbsd.h

GDBSERVER_DEPFILES= low-nbsd.o
