# Process this file with automake to generate Makefile.in

# As far as I can tell automake testing support assumes that the build
# system and the host system are the same.  So these tests will not
# work when building with a cross-compiler.

AUTOMAKE_OPTIONS =

# The two_file_test tests -fmerge-constants, so we simply always turn
# it on.  This may need to be controlled by a configure option
# eventually.
AM_CXXFLAGS = $(WARN_CXXFLAGS) $(LFS_CFLAGS) -fmerge-constants

INCLUDES = \
	-I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../../include \
	-I$(srcdir)/../../elfcpp \
	-DLOCALEDIR="\"$(datadir)/locale\"" \
	@INCINTL@

if THREADS
THREADSLIB = -lpthread
endif

# 'make clean' is good about deleting some intermediate files (such as
# .o's), but not all of them (such as .so's and .err files).  We
# improve on that here.  automake-1.9 info docs say "mostlyclean" is
# the right choice for files 'make' builds that people rebuild.
MOSTLYCLEANFILES = *.so


# We will add to these later, for each individual test.  Note
# that we add each test under check_SCRIPTS or check_PROGRAMS;
# the TESTS variable is automatically populated from these.
check_SCRIPTS =
check_DATA =
check_PROGRAMS =
TESTS = $(check_SCRIPTS) $(check_PROGRAMS)

# ---------------------------------------------------------------------
# These tests test the internals of gold (unittests).

# Infrastucture needed for the unittests
check_LIBRARIES = libgoldtest.a
libgoldtest_a_SOURCES = test.cc testmain.cc testfile.cc

DEPENDENCIES = \
	libgoldtest.a ../libgold.a ../../libiberty/libiberty.a $(LIBINTL_DEP)
LDADD = libgoldtest.a ../libgold.a ../../libiberty/libiberty.a $(LIBINTL) \
	$(THREADSLIB)


# The unittests themselves
check_PROGRAMS += object_unittest
object_unittest_SOURCES = object_unittest.cc

check_PROGRAMS += binary_unittest
binary_unittest_SOURCES = binary_unittest.cc


# ---------------------------------------------------------------------
# These tests test the output of gold (end-to-end tests).  In
# particular, they make sure that gold can link "difficult" object
# files, and the resulting object files run correctly.  These can only
# run if we've built ld-new for the native architecture (that is,
# we're not cross-compiling it), since we run ld-new as part of these
# tests.  We use the gcc-specific flag '-B' to use our linker instead
# of the default linker, which is why we only run our tests under gcc.

if NATIVE_LINKER
if GCC

# Infrastucture needed for the unittests: a directory where the linker
# is named 'ld'.  This is because the -B flag appends 'ld' to its arg.
gcctestdir/ld: ../ld-new
	test -d gcctestdir || mkdir -p gcctestdir
	rm -f gcctestdir/ld
	(cd gcctestdir && $(LN_S) ../../ld-new ld)

# Each of these .o's is a useful, small complete program.  They're
# particularly useful for making sure ld-new's flags do what they're
# supposed to (hence their names), but are used for many tests that
# don't actually involve analyzing input data.
flagstest_debug.o: constructor_test.cc
	$(CXXCOMPILE) -O0 -g -c -o $@ $<
flagstest_ndebug.o: constructor_test.cc
	$(CXXCOMPILE) -O0 -c -o $@ $<


check_PROGRAMS += basic_test
check_PROGRAMS += basic_static_test
check_PROGRAMS += basic_pic_test
check_PROGRAMS += basic_static_pic_test
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


check_PROGRAMS += constructor_test
check_PROGRAMS += constructor_static_test
constructor_test_SOURCES = constructor_test.cc
constructor_test_DEPENDENCIES = gcctestdir/ld
constructor_test_LDFLAGS = -Bgcctestdir/

constructor_static_test_SOURCES = $(constructor_test_SOURCES)
constructor_static_test_DEPENDENCIES = $(constructor_test_DEPENDENCIES)
constructor_static_test_LDFLAGS = $(constructor_test_LDFLAGS) -static


