# Target: VAX running NetBSD
TDEPFILES= vax-tdep.o vaxnbsd-tdep.o corelow.o nbsd-tdep.o solib.o \
	solib-svr4.o
TM_FILE= tm-nbsd.h
