# Target: AMD 29000

TM_FILE= tm-a29k.h
TDEPFILES= a29k-pinsn.o a29k-tdep.o remote-eb.o remote-adapt.o

MT_CFLAGS = -DNO_HIF_SUPPORT
