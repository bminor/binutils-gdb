dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)
AC_CONFIG_MACRO_DIRS([../m4 ../.. ../../config])

dnl The sim shouldn't be checking $target and changing behavior.  But it is,
dnl and until we clean that up, we need to expand --target for use below.
AC_CANONICAL_SYSTEM

#
# Select the IGEN architecture
#
SIM_MIPS_GEN=IGEN
sim_mips_igen_machine="-M mipsIV"
sim_mips_m16_machine="-M mips16,mipsIII"
sim_mips_igen_filter="32,64,f"
sim_mips_m16_filter="16"

case "${target}" in
  mips*tx39*)		SIM_MIPS_GEN=IGEN
			sim_mips_igen_filter="32,f"
			sim_mips_igen_machine="-M r3900"
			;;
  mips64vr41*)		SIM_MIPS_GEN=M16
			sim_mips_igen_machine="-M vr4100"
			sim_mips_m16_machine="-M vr4100"
                        ;;
  mips64*)
			SIM_MIPS_GEN=MULTI
			sim_mips_multi_configs="\
			  vr4100:mipsIII,mips16,vr4100:32,64:mips4100,mips4111\
			  vr4120:mipsIII,mips16,vr4120:32,64:mips4120\
			  vr5000:mipsIV:32,64,f:mips4300,mips5000,mips8000\
			  vr5400:mipsIV,vr5400:32,64,f:mips5400\
			  vr5500:mipsIV,vr5500:32,64,f:mips5500"
			sim_mips_multi_default=mips5000
			;;
  mips*-sde-elf* | mips*-mti-elf*)
			SIM_MIPS_GEN=MULTI
			sim_mips_multi_configs="\
			  micromips:micromips64,micromipsdsp:32,64,f:mips_micromips\
			  mipsisa64r2:mips64r2,mips16,mips16e,mdmx,dsp,dsp2,mips3d,smartmips:32,64,f:mipsisa32r2,mipsisa64r2,mipsisa32r5,mipsisa64r5\
			  mipsisa64r6:mips64r6:32,64,f:mipsisa32r6,mipsisa64r6"
			sim_mips_multi_default=mipsisa64r2
			;;
  mips16*)		SIM_MIPS_GEN=M16
			;;
  mipsisa32r2*)		SIM_MIPS_GEN=MULTI
			sim_mips_multi_configs="\
			  micromips:micromips32,micromipsdsp:32,f:mips_micromips\
			  mips32r2:mips32r2,mips3d,mips16,mips16e,mdmx,dsp,dsp2,smartmips:32,f:mipsisa32r2"
			sim_mips_multi_default=mipsisa32r2
			;;
  mipsisa32r6*)		SIM_MIPS_GEN=IGEN
			sim_mips_igen_machine="-M mips32r6"
			sim_mips_igen_filter="32,f"
			;;
  mipsisa32*)		SIM_MIPS_GEN=M16
			sim_mips_igen_machine="-M mips32,mips16,mips16e,smartmips"
			sim_mips_m16_machine="-M mips16,mips16e,mips32"
			sim_mips_igen_filter="32,f"
			;;
  mipsisa64r2*)		SIM_MIPS_GEN=M16
			sim_mips_igen_machine="-M mips64r2,mips3d,mips16,mips16e,mdmx,dsp,dsp2"
			sim_mips_m16_machine="-M mips16,mips16e,mips64r2"
			;;
  mipsisa64r6*)		SIM_MIPS_GEN=IGEN
			sim_mips_igen_machine="-M mips64r6"
			;;
  mipsisa64sb1*)	SIM_MIPS_GEN=IGEN
			sim_mips_igen_machine="-M mips64,mips3d,sb1"
			;;
  mipsisa64*)		SIM_MIPS_GEN=M16
			sim_mips_igen_machine="-M mips64,mips3d,mips16,mips16e,mdmx"
			sim_mips_m16_machine="-M mips16,mips16e,mips64"
			;;
  mips*lsi*)		SIM_MIPS_GEN=M16
			sim_mips_igen_machine="-M mipsIII,mips16"
			sim_mips_m16_machine="-M mips16,mipsIII"
			sim_mips_igen_filter="32,f"
			;;
  mips*)		SIM_MIPS_GEN=IGEN
			sim_mips_igen_filter="32,f"
			;;
esac

