# Target: PowerPC running eabi in little endian mode
TDEPFILES= rs6000-tdep.o
TM_FILE= tm-ppcle-eabi.h

SIM_OBS = remote-sim.o
SIM = ../sim/ppc/libsim.a
