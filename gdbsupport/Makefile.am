## Process this file with automake to generate Makefile.in
#
#   Copyright (C) 2020 Free Software Foundation, Inc.
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

AUTOMAKE_OPTIONS = no-dist foreign
ACLOCAL_AMFLAGS = -I . -I ../config

AM_CPPFLAGS = -I$(srcdir)/../include -I$(srcdir)/../gdb \
    -I../gnulib/import -I$(srcdir)/../gnulib/import \
    -I.. -I$(srcdir)/.. $(INCINTL) -I../bfd -I$(srcdir)/../bfd

override CXX += $(CXX_DIALECT)

override CC := $(CXX)
override CFLAGS := $(CXXFLAGS)

noinst_LIBRARIES = libgdbsupport.a

if SELFTEST
selftest = selftest.c
endif

libgdbsupport_a_SOURCES = \
    agent.c \
    btrace-common.c \
    buffer.c \
    cleanups.c \
    common-debug.c \
    common-exceptions.c	\
    common-inferior.c \
    common-regcache.c \
    common-utils.c \
    environ.c \
    errors.c \
    fileio.c \
    filestuff.c	\
    format.c \
    gdb-dlfcn.c	\
    gdb_tilde_expand.c \
    gdb_wait.c \
    gdb_vecs.c \
    job-control.c \
    netstuff.c \
    new-op.c \
    pathstuff.c	\
    print-utils.c \
    ptid.c \
    rsp-low.c \
    run-time-clock.c \
    safe-strerror.c \
    scoped_mmap.c \
    signals.c \
    signals-state-save-restore.c \
    tdesc.c \
    thread-pool.c \
    xml-utils.c	\
    $(selftest)

# Double-check that no defines are missing from our configury.
check-defines:
	cd $(srcdir) && emacs --script check-defines.el
