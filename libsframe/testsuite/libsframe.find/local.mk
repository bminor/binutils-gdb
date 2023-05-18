if HAVE_COMPAT_DEJAGNU
  check_PROGRAMS += %D%/findfre-1
endif

%C%_findfre_1_SOURCES = %D%/findfre-1.c
%C%_findfre_1_LDADD = ${top_builddir}/libsframe.la
%C%_findfre_1_CPPFLAGS = -I${top_srcdir}/../include -Wall
