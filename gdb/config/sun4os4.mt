# Target: Sun 4 or Sparcstation, running SunOS 4
# The system-supplied assembler re-orders the symbols so that gdb
# can't find "gcc_compiled.".
#msg If you compile your program with GCC, use the GNU assembler.
#msg

TDEPFILES= exec.o sparc-tdep.o sparc-pinsn.o solib.o
TM_FILE= tm-sun4os4.h
