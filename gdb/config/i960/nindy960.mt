# Target: Intel 80960, in an embedded system under the NINDY monitor
TDEPFILES= exec.o i960-pinsn.o i960-tdep.o nindy-tdep.o remote-nindy.o dcache.o nindy.o Onindy.o ttyflush.o
TM_FILE= tm-nindy960.h
# Don't use remote.o; it doesn't compile (and won't work) due to lack of
# BREAKPOINT.
REMOTE_O=
