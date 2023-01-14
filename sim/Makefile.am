## Process this file with automake to generate Makefile.in
#
#   Copyright (C) 1993-2021 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
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
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

AUTOMAKE_OPTIONS = dejagnu foreign no-dist subdir-objects
ACLOCAL_AMFLAGS = -Im4 -I.. -I../config

srcroot = $(srcdir)/..

SUBDIRS = @subdirs@ $(SIM_SUBDIRS)

SIM_PRIMARY_TARGET = @SIM_PRIMARY_TARGET@
AM_MAKEFLAGS = SIM_PRIMARY_TARGET=$(SIM_PRIMARY_TARGET)

# We don't set some of these vars here, but we need to define them so they may
# be used consistently in local.mk files we include below.
check_PROGRAMS =
noinst_LIBRARIES =
EXTRA_PROGRAMS =

CLEANFILES =
DISTCLEANFILES =
MOSTLYCLEANFILES = core

AM_CFLAGS = $(WERROR_CFLAGS) $(WARN_CFLAGS)
AM_CPPFLAGS = \
	-I$(srcroot)/include \
	$(SIM_INLINE)

COMPILE_FOR_BUILD = $(CC_FOR_BUILD) $(AM_CPPFLAGS) $(CFLAGS_FOR_BUILD)
LINK_FOR_BUILD = $(CC_FOR_BUILD) $(CFLAGS_FOR_BUILD) $(LDFLAGS_FOR_BUILD) -o $@

# Deps to add to the all-recursive target.  These are built before descending
# into any subdirs.
SIM_ALL_RECURSIVE_DEPS =

# Generate nltvals.def for newlib/libgloss using devo and build tree.
# This file is shipped with distributions so we build in the source dir.
# Use `make nltvals' to rebuild.
# Note: If gdb releases begin to contain target header files (not a good idea,
# but if they did ...), nltvals.def coud be generated at build time.
# An alternative is to slurp in the tables at runtime.
.PHONY: nltvals
nltvals:
	$(abs_srcdir)/common/gennltvals.py --cpp "$(CPP)" --output nltvals.def --srcroot $(srcroot)
	$(SHELL) $(srcroot)/move-if-change nltvals.def $(abs_srcdir)/common/nltvals.def

pkginclude_HEADERS = \
	$(srcroot)/include/sim/callback.h \
	$(srcroot)/include/sim/sim.h

include common/local.mk
if SIM_ENABLE_IGEN
include igen/local.mk
endif
include testsuite/local.mk

all-recursive: $(SIM_ALL_RECURSIVE_DEPS)
