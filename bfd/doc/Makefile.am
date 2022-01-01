## Process this file with automake to generate Makefile.in
#
#   Copyright (C) 2012-2022 Free Software Foundation, Inc.
#
# This file is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING3.  If not see
# <http://www.gnu.org/licenses/>.
#

AUTOMAKE_OPTIONS = info-in-builddir no-texinfo.tex foreign

DOCFILES = aoutx.texi  archive.texi archures.texi \
	bfdt.texi  cache.texi coffcode.texi \
	core.texi elf.texi elfcode.texi  format.texi \
	libbfd.texi bfdwin.texi bfdio.texi \
	opncls.texi  reloc.texi  section.texi  \
	syms.texi  targets.texi init.texi hash.texi linker.texi \
	mmo.texi \
	bfdver.texi

# SRCDOC, SRCPROT, SRCIPROT only used to sidestep Sun Make bug in interaction 
# between VPATH and suffix rules.  If you use GNU Make, perhaps other Makes,
# you don't need these three:
SRCDOC = $(srcdir)/../aoutx.h  $(srcdir)/../archive.c \
	$(srcdir)/../archures.c $(srcdir)/../bfd.c \
	$(srcdir)/../bfdio.c $(srcdir)/../bfdwin.c \
	$(srcdir)/../cache.c $(srcdir)/../coffcode.h \
	$(srcdir)/../corefile.c $(srcdir)/../elf.c \
	$(srcdir)/../elfcode.h  $(srcdir)/../format.c \
	$(srcdir)/../libbfd.c $(srcdir)/../opncls.c \
	$(srcdir)/../reloc.c  $(srcdir)/../section.c \
	$(srcdir)/../syms.c  $(srcdir)/../targets.c \
	$(srcdir)/../hash.c $(srcdir)/../linker.c \
	$(srcdir)/../mmo.c

SRCPROT = $(srcdir)/../archive.c $(srcdir)/../archures.c \
	$(srcdir)/../bfd.c $(srcdir)/../coffcode.h $(srcdir)/../corefile.c \
	$(srcdir)/../format.c $(srcdir)/../libbfd.c \
	$(srcdir)/../bfdio.c $(srcdir)/../bfdwin.c \
	$(srcdir)/../opncls.c $(srcdir)/../reloc.c \
	$(srcdir)/../section.c $(srcdir)/../syms.c \
	$(srcdir)/../targets.c $(srcdir)/../init.c

SRCIPROT = $(srcdir)/../cache.c $(srcdir)/../libbfd.c \
	$(srcdir)/../bfdio.c $(srcdir)/../bfdwin.c \
	$(srcdir)/../reloc.c $(srcdir)/../cpu-h8300.c \
	$(srcdir)/../cpu-i960.c $(srcdir)/../archures.c \
	$(srcdir)/../init.c

TEXIDIR = $(srcdir)/../../texinfo/fsf

info_TEXINFOS = bfd.texi
bfd_TEXINFOS = $(DOCFILES) bfdsumm.texi

AM_MAKEINFOFLAGS = --no-split

MKDOC = chew$(EXEEXT_FOR_BUILD)

AM_CPPFLAGS = -I.. -I$(srcdir)/.. -I$(srcdir)/../../include \
	-I$(srcdir)/../../intl -I../../intl

$(MKDOC): chew.stamp ; @true
chew.stamp: $(srcdir)/chew.c
	$(CC_FOR_BUILD) -o chw$$$$$(EXEEXT_FOR_BUILD) $(CFLAGS_FOR_BUILD) \
	  $(LDFLAGS_FOR_BUILD) $(H_CFLAGS) $(AM_CPPFLAGS) $(srcdir)/chew.c; \
	$(SHELL) $(srcdir)/../../move-if-change \
	  chw$$$$$(EXEEXT_FOR_BUILD) $(MKDOC); \
	touch $@

# We can't replace these rules with an implicit rule, because
# makes without VPATH support couldn't find the .h files in `..'.

# We do not depend on chew directly so that we can distribute the info
# files, and permit people to rebuild them, without requiring the makeinfo
# program.  If somebody tries to rebuild info, but none of the .texi files
# have changed, then nothing will be rebuilt.

