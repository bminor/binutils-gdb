## See sim/Makefile.am
##
## Copyright (C) 2020-2023 Free Software Foundation, Inc.
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

AM_CPPFLAGS_%C% = -DWITH_TARGET_WORD_BITSIZE=64
AM_CPPFLAGS_%C%_mloop_le.o = -DWANT_ISA_EBPFLE
AM_CPPFLAGS_%C%_mloop_be.o = -DWANT_ISA_EBPFBE
AM_CPPFLAGS_%C%_decode_le.o = -DWANT_ISA_EBPFLE
AM_CPPFLAGS_%C%_decode_be.o = -DWANT_ISA_EBPFBE
AM_CPPFLAGS_%C%_sem_le.o = -DWANT_ISA_EBPFLE
AM_CPPFLAGS_%C%_sem_be.o = -DWANT_ISA_EBPFBE

nodist_%C%_libsim_a_SOURCES = \
	%D%/modules.c
%C%_libsim_a_SOURCES = \
	$(common_libcommon_a_SOURCES)
%C%_libsim_a_LIBADD = \
	$(patsubst %,%D%/%,$(SIM_NEW_COMMON_OBJS)) \
	$(patsubst %,%D%/dv-%.o,$(SIM_HW_DEVICES)) \
	\
	%D%/cgen-run.o \
	%D%/cgen-scache.o \
	%D%/cgen-trace.o \
	%D%/cgen-utils.o \
	\
	%D%/arch.o \
	%D%/cpu.o \
	%D%/decode-le.o \
	%D%/decode-be.o \
	%D%/sem-le.o \
	%D%/sem-be.o \
	%D%/mloop-le.o \
	%D%/mloop-be.o \
	\
	%D%/bpf.o \
	%D%/bpf-helpers.o \
	%D%/sim-if.o \
	%D%/traps.o
$(%C%_libsim_a_OBJECTS) $(%C%_libsim_a_LIBADD): %D%/hw-config.h

noinst_LIBRARIES += %D%/libsim.a

## Override wildcards that trigger common/modules.c to be (incorrectly) used.
%D%/modules.o: %D%/modules.c

%D%/%.o: common/%.c ; $(SIM_COMPILE)
-@am__include@ %D%/$(DEPDIR)/*.Po

%C%_run_SOURCES =
%C%_run_LDADD = \
	%D%/nrun.o \
	%D%/libsim.a \
	$(SIM_COMMON_LIBS)

noinst_PROGRAMS += %D%/run

## List all generated headers to help Automake dependency tracking.
BUILT_SOURCES += \
	%D%/eng-le.h \
	%D%/eng-be.h
%C%_BUILD_OUTPUTS = \
	%D%/mloop-le.c \
	%D%/stamp-mloop-le \
	%D%/mloop-be.c \
	%D%/stamp-mloop-be

## Generating modules.c requires all sources to scan.
%D%/modules.c: | $(%C%_BUILD_OUTPUTS)

%D%/mloop-le.c %D%/eng-le.h: %D%/stamp-mloop-le ; @true
%D%/stamp-mloop-le: $(srccom)/genmloop.sh %D%/mloop.in
	$(AM_V_GEN)$(SHELL) $(srccom)/genmloop.sh -shell $(SHELL) \
		-mono -scache -prefix bpfbf_ebpfle -cpu bpfbf \
		-infile $(srcdir)/%D%/mloop.in \
		-outfile-prefix %D%/ -outfile-suffix -le
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/eng-le.hin %D%/eng-le.h
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/mloop-le.cin %D%/mloop-le.c
	$(AM_V_at)touch $@

%D%/mloop-be.c %D%/eng-be.h: %D%/stamp-mloop-be ; @true
%D%/stamp-mloop-be: $(srccom)/genmloop.sh %D%/mloop.in
	$(AM_V_GEN)$(SHELL) $(srccom)/genmloop.sh -shell $(SHELL) \
		-mono -scache -prefix bpfbf_ebpfbe -cpu bpfbf \
		-infile $(srcdir)/%D%/mloop.in \
		-outfile-prefix %D%/ -outfile-suffix -be
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/eng-be.hin %D%/eng-be.h
	$(AM_V_at)$(SHELL) $(srcroot)/move-if-change %D%/mloop-be.cin %D%/mloop-be.c
	$(AM_V_at)touch $@

MOSTLYCLEANFILES += $(%C%_BUILD_OUTPUTS)

## Target that triggers all cgen targets that works when --disable-cgen-maint.
%D%/cgen: %D%/cgen-arch %D%/cgen-cpu %D%/cgen-defs-le %D%/cgen-defs-be %D%/cgen-decode-le %D%/cgen-decode-be

%D%/cgen-arch:
	$(AM_V_GEN)mach=bpf cpu=bpfbf FLAGS="with-scache"; $(CGEN_GEN_ARCH)
%D%/arch.h %D%/arch.c %D%/cpuall.h: @CGEN_MAINT@ %D%/cgen-arch

%D%/cgen-cpu:
	$(AM_V_GEN)isa=ebpfle,ebpfbe cpu=bpfbf mach=bpf FLAGS="with-multiple-isa with-scache"; $(CGEN_GEN_CPU)
	$(AM_V_at)rm -f $(srcdir)/%D%/model.c
%D%/cpu.h %D%/cpu.c %D%/model.c: @CGEN_MAINT@ %D%/cgen-cpu

## We need to generate a group of files per ISA.
## For eBPF little-endian:
##    defs-le.h
##    sem-le.c, decode-le.c, decode-le.h
##    $(objdir)/mloop-le.c $(objdir)/eng-le.h
## For eBPF big-endian:
##    defs-be.h
##    sem-be.c, decode-be.c, decode-be.h
##    $(objdir)/mloop-be.c $(objdir)/eng-le.h
##
## The rules below take care of that.

%D%/cgen-defs-le:
	$(AM_V_GEN)isa=ebpfle cpu=bpfbf mach=bpf FLAGS="with-scache" SUFFIX="-le"; $(CGEN_GEN_DEFS)
%D%/defs-le.h: @CGEN_MAINT@ %D%/cgen-defs-le

%D%/cgen-defs-be:
	$(AM_V_GEN)isa=ebpfbe cpu=bpfbf mach=bpf FLAGS="with-scache" SUFFIX="-be"; $(CGEN_GEN_DEFS)
%D%/defs-be.h: @CGEN_MAINT@ %D%/cgen-defs-be

%D%/cgen-decode-le:
	$(AM_V_GEN)isa=ebpfle cpu=bpfbf mach=bpf FLAGS="with-scache" SUFFIX="-le" EXTRAFILES="$(CGEN_CPU_SEM)"; $(CGEN_GEN_DECODE)
%D%/sem-le.c %D%/decode-le.c %D%/decode-le.h: @CGEN_MAINT@ %D%/cgen-decode-vle

%D%/cgen-decode-be:
	$(AM_V_GEN)isa=ebpfbe cpu=bpfbf mach=bpf FLAGS="with-scache" SUFFIX="-be" EXTRAFILES="$(CGEN_CPU_SEM)"; $(CGEN_GEN_DECODE)
%D%/sem-be.c %D%/decode-be.c %D%/decode-be.h: @CGEN_MAINT@ %D%/cgen-decode-be
