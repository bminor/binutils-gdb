# Target: H8300 with HMS monitor, E7000 ICE and H8 simulator
TDEPFILES= exec.o h8300-tdep.o  ser-e7kpc.o remote-e7000.o remote-hms.o remote-sim.o ../sim/h8300/compile.o
TM_FILE= tm-h8300.h
