# Target: SuperH running NetBSD
TDEPFILES= sh-tdep.o sh-nbsd-tdep.o corelow.o solib.o solib-svr4.o
TM_FILE= tm-nbsd.h

SIM_OBS = remote-sim.o
SIM = ../sim/sh/libsim.a
