# Target: Hitachi Super-H ICE and simulator
TDEPFILES= exec.o sh-tdep.o remote-sim.o interp.o table.o remote-e7000.o  ser-e7kpc.o

over: all
interp.o:
	cp ../sim/sh/interp.o .

table.o:
	cp ../sim/sh/table.o .

TM_FILE= tm-sh.h
