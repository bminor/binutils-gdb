# This file contains common code used by all simulators.
#
# SIM_AC_COMMON invokes AC macros used by all simulators and by the common
# directory.  It is intended to be invoked before any target specific stuff.
# SIM_AC_OUTPUT is a cover function to AC_OUTPUT to generate the Makefile.
# It is intended to be invoked last.
#
# The simulator's configure.in should look like:
#
# dnl Process this file with autoconf to produce a configure script.
# sinclude(../common/aclocal.m4)
# AC_PREREQ(2.5)dnl
# AC_INIT(Makefile.in)
#
# SIM_AC_COMMON
#
# ... target specific stuff ...
#
# SIM_AC_OUTPUT

AC_DEFUN(SIM_AC_COMMON,
[
# autoconf.info says this should be called right after AC_INIT.
AC_CONFIG_HEADER(config.h:config.in)

AC_CONFIG_AUX_DIR(`cd $srcdir;pwd`/../..)
AC_CANONICAL_SYSTEM
AC_ARG_PROGRAM
AC_PROG_CC
AC_PROG_INSTALL

# Put a plausible default for CC_FOR_BUILD in Makefile.
AC_C_CROSS
if test "x$cross_compiling" = "xno"; then
  CC_FOR_BUILD='$(CC)'
else
  CC_FOR_BUILD=gcc
fi
AC_SUBST(CC_FOR_BUILD)

AC_SUBST(CFLAGS)
AC_SUBST(HDEFINES)
AR=${AR-ar}
AC_SUBST(AR)
AC_PROG_RANLIB

. ${srcdir}/../../bfd/configure.host

dnl Standard simulator options.
dnl Eventually all simulators will support these.
dnl Do not add any here that cannot be supported by all simulators.
dnl Do not add similar but different options to a particular simulator,
dnl all shall eventually behave the same way.


dnl This is a generic option to enable special byte swapping
dnl insns on *any* cpu.
AC_ARG_ENABLE(sim-bswap,
[  --enable-sim-bswap			Use Host specific BSWAP instruction.],
[case "${enableval}" in
  yes)	sim_bswap="-DWITH_BSWAP=1 -DUSE_BSWAP";;
  no)	sim_bswap="-DWITH_BSWAP=0";;
  *)	AC_MSG_ERROR("--enable-sim-bswap does not take a value"); sim_bswap="";;
esac
if test x"$silent" != x"yes" && test x"$sim_bswap" != x""; then
  echo "Setting bswap flags = $sim_bswap" 6>&1
fi],[sim_bswap=""])dnl
AC_SUBST(sim_bswap)


AC_ARG_ENABLE(sim-cflags,
[  --enable-sim-cflags=opts		Extra CFLAGS for use in building simulator],
[case "${enableval}" in
  yes)	 sim_cflags="-O2";;
  trace) AC_MSG_ERROR("Please use --enable-sim-debug instead."); sim_cflags="";;
  no)	 sim_cflags="";;
  *)	 sim_cflags=`echo "${enableval}" | sed -e "s/,/ /g"`;;
esac
if test x"$silent" != x"yes" && test x"$sim_cflags" != x""; then
  echo "Setting sim cflags = $sim_cflags" 6>&1
fi],[sim_cflags=""])dnl
AC_SUBST(sim_cflags)


dnl --enable-sim-debug is for developers of the simulator
dnl the allowable values are work-in-progress
AC_ARG_ENABLE(sim-debug,
[  --enable-sim-debug=opts		Enable debugging flags],
[case "${enableval}" in
  yes)	sim_debug="-DDEBUG=7";;
  no)	sim_debug="-DDEBUG=0";;
  *)	sim_debug="-DDEBUG='(${enableval})'";;
esac
if test x"$silent" != x"yes" && test x"$sim_debug" != x""; then
  echo "Setting sim debug = $sim_debug" 6>&1
fi],[sim_debug=""])dnl
AC_SUBST(sim_debug)


dnl --enable-sim-trace is for users of the simulator
dnl the allowable values are work-in-progress
AC_ARG_ENABLE(sim-trace,
[  --enable-sim-trace=opts		Enable tracing flags],
[case "${enableval}" in
  yes)	sim_trace="-DTRACE=1";;
  no)	sim_trace="-DTRACE=0";;
  *)	sim_trace="-DTRACE='(${enableval})'";;
