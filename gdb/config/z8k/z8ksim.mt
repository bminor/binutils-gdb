# Target: Z8000 simulator
TDEPFILES= exec.o z8k-tdep.o  remote-z8k.o ../sim/z8k/libsim.a ../opcodes/libopcodes.a
TM_FILE= tm-z8k.h
