# Target: H8300 with HMS monitor and H8 simulator
TDEPFILES= exec.o h8300-tdep.o  remote-hms.o remote-sim.o ../sim/h8300/code.o ../sim/h8300/perifs.o
TM_FILE= tm-h8300.h
