# Target:  Ericcson ES-1800 emulator (remote) for m68k.
# (Note: remote-es.o is also available from the standard monitor.mt
# configuration.  The only difference in this configuration is that it
# has its own DEFAULT_PROMPT and GDBINIT_FILENAME, if you don't care
# about those things you can just use monitor.mt).
TDEPFILES= exec.o m68k-pinsn.o m68k-tdep.o remote-es.o 
TM_FILE= tm-es1800.h
