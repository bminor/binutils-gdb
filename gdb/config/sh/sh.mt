# Target: Hitachi Super-H with ICE and simulator
TDEPFILES= sh-tdep.o remote-e7000.o ser-e7kpc.o
TM_FILE= tm-sh.h

SIM_OBS = remote-sim.o
SIM = ../sim/sh/libsim.a -lm
