# Target: PowerPC running eabi under the simulator
TDEPFILES= rs6000-tdep.o remote-ppcbug.o
TM_FILE= tm-ppc-eabi.h

SIM_OBS = remote-sim.o
SIM = ../sim/ppc/libsim.a
