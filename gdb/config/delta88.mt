# Target: Motorola 88000 
TDEPFILES= tdesc.o m88k-tdep.o m88k-pinsn.o exec.o coredep.o
TM_CDEPS = disasm.o table.o classify.o fcns.o
TM_CLIBS = $(TM_CDEPS)
TM_CFLAGS = -I${srcdir}/tdesc-lib
TM_FILE= tm-delta88.h
