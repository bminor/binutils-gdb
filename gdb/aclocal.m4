AC_DEFUN(CYGNUS_PATH_TK, [
#
# Ok, lets find the tk source trees so we can use the headers
# If the directory (presumably symlink) named "tk" exists, use that one
# in preference to any others.  Same logic is used when choosing library
# and again with Tcl.
#
AC_CHECKING(for Tk source directory)
TKHDIR=""
for i in `ls -d ${srcdir}/../tk* 2>/dev/null` ${srcdir}/../tk ; do
  if test -f $i/tk.h ; then
    TKHDIR="-I$i"
  fi
done
# if we can't find it, see if one is installed
if test x"$TKHDIR" = x ; then
  installed=0
  if test -f $prefix/include/tk.h; then
    installed=1 TKHDIR="-I$prefix/include"
  else
  AC_HEADER_CHECK(tk.h, installed=1)
  fi
  if test $installed -eq 0 ; then
    TKHDIR="# no Tk directory found"
    AC_MSG_WARN(Can't find Tk directory)
  fi
fi
if test x"$TKHDIR" != x ; then
  AC_MSG_RESULT(Setting TKHDIR to be $i)
fi

#
# Ok, lets find the tk library
# First, look for one uninstalled.  
#
TKLIB=""
AC_CHECKING(for Tk library)
for i in `ls -d ../tk* 2>/dev/null` ../tk ; do
  if test -f "$i/Makefile" ; then
    TKLIB=$i/libtk.a
  fi
done    
# If not found, look for installed version
if test x"$TKLIB" = x ; then
dnl This doesn't work because of unresolved symbols.
dnl  AC_HAVE_LIBRARY(libtk.a, installed=1, installed=0)
  if test -f $prefix/lib/libtk.a; then
    installed=1
  else
    installed=0
  fi
  if test $installed -eq 1 ; then
    TKLIB="-ltk"
  fi
fi

# If still not found, assume Tk simply hasn't been built yet
if test x"$TKLIB" = x ; then
  for i in `ls -d ../tk* 2>/dev/null` ../tk ; do
    if test -f "$i/tk.h" ; then
      TKLIB=$i/libtk.a
  fi
  done    
fi

if test x"$TKLIB" = x ; then
    TKLIB="# no Tk library found"
    AC_MSG_WARN(Can't find Tk library)
else
    AC_MSG_RESULT(setting TKLIB to be $TKLIB)
    no_tk=
fi

AC_SUBST(TKHDIR)
AC_SUBST(TKLIB)
])


AC_DEFUN(CYGNUS_PATH_TCL, [
#
# Ok, lets find the tcl source trees so we can use the headers
#
# Warning: transition of version 9 to 10 will break this algorithm
# because 10 sorts before 9.
#
AC_CHECKING(for Tcl source directory)
TCLHDIR=""
for i in `ls -d ${srcdir}/../tcl* 2>/dev/null` ${srcdir}/../tcl ; do
  if test -f $i/tclInt.h ; then
    TCLHDIR="-I$i"
  fi
done
# if we can't find it, see if one is installed
if test x"$TCLHDIR" = x ; then
  installed=0
  if test -f $prefix/include/tclInt.h; then
    installed=1 TCLHDIR="-I$prefix/include"
  else
  AC_HEADER_CHECK(tclInt.h, installed=1)
  fi
  if test $installed -eq 0 ; then
    TCLHDIR="# no Tcl directory found"
    AC_MSG_WARN(Can't find Tcl directory)
  fi
else
  AC_MSG_RESULT(setting TCLHDIR to be $i)
fi

#
# Ok, lets find the tcl library
# First, look for the latest uninstalled
#
TCLLIB=""
AC_CHECKING(for Tcl library)
for i in `ls -d ../tcl* 2>/dev/null` ../tcl ; do
  if test -f "$i/Makefile" ; then
    TCLLIB=$i/libtcl.a
  fi
done    
# If not found, look for installed version
if test x"$TCLLIB" = x ; then
dnl Don't use this, since we can't use it for libtk.a.
dnl  AC_HAVE_LIBRARY(libtcl.a, installed=1, installed=0)
  if test -f $prefix/lib/libtcl.a; then installed=1; else installed=0; fi
  if test $installed -eq 1 ; then
    TCLLIB="-ltcl"
  fi
fi
# If still not found, assume Tcl simply hasn't been built yet
if test x"$TCLLIB" = x ; then
  for i in `ls -d ../tcl* 2>/dev/null` ../tcl ; do
    if test -f "$i/tcl.h" ; then
      TCLLIB=$i/libtcl.a
  fi
  done    
fi

if test x"$TCLLIB" = x ; then
    TCLLIB="# no Tcl library found"
    AC_MSG_WARN(Can't find Tcl library)
else
    AC_MSG_RESULT(setting TCLLIB to be $TCLLIB)
fi

AC_SUBST(TCLHDIR)
AC_SUBST(TCLLIB)
])