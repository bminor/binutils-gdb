# Target: AMD x86-64 running GNU/Linux
TDEPFILES= x86-64-tdep.o x86-64-linux-tdep.o \
	i386-tdep.o i387-tdep.o i386-linux-tdep.o \
	solib.o solib-svr4.o solib-legacy.o corelow.o

TM_FILE=tm-x86-64linux.h
