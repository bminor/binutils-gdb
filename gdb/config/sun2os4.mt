# Target: Sun 2, running SunOS 4
# The system-supplied assembler re-orders the symbols so that gdb
# can't find "gcc_compiled.".
#msg If you compile your program with GCC, use the GNU assembler.
#msg

TDEPFILES= exec.o m68k-pinsn.o solib.o m68k-tdep.o
TM_FILE= tm-sun2os4.h
