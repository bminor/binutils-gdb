# Target: Intel 386 running SVR4
TDEPFILES= exec.o i386-pinsn.o i386-tdep.o i387-tdep.o solib.o
TM_FILE= tm-i386v4.h

# For now, we have to select, at compile time, a particular flavor of C++
# demangling to support.  For this machine, we select the style specified
# in the Annotated C++ Reference Manual, section 7.2.1c, which is what is
# used by the Lucid C++ compiler for most things.  But there are places
# where Lucid varies from the ARM, so select the Lucid specific code also.
DEMANGLE_OPTS=-DARM_DEMANGLING -DLUCID_DEMANGLING

