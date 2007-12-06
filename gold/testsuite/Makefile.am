# Process this file with automake to generate Makefile.in

# As far as I can tell automake testing support assumes that the build
# system and the host system are the same.  So these tests will not
# work when building with a cross-compiler.

AUTOMAKE_OPTIONS =

# The two_file_test tests -fmerge-constants, so we simply always turn
# it on.  This may need to be controlled by a configure option
# eventually.
AM_CXXFLAGS = $(WARN_CXXFLAGS) $(LFS_CXXFLAGS) -fmerge-constants

INCLUDES = \
	-I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../../include \
	-I$(srcdir)/../../elfcpp \
	-DLOCALEDIR="\"$(datadir)/locale\"" \
	@INCINTL@

if THREADS
THREADSLIB = -lpthread
endif

check_SCRIPTS = debug_msg.sh
check_DATA =

TESTS = object_unittest

if GCC

if NATIVE_LINKER

TESTS += debug_msg.sh undef_symbol.sh

check_DATA += debug_msg.err debug_msg_so.err debug_msg_ndebug.err
check_DATA += undef_symbol.err

NATIVE_PROGS = \
	constructor_test \
	constructor_static_test \
	two_file_test \
	two_file_static_test \
	two_file_pic_test \
	two_file_shared_1_test \
	two_file_shared_2_test \
	two_file_shared_1_pic_2_test \
	two_file_shared_2_pic_1_test \
	two_file_same_shared_test \
	two_file_separate_shared_12_test \
	two_file_separate_shared_21_test \
	exception_test \
	exception_static_test \
	exception_shared_1_test \
	exception_shared_2_test \
	exception_same_shared_test \
	exception_separate_shared_12_test \
	exception_separate_shared_21_test \
	weak_test \
	flagstest_compress_debug_sections \
	flagstest_o_specialfile \
	flagstest_o_specialfile_and_compress_debug_sections

if FN_PTRS_IN_SO_WITHOUT_PIC

NATIVE_PROGS += \
	two_file_shared_1_nonpic_test \
	two_file_shared_2_nonpic_test \
	two_file_same_shared_nonpic_test \
	two_file_separate_shared_12_nonpic_test \
	two_file_separate_shared_21_nonpic_test

endif

if TLS

NATIVE_PROGS += \
	tls_test \
	tls_pic_test \
	tls_shared_test

if STATIC_TLS

NATIVE_PROGS += \
	tls_static_test \
	tls_static_pic_test

endif

if FN_PTRS_IN_SO_WITHOUT_PIC

NATIVE_PROGS += \
	tls_shared_nonpic_test

endif
endif

NATIVE_TESTING = \
	basic_test \
	basic_pic_test \
	basic_static_test \
	basic_static_pic_test \
	$(NATIVE_PROGS)

endif
endif

TESTS += $(NATIVE_TESTING)

check_LIBRARIES = libgoldtest.a
libgoldtest_a_SOURCES = test.cc testmain.cc testfile.cc

DEPENDENCIES = \
	libgoldtest.a ../libgold.a ../../libiberty/libiberty.a $(LIBINTL_DEP)
LDADD = libgoldtest.a ../libgold.a ../../libiberty/libiberty.a $(LIBINTL) \
	$(THREADSLIB)

check_PROGRAMS = object_unittest $(NATIVE_PROGS)

object_unittest_SOURCES = object_unittest.cc

if GCC

if NATIVE_LINKER

gcctestdir/ld: ../ld-new
	test -d gcctestdir || mkdir -p gcctestdir
	rm -f gcctestdir/ld
	(cd gcctestdir && $(LN_S) ../../ld-new ld)

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
debug_msg.so: debug_msg.cc
	$(CXXCOMPILE) -O0 -g -shared -fPIC -w -o $@ $(srcdir)/debug_msg.cc
odr_violation1.so: odr_violation1.cc
	$(CXXCOMPILE) -O0 -g -shared -fPIC -w -o $@ $(srcdir)/odr_violation1.cc
