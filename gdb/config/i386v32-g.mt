# Target: Intel 386 running System V release 3.2, using COFF encapsulation

#msg Use of the COFF encapsulation features requires the GNU binary utilities
#msg to be ahead of their System V counterparts in your path.

TDEPFILES= exec.o i386-tdep.o i386-pinsn.o
TM_FILE= tm-i386v-g.h
