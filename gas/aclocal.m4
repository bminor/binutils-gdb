dnl GAS_CHECK_DECL_NEEDED(name, typedefname, typedef, headers)
AC_DEFUN(GAS_CHECK_DECL_NEEDED,[
AC_MSG_CHECKING(whether declaration is required for $1)
AC_CACHE_VAL(gas_cv_decl_needed_$1,
AC_TRY_LINK([$4],
[
typedef $3;
$2 x;
x = ($2) $1;
], gas_cv_decl_needed_$1=no, gas_cv_decl_needed_$1=yes))dnl
AC_MSG_RESULT($gas_cv_decl_needed_$1)
test $gas_cv_decl_needed_$1 = no || {
 ifelse(index($1,[$]),-1,
    [AC_DEFINE([NEED_DECLARATION_]translit($1, [a-z], [A-Z]))],
    [gas_decl_name_upcase=`echo $1 | tr '[a-z]' '[A-Z]'`
     AC_DEFINE_UNQUOTED(NEED_DECLARATION_$gas_decl_name_upcase)])
}
])dnl
dnl
dnl Some non-ANSI preprocessors botch requoting inside strings.  That's bad
dnl enough, but on some of those systems, the assert macro relies on requoting
dnl working properly!
dnl GAS_WORKING_ASSERT
AC_DEFUN(GAS_WORKING_ASSERT,
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
test $gas_cv_assert_ok = yes || AC_DEFINE(BROKEN_ASSERT)
])dnl
dnl
dnl Since many Bourne shell implementations lack subroutines, use this
dnl hack to simplify the code in configure.in.
dnl GAS_UNIQ(listvar)
AC_DEFUN(GAS_UNIQ,
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

# Check to see if we're running under Cygwin32, without using
# AC_CANONICAL_*.  If so, set output variable CYGWIN32 to "yes".
# Otherwise set it to "no".

AC_DEFUN(CY_CYGWIN32,
[AC_CACHE_CHECK(for Cygwin32 environment, am_cv_cygwin32,
[AC_TRY_COMPILE(,[int main () { return __CYGWIN32__; }],
am_cv_cygwin32=yes, am_cv_cygwin32=no)
rm -f conftest*])
CYGWIN32=
test "$am_cv_cygwin32" = yes && CYGWIN32=yes])
# Check to see if we're running under Win32, without using
# AC_CANONICAL_*.  If so, set output variable EXEEXT to ".exe".
# Otherwise set it to "".

dnl This knows we add .exe if we're building in the Cygwin32
dnl environment. But if we're not, then it compiles a test program
dnl to see if there is a suffix for executables.
AC_DEFUN(CY_EXEEXT,
dnl AC_REQUIRE([AC_PROG_CC])AC_REQUIRE([CY_CYGWIN32])
AC_MSG_CHECKING([for executable suffix])
[AC_CACHE_VAL(am_cv_exeext,
[if test "$CYGWIN32" = yes; then
am_cv_exeext=.exe
else
cat > am_c_test.c << 'EOF'
int main() {
/* Nothing needed here */
}
EOF
${CC-cc} -o am_c_test $CFLAGS $CPPFLAGS $LDFLAGS am_c_test.c $LIBS 1>&5
am_cv_exeext=`ls am_c_test.* | grep -v am_c_test.c | sed -e s/am_c_test//`
rm -f am_c_test*])
test x"${am_cv_exeext}" = x && am_cv_exeext=no
fi
EXEEXT=""
test x"${am_cv_exeext}" != xno && EXEEXT=${am_cv_exeext}
AC_MSG_RESULT(${am_cv_exeext})
AC_SUBST(EXEEXT)])
