sinclude(../bfd/warning.m4)
sinclude(../config/gettext-sister.m4)
sinclude(../config/nls.m4)
sinclude(../config/po.m4)
sinclude(../config/progtest.m4)

dnl See whether we need to use fopen-bin.h rather than fopen-same.h.
AC_DEFUN([BFD_BINARY_FOPEN],
[AC_REQUIRE([AC_CANONICAL_TARGET])
case "${host}" in
changequote(,)dnl
*-*-msdos* | *-*-go32* | *-*-mingw32* | *-*-cygwin* | *-*-windows*)
changequote([,])dnl
  AC_DEFINE(USE_BINARY_FOPEN, 1, [Use b modifier when opening binary files?]) ;;
esac])dnl

dnl GAS_CHECK_DECL_NEEDED(name, typedefname, typedef, headers)
AC_DEFUN([GAS_CHECK_DECL_NEEDED],[
AC_MSG_CHECKING(whether declaration is required for $1)
AC_CACHE_VAL(gas_cv_decl_needed_$1,
AC_TRY_LINK([$4],
[
typedef $3;
$2 x;
x = ($2) $1;
], gas_cv_decl_needed_$1=no, gas_cv_decl_needed_$1=yes))dnl
AC_MSG_RESULT($gas_cv_decl_needed_$1)
if test $gas_cv_decl_needed_$1 = yes; then
 AC_DEFINE([NEED_DECLARATION_]translit($1, [a-z], [A-Z]), 1,
	   [Define if $1 is not declared in system header files.])
fi
])dnl
dnl
dnl Some non-ANSI preprocessors botch requoting inside strings.  That's bad
dnl enough, but on some of those systems, the assert macro relies on requoting
dnl working properly!
dnl GAS_WORKING_ASSERT
AC_DEFUN([GAS_WORKING_ASSERT],
[AC_MSG_CHECKING([for working assert macro])
AC_CACHE_VAL(gas_cv_assert_ok,
AC_TRY_LINK([#include <assert.h>
#include <stdio.h>], [
/* check for requoting problems */
static int a, b, c, d;
static char *s;
assert (!strcmp(s, "foo bar baz quux"));
/* check for newline handling */
assert (a == b
        || c == d);
], gas_cv_assert_ok=yes, gas_cv_assert_ok=no))dnl
AC_MSG_RESULT($gas_cv_assert_ok)
test $gas_cv_assert_ok = yes || AC_DEFINE(BROKEN_ASSERT, 1, [assert broken?])
])dnl
dnl
dnl Since many Bourne shell implementations lack subroutines, use this
dnl hack to simplify the code in configure.in.
dnl GAS_UNIQ(listvar)
AC_DEFUN([GAS_UNIQ],
[_gas_uniq_list="[$]$1"
_gas_uniq_newlist=""
dnl Protect against empty input list.
for _gas_uniq_i in _gas_uniq_dummy [$]_gas_uniq_list ; do
  case [$]_gas_uniq_i in
  _gas_uniq_dummy) ;;
  *) case " [$]_gas_uniq_newlist " in
       *" [$]_gas_uniq_i "*) ;;
       *) _gas_uniq_newlist="[$]_gas_uniq_newlist [$]_gas_uniq_i" ;;
     esac ;;
  esac
done
$1=[$]_gas_uniq_newlist
])dnl

sinclude(../libtool.m4)
dnl The lines below arrange for aclocal not to bring libtool.m4
dnl AM_PROG_LIBTOOL into aclocal.m4, while still arranging for automake
dnl to add a definition of LIBTOOL to Makefile.in.
ifelse(yes,no,[
AC_DEFUN([AM_PROG_LIBTOOL],)
AC_DEFUN([AC_CHECK_LIBM],)
AC_SUBST(LIBTOOL)
])
