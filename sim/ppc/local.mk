## See sim/Makefile.am
##
## Copyright (C) 1994-2023 Free Software Foundation, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

%C%_run_SOURCES =
%C%_run_LDADD = \
	%D%/main.o \
	%D%/libsim.a \
	$(SIM_COMMON_LIBS)

## This makes sure common parts are available before building the arch-subdirs
## which will refer to these.
SIM_ALL_RECURSIVE_DEPS += common/libcommon.a
%D%/libsim.a: common/libcommon.a
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

## Helper targets for running make from the top-level due to run's sis.o.
%D%/main.o: %D%/%.o: %D%/%.c | %D%/libsim.a $(SIM_ALL_RECURSIVE_DEPS)
	$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

noinst_PROGRAMS += %D%/run

%D%/spreg.c: @MAINT@ %D%/ppc-spr-table %D%/spreg-gen.py %D%/$(am__dirstamp)
	$(AM_V_GEN)$(srcdir)/%D%/spreg-gen.py --source $@.tmp
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change $@.tmp $(srcdir)/%D%/spreg.c
	$(AM_V_at)touch $(srcdir)/%D%/spreg.c

%D%/spreg.h: @MAINT@ %D%/ppc-spr-table %D%/spreg-gen.py %D%/$(am__dirstamp)
	$(AM_V_GEN)$(srcdir)/%D%/spreg-gen.py --header $@.tmp
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change $@.tmp $(srcdir)/%D%/spreg.h
	$(AM_V_at)touch $(srcdir)/%D%/spreg.h

EXTRA_LIBRARIES += %D%/libigen.a
%C%_libigen_a_SOURCES = \
	%D%/table.c \
	%D%/lf.c \
	%D%/misc.c \
	%D%/ld-decode.c \
	%D%/ld-cache.c \
	%D%/filter.c \
	%D%/ld-insn.c \
	%D%/gen-model.c \
	%D%/gen-itable.c \
	%D%/gen-icache.c \
	%D%/gen-semantics.c \
	%D%/gen-idecode.c \
	%D%/gen-support.c
%C%_libigen_a_LIBADD = igen/filter_host.o

%C%_igen_SOURCES = %D%/igen.c
%C%_igen_LDADD = %D%/libigen.a

PPC_IGEN = %D%/igen$(EXEEXT)

SIM_ALL_RECURSIVE_DEPS += $(PPC_IGEN)

## These rules are copied from automake, but tweaked to use FOR_BUILD variables.
%D%/libigen.a: $(%C%_libigen_a_OBJECTS) $(%C%_libigen_a_DEPENDENCIES) $(EXTRA_%C%_libigen_a_DEPENDENCIES) %D%/$(am__dirstamp)
	$(AM_V_at)-rm -f $@
	$(AM_V_AR)$(AR_FOR_BUILD) $(ARFLAGS) $@ $(%C%_libigen_a_OBJECTS) $(%C%_libigen_a_LIBADD)
	$(AM_V_at)$(RANLIB_FOR_BUILD) $@

## igen is a build-time only tool.  Override the default rules for it.
%D%/igen$(EXEEXT): $(%C%_igen_OBJECTS) $(%C%_igen_DEPENDENCIES) %D%/$(am__dirstamp)
	$(AM_V_CCLD)$(LINK_FOR_BUILD) $(%C%_igen_OBJECTS) $(%C%_igen_LDADD)

$(%C%_libigen_a_OBJECTS) $(%C%_igen_OBJECTS): %D%/%.o: %D%/%.c
	$(AM_V_CC)$(COMPILE_FOR_BUILD) -I$(srcdir)/igen -I$(srcdir)/%D% -c $< -o $@

## Build some of the files in standalone mode for developers of igen itself.
%D%/%-main.o: %D%/%.c
	$(AM_V_CC)$(COMPILE_FOR_BUILD) -DMAIN -c $< -o $@

%C%_filter_SOURCES =
%C%_filter_LDADD = %D%/filter-main.o %D%/libigen.a

%C%_ld_cache_SOURCES =
%C%_ld_cache_LDADD = %D%/ld-cache-main.o %D%/libigen.a

%C%_ld_decode_SOURCES =
%C%_ld_decode_LDADD = %D%/ld-decode-main.o %D%/libigen.a

%C%_ld_insn_SOURCES =
%C%_ld_insn_LDADD = %D%/ld-insn-main.o %D%/libigen.a

%C%_IGEN_TOOLS = \
	$(PPC_IGEN) \
	%D%/filter \
	%D%/ld-cache \
	%D%/ld-decode \
	%D%/ld-insn
EXTRA_PROGRAMS += $(%C%_IGEN_TOOLS)
MOSTLYCLEANFILES += $(%C%_IGEN_TOOLS) %D%/libigen.a

%C%docdir = $(docdir)/%C%
%C%doc_DATA = %D%/BUGS %D%/INSTALL %D%/README %D%/RUN
