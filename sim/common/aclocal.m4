# This file contains common code used by all simulators.
# SIM_AC_COMMON invokes AC macros used by all simulators and by the
# common directory.
# SIM_AC_OUTPUT is a cover function to AC_OUTPUT to use generate the
# Makefile in a target specific directory.

AC_DEFUN(SIM_AC_COMMON,
[
# autoconf.info says this should be called right after AC_INIT.
AC_CONFIG_HEADER(config.h:config.in)

AC_CONFIG_AUX_DIR(`cd $srcdir;pwd`/../..)
AC_CANONICAL_SYSTEM
AC_ARG_PROGRAM
AC_PROG_CC
AC_PROG_INSTALL
AC_C_BIGENDIAN

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

dnl This is a generic option to enable special byte swapping
dnl insns on *any* cpu.
AC_ARG_ENABLE(sim-bswap,
[  --enable-sim-bswap			Use Host specific BSWAP instruction.],
[case "${enableval}" in
  yes)	sim_bswap="-DUSE_BSWAP";;
  no)	sim_bswap="";;
  *)	AC_MSG_ERROR("--enable-sim-bswap does not take a value"); sim_bswap="";;
esac
if test x"$silent" != x"yes" && test x"$sim_bswap" != x""; then
  echo "Setting bswap flags = $sim_bswap" 6>&1
fi],[sim_bswap=""])dnl
AC_SUBST(sim_bswap)

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

case "${target}" in
*-*-*)
	sim_link_files="${sim_link_files} ../common/nltvals.def"
	sim_link_links="${sim_link_links} targ-vals.def"
	;;
esac

dnl Stuff that gets inserted into the Makefile

COMMON_MAKEFILE_FRAG=makefile-temp-$$
cat > $COMMON_MAKEFILE_FRAG <<EOF
VPATH = ${srcdir}
srcdir = ${srcdir}
srcroot = \$(srcdir)/../..

prefix = ${prefix}
exec_prefix = ${exec_prefix}

host_alias = ${host_alias}
target_alias = ${target_alias}
program_transform_name = ${program_transform_name}
bindir = ${bindir}

libdir = ${libdir}
tooldir = \$(libdir)/\$(target_alias)

datadir = ${datadir}
mandir = ${mandir}
man1dir = \$(mandir)/man1
infodir = ${infodir}
includedir = ${includedir}

SHELL = /bin/sh

INSTALL = \$(srcroot)/install.sh -c
INSTALL_PROGRAM = ${INSTALL_PROGRAM}
INSTALL_DATA = ${INSTALL_DATA}
INSTALL_XFORM = \$(INSTALL) -t='\$(program_transform_name)'
INSTALL_XFORM1= \$(INSTALL_XFORM) -b=.1

CC = ${CC}
CC_FOR_BUILD = ${CC_FOR_BUILD}
CFLAGS = ${CFLAGS}
SIM_CFLAGS = ${sim_cflags}
SIM_DEBUG = ${sim_debug}
SIM_TRACE = ${sim_trace}
SIM_BSWAP = ${sim_bswap}

HDEFINES = ${HDEFINES}
TDEFINES =

AR = ${AR}
AR_FLAGS = rc
RANLIB = ${RANLIB}
MAKEINFO = makeinfo

# Each simulator's Makefile.in defines one or more of these variables
# as necessary.  The SIM_AC_OUTPUT macro then inserts those values
# at '## Config'.

# List of object files, less common parts.
#SIM_OBJS =
# List of flags to always pass to \$(CC).
#SIM_EXTRA_CFLAGS =
# List of extra libraries to link with.
#SIM_EXTRA_LIBS =
# List of extra program dependencies.
#SIM_EXTRA_LIBDEPS =
# Dependency of 'all' to build any extra files.
#SIM_EXTRA_ALL =
# Dependency of 'install' to install any extra files.
#SIM_EXTRA_INSTALL =
# Dependency of 'clean' to clean any extra files.
#SIM_EXTRA_CLEAN =

CONFIG_CFLAGS = ${DEFS} \$(SIM_CFLAGS) \$(SIM_DEBUG) \$(SIM_TRACE) \$(SIM_BSWAP) \
  \$(SIM_EXTRA_CFLAGS) \$(HDEFINES) \$(TDEFINES)
CSEARCH = -I. -I\$(srcdir) -I../common -I\$(srcdir)/../common \
  -I../../include -I\$(srcroot)/include \
  -I../../bfd -I\$(srcroot)/bfd -I\$(srcroot)/gdb \
  -I../../opcodes -I\$(srcroot)/opcodes
