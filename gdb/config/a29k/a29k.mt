# Target: AMD 29000.
# These defines should give you a gdb running on ? (sun3os4 if you like)
# that will be able to communicate over a serial line with either an 
# EB board (remote-eb.c),
# Adapt (remote-adapt.c),
# Or run native on an Ultracomputer.
TDEPFILES= exec.o a29k-pinsn.o a29k-tdep.o remote-eb.o remote-adapt.o 
TM_FILE= tm-a29k.h
MT_CFLAGS = -DNO_HIF_SUPPORT
