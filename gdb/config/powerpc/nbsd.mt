# Target: PowerPC, running NetBSD
TDEPFILES= rs6000-tdep.o ppc-linux-tdep.o
TM_FILE= tm-nbsd.h

GDBSERVER_DEPFILES= low-nbsd.o

SIM_OBS = remote-sim.o
SIM = ../sim/ppc/libsim.a