odr_violation2.so: odr_violation2.cc
	$(CXXCOMPILE) -O0 -g -shared -fPIC -w -o $@ $(srcdir)/odr_violation2.cc
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
debug_msg_ndebug.so: debug_msg.cc
	$(CXXCOMPILE) -O0 -g0 -shared -fPIC -w -o $@ $(srcdir)/debug_msg.cc
odr_violation1_ndebug.so: odr_violation1.cc
	$(CXXCOMPILE) -O0 -g0 -shared -fPIC -w -o $@ $(srcdir)/odr_violation1.cc
odr_violation2_ndebug.so: odr_violation2.cc
	$(CXXCOMPILE) -O0 -g0 -shared -fPIC -w -o $@ $(srcdir)/odr_violation2.cc
debug_msg_ndebug.err: debug_msg_ndebug.so odr_violation1_ndebug.so odr_violation2_ndebug.so gcctestdir/ld
	@echo $(CXXLINK) -Bgcctestdir/ -Wl,--detect-odr-violations -o debug_msg_ndebug debug_msg_ndebug.so odr_violation1_ndebug.so odr_violation2_ndebug.so "2>$@"
	@if $(CXXLINK) -Bgcctestdir/ -Wl,--detect-odr-violations -o debug_msg_ndebug debug_msg_ndebug.so odr_violation1_ndebug.so odr_violation2_ndebug.so 2>$@; \
	then \
	  echo 1>&2 "Link of debug_msg_ndebug should have failed"; \
	  rm -f $@; \
	  exit 1; \
	fi

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

# Test various flags.  The actual file we choose to link is pretty
# much arbitrary: we just want to make sure we can link it and run it
# successfully, even when using flags.

flagstest_debug.o: constructor_test.cc
	$(CXXCOMPILE) -O0 -g -c -o $@ $<
flagstest_ndebug.o: constructor_test.cc
	$(CXXCOMPILE) -O0 -c -o $@ $<

flagstest_compress_debug_sections: flagstest_debug.o
	$(CXXLINK) -Bgcctestdir/ -o $@ $< --compress-debug-sections=zlib
	test -s $@

flagstest_o_specialfile: flagstest_debug.o
	$(CXXLINK) -Bgcctestdir/ -o /dev/stdout $< 2>&1 | cat > $@
	chmod a+x $@
	test -s $@

# The specialfile output has a tricky case when we also compress debug
# sections, because it requires output-file resizing.
flagstest_o_specialfile_and_compress_debug_sections: flagstest_debug.o
	$(CXXLINK) -Bgcctestdir/ -o /dev/stdout $< --compress-debug-sections=zlib 2>&1 | cat > $@
	chmod a+x $@
	test -s $@


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

two_file_static_test_SOURCES = \
	two_file_test_1.cc two_file_test_2.cc two_file_test_main.cc
two_file_static_test_DEPENDENCIES = gcctestdir/ld
two_file_static_test_LDFLAGS = -Bgcctestdir/ -static

two_file_pic_test_SOURCES = two_file_test_main.cc
two_file_pic_test_DEPENDENCIES = \
	gcctestdir/ld two_file_test_1_pic.o two_file_test_2_pic.o
two_file_pic_test_LDFLAGS = -Bgcctestdir/
two_file_pic_test_LDADD = two_file_test_1_pic.o two_file_test_2_pic.o

two_file_shared_1_test_SOURCES = two_file_test_2.cc two_file_test_main.cc
two_file_shared_1_test_DEPENDENCIES = gcctestdir/ld two_file_shared_1.so
two_file_shared_1_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_shared_1_test_LDADD = two_file_shared_1.so

two_file_shared_2_test_SOURCES = two_file_test_1.cc two_file_test_main.cc
two_file_shared_2_test_DEPENDENCIES = gcctestdir/ld two_file_shared_2.so
two_file_shared_2_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_shared_2_test_LDADD = two_file_shared_2.so

