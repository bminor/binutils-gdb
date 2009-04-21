AC_PREREQ(2.59)
AC_INIT(refcard.tex)
sinclude(../../config/acx.m4)
AC_PROG_INSTALL
AC_PROG_LN_S

# Flags needed to enable html installing and building
AC_ARG_WITH(datarootdir,
AC_HELP_STRING([--with-datarootdir], [use datarootdir as the data root directory.]),
[datarootdir="\${prefix}/${withval}"],
[datarootdir="\${prefix}/share"])

AC_ARG_WITH(docdir,
AC_HELP_STRING([--with-docdir], [install documentation in this directory.]),
[docdir="\${prefix}/${withval}"],
[docdir="\${datarootdir}/doc"])

AC_ARG_WITH(pdfdir,
AC_HELP_STRING([--with-pdfdir], [install pdf in this directory.]),
[pdfdir="\${prefix}/${withval}"],
[pdfdir="\${docdir}"])

AC_ARG_WITH(htmldir,
AC_HELP_STRING([--with-htmldir], [install html in this directory.]),
[htmldir="\${prefix}/${withval}"],
[htmldir="\${docdir}"])

AC_SUBST(datarootdir)
AC_SUBST(docdir)
AC_SUBST(htmldir)
AC_SUBST(pdfdir)

ACX_PKGVERSION([GDB])
ACX_BUGURL([http://www.gnu.org/software/gdb/bugs/])
AC_OUTPUT(Makefile)
