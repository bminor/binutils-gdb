check_LTLIBRARIES =

if HAVE_DL_ITERATE_PHDR
# Build libsframest only when not cross-building
if !CROSS_COMPILE
check_LTLIBRARIES += %D%/libsframest.la
endif
endif

%C%_libsframest_la_SOURCES = %D%/sframe-stacktrace.c \
			     %D%/sframe-state.c \
			     %D%/sframe-stacktrace-err.c
%C%_libsframest_la_LIBADD = ${top_builddir}/libsframe.la
%C%_libsframest_la_CPPFLAGS = -I${top_srcdir}/../include \
			      -I${top_srcdir}/%D%/include \
			      -B${top_builddir}/tmpdir/libsframe \
			      -Wall -Wno-unused-but-set-variable
%C%_libsframest_la_CFLAGS = -B${top_builddir}/tmpdir/libsframe \
			    -Wa,--gsframe
%C%_libsframest_la_LDFLAGS = -B${top_builddir}/tmpdir/libsframe
