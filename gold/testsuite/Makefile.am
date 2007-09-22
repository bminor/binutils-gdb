# Process this file with automake to generate Makefile.in

# As far as I can tell automake testing support assumes that the build
# system and the host system are the same.  So these tests will not
# work when building with a cross-compiler.

AUTOMAKE_OPTIONS =

AM_CXXFLAGS = $(WARN_CXXFLAGS) $(LFS_CXXFLAGS)

INCLUDES = -D_GNU_SOURCE \
	-I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../../include \
	-I$(srcdir)/../../elfcpp \
	-DLOCALEDIR="\"$(datadir)/locale\"" \
	@INCINTL@

if GCC
if NATIVE_LINKER
NATIVE_TESTING = t1 t1_pic
endif
endif

TESTS = object_unittest $(NATIVE_TESTING)

check_LIBRARIES = libgoldtest.a
libgoldtest_a_SOURCES = test.cc testmain.cc testfile.cc

LDADD = libgoldtest.a ../libgold.a

check_PROGRAMS = object_unittest

object_unittest_SOURCES = object_unittest.cc

if GCC
if NATIVE_LINKER

gcctestdir/ld: ../ld-new
	test -d gcctestdir || mkdir -p gcctestdir
	rm -f gcctestdir/ld
	(cd gcctestdir && $(LN_S) ../../ld-new ld)

# Override the default CXXFLAGS--we don't want any optimization
t1.o: t1.cc
	$(CXXCOMPILE) -O0 -c -o $@ $<
t1: t1.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ t1.o

t1_pic.o: t1.cc
	$(CXXCOMPILE) -O0 -c -fpic -o $@ $<
t1_pic: t1_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ t1_pic.o

endif
endif