check_PROGRAMS += two_file_test
check_PROGRAMS += two_file_static_test
check_PROGRAMS += two_file_pic_test
two_file_test_SOURCES = \
	two_file_test_1.cc \
	two_file_test_1b.cc \
	two_file_test_2.cc \
	two_file_test_main.cc \
	two_file_test.h
two_file_test_DEPENDENCIES = gcctestdir/ld
two_file_test_LDFLAGS = -Bgcctestdir/

two_file_static_test_SOURCES = $(two_file_test_SOURCES)
two_file_static_test_DEPENDENCIES = $(two_file_test_DEPENDENCIES)
two_file_static_test_LDFLAGS = $(two_file_test_LDFLAGS) -static

two_file_pic_test_SOURCES = two_file_test_main.cc
two_file_pic_test_DEPENDENCIES = \
	gcctestdir/ld two_file_test_1_pic.o two_file_test_1b_pic.o two_file_test_2_pic.o
two_file_pic_test_LDFLAGS = -Bgcctestdir/
two_file_pic_test_LDADD = two_file_test_1_pic.o two_file_test_1b_pic.o two_file_test_2_pic.o


check_PROGRAMS += two_file_shared_1_test
check_PROGRAMS += two_file_shared_2_test
check_PROGRAMS += two_file_shared_1_pic_2_test
check_PROGRAMS += two_file_shared_2_pic_1_test
check_PROGRAMS += two_file_same_shared_test
check_PROGRAMS += two_file_separate_shared_12_test
check_PROGRAMS += two_file_separate_shared_21_test
two_file_test_1_pic.o: two_file_test_1.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
two_file_test_1b_pic.o: two_file_test_1b.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
two_file_test_2_pic.o: two_file_test_2.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
two_file_shared_1.so: two_file_test_1_pic.o two_file_test_1b_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_1_pic.o two_file_test_1b_pic.o
two_file_shared_2.so: two_file_test_2_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_2_pic.o
two_file_shared.so: two_file_test_1_pic.o two_file_test_1b_pic.o two_file_test_2_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_1_pic.o two_file_test_1b_pic.o two_file_test_2_pic.o

two_file_shared_1_test_SOURCES = two_file_test_2.cc two_file_test_main.cc
two_file_shared_1_test_DEPENDENCIES = gcctestdir/ld two_file_shared_1.so
two_file_shared_1_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_shared_1_test_LDADD = two_file_shared_1.so

two_file_shared_2_test_SOURCES = two_file_test_1.cc two_file_test_1b.cc two_file_test_main.cc
two_file_shared_2_test_DEPENDENCIES = gcctestdir/ld two_file_shared_2.so
two_file_shared_2_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_shared_2_test_LDADD = two_file_shared_2.so

two_file_shared_1_pic_2_test_SOURCES = two_file_test_main.cc
two_file_shared_1_pic_2_test_DEPENDENCIES = \
	gcctestdir/ld two_file_shared_1.so two_file_test_1_pic.o two_file_test_1b_pic.o
two_file_shared_1_pic_2_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_shared_1_pic_2_test_LDADD = two_file_test_1_pic.o two_file_test_1b_pic.o two_file_shared_2.so

two_file_shared_2_pic_1_test_SOURCES = two_file_test_main.cc
two_file_shared_2_pic_1_test_DEPENDENCIES = \
	gcctestdir/ld two_file_shared_2.so two_file_test_2_pic.o
two_file_shared_2_pic_1_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_shared_2_pic_1_test_LDADD = two_file_test_2_pic.o two_file_shared_1.so

two_file_same_shared_test_SOURCES = two_file_test_main.cc
two_file_same_shared_test_DEPENDENCIES = gcctestdir/ld two_file_shared.so
two_file_same_shared_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_same_shared_test_LDADD = two_file_shared.so

