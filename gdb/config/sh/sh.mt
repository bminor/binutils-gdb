# Target: Hitachi Super-H with ICE and simulator
TDEPFILES= sh-tdep.o monitor.o sh3-rom.o remote-e7000.o ser-e7kpc.o dsrec.o solib.o solib-svr4.o
TM_FILE= tm-linux.h

SIM_OBS = remote-sim.o
SIM = ../sim/sh/libsim.a
