# Target: TI TMS320C80 (MVP) processor
TDEPFILES= tic80-tdep.o
TM_FILE= tm-tic80.h

# Disable using the simulator until it is ready.
#SIM_OBS = remote-sim.o
#SIM = ../sim/tic80/libsim.a -lm