two_file_separate_shared_12_test_SOURCES = two_file_test_main.cc
two_file_separate_shared_12_test_DEPENDENCIES = \
	gcctestdir/ld two_file_shared_1.so two_file_shared_2.so
two_file_separate_shared_12_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_separate_shared_12_test_LDADD = \
	two_file_shared_1.so two_file_shared_2.so

two_file_separate_shared_21_test_SOURCES = two_file_test_main.cc
two_file_separate_shared_21_test_DEPENDENCIES = \
	gcctestdir/ld two_file_shared_1.so two_file_shared_2.so
two_file_separate_shared_21_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_separate_shared_21_test_LDADD = \
	two_file_shared_2.so two_file_shared_1.so

check_PROGRAMS += two_file_relocatable_test
two_file_relocatable_test_SOURCES = two_file_test_main.cc
two_file_relocatable_test_DEPENDENCIES = \
	gcctestdir/ld two_file_relocatable.o
two_file_relocatable_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_relocatable_test_LDADD = two_file_relocatable.o
two_file_relocatable.o: gcctestdir/ld two_file_test_1.o two_file_test_1b.o two_file_test_2.o
	gcctestdir/ld -r -o $@ two_file_test_1.o two_file_test_1b.o two_file_test_2.o

# The nonpic tests will fail on platforms which can not put non-PIC
# code into shared libraries, so we just don't run them in that case.
if FN_PTRS_IN_SO_WITHOUT_PIC

check_PROGRAMS += two_file_shared_1_nonpic_test
check_PROGRAMS += two_file_shared_2_nonpic_test
check_PROGRAMS += two_file_same_shared_nonpic_test
check_PROGRAMS += two_file_separate_shared_12_nonpic_test
check_PROGRAMS += two_file_separate_shared_21_nonpic_test
check_PROGRAMS += two_file_mixed_shared_test
check_PROGRAMS += two_file_mixed_2_shared_test
two_file_shared_1_nonpic.so: two_file_test_1.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_1.o two_file_test_1b.o
two_file_shared_2_nonpic.so: two_file_test_2.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_2.o
two_file_shared_nonpic.so: two_file_test_1.o two_file_test_1b.o two_file_test_2.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_1.o two_file_test_1b.o two_file_test_2.o
two_file_shared_mixed.so: two_file_test_1_pic.o two_file_test_1b_pic.o two_file_test_2.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_1_pic.o two_file_test_1b_pic.o two_file_test_2.o
two_file_shared_mixed_1.so: two_file_test_1.o two_file_test_1b_pic.o two_file_shared_2.so gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_1.o two_file_test_1b_pic.o two_file_shared_2.so

two_file_shared_1_nonpic_test_SOURCES = \
	two_file_test_2.cc two_file_test_main.cc
two_file_shared_1_nonpic_test_DEPENDENCIES = \
	gcctestdir/ld two_file_shared_1_nonpic.so
two_file_shared_1_nonpic_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_shared_1_nonpic_test_LDADD = two_file_shared_1_nonpic.so

two_file_shared_2_nonpic_test_SOURCES = \
	two_file_test_1.cc two_file_test_1b.cc two_file_test_main.cc
two_file_shared_2_nonpic_test_DEPENDENCIES = \
	gcctestdir/ld two_file_shared_2_nonpic.so
two_file_shared_2_nonpic_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_shared_2_nonpic_test_LDADD = two_file_shared_2_nonpic.so

two_file_same_shared_nonpic_test_SOURCES = two_file_test_main.cc
two_file_same_shared_nonpic_test_DEPENDENCIES = \
	gcctestdir/ld two_file_shared_nonpic.so
two_file_same_shared_nonpic_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_same_shared_nonpic_test_LDADD = two_file_shared_nonpic.so

two_file_separate_shared_12_nonpic_test_SOURCES = two_file_test_main.cc
two_file_separate_shared_12_nonpic_test_DEPENDENCIES = \
	gcctestdir/ld two_file_shared_1_nonpic.so two_file_shared_2_nonpic.so
