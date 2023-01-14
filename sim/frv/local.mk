## See sim/Makefile.am
##
## Copyright (C) 1998-2023 Free Software Foundation, Inc.
## Contributed by Red Hat.
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

AM_CPPFLAGS_%C% = $(SIM_FRV_TRAPDUMP_FLAGS)

## Some modules don't build cleanly yet.
AM_CFLAGS_%C%_memory.o = -Wno-error
AM_CFLAGS_%C%_sem.o = -Wno-error

%C%_libsim_a_SOURCES =
%C%_libsim_a_LIBADD = \
	$(common_libcommon_a_OBJECTS) \
	$(patsubst %,%D%/%,$(SIM_NEW_COMMON_OBJS)) \
	$(patsubst %,%D%/dv-%.o,$(SIM_HW_DEVICES)) \
	%D%/modules.o \
	\
	%D%/cgen-accfp.o \
	%D%/cgen-fpu.o \
	%D%/cgen-run.o \
	%D%/cgen-scache.o \
	%D%/cgen-trace.o \
	%D%/cgen-utils.o \
	\
	%D%/arch.o \
	%D%/cgen-par.o \
	%D%/cpu.o \
	%D%/decode.o \
	%D%/frv.o \
	%D%/mloop.o \
	%D%/model.o \
	%D%/sem.o \
	\
	%D%/cache.o \
	%D%/interrupts.o \
	%D%/memory.o \
	%D%/options.o \
	%D%/pipeline.o \
	%D%/profile.o \
	%D%/profile-fr400.o \
	%D%/profile-fr450.o \
	%D%/profile-fr500.o \
	%D%/profile-fr550.o \
	%D%/registers.o \
	%D%/reset.o \
	%D%/sim-if.o \
	%D%/traps.o
$(%C%_libsim_a_OBJECTS) $(%C%_libsim_a_LIBADD): %D%/hw-config.h

noinst_LIBRARIES += %D%/libsim.a

%D%/%.o: common/%.c ; $(SIM_COMPILE)
-@am__include@ %D%/$(DEPDIR)/*.Po

%C%_run_SOURCES =
%C%_run_LDADD = \
	%D%/nrun.o \
	%D%/libsim.a \
	$(SIM_COMMON_LIBS)

noinst_PROGRAMS += %D%/run

%C%docdir = $(docdir)/%C%
%C%doc_DATA = %D%/README

## List all generated headers to help Automake dependency tracking.
BUILT_SOURCES += %D%/eng.h
%C%_BUILD_OUTPUTS = \
	%D%/mloop.c \
	%D%/stamp-mloop

## This makes sure build tools are available before building the arch-subdirs.
SIM_ALL_RECURSIVE_DEPS += $(%C%_BUILD_OUTPUTS)
%D%/modules.c: | $(%C%_BUILD_OUTPUTS)

## FIXME: Use of `mono' is wip.
%D%/mloop.c %D%/eng.h: %D%/stamp-mloop ; @true
%D%/stamp-mloop: $(srccom)/genmloop.sh %D%/mloop.in
	$(AM_V_GEN)$(SHELL) $(srccom)/genmloop.sh -shell $(SHELL) \
		-mono -scache -parallel-generic-write -parallel-only \
		-cpu frvbf \
		-infile $(srcdir)/%D%/mloop.in -outfile-prefix %D%/
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/eng.hin %D%/eng.h
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/mloop.cin %D%/mloop.c
	$(AM_V_at)touch $@

MOSTLYCLEANFILES += $(%C%_BUILD_OUTPUTS)

## Target that triggers all cgen targets that works when --disable-cgen-maint.
%D%/cgen: %D%/cgen-arch %D%/cgen-cpu-decode

%D%/cgen-arch:
	$(AM_V_GEN)mach=all FLAGS="with-scache"; $(CGEN_GEN_ARCH)
%D%/arch.h %D%/arch.c %D%/cpuall.h: @CGEN_MAINT@ %D%/cgen-arch

%D%/cgen-cpu-decode:
	$(AM_V_GEN)cpu=frvbf mach=frv,fr550,fr500,fr450,fr400,tomcat,simple FLAGS="with-scache with-profile=fn with-generic-write with-parallel-only" EXTRAFILES="$(CGEN_CPU_SEM)"; $(CGEN_GEN_CPU_DECODE)
%D%/cpu.h %D%/sem.c %D%/model.c %D%/decode.c %D%/decode.h: @CGEN_MAINT@ %D%/cgen-cpu-decode
