# Target: PowerPC running eabi
TDEPFILES= rs6000-tdep.o
TM_FILE= tm-ppc-eabi.h

SIM_OBS = remote-sim.o
SIM = ../sim/ppc/libsim.a
