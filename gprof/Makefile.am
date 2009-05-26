## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = cygnus
ACLOCAL_AMFLAGS = -I .. -I ../config -I ../bfd

SUFFIXES = .m

SUBDIRS = po

BASEDIR = $(srcdir)/..
BFDDIR = $(BASEDIR)/bfd
INCDIR	= $(BASEDIR)/include

WARN_CFLAGS = @WARN_CFLAGS@
NO_WERROR = @NO_WERROR@
AM_CFLAGS = $(WARN_CFLAGS)

MKDEP = gcc -MM

INCLUDES = -DDEBUG -I../bfd -I$(srcdir)/../include \
	-I$(srcdir)/../bfd @INCINTL@ -I. \
	-DLOCALEDIR="\"$(datadir)/locale\""

bin_PROGRAMS = gprof

## Convenience var listing pure sources.
sources = basic_blocks.c call_graph.c cg_arcs.c cg_dfn.c \
	cg_print.c corefile.c gmon_io.c gprof.c hertz.c hist.c source.c \
	search_list.c symtab.c sym_ids.c utils.c \
	i386.c alpha.c vax.c tahoe.c sparc.c mips.c
gprof_SOURCES = $(sources) flat_bl.c bsd_callg_bl.c fsf_callg_bl.c
gprof_DEPENDENCIES = ../bfd/libbfd.la ../libiberty/libiberty.a $(LIBINTL_DEP)
gprof_LDADD = ../bfd/libbfd.la ../libiberty/libiberty.a $(LIBINTL)

noinst_HEADERS = \
	basic_blocks.h call_graph.h cg_arcs.h cg_dfn.h cg_print.h \
	corefile.h gmon.h gmon_io.h gmon_out.h gprof.h hertz.h hist.h \
	search_list.h source.h sym_ids.h symtab.h utils.h

BUILT_SOURCES = flat_bl.c bsd_callg_bl.c fsf_callg_bl.c
EXTRA_DIST    = $(BUILT_SOURCES) bbconv.pl $(man_MANS)

diststuff: $(BUILT_SOURCES) info $(man_MANS)

# This empty rule is a hack against gmake patched by Apple.
%.o:%.m

.m.c:
	awk -f $(srcdir)/gen-c-prog.awk > ./$*.c \
	    FUNCTION=`(echo $*|sed -e 's,.*/,,g' -e 's/_bl//')`_blurb \
	    FILE=$*.m $(srcdir)/$*.m

POTFILES = $(sources) $(noinst_HEADERS)
po/POTFILES.in: @MAINT@ Makefile
	for f in $(POTFILES); do echo $$f; done | LC_ALL=C sort > tmp \
	  && mv tmp $(srcdir)/po/POTFILES.in

MANCONF  = -Dman

TEXI2POD = perl $(srcdir)/../etc/texi2pod.pl $(AM_MAKEINFOFLAGS)

POD2MAN = pod2man --center="GNU" --release="binutils-$(VERSION)" --section=1

info_TEXINFOS = gprof.texi
gprof_TEXINFOS = config.texi
man_MANS = gprof.1

AM_MAKEINFOFLAGS = -I "$(srcdir)" -I "$(top_srcdir)/../bfd/doc" -I ../bfd/doc
TEXI2DVI = texi2dvi -I "$(srcdir)" -I "$(top_srcdir)/../bfd/doc" -I ../bfd/doc

config.texi:
	echo "@set top_srcdir $(top_srcdir)" >> ./config.texi

# Build the man page from the texinfo file
# The sed command removes the no-adjust Nroff command so that
# the man output looks standard.
gprof.1: $(srcdir)/gprof.texi config.texi
	touch $@
	-$(TEXI2POD) $(MANCONF) -Dgprof < $(srcdir)/gprof.texi > gprof.pod
	-($(POD2MAN) gprof.pod | \
	       sed -e '/^.if n .na/d' > $@.T$$$$ && \
	       mv -f $@.T$$$$ $@) || \
	       (rm -f $@.T$$$$ && exit 1)
	rm -f gprof.pod

