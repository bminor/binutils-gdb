## See sim/Makefile.am
##
## Copyright (C) 1995-2022 Free Software Foundation, Inc.
## Written by Cygnus Support.
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
	%D%/nrun.o \
	%D%/libsim.a \
	$(SIM_COMMON_LIBS)

noinst_PROGRAMS += %D%/run

%C%_SIM_EXTRA_HW_DEVICES = tx3904cpu tx3904irc tx3904tmr tx3904sio
AM_MAKEFLAGS += %C%_SIM_EXTRA_HW_DEVICES="$(%C%_SIM_EXTRA_HW_DEVICES)"

%C%_BUILT_SRC_FROM_IGEN_ITABLE = \
	%D%/itable.h \
	%D%/itable.c
%C%_BUILD_OUTPUTS = \
	$(%C%_BUILT_SRC_FROM_IGEN_ITABLE) \
	%D%/stamp-igen-itable

## This makes sure build tools are available before building the arch-subdirs.
SIM_ALL_RECURSIVE_DEPS += $(%C%_BUILD_OUTPUTS)

$(%C%_BUILT_SRC_FROM_IGEN_ITABLE): %D%/stamp-igen-itable

%C%_IGEN_TRACE = # -G omit-line-numbers # -G trace-rule-selection -G trace-rule-rejection -G trace-entries # -G trace-all
%C%_IGEN_INSN = $(srcdir)/%D%/mips.igen
%C%_IGEN_INSN_INC = \
	%D%/dsp.igen \
	%D%/dsp2.igen \
	%D%/m16.igen \
	%D%/m16e.igen \
	%D%/mdmx.igen \
	%D%/micromipsdsp.igen \
	%D%/micromips.igen \
	%D%/mips3264r2.igen \
	%D%/mips3264r6.igen \
	%D%/mips3d.igen \
	%D%/sb1.igen \
	%D%/tx.igen \
	%D%/vr.igen

## NB:	Since these can be built by a number of generators, care
##	must be taken to ensure that they are only dependant on
##	one of those generators.
%D%/stamp-igen-itable: $(%C%_IGEN_INSN) $(%C%_IGEN_INSN_INC) $(IGEN)
	$(AM_V_GEN)$(IGEN_RUN) \
		$(%C%_IGEN_TRACE) \
		-I $(srcdir)/%D% \
		-Werror \
		-Wnodiscard \
		-Wnowidth \
		-Wnounimplemented \
		$(SIM_MIPS_IGEN_ITABLE_FLAGS) \
		-G gen-direct-access \
		-G gen-zero-r0 \
		-i $(%C%_IGEN_INSN) \
		-n itable.h    -ht %D%/itable.h \
		-n itable.c    -t  %D%/itable.c
	$(AM_V_at)touch $@

MOSTLYCLEANFILES += $(%C%_BUILD_OUTPUTS)
## These are created by mips/acinclude.m4 during configure time.
DISTCLEANFILES += %D%/multi-include.h %D%/multi-run.c
