## Process this file with automake to generate Makefile.in
##
#   Copyright (C) 1993-2022 Free Software Foundation, Inc.
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

srccom = $(srcdir)/common
srcroot = $(srcdir)/..

SUBDIRS = @subdirs@ $(SIM_SUBDIRS)

SIM_PRIMARY_TARGET = @SIM_PRIMARY_TARGET@
AM_MAKEFLAGS = SIM_PRIMARY_TARGET=$(SIM_PRIMARY_TARGET)

## We don't set some of these vars here, but we need to define them so they may
## be used consistently in local.mk files we include below.
pkginclude_HEADERS =
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

## Deps to add to the all-recursive target.  These are built before descending
## into any subdirs.
SIM_ALL_RECURSIVE_DEPS =

# Generate target constants for newlib/libgloss from its source tree.
# This file is shipped with distributions so we build in the source dir.
# Use `make nltvals' to rebuild.
.PHONY: nltvals
nltvals:
	$(srccom)/gennltvals.py --cpp "$(CPP)"

if ENABLE_SIM
pkginclude_HEADERS += \
	$(srcroot)/include/sim/callback.h \
	$(srcroot)/include/sim/sim.h
endif

include common/local.mk
if SIM_ENABLE_IGEN
include igen/local.mk
endif
include testsuite/local.mk

if SIM_ENABLE_ARCH_arm
include arm/local.mk
endif
if SIM_ENABLE_ARCH_bpf
include bpf/local.mk
endif
if SIM_ENABLE_ARCH_cr16
include cr16/local.mk
endif
if SIM_ENABLE_ARCH_cris
include cris/local.mk
endif
if SIM_ENABLE_ARCH_d10v
include d10v/local.mk
endif
if SIM_ENABLE_ARCH_erc32
include erc32/local.mk
endif
if SIM_ENABLE_ARCH_frv
include frv/local.mk
endif
if SIM_ENABLE_ARCH_iq2000
include iq2000/local.mk
endif
if SIM_ENABLE_ARCH_lm32
include lm32/local.mk
endif
if SIM_ENABLE_ARCH_m32c
include m32c/local.mk
endif
if SIM_ENABLE_ARCH_m32r
include m32r/local.mk
endif
if SIM_ENABLE_ARCH_m68hc11
include m68hc11/local.mk
endif
if SIM_ENABLE_ARCH_mn10300
include mn10300/local.mk
endif
if SIM_ENABLE_ARCH_moxie
include moxie/local.mk
endif
if SIM_ENABLE_ARCH_or1k
include or1k/local.mk
endif
if SIM_ENABLE_ARCH_ppc
include ppc/local.mk
endif
if SIM_ENABLE_ARCH_rx
include rx/local.mk
endif
if SIM_ENABLE_ARCH_sh
include sh/local.mk
endif
if SIM_ENABLE_ARCH_v850
include v850/local.mk
endif

all-recursive: $(SIM_ALL_RECURSIVE_DEPS)
