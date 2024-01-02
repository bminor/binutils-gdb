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

SIM_ALL_RECURSIVE_DEPS += %D%/defines.h
%D%/defines.h: %D%/stamp-defines ; @true
%D%/stamp-defines: config.h Makefile
	$(AM_V_GEN)sed -n -e '/^#define HAVE_/s/ 1$$/",/' -e '/^#define HAVE_/s//"HAVE_/p' < config.h > %D%/defines.hin
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/defines.hin %D%/defines.h
	$(AM_V_at)touch $@

MOSTLYCLEANFILES += %D%/defines.h %D%/stamp-defines

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
	%D%/dumpf.c \
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
%C%_libigen_a_LIBADD = \
	igen/filter_host.o \
	igen/misc.o

%C%_igen_SOURCES = %D%/igen.c
%C%_igen_LDADD = %D%/libigen.a

# igen leaks memory, and therefore makes AddressSanitizer unhappy.  Disable
# leak detection while running it.
PPC_IGEN = %D%/igen$(EXEEXT)
PPC_IGEN_RUN = ASAN_OPTIONS=detect_leaks=0 $(PPC_IGEN) $(%C%_IGEN_FLAGS)

%C%_IGEN_FLAGS = \
	@sim_ppc_decode_mechanism@ \
	@sim_ppc_dup@ \
	@sim_ppc_jump@ \
	@sim_ppc_filter@ \
	@sim_ppc_icache@ \
	@sim_ppc_igen_smp@ \
	@sim_ppc_line_nr@

## List all generated headers to help Automake dependency tracking.
BUILT_SOURCES += \
	%D%/icache.h \
	%D%/idecode.h \
	%D%/semantics.h \
	%D%/model.h \
	%D%/support.h \
	%D%/itable.h
%C%_BUILT_SRC_FROM_IGEN = \
	%D%/icache.h \
	%D%/icache.c \
	%D%/idecode.h \
	%D%/idecode.c \
	%D%/semantics.h \
	%D%/semantics.c \
	%D%/model.h \
	%D%/model.c \
	%D%/support.h \
	%D%/support.c \
	%D%/itable.h \
	%D%/itable.c
%C%_BUILD_OUTPUTS = \
	$(%C%_BUILT_SRC_FROM_IGEN) \
	%D%/stamp-igen

SIM_ALL_RECURSIVE_DEPS += %D%/stamp-igen
$(%C%_BUILT_SRC_FROM_IGEN): %D%/stamp-igen

%C%_IGEN_OPCODE_RULES = %D%/@sim_ppc_opcode@
%D%/stamp-igen: %D%/powerpc.igen %D%/altivec.igen %D%/e500.igen $(%C%_IGEN_OPCODE_RULES) $(PPC_IGEN)
	$(AM_V_GEN)$(PPC_IGEN_RUN) \
		-o $(srcdir)/$(%C%_IGEN_OPCODE_RULES) \
		-I $(srcdir)/%D% -i $(srcdir)/%D%/powerpc.igen \
		-n icache.h    -hc %D%/icache.h \
		-n icache.c    -c  %D%/icache.c \
		-n semantics.h -hs %D%/semantics.h \
		-n semantics.c -s  %D%/semantics.c \
		-n idecode.h   -hd %D%/idecode.h \
		-n idecode.c   -d  %D%/idecode.c \
		-n itable.h    -ht %D%/itable.h \
		-n itable.c    -t  %D%/itable.c \
		-n model.h     -hm %D%/model.h \
		-n model.c     -m  %D%/model.c \
		-n support.h   -hf %D%/support.h \
		-n support.c   -f  %D%/support.c
	$(AM_V_at)touch $@

MOSTLYCLEANFILES += $(%C%_BUILD_OUTPUTS)

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

IGEN_OPCODE_RULES = @sim_ppc_opcode@

## Real hardware.
## NB: The first for loop is to remove duplicates.
%C%_HW_SRC = $(sim_ppc_hw_src:%=%D%/%)
%D%/hw.c %D%/hw.h: %D%/stamp-hw ; @true
%D%/stamp-hw: Makefile $(%C%_HW_SRC) $(srcroot)/move-if-change
	$(AM_V_GEN)\
	f=""; \
	for i in $(%C%_HW_SRC) ; do \
	  case " $$f " in \
	    *" $$i "*) ;; \
	    *) f="$$f $$i" ;; \
	  esac ; \
	done ; \
	for hw in $$f ; do echo $$hw ; done \
	| sed -e 's/^.*\(hw_.*\)\.c/\1/' \
		-e 's/^/extern const device_descriptor /' \
		-e 's/$$/_device_descriptor\[\];/' \
		> %D%/hw.hin; \
	f=""; \
	for i in $(%C%_HW_SRC) ; do \
	  case " $$f " in \
	    *" $$i "*) ;; \
	    *) f="$$f $$i" ;; \
	  esac ; \
	done ; \
	for hw in $$f ; do echo $$hw ; done \
	| sed -e 's/^.*\(hw_.*\)\.c/\1/' \
		-e 's/^/    /' \
		-e 's/$$/_device_descriptor,/' \
		> %D%/hw.cin
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/hw.hin %D%/hw.h
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/hw.cin %D%/hw.c
	$(AM_V_at)touch $@

BUILT_SOURCES += \
	%D%/hw.h
%C%_BUILD_OUTPUTS += \
	%D%/hw.c \
	%D%/hw.h \
	%D%/stamp-hw
SIM_ALL_RECURSIVE_DEPS += %D%/stamp-hw
%D%/hw.c %D%/hw.h: %D%/stamp-igen

%C%docdir = $(docdir)/%C%
%C%doc_DATA = %D%/BUGS %D%/INSTALL %D%/README %D%/RUN
