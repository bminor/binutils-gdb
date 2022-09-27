dnl SFRAME_CHECK_HAVE_SFRAME_AS
dnl ----------------------
dnl Defines:
dnl sframe_cv_have_sframe_as
dnl

# Check whether the assembler supports generation of SFrame
# unwind information.
AC_DEFUN([SFRAME_CHECK_HAVE_SFRAME_AS],
[
  AC_CACHE_CHECK([if as supports --gsframe], sframe_cv_have_sframe_as,
  [
    ac_save_CFLAGS="$CFLAGS"
    CFLAGS="$CFLAGS -Wa,--gsframe"
    AC_MSG_CHECKING([for as that supports --gsframe])
    AC_TRY_COMPILE([], [return 0;], [sframe_cv_have_sframe_as=yes], [sframe_cv_have_sframe_as=no])
  ])
  AC_MSG_RESULT($sframe_cv_have_sframe_as)
  CFLAGS="$ac_save_CFLAGS"
])
