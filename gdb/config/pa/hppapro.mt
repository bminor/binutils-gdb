# Communicate with a PA based debug monitor.  Communications
# is facilitated via either a serial line, or a TCP or TELNET connection to
# a serial line on a terminal multiplexor.
TDEPFILES= hppa-pinsn.o hppa-tdep.o
REMOTE_O=dcache.o remote.o remote-utils.o op50-rom.o w89k-rom.o monitor.o
XDEPFILES= ser-tcp.o
TM_FILE= tm-hppap.h