REGEN_TEXI = \
	( \
	set -e; \
	./$(MKDOC) -f $(srcdir)/doc.str < $< > $@.tmp; \
	texi=$@; \
	texi=$${texi%.stamp}.texi; \
	test -e $$texi || test ! -f $(srcdir)/$$texi || $(LN_S) $(srcdir)/$$texi .; \
	$(SHELL) $(srcdir)/../../move-if-change $@.tmp $$texi; \
	touch $@; \
	)

aoutx.texi: aoutx.stamp ; @true
aoutx.stamp: $(srcdir)/../aoutx.h $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

archive.texi: archive.stamp ; @true
archive.stamp: $(srcdir)/../archive.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

archures.texi: archures.stamp ; @true
archures.stamp: $(srcdir)/../archures.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

# We use bfdt.texi, rather than bfd.texi, to avoid conflicting with
# bfd.texi on an 8.3 filesystem.
bfdt.texi: bfdt.stamp ; @true
bfdt.stamp: $(srcdir)/../bfd.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

cache.texi: cache.stamp ; @true
cache.stamp: $(srcdir)/../cache.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

coffcode.texi: coffcode.stamp ; @true
coffcode.stamp: $(srcdir)/../coffcode.h $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

core.texi: core.stamp ; @true
core.stamp: $(srcdir)/../corefile.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

elf.texi: elf.stamp ; @true
elf.stamp: $(srcdir)/../elf.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

elfcode.texi: elfcode.stamp ; @true
elfcode.stamp: $(srcdir)/../elfcode.h $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

mmo.texi: mmo.stamp ; @true
mmo.stamp: $(srcdir)/../mmo.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

format.texi: format.stamp ; @true
format.stamp: $(srcdir)/../format.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

libbfd.texi: libbfd.stamp ; @true
libbfd.stamp: $(srcdir)/../libbfd.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

bfdio.texi: bfdio.stamp ; @true
bfdio.stamp: $(srcdir)/../bfdio.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

bfdwin.texi: bfdwin.stamp ; @true
bfdwin.stamp: $(srcdir)/../bfdwin.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

opncls.texi: opncls.stamp ; @true
opncls.stamp: $(srcdir)/../opncls.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

reloc.texi: reloc.stamp ; @true
reloc.stamp: $(srcdir)/../reloc.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

section.texi: section.stamp ; @true
section.stamp: $(srcdir)/../section.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

syms.texi: syms.stamp ; @true
syms.stamp: $(srcdir)/../syms.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

targets.texi: targets.stamp ; @true
targets.stamp: $(srcdir)/../targets.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

init.texi: init.stamp ; @true
init.stamp: $(srcdir)/../init.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

hash.texi: hash.stamp ; @true
hash.stamp: $(srcdir)/../hash.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

linker.texi: linker.stamp ; @true
linker.stamp: $(srcdir)/../linker.c $(srcdir)/doc.str $(MKDOC)
	$(AM_V_GEN)$(REGEN_TEXI)

bfdver.texi: $(srcdir)/Makefile.in
	@echo "creating $@"; \
	echo "@set VERSION $(VERSION)" > bfdver.texi; \
	if test -n "$(PKGVERSION)"; then \
	  echo "@set VERSION_PACKAGE $(PKGVERSION)" >> bfdver.texi; \
	fi; \
	echo "@set UPDATED `date '+%B %Y'`" >> bfdver.texi; \
	if test -n "$(REPORT_BUGS_TEXI)"; then \
	  echo "@set BUGURL $(REPORT_BUGS_TEXI)" >> bfdver.texi; \
	fi

noinst_TEXINFOS = bfdint.texi

MOSTLYCLEANFILES = $(MKDOC) *.o *.stamp

DISTCLEANFILES = bfd.?? bfd.??? bfd.h libbfd.h libcoff.h texput.log

MAINTAINERCLEANFILES = $(DOCFILES)

# We want install to imply install-info as per GNU standards, despite the
# cygnus option.
install: install-info

html-local: bfd/index.html
bfd/index.html: bfd.texi $(bfd_TEXINFOS)
	$(AM_V_at)$(MAKEINFOHTML) $(AM_MAKEINFOHTMLFLAGS) $(MAKEINFOFLAGS) \
	  --split=node -I$(srcdir) $(srcdir)/bfd.texi

MAINTAINERCLEANFILES += bfd.info
