# Target: AMD 29000 running Unix on New York Univerisity processor board.
TDEPFILES= exec.o am29k-pinsn.o am29k-tdep.o
TM_FILE= tm-ultra3.h
# SYM1 is some OS they have.
TM_CFLAGS = -DSYM1
