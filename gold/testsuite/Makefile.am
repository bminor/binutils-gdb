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
NATIVE_TESTING = \
	basic_test \
	basic_pic_test \
	basic_static_test \
	basic_static_pic_test
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
basic_test.o: basic_test.cc
	$(CXXCOMPILE) -O0 -c -o $@ $<
basic_test: basic_test.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ basic_test.o
basic_static_test: basic_test.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -static basic_test.o

basic_pic_test.o: basic_test.cc
	$(CXXCOMPILE) -O0 -c -fpic -o $@ $<
basic_pic_test: basic_pic_test.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ basic_pic_test.o
basic_static_pic_test: basic_pic_test.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -static basic_pic_test.o

endif
endif
