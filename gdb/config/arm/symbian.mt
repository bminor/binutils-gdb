# Target: ARM embedded system
TDEPFILES= arm-tdep.o prologue-value.o solib-target.o symbian-tdep.o
DEPRECATED_TM_FILE= tm-embed.h

SIM_OBS = remote-sim.o
SIM = ../sim/arm/libsim.a
