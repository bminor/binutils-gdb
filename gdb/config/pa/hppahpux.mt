# Target: HP PA-RISC running hpux
MT_CFLAGS = -DPA_SOM_ONLY=1
TDEPFILES= hppa-tdep.o hppa-hpux-tdep.o somread.o hpread.o somsolib.o
DEPRECATED_TM_FILE= tm-hppah.h
