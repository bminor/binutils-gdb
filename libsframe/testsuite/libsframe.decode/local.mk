check_PROGRAMS =
if HAVE_COMPAT_DEJAGNU
  check_PROGRAMS += %D%/be-flipping %D%/frecnt-1 %D%/frecnt-2
endif

%C%_be_flipping_SOURCES = %D%/be-flipping.c
%C%_be_flipping_LDADD = $(testsuite_LDADD)
%C%_be_flipping_CPPFLAGS = $(testsuite_CPPFLAGS)

%C%_frecnt_1_SOURCES = %D%/frecnt-1.c
%C%_frecnt_1_LDADD = $(testsuite_LDADD)
%C%_frecnt_1_CPPFLAGS = $(testsuite_CPPFLAGS)

%C%_frecnt_2_SOURCES = %D%/frecnt-2.c
%C%_frecnt_2_LDADD = $(testsuite_LDADD)
%C%_frecnt_2_CPPFLAGS = $(testsuite_CPPFLAGS)
