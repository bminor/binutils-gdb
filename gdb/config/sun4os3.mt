# Target: Sun 4 or Sparcstation, running SunOS 3
# The system-supplied assembler re-orders the symbols so that gdb
# can't find "gcc_compiled.".
#msg If you compile your program with GCC, use the GNU assembler.
#msg

TDEPFILES= exec.o sparc-tdep.o sparc-pinsn.o
TM_FILE= tm-sparc.h
