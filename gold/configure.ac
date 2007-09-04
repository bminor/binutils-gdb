dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT
AC_CONFIG_SRCDIR([gold.cc])

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(gold, 0.1)

AM_CONFIG_HEADER(config.h:config.in)

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
  else
    case "$targ" in
    i?86-*) targ_32_little=yes ;;
    x86_64-*) targ_64_little=yes ;;
    *)
      AC_MSG_ERROR("target $targ: unknown size and endianness")
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

AC_PROG_CC
AC_PROG_CXX
AC_PROG_YACC
AC_PROG_RANLIB
AC_PROG_INSTALL
ZW_GNU_GETTEXT_SISTER_DIR
AM_PO_SUBDIRS

AC_C_BIGENDIAN

AC_EXEEXT

AM_BINUTILS_WARNINGS

WARN_CXXFLAGS=`echo ${WARN_CFLAGS} | sed -e 's/-Wstrict-prototypes//' -e 's/-Wmissing-prototypes//'`
AC_SUBST(WARN_CXXFLAGS)

dnl Force support for large files by default.  This may need to be
dnl host dependent.  If build == host, we can check getconf LFS_CFLAGS.
LFS_CXXFLAGS="-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64"
AC_SUBST(LFS_CXXFLAGS)

AC_LANG_PUSH(C++)

AC_CHECK_HEADERS(tr1/unordered_set tr1/unordered_map)
AC_CHECK_HEADERS(ext/hash_map ext/hash_set)

dnl Test whether the compiler can specify a member templates to call.
AC_COMPILE_IFELSE([
class c { public: template<int i> void fn(); };
template<int i> void foo(c cv) { cv.fn<i>(); }
template void foo<1>(c cv);],
[AC_DEFINE(HAVE_MEMBER_TEMPLATE_SPECIFICATIONS, [],
  [Whether the C++ compiler can call a template member with no arguments])])

AC_LANG_POP(C++)

AM_MAINTAINER_MODE

AC_OUTPUT(Makefile testsuite/Makefile po/Makefile.in:po/Make-in)
