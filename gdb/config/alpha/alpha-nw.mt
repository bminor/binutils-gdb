# Target: Little-endian Alpha
TDEPFILES= alpha-tdep.o exec.o
TM_FILE= tm-alpha.h
#
# Can't define this in tm.h because defs.h uses it before including tm.h
# FIXME: Needs to be at least 64-bits (LONGEST for e.g. 386 x alpha), but
# CORE_ADDR is typedef'd before LONGEST is defined.
MT_CFLAGS = -DCORE_ADDR_TYPE="unsigned long"
