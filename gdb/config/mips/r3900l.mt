# Target: Big-endian mips board, typically an IDT.
TDEPFILES= mips-tdep.o remote-mips.o remote-array.o
TM_FILE= tm-r3900l.h
SIM_OBS = remote-sim.o
SIM = ../sim/mips/libsim.a
