# Target: ARM based machine running GNU/Linux
TM_FILE= tm-linux.h
TDEPFILES= arm-tdep.o arm-linux-tdep.o solib.o solib-svr4.o solib-legacy.o

GDBSERVER_DEPFILES= low-linux.o
