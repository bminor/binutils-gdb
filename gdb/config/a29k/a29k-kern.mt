# OBSOLETE # Target: Remote AMD 29000 that runs Unix kernel on NYU Ultra3 processor board
# OBSOLETE 
# OBSOLETE # This builds a gdb that should run on a host (we use sun3os4) that
# OBSOLETE # then communicates over the serial line to either an Adapt or MiniMon,
# OBSOLETE # for use in debugging Unix kernels.
# OBSOLETE # As compared to ordinary remote 29K debugging, this changes the register
# OBSOLETE # numbering a bit, to hold kernel regs, and adds support for looking at
# OBSOLETE # the upage.
# OBSOLETE 
# OBSOLETE TDEPFILES= a29k-tdep.o remote-mm.o remote-adapt.o
# OBSOLETE TM_FILE= tm-ultra3.h
# OBSOLETE 
# OBSOLETE MT_CFLAGS = -DKERNEL_DEBUGGING -DNO_HIF_SUPPORT
