# Target: SPARC64 (with simulator)
# FIXME: May not always want the simulator.

# Need gcc for long long support.
CC = gcc

# Regular "have long long" detection is disabled for now.
MH_CFLAGS = -DFORCE_LONG_LONG -I${srcdir}/../sim/sp64
SIMFILES = remote-sim.o ../sim/sp64/libsim.a

# solib.o and procfs.o taken out for now.  We don't have shared libraries yet,
# and the elf version requires procfs.o but the a.out version doesn't.
# Then again, having procfs.o in a target makefile fragment seems wrong.
TDEPFILES = exec.o sparc-pinsn.o sparc-tdep.o sp64-tdep.o $(SIMFILES)

# The simulator uses the sqrt() function.
TM_CLIBS = -lm

# configure.in requires "TM_FILE= ".
TM_FILE= tm-sp64.h
