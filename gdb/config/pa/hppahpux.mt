# TARGET: HP PA-RISC running hpux
TDEPFILES= hppa-pinsn.o hppa-tdep.o exec.o
XDEPFILES= ser-tcp.o
TM_FILE= tm-hppah.h
REMOTE_O=dcache.o remote-pa.o remote-utils.o