# The MULTI generator can combine several simulation engines into one.
# executable.  A configuration which uses the MULTI should set two
# variables: ${sim_mips_multi_configs} and ${sim_mips_multi_default}.
#
# ${sim_mips_multi_configs} is the list of engines to build.  Each
# space-separated entry has the form NAME:MACHINE:FILTER:BFDMACHS,
# where:
#
# - NAME is a C-compatible prefix for the engine,
# - MACHINE is a -M argument,
# - FILTER is a -F argument, and
# - BFDMACHS is a comma-separated list of bfd machines that the
#     simulator can run.
#
# Each entry will have a separate simulation engine whose prefix is
# m32<NAME>.  If the machine list includes "mips16", there will also
# be a mips16 engine, prefix m16<NAME>.  The mips16 engine will be
# generated using the same machine list as the 32-bit version,
# but the filter will be "16" instead of FILTER.
#
# The simulator compares the bfd mach against BFDMACHS to decide
# which engine to use.  Entries in BFDMACHS should be bfd_mach
# values with "bfd_mach_" removed.  ${sim_mips_multi_default} says
# which entry should be the default.
SIM_MIPS_MULTI_FLAGS=
SIM_MIPS_MULTI_SRC=
SIM_MIPS_MULTI_OBJ=
SIM_MIPS_MULTI_IGEN_CONFIGS=
if test ${SIM_MIPS_GEN} = MULTI; then

  # Simple sanity check.
  if test -z "${sim_mips_multi_configs}" || test -z "${sim_mips_multi_default}"; then
    AC_MSG_ERROR(Error in configure.ac: MULTI simulator not set up correctly)
  fi

  # Start in a known state.
  rm -f multi-include.h multi-run.c
  sim_mips_seen_default=no

  cat << __EOF__ > multi-run.c
/* Main entry point for MULTI simulators.
   Copyright (C) 2003-2022 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   --

   This file was generated by sim/mips/configure.  */

#include "sim-main.h"
#include "multi-include.h"
#include "elf-bfd.h"
#include "elfxx-mips.h"
#include "elf/mips.h"

#define SD sd
#define CPU cpu

