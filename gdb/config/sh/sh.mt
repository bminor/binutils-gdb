# Target: Hitachi Super-H ICE and simulator
TDEPFILES=  sh-tdep.o remote-sim.o ../sim/sh/libsim.a remote-e7000.o  ser-e7kpc.o
over: all

TM_FILE= tm-sh.h