ALL_CFLAGS = \$(CONFIG_CFLAGS) \$(CSEARCH) \$(CFLAGS)
BUILD_CFLAGS = -g -O \$(CSEARCH)

.NOEXPORT:
MAKEOVERRIDES=

LIBIBERTY_LIB = ../../libiberty/libiberty.a
BFD_LIB = ../../bfd/libbfd.a
OPCODES_LIB = ../../opcodes/libopcodes.a
CONFIG_LIBS = ${LIBS}
LIBDEPS = \$(BFD_LIB) \$(OPCODES_LIB) \$(LIBIBERTY_LIB) \
	\$(SIM_EXTRA_LIBDEPS)
EXTRA_LIBS = \$(BFD_LIB) \$(OPCODES_LIB) \$(LIBIBERTY_LIB) \
	\$(CONFIG_LIBS) \$(SIM_EXTRA_LIBS)

LIB_OBJS = callback.o targ-map.o \$(SIM_OBJS)

all: run libsim.a \$(SIM_EXTRA_ALL)

libsim.a: \$(LIB_OBJS)
	rm -f libsim.a
	\$(AR) \$(ARFLAGS) libsim.a \$(LIB_OBJS)
	\$(RANLIB) libsim.a

run: run.o libsim.a \$(LIBDEPS)
	\$(CC) \$(ALL_CFLAGS) -o run \
	  run.o libsim.a \$(EXTRA_LIBS)

run.o: \$(srcdir)/../common/run.c config.h tconfig.h \
	  \$(srcroot)/include/callback.h
	\$(CC) -c \$(srcdir)/../common/run.c \$(ALL_CFLAGS)

callback.o: \$(srcdir)/../common/callback.c config.h tconfig.h \
	  \$(srcroot)/include/callback.h targ-vals.h
	\$(CC) -c \$(srcdir)/../common/callback.c \$(ALL_CFLAGS)

gentmap: \$(srcdir)/../common/gentmap.c targ-vals.def
	\$(CC_FOR_BUILD) \$(srcdir)/../common/gentmap.c -o gentmap \$(BUILD_CFLAGS) -I\$(srcdir)/../common

targ-vals.h: gentmap
	rm -f targ-vals.h
	./gentmap -h >targ-vals.h

targ-map.c: gentmap
	rm -f targ-map.c
	./gentmap -c >targ-map.c

install: install-common \$(SIM_EXTRA_INSTALL)

install-common:
	\$(INSTALL_XFORM) run \$(bindir)/run

check:

info:
clean-info:
install-info:

tags etags: TAGS

TAGS: force
	etags *.c *.h

clean: \$(SIM_EXTRA_CLEAN)
	rm -f *.[[oa]] *~ core gentmap targ-map.c targ-vals.h 
	rm -f run libsim.a

distclean mostlyclean maintainer-clean realclean: clean
	rm -f TAGS
	rm -f Makefile config.cache config.log config.status
	rm -f tconfig.h config.h stamp-h targ-vals.def

.c.o:
	\$(CC) -c \$(ALL_CFLAGS) \$<

# Dummy target to force execution of dependent targets.
force:

Makefile: Makefile.in config.status
	CONFIG_HEADERS= \$(SHELL) ./config.status

config.status: configure
	\$(SHELL) ./config.status --recheck

config.h: stamp-h ; @true
stamp-h: config.in config.status
	CONFIG_FILES= CONFIG_HEADERS=config.h:config.in \$(SHELL) ./config.status

# We can't add dependencies to configure because it causes too much trouble
# to end users if configure's timestamp is out of sync.
.PHONY: run-autoconf
run-autoconf:
	cd \$(srcdir) && autoconf -l ../common
EOF

dnl end of COMMON_MAKEFILE_FRAG

AC_SUBST_FILE(COMMON_MAKEFILE_FRAG)

]) dnl End of SIM_AC_COMMON

dnl Generate the Makefile in a target specific directory.
dnl Substitutions aren't performed on the file in AC_SUBST_FILE,
dnl so this is a cover macro to tuck the details away of how we cope.
dnl It also inserts default definitions of the SIM_FOO variables.

AC_DEFUN(SIM_AC_OUTPUT,
[
AC_LINK_FILES($sim_link_files, $sim_link_links)
 AC_OUTPUT(Makefile,[
 case "x$CONFIG_HEADERS" in xconfig.h:config.in) echo > stamp-h ;; esac
 ])

rm $COMMON_MAKEFILE_FRAG

])dnl End of SIM_AC_OUTPUT