.PHONY: install-pdf install-pdf-am install-pdf-recursive

pdf__strip_dir = `echo $$p | sed -e 's|^.*/||'`;

install-pdf: install-pdf-recursive install-pdf-am
 
install-pdf-am: $(PDFS)
	@$(NORMAL_INSTALL)
	test -z "$(pdfdir)" || $(mkinstalldirs) "$(DESTDIR)$(pdfdir)"
	@list='$(PDFS)'; for p in $$list; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  f=$(pdf__strip_dir) \
	  echo " $(INSTALL_DATA) '$$d$$p' '$(DESTDIR)$(pdfdir)/$$f'"; \
	  $(INSTALL_DATA) "$$d$$p" "$(DESTDIR)$(pdfdir)/$$f"; \
	done

install-pdf-recursive:
	@failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	dot_seen=no; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    dot_seen=yes; \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	  || eval $$failcom; \
	done; \
	if test "$$dot_seen" = "no"; then \
	  $(MAKE) $(AM_MAKEFLAGS) "$$target-am" || exit 1; \
	fi; test -z "$$fail"

.PHONY: install-html install-html-am install-html-recursive

html__strip_dir = `echo $$p | sed -e 's|^.*/||'`;

install-html: install-html-recursive  install-html-am

install-html-am: $(HTMLS)
	@$(NORMAL_INSTALL)
	test -z "$(htmldir)" || $(mkdir_p) "$(DESTDIR)$(htmldir)"
	@list='$(HTMLS)'; for p in $$list; do \
	  if test -f "$$p" || test -d "$$p"; then d=""; else d="$(srcdir)/"; fi; \
	  f=$(html__strip_dir) \
	  if test -d "$$d$$p"; then \
	    echo " $(mkdir_p) '$(DESTDIR)$(htmldir)/$$f'"; \
	    $(mkdir_p) "$(DESTDIR)$(htmldir)/$$f" || exit 1; \
	    echo " $(INSTALL_DATA) '$$d$$p'/* '$(DESTDIR)$(htmldir)/$$f'"; \
	    $(INSTALL_DATA) "$$d$$p"/* "$(DESTDIR)$(htmldir)/$$f"; \
	  else \
	    echo " $(INSTALL_DATA) '$$d$$p' '$(DESTDIR)$(htmldir)/$$f'"; \
	    $(INSTALL_DATA) "$$d$$p" "$(DESTDIR)$(htmldir)/$$f"; \
	  fi; \
	done

install-html-recursive:
	@failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	dot_seen=no; \
	target=`echo $@ | sed s/-recursive//`; \
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  echo "Making $$target in $$subdir"; \
	  if test "$$subdir" = "."; then \
	    dot_seen=yes; \
	    local_target="$$target-am"; \
	  else \
	    local_target="$$target"; \
	  fi; \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$local_target) \
	  || eval $$failcom; \
	done; \
	if test "$$dot_seen" = "no"; then \
	  $(MAKE) $(AM_MAKEFLAGS) "$$target-am" || exit 1; \
	fi; test -z "$$fail"

# We want install to imply install-info as per GNU standards, despite the
# cygnus option.
install-data-local: install-info

# Targets to rebuild dependencies in this Makefile.
# Have to get rid of DEP1 here so that "$?" later includes all sources.
DEP: dep.sed $(gprof_SOURCES) $(noinst_HEADERS) gconfig.h
	rm -f DEP1
	$(MAKE) MKDEP="$(MKDEP)" DEP1
	echo '# IF YOU PUT ANYTHING HERE IT WILL GO AWAY' >> DEP1
	if grep ' /' DEP1 > /dev/null 2> /dev/null; then \
	  echo 'make DEP failed!'; exit 1; \
	else \
	  mv -f DEP1 $@; \
	fi

DEP1: $(gprof_SOURCES)
	echo '# DO NOT DELETE THIS LINE -- mkdep uses it.' > DEP2
	echo '# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.' >> DEP2
	for f in $?; do \
	  $(MKDEP) $(INCLUDES) $(CFLAGS) $$f > DEPA; \
	  sed -n -e '1s/: .*/: \\/p' -e q < DEPA >> DEP2; \
	  sed -e '1s/.*: //' -f dep.sed < DEPA | LC_ALL=C sort | uniq | \
	    sed -e 's/^[AB]/  /' -e '$$s/ \\$$//' >> DEP2; \
	done
	rm -f DEPA
	mv -f DEP2 $@

