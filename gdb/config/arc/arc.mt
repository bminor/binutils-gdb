# Target: arc processor
TDEPFILES= arc-tdep.o monitor.o arc-rom.o dsrec.o remote-arc-sdi.o
SIM_OBS = remote-sim.o
SIM = ../sim/arc/libsim.a
