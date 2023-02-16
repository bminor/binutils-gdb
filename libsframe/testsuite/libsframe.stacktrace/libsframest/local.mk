check_LTLIBRARIES =

if HAVE_SFRAME_AS
if HAVE_DL_ITERATE_PHDR
  check_LTLIBRARIES += %D%/libsframest.la
endif
endif

%C%_libsframest_la_SOURCES = %D%/sframe-stacktrace.c \
			     %D%/sframe-state.c \
			     %D%/sframe-stacktrace-err.c
%C%_libsframest_la_LIBADD = ${top_builddir}/libsframe.la
%C%_libsframest_la_CPPFLAGS = -I${top_srcdir}/../include \
			      -I${top_srcdir}/%D%/include \
			      -Wall -Wno-unused-but-set-variable
%C%_libsframest_la_CFLAGS = -Wa,--gsframe
