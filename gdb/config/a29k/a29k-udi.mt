# Target: AMD 29000 on EB29K board over a serial line.
TDEPFILES= exec.o a29k-pinsn.o remote-udi.o a29k-tdep.o udip2soc.o udr.o \
  udi2go32.o
REMOTE_O=
TM_FILE= tm-a29k.h
# The following is for ../include/a.out.encap.h
MT_CFLAGS = -Da29k -DREMOTE -DAMD_COFF $(HOST_IPC)
