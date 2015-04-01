dnl A function to check if the system's zlib library should be used.  The
dnl builtin zlib dnl is used by default unless the user configured with
dnl --with-system-zlib.

AC_DEFUN([AM_ZLIB],
[
  # Use the system's zlib library.
  zlibdir=-L../zlib
  zlibinc="-I\$(srcdir)/../zlib"
  AC_ARG_WITH(system-zlib,
  [AS_HELP_STRING([--with-system-zlib], [use installed libz])],
  if test x$with_system_zlib = xyes ; then
    zlibdir=
    zlibinc=
  fi
  )
  AC_SUBST(zlibdir)
  AC_SUBST(zlibinc)
])