two_file_separate_shared_12_nonpic_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_separate_shared_12_nonpic_test_LDADD = \
	two_file_shared_1_nonpic.so two_file_shared_2_nonpic.so

two_file_separate_shared_21_nonpic_test_SOURCES = two_file_test_main.cc
two_file_separate_shared_21_nonpic_test_DEPENDENCIES = \
	gcctestdir/ld two_file_shared_1_nonpic.so two_file_shared_2_nonpic.so
two_file_separate_shared_21_nonpic_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_separate_shared_21_nonpic_test_LDADD = \
	two_file_shared_2_nonpic.so two_file_shared_1_nonpic.so

two_file_mixed_shared_test_SOURCES = two_file_test_main.cc
two_file_mixed_shared_test_DEPENDENCIES = gcctestdir/ld two_file_shared_mixed.so
two_file_mixed_shared_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_mixed_shared_test_LDADD = two_file_shared_mixed.so

two_file_mixed_2_shared_test_SOURCES = two_file_test_main.cc
two_file_mixed_2_shared_test_DEPENDENCIES = gcctestdir/ld two_file_shared_mixed_1.so two_file_shared_2.so
two_file_mixed_2_shared_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_mixed_2_shared_test_LDADD = two_file_shared_mixed_1.so two_file_shared_2.so

endif FN_PTRS_IN_SO_WITHOUT_PIC

check_PROGRAMS += common_test_1
common_test_1_SOURCES = common_test_1.c
common_test_1_DEPENDENCIES = gcctestdir/ld
common_test_1_LDFLAGS = -Bgcctestdir/

check_PROGRAMS += exception_test
check_PROGRAMS += exception_static_test
check_PROGRAMS += exception_shared_1_test
check_PROGRAMS += exception_shared_2_test
check_PROGRAMS += exception_same_shared_test
check_PROGRAMS += exception_separate_shared_12_test
check_PROGRAMS += exception_separate_shared_21_test
exception_test_1_pic.o: exception_test_1.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
exception_test_2_pic.o: exception_test_2.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
exception_shared_1.so: exception_test_1_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared exception_test_1_pic.o
exception_shared_2.so: exception_test_2_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared exception_test_2_pic.o
exception_shared.so: exception_test_1_pic.o exception_test_2_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared exception_test_1_pic.o exception_test_2_pic.o

exception_test_SOURCES = \
	exception_test_main.cc \
	exception_test_1.cc \
	exception_test_2.cc \
	exception_test.h
exception_test_DEPENDENCIES = gcctestdir/ld
exception_test_LDFLAGS = -Bgcctestdir/

exception_static_test_SOURCES = $(exception_test_SOURCES)
exception_static_test_DEPENDENCIES = $(exception_test_DEPENDENCIES)
exception_static_test_LDFLAGS = $(exception_test_LDFLAGS) -static

exception_shared_1_test_SOURCES = exception_test_2.cc exception_test_main.cc
exception_shared_1_test_DEPENDENCIES = gcctestdir/ld exception_shared_1.so
exception_shared_1_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
exception_shared_1_test_LDADD = exception_shared_1.so

exception_shared_2_test_SOURCES = exception_test_1.cc exception_test_main.cc
exception_shared_2_test_DEPENDENCIES = gcctestdir/ld exception_shared_2.so
exception_shared_2_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
exception_shared_2_test_LDADD = exception_shared_2.so

exception_same_shared_test_SOURCES = exception_test_main.cc
exception_same_shared_test_DEPENDENCIES = gcctestdir/ld exception_shared.so
exception_same_shared_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
exception_same_shared_test_LDADD = exception_shared.so

exception_separate_shared_12_test_SOURCES = exception_test_main.cc
exception_separate_shared_12_test_DEPENDENCIES = \
	gcctestdir/ld exception_shared_1.so exception_shared_2.so
exception_separate_shared_12_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
exception_separate_shared_12_test_LDADD = \
	exception_shared_1.so exception_shared_2.so

