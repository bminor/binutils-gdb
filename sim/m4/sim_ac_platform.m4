dnl   Copyright (C) 1997-2021 Free Software Foundation, Inc.
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.
dnl
dnl Check for various platform settings.
AC_DEFUN([SIM_AC_PLATFORM],
[dnl
dnl Check for common headers.
dnl NB: You can assume C11 headers exist.
AC_CHECK_HEADERS_ONCE(m4_flatten([
  dlfcn.h
  fcntl.h
  fpu_control.h
  termios.h
  unistd.h
  utime.h
  linux/if_tun.h
  linux/mii.h
  linux/types.h
  net/if.h
  netinet/in.h
  netinet/tcp.h
  sys/ioctl.h
  sys/mman.h
  sys/mount.h
  sys/param.h
  sys/resource.h
  sys/socket.h
  sys/stat.h
  sys/statfs.h
  sys/termio.h
  sys/termios.h
  sys/types.h
  sys/vfs.h
]))
AC_HEADER_DIRENT

AC_CHECK_FUNCS_ONCE(m4_flatten([
  __setfpucw
  access
  aint
  anint
  cfgetispeed
  cfgetospeed
  cfsetispeed
  cfsetospeed
  chdir
  chmod
  dup
  dup2
  execv
  execve
  fcntl
  fork
  fstat
  fstatfs
  ftruncate
  getdirentries
  getegid
  geteuid
  getgid
  getpid
  getppid
  getrusage
  gettimeofday
  getuid
  ioctl
  kill
  link
  lseek
  mmap
  munmap
  pipe
  posix_fallocate
  pread
  rmdir
  setregid
  setreuid
  setgid
  setuid
  sigaction
  sigprocmask
  sqrt
  stat
  strsignal
  symlink
  tcdrain
  tcflow
  tcflush
  tcgetattr
  tcgetpgrp
  tcsendbreak
  tcsetattr
  tcsetpgrp
  time
  truncate
  umask
  unlink
  utime
]))

AC_STRUCT_ST_BLKSIZE
AC_STRUCT_ST_BLOCKS
AC_STRUCT_ST_RDEV
AC_STRUCT_TIMEZONE

AC_CHECK_MEMBERS([[struct stat.st_dev], [struct stat.st_ino],
[struct stat.st_mode], [struct stat.st_nlink], [struct stat.st_uid],
[struct stat.st_gid], [struct stat.st_rdev], [struct stat.st_size],
[struct stat.st_blksize], [struct stat.st_blocks], [struct stat.st_atime],
[struct stat.st_mtime], [struct stat.st_ctime]], [], [],
[[#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif]])

AC_CHECK_TYPES([__int128])
AC_CHECK_TYPES(socklen_t, [], [],
[#include <sys/types.h>
#include <sys/socket.h>
])

dnl Types used by common code
AC_TYPE_GETGROUPS
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T
AC_TYPE_UID_T

dnl We don't use gettext, but bfd does.  So we do the appropriate checks
dnl to see if there are intl libraries we should link against.
ALL_LINGUAS=
ZW_GNU_GETTEXT_SISTER_DIR

dnl BFD conditionally uses zlib, so we must link it in if libbfd does, by
dnl using the same condition.
AM_ZLIB

dnl BFD uses libdl when when plugins enabled.
AC_PLUGINS
AM_CONDITIONAL(PLUGINS, test "$plugins" = yes)
LT_INIT([dlopen])
AC_SUBST(lt_cv_dlopen_libs)

dnl Libraries.
AC_CHECK_LIB(socket, bind)
AC_CHECK_LIB(nsl, gethostbyname)
AC_CHECK_LIB(m, fabs)
AC_CHECK_LIB(m, log2)

PKG_CHECK_MODULES(SDL, sdl, [dnl
  AC_CHECK_LIB(dl, dlopen, [dnl
    SDL_CFLAGS="${SDL_CFLAGS} -DHAVE_SDL"
    SDL_LIBS="-ldl"
    ], [SDL_CFLAGS= SDL_LIBS=])
  ], [:])
AC_SUBST(SDL_CFLAGS)
AC_SUBST(SDL_LIBS)

dnl In the Cygwin environment, we need some additional flags.
AC_CACHE_CHECK([for cygwin], sim_cv_os_cygwin,
[AC_EGREP_CPP(lose, [
#ifdef __CYGWIN__
lose
#endif],[sim_cv_os_cygwin=yes],[sim_cv_os_cygwin=no])])

dnl Keep in sync with gdb's configure.ac list.
AC_SEARCH_LIBS(tgetent, [termcap tinfo curses ncurses],
  [TERMCAP_LIB=$ac_cv_search_tgetent], [TERMCAP_LIB=""])
if test x$sim_cv_os_cygwin = xyes; then
  TERMCAP_LIB="${TERMCAP_LIB} -luser32"
fi
AC_SUBST(TERMCAP_LIB)

dnl We prefer the in-tree readline.  Top-level dependencies make sure
dnl src/readline (if it's there) is configured before src/sim.
if test -r ../readline/Makefile; then
  READLINE_LIB=../../readline/readline/libreadline.a
  READLINE_CFLAGS='-I$(READLINE_SRC)/..'
else
  AC_CHECK_LIB(readline, readline, READLINE_LIB=-lreadline,
	       AC_ERROR([the required "readline" library is missing]), $TERMCAP_LIB)
  READLINE_CFLAGS=
fi
AC_SUBST(READLINE_LIB)
AC_SUBST(READLINE_CFLAGS)
])
