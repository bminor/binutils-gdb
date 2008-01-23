dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

AC_INIT(gold, 0.1)
AC_CONFIG_SRCDIR(gold.cc)

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AM_CONFIG_HEADER(config.h:config.in)

AC_ARG_WITH(sysroot,
[  --with-sysroot[=DIR]    search for usr/lib et al within DIR],
[sysroot=$withval], [sysroot=no])

if test "$sysroot" = "yes"; then
  sysroot='${exec_prefix}/${target_alias}/sys-root'
elif test "$sysroot" = "no"; then
  sysroot=
fi

sysroot_relocatable=0
if test -n "$sysroot"; then
  case "sysroot" in
    "${prefix}" | "${prefix}/"* | \
    "${exec_prefix}" | "${exec_prefix}/"* | \
    '${prefix}' | '${prefix}/'*| \
    '${exec_prefix}' | '${exec_prefix}/'*)
      sysroot_relocatable=1
      ;;
  esac
fi

AC_DEFINE_UNQUOTED(TARGET_SYSTEM_ROOT, "$sysroot",
  [System root for target files])
AC_DEFINE_UNQUOTED(TARGET_SYSTEM_ROOT_RELOCATABLE, $sysroot_relocatable,
  [Whether the system root can be relocated])

dnl For now threads are a configure time option.
AC_ARG_ENABLE([threads],
[  --enable-threads        multi-threaded linking],
[case "${enableval}" in
  yes | "") threads=yes ;;
  no) threads=no ;;
  *) threads=yes ;;
 esac],
[threads=no])
if test "$threads" = "yes"; then
  AC_DEFINE(ENABLE_THREADS, 1,
	    [Define to do multi-threaded linking])
fi
AM_CONDITIONAL(THREADS, test "$threads" = "yes")

AC_ARG_ENABLE([targets],
[  --enable-targets        alternative target configurations],
[case "${enableval}" in
  yes | "")
    AC_MSG_ERROR([--enable-targets option must specify target names or 'all'])
    ;;
  no)
    enable_targets=
    ;;
  *)
    enable_targets=$enableval
    ;;
esac],
[# For now, enable all targets by default
 enable_targets=all
])

# Canonicalize the enabled targets.
if test -n "$enable_targets"; then
  for targ in `echo $enable_targets | sed -e 's/,/ /g'`; do
    result=`$ac_config_sub $targ 2>/dev/null`
    if test -n "$result"; then
      canon_targets="$canon_targets $result"
    else
      # Permit unrecognized target names, like "all".
      canon_targets="$canon_targets $targ"
    fi
  done
fi

# See which specific instantiations we need.
targetobjs=
all_targets=
for targ in $target $canon_targets; do
  targ_32_little=
  targ_32_big=
  targ_64_little=
  targ_64_big=
  if test "$targ" = "all"; then
    targ_32_little=yes
    targ_32_big=yes
    targ_64_little=yes
    targ_64_big=yes
    all_targets=yes
  else
    case "$targ" in
    i?86-*)
      targ_32_little=yes
      targetobjs="$targetobjs i386.\$(OBJEXT)"
      ;;
    x86_64-*)
      targ_64_little=yes
      targetobjs="$targetobjs x86_64.\$(OBJEXT)"
      ;;
    *)
      AC_MSG_ERROR("unsupported target $targ")
      ;;
    esac
  fi
done

if test -n "$targ_32_little"; then
  AC_DEFINE(HAVE_TARGET_32_LITTLE, 1,
	      [Define to support 32-bit little-endian targets])
fi
if test -n "$targ_32_big"; then
  AC_DEFINE(HAVE_TARGET_32_BIG, 1,
	      [Define to support 32-bit big-endian targets])
fi
if test -n "$targ_64_little"; then
  AC_DEFINE(HAVE_TARGET_64_LITTLE, 1,
	      [Define to support 64-bit little-endian targets])
fi
if test -n "$targ_64_big"; then
  AC_DEFINE(HAVE_TARGET_64_BIG, 1,
	      [Define to support 64-bit big-endian targets])