exception_separate_shared_21_test_SOURCES = exception_test_main.cc
exception_separate_shared_21_test_DEPENDENCIES = \
	gcctestdir/ld exception_shared_1.so exception_shared_2.so
exception_separate_shared_21_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
exception_separate_shared_21_test_LDADD = \
	exception_shared_2.so exception_shared_1.so


check_PROGRAMS += weak_test
weak_test_SOURCES = weak_test.cc
weak_test_DEPENDENCIES = gcctestdir/ld
weak_test_LDFLAGS = -Bgcctestdir/


if TLS

check_PROGRAMS += tls_test
check_PROGRAMS += tls_pic_test
check_PROGRAMS += tls_shared_test
check_PROGRAMS += tls_shared_ie_test
tls_test_pic.o: tls_test.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
tls_test_file2_pic.o: tls_test_file2.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
tls_test_shared.so: tls_test_pic.o tls_test_file2_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared tls_test_pic.o tls_test_file2_pic.o

tls_test_pic_ie.o: tls_test.cc
	$(CXXCOMPILE) -c -fpic -ftls-model=initial-exec -o $@ $<
tls_test_file2_pic_ie.o: tls_test_file2.cc
	$(CXXCOMPILE) -c -fpic -ftls-model=initial-exec -o $@ $<
tls_test_ie_shared.so: tls_test_pic_ie.o tls_test_file2_pic_ie.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared tls_test_pic_ie.o tls_test_file2_pic_ie.o

tls_test_SOURCES = tls_test.cc tls_test_file2.cc tls_test_main.cc tls_test.h
tls_test_DEPENDENCIES = gcctestdir/ld
tls_test_LDFLAGS = -Bgcctestdir/
tls_test_LDADD = -lpthread

tls_pic_test_SOURCES = tls_test_main.cc
tls_pic_test_DEPENDENCIES = gcctestdir/ld tls_test_pic.o tls_test_file2_pic.o
tls_pic_test_LDFLAGS = -Bgcctestdir/
tls_pic_test_LDADD = tls_test_pic.o tls_test_file2_pic.o -lpthread

tls_shared_test_SOURCES = tls_test_main.cc
tls_shared_test_DEPENDENCIES = gcctestdir/ld tls_test_shared.so
tls_shared_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
tls_shared_test_LDADD = tls_test_shared.so -lpthread

tls_shared_ie_test_SOURCES = tls_test_main.cc
tls_shared_ie_test_DEPENDENCIES = gcctestdir/ld tls_test_ie_shared.so
tls_shared_ie_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
tls_shared_ie_test_LDADD = tls_test_ie_shared.so -lpthread

if STATIC_TLS
check_PROGRAMS += tls_static_test
check_PROGRAMS += tls_static_pic_test

tls_static_test_SOURCES = $(tls_test_SOURCES)
tls_static_test_DEPENDENCIES = $(tls_test_DEPENDENCIES)
tls_static_test_LDFLAGS = $(tls_test_LDFLAGS) -static
tls_static_test_LDADD = $(tls_test_LDADD)

tls_static_pic_test_SOURCES = $(tls_pic_test_SOURCES)
tls_static_pic_test_DEPENDENCIES = $(tls_pic_test_DEPENDENCIES)
tls_static_pic_test_LDFLAGS = $(tls_pic_test_LDFLAGS) -static
tls_static_pic_test_LDADD = $(tls_pic_test_LDADD)
endif

if FN_PTRS_IN_SO_WITHOUT_PIC
check_PROGRAMS += tls_shared_nonpic_test
tls_test_shared_nonpic.so: tls_test.o tls_test_file2.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared tls_test.o tls_test_file2.o

tls_shared_nonpic_test_SOURCES = tls_test_main.cc
tls_shared_nonpic_test_DEPENDENCIES = gcctestdir/ld tls_test_shared_nonpic.so
tls_shared_nonpic_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
tls_shared_nonpic_test_LDADD = tls_test_shared_nonpic.so -lpthread
endif FN_PTRS_IN_SO_WITHOUT_PIC

