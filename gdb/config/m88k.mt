# Target: Motorola 88000 running DGUX
TDEPFILES= tdesc.o m88k-tdep.o m88k-pinsn.o exec.o coredep.o
TM_CLIBS= tdesc-lib/libdc.o
TM_CDEPS= tdesc-lib/libdc.o
TM_FILE= tm-88k.h
