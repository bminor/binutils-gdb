# Target: SPARC simulator
# Which simulator has deliberately been avoided.  There are two and I'm
# currently playing with both. /dje

# Need gcc for long long support.
CC = gcc
# Regular "have long long" detection is disabled for now.
MH_CFLAGS = -DFORCE_LONG_LONG -I${srcdir}/../sim/sp64
TDEPFILES= exec.o solib.o sparc-pinsn.o sparc-tdep.o sp64-tdep.o remote-sim.o ../sim/sp64/libsim.a
TM_FILE= tm-sp64sim.h
