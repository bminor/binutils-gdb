# Target: H8300 with HMS monitor and H8 simulator
TDEPFILES= exec.o h8300-tdep.o  remote-hms.o remote-sim.o ../h8300sim/code.o ../h8300sim/perifs.o
TM_FILE= tm-h8300.h
