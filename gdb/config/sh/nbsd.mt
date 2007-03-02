# Target: NetBSD/sh
TDEPFILES= sh-tdep.o shnbsd-tdep.o \
	corelow.o solib.o solib-svr4.o
DEPRECATED_TM_FILE= tm-sh.h

SIM_OBS = remote-sim.o
SIM = ../sim/sh/libsim.a
