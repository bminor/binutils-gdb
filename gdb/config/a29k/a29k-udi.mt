# Target: AMD 29000 on EB29K board over a serial line

TM_FILE= tm-a29k.h
TDEPFILES= exec.o a29k-pinsn.o a29k-tdep.o remote-udi.o \
	udip2soc.o udr.o udi2go32.o

# Disable standard remote support.
REMOTE_O=

MT_CFLAGS = $(HOST_IPC)