fi

if test -n "$all_targets"; then
  TARGETOBJS='$(ALL_TARGETOBJS)'
else
  TARGETOBJS="$targetobjs"
fi
AC_SUBST(TARGETOBJS)

AC_PROG_CC
AC_PROG_CXX
AC_PROG_YACC
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_LN_S

AC_GNU_SOURCE

ZW_GNU_GETTEXT_SISTER_DIR
AM_PO_SUBDIRS

AC_C_BIGENDIAN

AC_EXEEXT

AM_CONDITIONAL(NATIVE_LINKER,
  test "x$target_alias" = "x" -o "x$host_alias" = "x$target_alias")
AM_CONDITIONAL(GCC, test "$GCC" = yes)
AM_CONDITIONAL(OBJDUMP_AND_CPPFILT,
	[objdump --help >/dev/null && c++filt --help >/dev/null])

dnl Some architectures do not support taking pointers of functions
dnl defined in shared libraries except in -fPIC mode.  We need to
dnl tell the unittest framework if we're compiling for one of those
dnl targets, so it doesn't try to run the tests that do that.
AM_CONDITIONAL(FN_PTRS_IN_SO_WITHOUT_PIC, [
  case $target_cpu in
    i?86) true;;
    x86_64) false;;
    *) true;;
  esac])

dnl Test for __thread support.
AC_CACHE_CHECK([for thread support], [gold_cv_c_thread],
[AC_COMPILE_IFELSE([__thread int i = 1;],
[gold_cv_c_thread=yes], [gold_cv_c_thread=no])])

AM_CONDITIONAL(TLS, test "$gold_cv_c_thread" = "yes")

dnl On GNU/Linux TLS in static programs only works when using glibc
dnl 2.4 or later.
AC_CACHE_CHECK([for glibc >= 2.4], [gold_cv_lib_glibc24],
[AC_COMPILE_IFELSE([
#include <features.h>
#if !defined __GLIBC__
error
#elif __GLIBC__ < 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ < 4)
error
#endif
], [gold_cv_lib_glibc24=yes], [gold_cv_lib_glibc24=no])])

AM_CONDITIONAL(STATIC_TLS, test "$gold_cv_lib_glibc24" = "yes")

AM_BINUTILS_WARNINGS

WARN_CXXFLAGS=`echo ${WARN_CFLAGS} | sed -e 's/-Wstrict-prototypes//' -e 's/-Wmissing-prototypes//'`
AC_SUBST(WARN_CXXFLAGS)

dnl Force support for large files by default.  This may need to be
dnl host dependent.  If build == host, we can check getconf LFS_CFLAGS.
LFS_CFLAGS="-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
AC_SUBST(LFS_CFLAGS)

AC_REPLACE_FUNCS(pread)

# Link in zlib if we can.  This allows us to write compressed sections.
AC_SEARCH_LIBS(zlibVersion, z, [AC_CHECK_HEADERS(zlib.h)])

AC_LANG_PUSH(C++)

AC_CHECK_HEADERS(tr1/unordered_set tr1/unordered_map)
AC_CHECK_HEADERS(ext/hash_map ext/hash_set)
AC_CHECK_FUNCS(mallinfo)

dnl Test whether the compiler can specify a member templates to call.
AC_CACHE_CHECK([template member calls], [gold_cv_c_membertemplates],
[AC_COMPILE_IFELSE([
class c { public: template<int i> void fn(); };
template<int i> void foo(c cv) { cv.fn<i>(); }
template void foo<1>(c cv);],
[gold_cv_c_membertemplates=yes], [gold_cv_c_membertemplates=no])])

if test "$gold_cv_c_membertemplates" = "yes"; then
  AC_DEFINE(HAVE_MEMBER_TEMPLATE_SPECIFICATIONS, [],
    [Whether the C++ compiler can call a template member with no arguments])
fi

AC_LANG_POP(C++)

AM_MAINTAINER_MODE

AC_OUTPUT(Makefile testsuite/Makefile po/Makefile.in:po/Make-in)
