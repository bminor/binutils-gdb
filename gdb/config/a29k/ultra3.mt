# Target: AMD 29000 running Unix on New York Univerisity processor board.
TDEPFILES= a29k-pinsn.o a29k-tdep.o
TM_FILE= tm-ultra3.h
# SYM1 is some OS they have.
MT_CFLAGS = -DSYM1