void
sim_engine_run (SIM_DESC sd,
		int next_cpu_nr,
		int nr_cpus,
		int signal) /* ignore */
{
  int mach;

  if (STATE_ARCHITECTURE (sd) == NULL)
    mach = bfd_mach_${sim_mips_multi_default};
  else if (elf_elfheader (STATE_PROG_BFD (sd))->e_flags
	   & EF_MIPS_ARCH_ASE_MICROMIPS)
    mach = bfd_mach_mips_micromips;
  else
  {
     mach = _bfd_elf_mips_mach (elf_elfheader (STATE_PROG_BFD (sd))->e_flags);
     if (!mach)
       mach = STATE_ARCHITECTURE (SD)->mach;
  }

  switch (mach)
    {
__EOF__

  for fc in ${sim_mips_multi_configs}; do

    # Split up the entry.  ${c} contains the first three elements.
    # Note: outer sqaure brackets are m4 quotes.
    c=`echo ${fc} | sed ['s/:[^:]*$//']`
    bfdmachs=`echo ${fc} | sed 's/.*://'`
    name=`echo ${c} | sed 's/:.*//'`
    machine=`echo ${c} | sed 's/.*:\(.*\):.*/\1/'`
    filter=`echo ${c} | sed 's/.*://'`

    # Build the following lists:
    #
    #   SIM_MIPS_MULTI_FLAGS: all -M and -F flags used by the simulator
    #   SIM_MIPS_MULTI_SRC: all makefile-generated source files
    #   SIM_MIPS_MULTI_OBJ: the objects for ${SIM_MIPS_MULTI_SRC}
    #   SIM_MIPS_MULTI_IGEN_CONFIGS: igen configuration strings.
    #
    # Each entry in ${SIM_MIPS_MULTI_IGEN_CONFIGS} is a prefix (m32
    # or m16) followed by the NAME, MACHINE and FILTER part of
    # the ${sim_mips_multi_configs} entry.
    SIM_MIPS_MULTI_FLAGS="${SIM_MIPS_MULTI_FLAGS} -F ${filter} -M ${machine}"

    # Check whether special handling is needed.
    case ${c} in
      *:*mips16*:*)
	# Run igen twice, once for normal mode and once for mips16.
	ws="m32 m16"

	# The top-level function for the mips16 simulator is
	# in a file m16${name}_run.c, generated by the
	# tmp-run-multi Makefile rule.
	SIM_MIPS_MULTI_SRC="${SIM_MIPS_MULTI_SRC} m16${name}_run.c"
	SIM_MIPS_MULTI_OBJ="${SIM_MIPS_MULTI_OBJ} m16${name}_run.o"
	SIM_MIPS_MULTI_FLAGS="${SIM_MIPS_MULTI_FLAGS} -F 16"
	;;
      *:*micromips32*:*)
	# Run igen thrice, once for micromips32, once for micromips16,
	# and once for m32.
	ws="micromips_m32 micromips16 micromips32"

	# The top-level function for the micromips simulator is
	# in a file micromips${name}_run.c, generated by the
	# tmp-run-multi Makefile rule.
	SIM_MIPS_MULTI_SRC="${SIM_MIPS_MULTI_SRC} micromips${name}_run.c"
	SIM_MIPS_MULTI_OBJ="${SIM_MIPS_MULTI_OBJ} micromips${name}_run.o"
	SIM_MIPS_MULTI_FLAGS="${SIM_MIPS_MULTI_FLAGS} -F 16,32"
	;;
      *:*micromips64*:*)
	# Run igen thrice, once for micromips64, once for micromips16,
	# and once for m64.
	ws="micromips_m64 micromips16 micromips64"

	# The top-level function for the micromips simulator is
	# in a file micromips${name}_run.c, generated by the
	# tmp-run-multi Makefile rule.
	SIM_MIPS_MULTI_SRC="${SIM_MIPS_MULTI_SRC} micromips${name}_run.c"
	SIM_MIPS_MULTI_OBJ="${SIM_MIPS_MULTI_OBJ} micromips${name}_run.o"
	SIM_MIPS_MULTI_FLAGS="${SIM_MIPS_MULTI_FLAGS} -F 16,32,64"
	;;
      *)
	ws=m32
	;;
    esac

    # Now add the list of igen-generated files to ${SIM_MIPS_MULTI_SRC}
    # and ${SIM_MIPS_MULTI_OBJ}.
    for w in ${ws}; do
      for base in engine icache idecode model semantics support; do
	SIM_MIPS_MULTI_SRC="${SIM_MIPS_MULTI_SRC} ${w}${name}_${base}.c"
	SIM_MIPS_MULTI_SRC="${SIM_MIPS_MULTI_SRC} ${w}${name}_${base}.h"
	SIM_MIPS_MULTI_OBJ="${SIM_MIPS_MULTI_OBJ} ${w}${name}_${base}.o"
      done
      SIM_MIPS_MULTI_IGEN_CONFIGS="${SIM_MIPS_MULTI_IGEN_CONFIGS} ${w}${c}"
    done

    # Add an include for the engine.h file.  This file declares the
    # top-level foo_engine_run() function.
    echo "#include \"${w}${name}_engine.h\"" >> multi-include.h

    # Add case statements for this engine to sim_engine_run().
    for mach in `echo ${bfdmachs} | sed 's/,/ /g'`; do
      echo "    case bfd_mach_${mach}:" >> multi-run.c
      if test ${mach} = ${sim_mips_multi_default}; then
	echo "    default:" >> multi-run.c
	sim_mips_seen_default=yes
      fi
    done
    echo "      ${w}${name}_engine_run (sd, next_cpu_nr, nr_cpus, signal);" \
      >> multi-run.c
    echo "      break;" >> multi-run.c
  done

  # Check whether we added a 'default:' label.
  if test ${sim_mips_seen_default} = no; then
    AC_MSG_ERROR(Error in configure.ac: \${sim_mips_multi_configs} doesn't have an entry for \${sim_mips_multi_default})
  fi

  cat << __EOF__ >> multi-run.c
    }
}
__EOF__
else
  # For clean-extra
  SIM_MIPS_MULTI_SRC=doesnt-exist.c
fi
SIM_MIPS_IGEN_FLAGS="-F ${sim_mips_igen_filter} ${sim_mips_igen_machine} ${sim_igen_smp}"
SIM_MIPS_M16_FLAGS="-F ${sim_mips_m16_filter} ${sim_mips_m16_machine} ${sim_igen_smp}"
SIM_MIPS_MICROMIPS16_FLAGS="-F ${sim_mips_micromips16_filter} ${sim_mips_micromips16_machine} ${sim_igen_smp}"
SIM_MIPS_MICROMIPS_FLAGS="-F ${sim_mips_micromips_filter} ${sim_mips_micromips_machine} ${sim_igen_smp}"
AC_SUBST(SIM_MIPS_IGEN_FLAGS)
AC_SUBST(SIM_MIPS_M16_FLAGS)
AC_SUBST(SIM_MIPS_MICROMIPS_FLAGS)
AC_SUBST(SIM_MIPS_MICROMIPS16_FLAGS)
AC_SUBST(SIM_MIPS_GEN)
AC_SUBST(SIM_MIPS_MULTI_FLAGS)
AC_SUBST(SIM_MIPS_MULTI_IGEN_CONFIGS)
AC_SUBST(SIM_MIPS_MULTI_SRC)
AC_SUBST(SIM_MIPS_MULTI_OBJ)

SIM_AC_OUTPUT
