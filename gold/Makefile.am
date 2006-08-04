# Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS =

SUBDIRS = po

tooldir = $(exec_prefix)/$(target_alias)

ACLOCAL_AMFLAGS = -I ../bfd -I ../config

AM_CXXFLAGS = $(WARN_CXXFLAGS) $(LFS_CXXFLAGS)

INCLUDES = -D_GNU_SOURCE \
	-I$(srcdir) -I$(srcdir)/../include -I$(srcdir)/../elfcpp \
	-DLOCALEDIR="\"$(datadir)/locale\"" \
	@INCINTL@

noinst_PROGRAMS = ld-new

CFILES = \
	dirsearch.cc \
	fileread.cc \
	gold.cc \
	gold-threads.cc \
	object.cc \
	options.cc \
	readsyms.cc \
	workqueue.cc

HFILES = \
	dirsearch.h \
	fileread.h \
	gold.h \
	gold-threads.h \
	object.h \
	options.h \
	readsyms.h \
	symtab.h \
	target.h \
	targetsize.h \
	workqueue.h

OFILES = gold.o options.o

POTFILES= $(CFILES) $(HFILES)

po/POTFILES.in: @MAINT@ Makefile
	for f in $(POTFILES); do echo $$f; done | LC_COLLATE= sort > tmp \
	  && mv tmp $(srcdir)/po/POTFILES.in

ld_new_SOURCES = $(CFILES) $(HFILES)
ld_new_DEPENDENCIES = $(LIBINTL_DEP)
ld_new_LDADD = $(LIBINTL)

.PHONY: install-exec-local

install-exec-local: ld-new$(EXEEXT)
	$(mkinstalldirs) $(DESTDIR)$(bindir) $(DESTDIR)$(tooldir)/bin
	n=`echo ld | sed '$(transform)'; \
	$(INSTALL_PROGRAM) ld-new$(EXEEXT) $(DESTDIR)$(bindir)/$${n}$(EXEEXT); \
	if test "$(bindir)" != "$(tooldir)/bin"; then \
	  rm -f $(DESTDIR)$(tooldir)/bin/ld$(EXEEXT); \
	  ln $(DESTDIR)$(bindir)/$${n}$(EXEEXT) $(DESTDIR)$(tooldir)/bin/ld$(EXEEXT) >/dev/null 2>/dev/null \
	    || $(INSTALL_PROGRAM) ld-new$(EXEEXT) $(DESTDIR)$(tooldir)/bin/ld$(EXEEXT); \
	fi

# We want install to imply install-info as per GNU standards, despite
# the cygnus option.
install-data-local: install-info
