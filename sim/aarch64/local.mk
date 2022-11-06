## See sim/Makefile.am
##
## Copyright (C) 2015-2022 Free Software Foundation, Inc.
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

AM_CPPFLAGS_%D% = -DSIM_TOPDIR_BUILD

%C%_libsim_a_SOURCES = \
	%D%/cpustate.c \
	%D%/interp.c \
	%D%/memory.c \
	%D%/modules.c \
	%D%/simulator.c
%C%_libsim_a_LIBADD = \
	$(common_libcommon_a_OBJECTS) \
	$(patsubst %,%D%/%,$(SIM_NEW_COMMON_OBJS)) \
	$(patsubst %,%D%/dv-%.o,$(SIM_HW_DEVICES)) \
	%D%/sim-resume.o

noinst_LIBRARIES += %D%/libsim.a

%C%_run_SOURCES =
%C%_run_LDADD = \
	%D%/nrun.o \
	%D%/libsim.a \
	$(SIM_COMMON_LIBS)

noinst_PROGRAMS += %D%/run

SIM_ALL_RECURSIVE_DEPS += %D%/modules.c

$(%C%_libsim_a_OBJECTS) $(%C%_run_OBJECTS) $(%C%_libsim_a_LIBADD): | $(SIM_ALL_RECURSIVE_DEPS)

%D%/%.o: common/%.c | $(SIM_ALL_RECURSIVE_DEPS)
	$(AM_V_CC)$(COMPILE) -c -o $@ $<

%D%/modules.c: %D%/stamp-modules ; @true
%D%/stamp-modules: Makefile $(%C%_libsim_a_SOURCES) ; $(GEN_MODULES_C)