two_file_shared_1_pic_2_test_SOURCES = two_file_test_main.cc
two_file_shared_1_pic_2_test_DEPENDENCIES = \
	gcctestdir/ld two_file_shared_1.so two_file_test_1_pic.o
two_file_shared_1_pic_2_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
two_file_shared_1_pic_2_test_LDADD = two_file_test_1_pic.o two_file_shared_2.so

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

two_file_test_1_pic.o: two_file_test_1.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<
two_file_test_2_pic.o: two_file_test_2.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<

two_file_shared_1.so: two_file_test_1_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_1_pic.o
two_file_shared_2.so: two_file_test_2_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_2_pic.o
two_file_shared.so: two_file_test_1_pic.o two_file_test_2_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_1_pic.o two_file_test_2_pic.o

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

two_file_shared_1_nonpic.so: two_file_test_1.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_1.o
two_file_shared_2_nonpic.so: two_file_test_2.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_2.o
two_file_shared_nonpic.so: two_file_test_1.o two_file_test_2.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared two_file_test_1.o two_file_test_2.o

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

exception_shared_1.so: exception_test_1_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared exception_test_1_pic.o
exception_shared_2.so: exception_test_2_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared exception_test_2_pic.o
exception_shared.so: exception_test_1_pic.o exception_test_2_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared exception_test_1_pic.o exception_test_2_pic.o

weak_test_SOURCES = weak_test.cc
weak_test_LDFLAGS = -Bgcctestdir/

if TLS

tls_test_SOURCES = tls_test.cc tls_test_file2.cc tls_test_main.cc tls_test.h
tls_test_DEPENDENCIES = gcctestdir/ld
tls_test_LDFLAGS = -Bgcctestdir/
tls_test_LDADD = -lpthread

tls_pic_test_SOURCES = tls_test_main.cc
tls_pic_test_DEPENDENCIES = gcctestdir/ld tls_test_pic.o tls_test_file2_pic.o
tls_pic_test_LDFLAGS = -Bgcctestdir/
tls_pic_test_LDADD = tls_test_pic.o tls_test_file2_pic.o -lpthread

tls_static_test_SOURCES = tls_test.cc tls_test_file2.cc tls_test_main.cc
tls_static_test_DEPENDENCIES = gcctestdir/ld
tls_static_test_LDFLAGS = -Bgcctestdir/ -static
tls_static_test_LDADD = -lpthread

tls_static_pic_test_SOURCES = tls_test_main.cc
tls_static_pic_test_DEPENDENCIES = gcctestdir/ld tls_test_pic.o tls_test_file2_pic.o
tls_static_pic_test_LDFLAGS = -Bgcctestdir/ -static
tls_static_pic_test_LDADD = tls_test_pic.o tls_test_file2_pic.o -lpthread

tls_shared_test_SOURCES = tls_test_main.cc
tls_shared_test_DEPENDENCIES = gcctestdir/ld tls_test_shared.so
tls_shared_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
tls_shared_test_LDADD = tls_test_shared.so -lpthread

tls_test_pic.o: tls_test.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<

tls_test_file2_pic.o: tls_test_file2.cc
	$(CXXCOMPILE) -c -fpic -o $@ $<

tls_test_shared.so: tls_test_pic.o tls_test_file2_pic.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared tls_test_pic.o tls_test_file2_pic.o

if FN_PTRS_IN_SO_WITHOUT_PIC

tls_shared_nonpic_test_SOURCES = tls_test_main.cc
tls_shared_nonpic_test_DEPENDENCIES = gcctestdir/ld tls_test_shared_nonpic.so
tls_shared_nonpic_test_LDFLAGS = -Bgcctestdir/ -Wl,-R,.
tls_shared_nonpic_test_LDADD = tls_test_shared_nonpic.so -lpthread

tls_test_shared_nonpic.so: tls_test.o tls_test_file2.o gcctestdir/ld
	$(CXXLINK) -Bgcctestdir/ -shared tls_test.o tls_test_file2.o

endif

endif

endif
endif