dep.sed: dep-in.sed config.status
	objdir=`pwd`; \
	sed <$(srcdir)/dep-in.sed >dep.sed	\
		-e 's!@INCDIR@!$(INCDIR)!'	\
		-e 's!@BFDDIR@!$(BFDDIR)!'	\
		-e 's!@SRCDIR@!$(srcdir)!'	\
		-e "s!@OBJDIR@!$${objdir}!"	\
		-e 's!@TOPDIR@!'`echo $(srcdir) | sed -e s,/gprof$$,,`'!'

dep: DEP
	sed -e '/^..DO NOT DELETE THIS LINE/,$$d' < Makefile > tmp-Makefile
	cat DEP >> tmp-Makefile
	$(SHELL) $(srcdir)/../move-if-change tmp-Makefile Makefile

dep-in: DEP
	sed -e '/^..DO NOT DELETE THIS LINE/,$$d' < $(srcdir)/Makefile.in > tmp-Makefile.in
	cat DEP >> tmp-Makefile.in
	$(SHELL) $(srcdir)/../move-if-change tmp-Makefile.in $(srcdir)/Makefile.in

dep-am: DEP
	sed -e '/^..DO NOT DELETE THIS LINE/,$$d' < $(srcdir)/Makefile.am > tmp-Makefile.am
	cat DEP >> tmp-Makefile.am
	$(SHELL) $(srcdir)/../move-if-change tmp-Makefile.am $(srcdir)/Makefile.am

.PHONY: dep dep-in dep-am

CLEANFILES = dep.sed DEP DEPA DEP1 DEP2

MAINTAINERCLEANFILES = gprof.info

# Automake 1.9 will only build info files in the objdir if they are
# mentioned in DISTCLEANFILES.  It doesn't have to be unconditional,
# though, so we use a bogus condition.
if GENINSRC_NEVER
DISTCLEANFILES = gprof.info
endif

# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
basic_blocks.o: \
  basic_blocks.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  basic_blocks.h \
  corefile.h \
  gconfig.h \
  gmon_io.h \
  gmon_out.h \
  gprof.h \
  search_list.h \
  source.h \
  sym_ids.h \
  symtab.h
call_graph.o: \
  call_graph.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  call_graph.h \
  cg_arcs.h \
  corefile.h \
  gconfig.h \
  gmon_io.h \
  gmon_out.h \
  gprof.h \
  search_list.h \
  source.h \
  sym_ids.h \
  symtab.h
cg_arcs.o: \
  cg_arcs.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  call_graph.h \
  cg_arcs.h \
  cg_dfn.h \
  cg_print.h \
  gconfig.h \
  gprof.h \
  search_list.h \
  source.h \
  sym_ids.h \
  symtab.h \
  utils.h
cg_dfn.o: \
  cg_dfn.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  cg_arcs.h \
  cg_dfn.h \
  gconfig.h \
  gprof.h \
  search_list.h \
  source.h \
  symtab.h \
  utils.h
cg_print.o: \
  cg_print.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  cg_arcs.h \
  cg_print.h \
  corefile.h \
  gconfig.h \
  gprof.h \
  hist.h \
  search_list.h \
  source.h \
  symtab.h \
  utils.h