esac
if test x"$silent" != x"yes" && test x"$sim_trace" != x""; then
  echo "Setting sim trace = $sim_trace" 6>&1
fi],[sim_trace=""])dnl
AC_SUBST(sim_trace)


dnl These are available to append to as desired.
sim_link_files=
sim_link_links=

dnl Create tconfig.h either from simulator's tconfig.in or default one
dnl in common.
sim_link_links=tconfig.h
if test -f ${srcdir}/tconfig.in
then
  sim_link_files=tconfig.in
else
  sim_link_files=../common/tconfig.in
fi

# targ-vals.def points to the libc macro description file.
case "${target}" in
*-*-*) TARG_VALS_DEF=../common/nltvals.def ;;
esac
sim_link_files="${sim_link_files} ${TARG_VALS_DEF}"
sim_link_links="${sim_link_links} targ-vals.def"

]) dnl End of SIM_AC_COMMON


dnl Almost standard simulator options.
dnl Eventually all simulators will support these.


dnl --enable-sim-endian={yes,no,big,little} is for simulators
dnl that support both big and little endian targets.
AC_DEFUN(SIM_AC_OPTION_ENDIAN,
[
default_sim_endian="ifelse([$1],,,-DWITH_TARGET_BYTE_ORDER=[$1])"
AC_ARG_ENABLE(sim-endian,
[  --enable-sim-endian=endian		Specify target byte endian orientation.],
[case "${enableval}" in
  yes)	case "$target" in
	  *powerpc-*) sim_endian="-DWITH_TARGET_BYTE_ORDER=BIG_ENDIAN";;
	  *powerpcle-*) sim_endian="-DWITH_TARGET_BYTE_ORDER=LITTLE_ENDIAN";;
	  *) echo "Unknown target $target" 1>&6; sim_endian="-DWITH_TARGET_BYTE_ORDER=0";;
	esac;;
  no)	 sim_endian="-DWITH_TARGET_BYTE_ORDER=0";;
  b*|B*) sim_endian="-DWITH_TARGET_BYTE_ORDER=BIG_ENDIAN";;
  l*|L*) sim_endian="-DWITH_TARGET_BYTE_ORDER=LITTLE_ENDIAN";;
  *)	 AC_MSG_ERROR("Unknown value $enableval for --enable-sim-endian"); sim_endian="";;
esac
if test x"$silent" != x"yes" && test x"$sim_endian" != x""; then
  echo "Setting endian flags = $sim_endian" 6>&1
fi],[sim_endian="${default_sim_endian}"])dnl
])


dnl --enable-sim-hostendian is for users of the simulator when
dnl they find that AC_C_BIGENDIAN does not function correctly
dnl (for instance in a canadian cross)
AC_DEFUN(SIM_AC_OPTION_HOSTENDIAN,
[
AC_ARG_ENABLE(sim-hostendian,
[  --enable-sim-hostendain=end		Specify host byte endian orientation.],
[case "${enableval}" in
  no)	 sim_hostendian="-DWITH_HOST_BYTE_ORDER=0";;
  b*|B*) sim_hostendian="-DWITH_HOST_BYTE_ORDER=BIG_ENDIAN";;
  l*|L*) sim_hostendian="-DWITH_HOST_BYTE_ORDER=LITTLE_ENDIAN";;
  *)	 AC_MSG_ERROR("Unknown value $enableval for --enable-sim-hostendian"); sim_hostendian="";;
esac
if test x"$silent" != x"yes" && test x"$sim_hostendian" != x""; then
  echo "Setting hostendian flags = $sim_hostendian" 6>&1
fi],[
if test "x$cross_compiling" = "xno"; then
  AC_C_BIGENDIAN
  if test $ac_cv_c_bigendian = yes; then
    sim_hostendian="-DWITH_HOST_BYTE_ORDER=BIG_ENDIAN"
  else
    sim_hostendian="-DWITH_HOST_BYTE_ORDER=LITTLE_ENDIAN"
  fi
else
  sim_hostendian="-DWITH_HOST_BYTE_ORDER=0"
fi])dnl
])


