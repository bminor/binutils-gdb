# Target: PowerPC running eabi under the simulator
TDEPFILES= rs6000-tdep.o monitor.o dsrec.o ppcbug-rom.o ppc-bdm.o ocd.o ser-ocd.o
TM_FILE= tm-ppc-eabi.h

SIM_OBS = remote-sim.o
SIM = ../sim/ppc/libsim.a
