# Target: AMD 29000 running Unix on New York University processor board

TM_FILE= tm-ultra3.h
TDEPFILES= a29k-pinsn.o a29k-tdep.o

# SYM1 is some OS they have.
MT_CFLAGS = -DSYM1
