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

%C%_libsim_a_SOURCES =
%C%_libsim_a_LIBADD = \
	$(common_libcommon_a_OBJECTS) \
	$(patsubst %,%D%/%,$(SIM_NEW_COMMON_OBJS)) \
	$(patsubst %,%D%/dv-%.o,$(SIM_HW_DEVICES)) \
	%D%/cpustate.o \
	%D%/interp.o \
	%D%/memory.o \
	%D%/modules.o \
	%D%/sim-resume.o \
	%D%/simulator.o

noinst_LIBRARIES += %D%/libsim.a

%C%_run_SOURCES =
%C%_run_LDADD = \
	%D%/nrun.o \
	%D%/libsim.a \
	$(SIM_COMMON_LIBS)

noinst_PROGRAMS += %D%/run

SIM_ALL_RECURSIVE_DEPS += %D%/modules.c

$(%C%_libsim_a_LIBADD): | $(SIM_ALL_RECURSIVE_DEPS)

%D%/%.o: %D%/%.c | $(SIM_ALL_RECURSIVE_DEPS)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)

%D%/%.o: common/%.c | $(SIM_ALL_RECURSIVE_DEPS)
	$(AM_V_at)$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)
