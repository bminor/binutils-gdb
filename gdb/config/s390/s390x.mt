# Target: S390 running Linux
TM_FILE= tm-linux.h
TDEPFILES=s390-tdep.o solib.o
# Post 5.0 tdep-files
TDEPFILES+=solib-svr4.o solib-legacy.o

# needed for gdbserver.
MT_CFLAGS= -DCONFIG_ARCH_S390X
