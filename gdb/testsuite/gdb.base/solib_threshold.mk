# Make file for solib_threshold test

OBJDIR=.
SRCDIR=.
CFLAGS = +DA1.1 -g

# This is how to build this generator.
gen_solib_threshold.o: ${SRCDIR}/gen_solib_threshold.c
	$(CC) $(CFLAGS) -o gen_solib_threshold.o -c ${SRCDIR}/gen_solib_threshold.c
gen_solib_threshold: gen_solib_threshold.o
	$(CC) $(CFLAGS) -o gen_solib_threshold gen_solib_threshold.o

# This is how to run this generator.
# This target should be made before the 'all' target,
# to ensure that the shlib sources are all available.
require_shlibs: gen_solib_threshold
	if ! [ -a lib00_solib_threshold.c ] ; then \
	  gen_solib_threshold ; \
	fi
	if ! [ -a lib01_solib_threshold.c ] ; then \
	  gen_solib_threshold ; \
	fi
	if ! [ -a lib02_solib_threshold.c ] ; then \
	  gen_solib_threshold ; \
	fi

# This is how to build all the shlibs.
# Be sure to first make the require_shlibs target!
lib00_solib_threshold.o: lib00_solib_threshold.c
	$(CC) $(CFLAGS) +Z -o lib00_solib_threshold.o -c lib00_solib_threshold.c
lib00_solib_threshold.sl: lib00_solib_threshold.o
	$(LD) $(LDFLAGS) -b -o lib00_solib_threshold.sl lib00_solib_threshold.o
lib01_solib_threshold.o: lib01_solib_threshold.c
	$(CC) $(CFLAGS) +Z -o lib01_solib_threshold.o -c lib01_solib_threshold.c
lib01_solib_threshold.sl: lib01_solib_threshold.o
	$(LD) $(LDFLAGS) -b -o lib01_solib_threshold.sl lib01_solib_threshold.o
lib02_solib_threshold.o: lib02_solib_threshold.c
	$(CC) $(CFLAGS) +Z -o lib02_solib_threshold.o -c lib02_solib_threshold.c
lib02_solib_threshold.sl: lib02_solib_threshold.o
	$(LD) $(LDFLAGS) -b -o lib02_solib_threshold.sl lib02_solib_threshold.o




# For convenience, here's names for all pieces of all shlibs.
SHLIB_SOURCES = \
	lib00_solib_threshold.c \
	lib01_solib_threshold.c \
	lib02_solib_threshold.c 

SHLIB_OBJECTS = $(SHLIB_SOURCES:.c=.o)
SHLIBS = $(SHLIB_SOURCES:.c=.sl)
SHLIB_NAMES = $(SHLIB_SOURCES:.c=)
EXECUTABLES = $(SHLIBS) gen_solib_threshold solib_threshold
OBJECT_FILES = $(SHLIB_OBJECTS) gen_solib_threshold.o solib_threshold.o

shlib_objects: $(SHLIB_OBJECTS)
shlibs: $(SHLIBS)

# This is how to build the debuggable testcase that uses the shlibs.
solib_threshold.o: solib_threshold.c
	$(CC) $(CFLAGS) -o solib_threshold.o -c solib_threshold.c
solib_threshold: shlibs solib_threshold.o
	$(LD) $(LDFLAGS) -o solib_threshold -lc -L${OBJDIR} -c solib_threshold.link_opts /opt/langtools/lib/end.o /lib/crt0.o solib_threshold.o

# Yeah, but you should first make the require_shlibs target!
all: solib_threshold gen_solib_threshold

# To remove everything built via this makefile...
clean:
	rm -f lib0*_solib_threshold.*
	rm -f *.o gen_solib_threshold solib_threshold.link_opts solib_threshold.c
	rm -f solib_threshold
