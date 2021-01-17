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
ACLOCAL_AMFLAGS = -I.. -I../config

srcroot = $(srcdir)/..

SUBDIRS = @subdirs@

MOSTLYCLEANFILES = core

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

include testsuite/local.mk