dnl --enable-sim-inline is for users that wish to ramp up the simulator's
dnl performance by inlining functions.
AC_DEFUN(SIM_AC_OPTION_INLINE,
[
default_sim_inline="ifelse([$1],,,-DDEFAULT_INLINE=[$1])"
AC_ARG_ENABLE(sim-inline,
[  --enable-sim-inline=inlines		Specify which functions should be inlined.],
[sim_inline=""
case "$enableval" in
  no)		sim_inline="-DDEFAULT_INLINE=0";;
  0)		sim_inline="-DDEFAULT_INLINE=0";;
  yes | 2)	sim_inline="-DDEFAULT_INLINE=ALL_INLINE";;
  1)		sim_inline="-DDEFAULT_INLINE=INLINE_LOCALS";;
  *) for x in `echo "$enableval" | sed -e "s/,/ /g"`; do
       new_flag=""
       case "$x" in
	 *_INLINE=*)	new_flag="-D$x";;
	 *=*)		new_flag=`echo "$x" | sed -e "s/=/_INLINE=/" -e "s/^/-D/"`;;
	 *_INLINE)	new_flag="-D$x=ALL_INLINE";;
	 *)		new_flag="-D$x""_INLINE=ALL_INLINE";;
       esac
       if test x"$sim_inline" = x""; then
	 sim_inline="$new_flag"
       else
	 sim_inline="$sim_inline $new_flag"
       fi
     done;;
esac
if test x"$silent" != x"yes" && test x"$sim_inline" != x""; then
  echo "Setting inline flags = $sim_inline" 6>&1
fi],[if test x"$GCC" != "x" -a x"${default_sim_inline}" != "x" ; then
  sim_inline="${default_sim_inline}"
  if test x"$silent" != x"yes"; then
    echo "Setting inline flags = $sim_inline" 6>&1
  fi
else
  sim_inline=""
fi])dnl
])


dnl --enable-sim-warnings is for developers of the simulator.
dnl it enables extra GCC specific warnings.
AC_DEFUN(SIM_AC_OPTION_WARNINGS,
[
AC_ARG_ENABLE(sim-warnings,
[  --enable-sim-warnings=opts		Extra CFLAGS for turning on compiler warnings except for idecode.o, semantics.o and psim.o],
[case "${enableval}" in
  yes)	sim_warnings="-Werror -Wall -Wpointer-arith -Wmissing-prototypes -Wmissing-declarations ";;
  no)	sim_warnings="-w";;
  *)	sim_warnings=`echo "${enableval}" | sed -e "s/,/ /g"`;;
esac
if test x"$silent" != x"yes" && test x"$sim_warnings" != x""; then
  echo "Setting warning flags = $sim_warnings" 6>&1
fi],[sim_warnings=""])dnl
])




dnl Generate the Makefile in a target specific directory.
dnl Substitutions aren't performed on the file in AC_SUBST_FILE,
dnl so this is a cover macro to tuck the details away of how we cope.
dnl We cope by having autoconf generate two files and then merge them into
dnl one afterwards.  The two pieces of the common fragment are inserted into
dnl the target's fragment at the appropriate points.

AC_DEFUN(SIM_AC_OUTPUT,
[
dnl Optional options
AC_SUBST(sim_endian)
AC_SUBST(sim_hostendian)
AC_SUBST(sim_inline)
AC_SUBST(sim_warnings)
dnl
AC_LINK_FILES($sim_link_files, $sim_link_links)
AC_OUTPUT(Makefile.sim:Makefile.in Make-common.sim:../common/Make-common.in,
[case "x$CONFIG_FILES" in xMakefile*)
   echo "Merging Makefile.sim+Make-common.sim into Makefile ..."
   rm -f Makesim1.tmp Makesim2.tmp Makefile
   sed -n -e '/^## COMMON_PRE_/,/^## End COMMON_PRE_/ p' <Make-common.sim >Makesim1.tmp
   sed -n -e '/^## COMMON_POST_/,/^## End COMMON_POST_/ p' <Make-common.sim >Makesim2.tmp
   sed -e '/^## COMMON_PRE_/ r Makesim1.tmp' \
	-e '/^## COMMON_POST_/ r Makesim2.tmp' \
	<Makefile.sim >Makefile
   rm -f Makefile.sim Make-common.sim Makesim1.tmp Makesim2.tmp
   ;;
 esac
 case "x$CONFIG_HEADERS" in xconfig.h:config.in) echo > stamp-h ;; esac
])
])