corefile.o: \
  corefile.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/safe-ctype.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  corefile.h \
  gconfig.h \
  gprof.h \
  hist.h \
  search_list.h \
  source.h \
  symtab.h
gmon_io.o: \
  gmon_io.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/binary-io.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  basic_blocks.h \
  call_graph.h \
  cg_arcs.h \
  corefile.h \
  gconfig.h \
  gmon.h \
  gmon_io.h \
  gmon_out.h \
  gprof.h \
  hertz.h \
  hist.h \
  search_list.h \
  source.h \
  symtab.h
gprof.o: \
  gprof.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/demangle.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/bfdver.h \
  ../bfd/config.h \
  basic_blocks.h \
  call_graph.h \
  cg_arcs.h \
  cg_print.h \
  corefile.h \
  gconfig.h \
  gmon_io.h \
  gprof.h \
  hertz.h \
  hist.h \
  search_list.h \
  source.h \
  sym_ids.h \
  symtab.h
hertz.o: \
  hertz.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  gconfig.h \
  gprof.h \
  hertz.h
hist.o: \
  hist.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  corefile.h \
  gconfig.h \
  gmon_io.h \
  gmon_out.h \
  gprof.h \
  hist.h \
  search_list.h \
  source.h \
  sym_ids.h \
  symtab.h \
  utils.h
source.o: \
  source.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  gconfig.h \
  gprof.h \
  search_list.h \
  source.h
search_list.o: \
  search_list.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  gconfig.h \
  gprof.h \
  search_list.h
symtab.o: \
  symtab.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  cg_arcs.h \
  corefile.h \
  gconfig.h \
  gprof.h \
  search_list.h \
  source.h \
  symtab.h
sym_ids.o: \
  sym_ids.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/safe-ctype.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  cg_arcs.h \
  corefile.h \
  gconfig.h \
  gprof.h \
  search_list.h \
  source.h \
  sym_ids.h \
  symtab.h
utils.o: \
  utils.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/demangle.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/libiberty.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  cg_arcs.h \
  corefile.h \
  gconfig.h \
  gprof.h \
  search_list.h \
  source.h \
  symtab.h \
  utils.h
i386.o: \
  i386.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  cg_arcs.h \
  corefile.h \
  gconfig.h \
  gprof.h \
  hist.h \
  search_list.h \
  source.h \
  symtab.h
alpha.o: \
  alpha.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  cg_arcs.h \
  corefile.h \
  gconfig.h \
  gprof.h \
  hist.h \
  search_list.h \
  source.h \
  symtab.h
vax.o: \
  vax.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  cg_arcs.h \
  corefile.h \
  gconfig.h \
  gprof.h \
  hist.h \
  search_list.h \
  source.h \
  symtab.h
tahoe.o: \
  tahoe.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  cg_arcs.h \
  corefile.h \
  gconfig.h \
  gprof.h \
  hist.h \
  search_list.h \
  source.h \
  symtab.h
sparc.o: \
  sparc.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  cg_arcs.h \
  corefile.h \
  gconfig.h \
  gprof.h \
  hist.h \
  search_list.h \
  source.h \
  symtab.h
mips.o: \
  mips.c \
  $(BFDDIR)/sysdep.h \
  $(INCDIR)/ansidecl.h \
  $(INCDIR)/filenames.h \
  $(INCDIR)/fopen-same.h \
  $(INCDIR)/symcat.h \
  ../bfd/bfd.h \
  ../bfd/config.h \
  cg_arcs.h \
  corefile.h \
  gconfig.h \
  gprof.h \
  hist.h \
  search_list.h \
  source.h \
  symtab.h
flat_bl.o: \
  flat_bl.c \
  $(INCDIR)/ansidecl.h
bsd_callg_bl.o: \
  bsd_callg_bl.c \
  $(INCDIR)/ansidecl.h
fsf_callg_bl.o: \
  fsf_callg_bl.c \
  $(INCDIR)/ansidecl.h
# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
