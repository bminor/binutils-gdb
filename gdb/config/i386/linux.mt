# Target: Intel 386 running GNU/Linux
TDEPFILES= i386-tdep.o i386-linux-tdep.o i387-tdep.o solib.o solib-svr4.o 
TM_FILE= tm-linux.h

GDBSERVER_DEPFILES= low-linux.o
