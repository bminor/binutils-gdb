# Target: Fujitsu fr30 processor
TDEPFILES= fr30-tdep.o monitor.o fr30-rom.o dsrec.o
TM_FILE= tm-fr30.h
SIM_OBS = remote-sim.o
SIM = ../sim/fr30/libsim.a
GDBSERVER_DEPFILES= low-sim.o
GDBSERVER_LIBS = ../../sim/fr30/libsim.a ../../bfd/libbfd.a ../../libiberty/libiberty.a ../../opcodes/libopcodes.a