endif TLS


# Test --detect-odr-violations
check_SCRIPTS += debug_msg.sh

# Create the data files that debug_msg.sh analyzes.
check_DATA += debug_msg.err
MOSTLYCLEANFILES += debug_msg.err
debug_msg.o: debug_msg.cc
	$(CXXCOMPILE) -O0 -g -c -w -o $@ $(srcdir)/debug_msg.cc
odr_violation1.o: odr_violation1.cc
	$(CXXCOMPILE) -O0 -g -c -w -o $@ $(srcdir)/odr_violation1.cc
odr_violation2.o: odr_violation2.cc
	$(CXXCOMPILE) -O0 -g -c -w -o $@ $(srcdir)/odr_violation2.cc
debug_msg.err: debug_msg.o odr_violation1.o odr_violation2.o gcctestdir/ld
	@echo $(CXXLINK) -Bgcctestdir/ -Wl,--detect-odr-violations -o debug_msg debug_msg.o odr_violation1.o odr_violation2.o "2>$@"
	@if $(CXXLINK) -Bgcctestdir/ -Wl,--detect-odr-violations -o debug_msg debug_msg.o odr_violation1.o odr_violation2.o 2>$@; \
	then \
	  echo 1>&2 "Link of debug_msg should have failed"; \
	  rm -f $@; \
	  exit 1; \
	fi

# See if we can also detect problems when we're linking .so's, not .o's.
check_DATA += debug_msg_so.err
MOSTLYCLEANFILES += debug_msg_so.err
debug_msg.so: debug_msg.cc gcctestdir/ld
	$(CXXCOMPILE) -Bgcctestdir/ -O0 -g -shared -fPIC -w -o $@ $(srcdir)/debug_msg.cc
odr_violation1.so: odr_violation1.cc gcctestdir/ld
	$(CXXCOMPILE) -Bgcctestdir/ -O0 -g -shared -fPIC -w -o $@ $(srcdir)/odr_violation1.cc
odr_violation2.so: odr_violation2.cc gcctestdir/ld
	$(CXXCOMPILE) -Bgcctestdir/ -O0 -g -shared -fPIC -w -o $@ $(srcdir)/odr_violation2.cc
debug_msg_so.err: debug_msg.so odr_violation1.so odr_violation2.so gcctestdir/ld
	@echo $(CXXLINK) -Bgcctestdir/ -Wl,--detect-odr-violations -o debug_msg_so debug_msg.so odr_violation1.so odr_violation2.so "2>$@"
	@if $(CXXLINK) -Bgcctestdir/ -Wl,--detect-odr-violations -o debug_msg_so debug_msg.so odr_violation1.so odr_violation2.so 2>$@; \
	then \
	  echo 1>&2 "Link of debug_msg_so should have failed"; \
	  rm -f $@; \
	  exit 1; \
	fi

# We also want to make sure we do something reasonable when there's no
# debug info available.  For the best test, we use .so's.
check_DATA += debug_msg_ndebug.err
MOSTLYCLEANFILES += debug_msg_ndebug.err
debug_msg_ndebug.so: debug_msg.cc gcctestdir/ld
	$(CXXCOMPILE) -Bgcctestdir/ -O0 -g0 -shared -fPIC -w -o $@ $(srcdir)/debug_msg.cc
odr_violation1_ndebug.so: odr_violation1.cc gcctestdir/ld
	$(CXXCOMPILE) -Bgcctestdir/ -O0 -g0 -shared -fPIC -w -o $@ $(srcdir)/odr_violation1.cc
odr_violation2_ndebug.so: odr_violation2.cc gcctestdir/ld
	$(CXXCOMPILE) -Bgcctestdir/ -O0 -g0 -shared -fPIC -w -o $@ $(srcdir)/odr_violation2.cc
