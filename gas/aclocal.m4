dnl
dnl This ugly hack is needed because the Cygnus configure script won't
dnl tell us what CC is going to be, and "cc" isn't always right.  (The
dnl top-level Makefile will always override anything we choose here, so
dnl the usual gcc/cc selection is useless.)
dnl
dnl It knows where it is in the tree; don't try using it elsewhere.
dnl
undefine([AC_PROG_CC])dnl
define(AC_PROG_CC,
[AC_BEFORE([$0], [AC_PROG_CPP])dnl
AC_PROVIDE([$0])dnl
dnl
dnl The ugly bit...
dnl
AC_MSG_CHECKING([for CC])
dnl Don't bother with cache.
test -z "$CC" && CC=`egrep '^CC *=' ../Makefile | tail -1 | sed 's/^CC *= *//'`
test -z "$CC" && CC=cc
AC_MSG_RESULT(setting CC to $CC)
AC_SUBST(CC)
dnl
dnl
# Find out if we are using GNU C, under whatever name.
cat > conftest.c <<EOF
#ifdef __GNUC__
  yes
#endif
EOF
${CC-cc} -E conftest.c > conftest.out 2>&1
if egrep yes conftest.out >/dev/null 2>&1; then
  GCC=yes
else
  GCC=
fi
rm -f conftest*
])dnl
