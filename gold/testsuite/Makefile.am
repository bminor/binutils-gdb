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

NATIVE_PROGS = \
	constructor_test \
	constructor_static_test \
	two_file_test \
	two_file_shared_1_test \
	two_file_shared_2_test \
	two_file_same_shared_test \
	two_file_separate_shared_12_test \
	two_file_separate_shared_21_test \
	exception_test \
	exception_static_test \
	exception_shared_1_test \
	exception_shared_2_test \
	exception_same_shared_test \
	exception_separate_shared_12_test \
	exception_separate_shared_21_test

if FN_PTRS_IN_SO_WITHOUT_PIC

NATIVE_PROGS += \
	two_file_shared_1_nonpic_test \
	two_file_shared_2_nonpic_test \
	two_file_same_shared_nonpic_test \
	two_file_separate_shared_12_nonpic_test \
	two_file_separate_shared_21_nonpic_test

endif

NATIVE_TESTING = \
	basic_test \
	basic_pic_test \
	basic_static_test \
	basic_static_pic_test \
	$(NATIVE_PROGS)

endif
endif

TESTS = object_unittest $(NATIVE_TESTING)

check_LIBRARIES = libgoldtest.a
libgoldtest_a_SOURCES = test.cc testmain.cc testfile.cc

DEPENDENCIES = \
	libgoldtest.a ../libgold.a ../../libiberty/libiberty.a $(LIBINTL_DEP)
LDADD = libgoldtest.a ../libgold.a ../../libiberty/libiberty.a $(LIBINTL)

check_PROGRAMS = object_unittest $(NATIVE_PROGS)

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

constructor_test_SOURCES = constructor_test.cc
constructor_test_DEPENDENCIES = gcctestdir/ld
constructor_test_LDFLAGS = -Bgcctestdir/

constructor_static_test_SOURCES = constructor_test.cc
constructor_static_test_DEPENDENCIES = gcctestdir/ld
constructor_static_test_LDFLAGS = -Bgcctestdir/ -static

two_file_test_SOURCES = \
	two_file_test_1.cc \
	two_file_test_2.cc \
	two_file_test_main.cc \
	two_file_test.h
two_file_test_DEPENDENCIES = gcctestdir/ld
two_file_test_LDFLAGS = -Bgcctestdir/

two_file_shared_1_test_SOURCES = two_file_test_2.cc two_file_test_main.cc
two_file_shared_1_test_DEPENDENCIES = gcctestdir/ld two_file_shared_1.so
two_file_shared_1_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_shared_1_test_LDADD = two_file_shared_1.so

two_file_shared_2_test_SOURCES = two_file_test_1.cc two_file_test_main.cc
two_file_shared_2_test_DEPENDENCIES = gcctestdir/ld two_file_shared_2.so
two_file_shared_2_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_shared_2_test_LDADD = two_file_shared_2.so

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

two_file_test_1_pic.o: two_file_test_1.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
two_file_test_2_pic.o: two_file_test_2.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<

two_file_shared_1.so: two_file_test_1_pic.o
	$(CXXLINK) -shared two_file_test_1_pic.o
two_file_shared_2.so: two_file_test_2_pic.o
	$(CXXLINK) -shared two_file_test_2_pic.o
two_file_shared.so: two_file_test_1_pic.o two_file_test_2_pic.o
	$(CXXLINK) -shared two_file_test_1_pic.o two_file_test_2_pic.o

# The nonpic tests will fail on platforms which can not put non-PIC
# code into shared libraries, so we just don't run them in that case.

if FN_PTRS_IN_SO_WITHOUT_PIC

two_file_shared_1_nonpic_test_SOURCES = \
	two_file_test_2.cc two_file_test_main.cc
two_file_shared_1_nonpic_test_DEPENDENCIES = \
	gcctestdir/ld two_file_shared_1_nonpic.so
two_file_shared_1_nonpic_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_shared_1_nonpic_test_LDADD = two_file_shared_1_nonpic.so

two_file_shared_2_nonpic_test_SOURCES = \
	two_file_test_1.cc two_file_test_main.cc
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

two_file_shared_1_nonpic.so: two_file_test_1.o
	$(CXXLINK) -shared two_file_test_1.o
two_file_shared_2_nonpic.so: two_file_test_2.o
	$(CXXLINK) -shared two_file_test_2.o
two_file_shared_nonpic.so: two_file_test_1.o two_file_test_2.o
	$(CXXLINK) -shared two_file_test_1.o two_file_test_2.o

endif

exception_test_SOURCES = \
	exception_test_main.cc \
	exception_test_1.cc \
	exception_test_2.cc \
	exception_test.h
exception_test_DEPENDENCIES = gcctestdir/ld
exception_test_LDFLAGS = -Bgcctestdir/

exception_static_test_SOURCES = \
	exception_test_main.cc \
	exception_test_1.cc \
	exception_test_2.cc
exception_static_test_DEPENDENCIES = gcctestdir/ld
exception_static_test_LDFLAGS = -Bgcctestdir/ -static

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

exception_test_1_pic.o: exception_test_1.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
exception_test_2_pic.o: exception_test_2.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<

exception_shared_1.so: exception_test_1_pic.o
	$(CXXLINK) -shared exception_test_1_pic.o
exception_shared_2.so: exception_test_2_pic.o
	$(CXXLINK) -shared exception_test_2_pic.o
exception_shared.so: exception_test_1_pic.o exception_test_2_pic.o
	$(CXXLINK) -shared exception_test_1_pic.o exception_test_2_pic.o

endif
endif