debug_msg_ndebug.err: debug_msg_ndebug.so odr_violation1_ndebug.so odr_violation2_ndebug.so gcctestdir/ld
	@echo $(CXXLINK) -Bgcctestdir/ -Wl,--detect-odr-violations -o debug_msg_ndebug debug_msg_ndebug.so odr_violation1_ndebug.so odr_violation2_ndebug.so "2>$@"
	@if $(CXXLINK) -Bgcctestdir/ -Wl,--detect-odr-violations -o debug_msg_ndebug debug_msg_ndebug.so odr_violation1_ndebug.so odr_violation2_ndebug.so 2>$@; \
	then \
	  echo 1>&2 "Link of debug_msg_ndebug should have failed"; \
	  rm -f $@; \
	  exit 1; \
	fi


# Similar to --detect-odr-violations: check for undefined symbols in .so's
check_SCRIPTS += undef_symbol.sh
check_DATA += undef_symbol.err
MOSTLYCLEANFILES += undef_symbol.err
undef_symbol.o: undef_symbol.cc
	$(CXXCOMPILE) -O0 -g -c -fPIC $<
undef_symbol.so: undef_symbol.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared undef_symbol.o
undef_symbol.err: undef_symbol_main.o undef_symbol.so gcctestdir/ld
	@echo $(CXXLINK) -Bgcctestdir/ -o undef_symbol_test undef_symbol_main.o undef_symbol.so "2>$@"
	@if $(CXXLINK) -Bgcctestdir/ -o undef_symbol_test undef_symbol_main.o undef_symbol.so 2>$@; \
	then \
	  echo 1>&2 "Link of undef_symbol_test should have failed"; \
	  rm -f $@; \
	  exit 1; \
	fi


# Test --compress-debug-sections.  FIXME: check we actually compress.
check_PROGRAMS += flagstest_compress_debug_sections
flagstest_compress_debug_sections: flagstest_debug.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -o $@ $< --compress-debug-sections=zlib
	test -s $@


# Test -o when emitting to a special file (such as something in /dev).
check_PROGRAMS += flagstest_o_specialfile
flagstest_o_specialfile: flagstest_debug.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -o /dev/stdout $< 2>&1 | cat > $@
	chmod a+x $@
	test -s $@

# The specialfile output has a tricky case when we also compress debug
# sections, because it requires output-file resizing.
check_PROGRAMS += flagstest_o_specialfile_and_compress_debug_sections
flagstest_o_specialfile_and_compress_debug_sections: flagstest_debug.o \
		gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -o /dev/stdout $< --compress-debug-sections=zlib 2>&1 | cat > $@
	chmod a+x $@
	test -s $@

# Test symbol versioning.
check_PROGRAMS += ver_test
ver_test_SOURCES = ver_test_main.cc
ver_test_DEPENDENCIES = gcctestdir/ld ver_test_1.so ver_test_2.so ver_test_4.so
ver_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
ver_test_LDADD = ver_test_1.so ver_test_2.so ver_test_4.so
ver_test_1.so: ver_test_1.o ver_test_2.so ver_test_3.o ver_test_4.so gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared ver_test_1.o ver_test_2.so ver_test_3.o ver_test_4.so
ver_test_2.so: ver_test_2.o $(srcdir)/ver_test_2.script ver_test_4.so gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared -Wl,--version-script,$(srcdir)/ver_test_2.script ver_test_2.o ver_test_4.so
ver_test_4.so: ver_test_4.o $(srcdir)/ver_test_4.script gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared -Wl,--version-script,$(srcdir)/ver_test_4.script ver_test_4.o
ver_test_1.o: ver_test_1.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
ver_test_2.o: ver_test_2.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
ver_test_3.o: ver_test_3.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
ver_test_4.o: ver_test_4.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<

check_PROGRAMS += ver_test_2
ver_test_2_SOURCES = ver_test_main_2.cc
ver_test_2_DEPENDENCIES = gcctestdir/ld ver_test_4.so ver_test_2.so
ver_test_2_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
ver_test_2_LDADD = ver_test_4.so ver_test_2.so

