# Target: Altos 3068 (68k, System V release 2), using COFF encapsulation

#msg Use of the coff encapsulation features require the GNU binutils utilities
#msg To be ahead of their System V counterparts in your path.

TDEPFILES= m68k-pinsn.o exec.o
TM_FILE= tm-altosgas.h
