# Target: Motorola rom68k boot monitor (from Integrated Systems)
# Target: Motorola bug monitor.
# These defines should give you a gdb running on anything that will be able to
# communicate with a m68k based debug monitor.  Communications
# is facilitated via either a serial line, or a TCP or TELNET connection to
# a serial line on a terminal multiplexor.
TDEPFILES= hppa-pinsn.o hppa-tdep.o
REMOTE_O=dcache.o remote.o remote-utils.o exec.o op50-rom.o w89k-rom.o monitor.o
XDEPFILES= ser-tcp.o
TM_FILE= tm-hppab.h