if READELF

check_SCRIPTS += ver_test_2.sh
check_DATA += ver_test_2.syms
ver_test_2.syms: ver_test_2
	readelf -s $< >$@ 2>/dev/null

endif

if READELF

ver_test_5.so: ver_test_5.o $(srcdir)/ver_test_5.script ver_test_4.so gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared -Wl,--version-script,$(srcdir)/ver_test_5.script ver_test_5.o ver_test_4.so
ver_test_5.o: ver_test_5.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
check_SCRIPTS += ver_test_5.sh
check_DATA += ver_test_5.syms
ver_test_5.syms: ver_test_5.so
	readelf -s $< >$@ 2>/dev/null

endif

check_PROGRAMS += ver_test_6
ver_test_6_SOURCES = ver_test_6.c
ver_test_6_DEPENDENCIES = gcctestdir/ld ver_test_2.so
ver_test_6_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
ver_test_6_LDADD = ver_test_2.so

check_PROGRAMS += script_test_1
script_test_1_SOURCES = script_test_1.cc
script_test_1_DEPENDENCIES = gcctestdir/ld script_test_1.t
script_test_1_LDFLAGS = -Bgcctestdir/ -Wl,-R,. -T $(srcdir)/script_test_1.t

check_PROGRAMS += script_test_2
script_test_2_SOURCES = script_test_2.cc script_test_2a.cc script_test_2b.cc
script_test_2_DEPENDENCIES = gcctestdir/ld script_test_2.t
script_test_2_LDFLAGS = -Bgcctestdir/ -Wl,-R,. -T $(srcdir)/script_test_2.t

check_PROGRAMS += justsyms
justsyms_SOURCES = justsyms_1.cc
justsyms_DEPENDENCIES = gcctestdir/ld justsyms_2r.o
justsyms_LDFLAGS = -Bgcctestdir/ -Wl,-R,justsyms_2r.o
justsyms_2.o: justsyms_2.cc
	$(CXXCOMPILE) -c -o $@ $<
justsyms_2r.o: justsyms_2.o gcctestdir/ld
	gcctestdir/ld -o $@ -r -T $(srcdir)/justsyms.t justsyms_2.o

check_PROGRAMS += binary_test
binary_test_SOURCES = binary_test.cc
binary_test_DEPENDENCIES = gcctestdir/ld binary.txt
binary_test_LDFLAGS = -Bgcctestdir/ -Wl,--format,binary,binary.txt,--format,elf
# Copy the file to the build directory to avoid worrying about the
# full pathname in the generated symbols.
binary.txt: $(srcdir)/binary.in
	rm -f $@
	$(LN_S) $< $@

if OBJDUMP_AND_CPPFILT
check_SCRIPTS += ver_matching_test.sh
check_DATA += ver_matching_test.stdout
MOSTLYCLEANFILES += ver_matching_test.stdout
ver_matching_def.so: ver_matching_def.cc gcctestdir/ld
	$(CXXLINK) -O0 -Bgcctestdir/ -shared $(srcdir)/ver_matching_def.cc -Wl,--version-script=$(srcdir)/version_script.map
ver_matching_test.stdout: ver_matching_def.so
	objdump -T ver_matching_def.so | c++filt > ver_matching_test.stdout

check_PROGRAMS += script_test_3
check_SCRIPTS += script_test_3.sh
check_DATA += script_test_3.stdout
MOSTLYCLEANFILES += script_test_3.stdout
script_test_3: basic_test.o gcctestdir/ld script_test_3.t
	$(CXXLINK) -Bgcctestdir/ basic_test.o -T $(srcdir)/script_test_3.t
script_test_3.stdout: script_test_3
	objdump -p script_test_3 > script_test_3.stdout
endif OBJDUMP_AND_CPPFILT

endif GCC
endif NATIVE_LINKER
