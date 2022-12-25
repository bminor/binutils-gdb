## See sim/Makefile.am
##
## Copyright (C) 1996-2023 Free Software Foundation, Inc.
## Contributed by Cygnus Support.
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

%C%_SIM_EXTRA_HW_DEVICES = m32r_cache m32r_uart
AM_MAKEFLAGS += %C%_SIM_EXTRA_HW_DEVICES="$(%C%_SIM_EXTRA_HW_DEVICES)"

## List all generated headers to help Automake dependency tracking.
BUILT_SOURCES += \
	%D%/eng.h \
	%D%/engx.h \
	%D%/eng2.h
%C%_BUILD_OUTPUTS = \
	%D%/mloop.c \
	%D%/stamp-mloop \
	%D%/mloopx.c \
	%D%/stamp-mloop-x \
	%D%/mloop2.c \
	%D%/stamp-mloop-2

## This makes sure build tools are available before building the arch-subdirs.
SIM_ALL_RECURSIVE_DEPS += $(%C%_BUILD_OUTPUTS)
%D%/modules.c: | $(%C%_BUILD_OUTPUTS)

## FIXME: Use of `mono' is wip.
%D%/mloop.c %D%/eng.h: %D%/stamp-mloop ; @true
%D%/stamp-mloop: $(srccom)/genmloop.sh %D%/mloop.in
	$(AM_V_GEN)$(SHELL) $(srccom)/genmloop.sh -shell $(SHELL) \
		-mono -fast -pbb -switch sem-switch.c \
		-cpu m32rbf \
		-infile $(srcdir)/%D%/mloop.in -outfile-prefix %D%/
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/eng.hin %D%/eng.h
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/mloop.cin %D%/mloop.c
	$(AM_V_at)touch $@

## FIXME: Use of `mono' is wip.
%D%/mloopx.c %D%/engx.h: %D%/stamp-mloop ; @true
%D%/stamp-mloop-x: $(srccom)/genmloop.sh %D%/mloop.in
	$(AM_V_GEN)$(SHELL) $(srccom)/genmloop.sh -shell $(SHELL) \
		-mono -no-fast -pbb -parallel-write -switch semx-switch.c \
		-cpu m32rxf \
		-infile $(srcdir)/%D%/mloopx.in -outfile-prefix %D%/ -outfile-suffix x
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/engx.hin %D%/engx.h
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/mloopx.cin %D%/mloopx.c
	$(AM_V_at)touch $@

## FIXME: Use of `mono' is wip.
%D%/mloop2.c %D%/eng2.h: %D%/stamp-mloop ; @true
%D%/stamp-mloop-2: $(srccom)/genmloop.sh %D%/mloop.in
	$(AM_V_GEN)$(SHELL) $(srccom)/genmloop.sh -shell $(SHELL) \
		-mono -no-fast -pbb -parallel-write -switch sem2-switch.c \
		-cpu m32r2f \
		-infile $(srcdir)/%D%/mloop2.in -outfile-prefix %D%/ -outfile-suffix 2
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/eng2.hin %D%/eng2.h
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/mloop2.cin %D%/mloop2.c
	$(AM_V_at)touch $@

MOSTLYCLEANFILES += $(%C%_BUILD_OUTPUTS)

## Target that triggers all cgen targets that works when --disable-cgen-maint.
%D%/cgen: %D%/cgen-arch %D%/cgen-cpu-decode %D%/cgen-cpu-decode-x %D%/cgen-cpu-decode-2

%D%/cgen-arch:
	$(AM_V_GEN)mach=all FLAGS="with-scache with-profile=fn"; $(CGEN_GEN_ARCH)
%D%/arch.h %D%/arch.c %D%/cpuall.h: @CGEN_MAINT@ %D%/cgen-arch

%D%/cgen-cpu-decode:
	$(AM_V_GEN)cpu=m32rbf mach=m32r FLAGS="with-scache with-profile=fn" EXTRAFILES="$(CGEN_CPU_SEM) $(CGEN_CPU_SEMSW)"; $(CGEN_GEN_CPU_DECODE)
%D%/cpu.h %D%/sem.c %D%/sem-switch.c %D%/model.c %D%/decode.c %D%/decode.h: @CGEN_MAINT@ %D%/cgen-cpu-decode

%D%/cgen-cpu-decode-x:
	$(AM_V_GEN)cpu=m32rxf mach=m32rx SUFFIX=x FLAGS="with-scache with-profile=fn" EXTRAFILES="$(CGEN_CPU_SEMSW)"; $(CGEN_GEN_CPU_DECODE)
%D%/cpux.h %D%/semx-switch.c %D%/modelx.c %D%/decodex.c %D%/decodex.h: @CGEN_MAINT@ %D%/cgen-cpu-decode-x

%D%/cgen-cpu-decode-2:
	$(AM_V_GEN)cpu=m32r2f mach=m32r2 SUFFIX=2 FLAGS="with-scache with-profile=fn" EXTRAFILES="$(CGEN_CPU_SEMSW)"; $(CGEN_GEN_CPU_DECODE)
%D%/cpu2.h %D%/sem2-switch.c %D%/model2.c %D%/decode2.c %D%/decode2.h: @CGEN_MAINT@ %D%/cgen-cpu-decode-2
