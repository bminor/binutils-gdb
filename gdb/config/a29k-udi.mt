# Target: AMD 29000 on EB29K board over a serial line.
TDEPFILES= exec.o am29k-pinsn.o remote-udi.o am29k-tdep.o udip2soc.o udr.o
TM_FILE= tm-29k.h
# The following is for ../include/a.out.encap.h
MT_CFLAGS = -Da29k -DREMOTE -DTARGET=TARGET_AM29K -DAMD_COFF
