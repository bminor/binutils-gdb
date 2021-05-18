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
  sys/param.h
  sys/resource.h
  sys/select.h
  sys/socket.h
  sys/stat.h
]))

AC_CHECK_FUNCS_ONCE(m4_flatten([
  __setfpucw
  aint
  anint
  chmod
  execv
  execve
  fork
  ftruncate
  geteuid
  getegid
  getgid
  getrusage
  getuid
  kill
  lstat
  mmap
  munmap
  posix_fallocate
  pread
  setuid
  setgid
  sigaction
  sqrt
  strsignal
  time
  truncate
  utime
]))

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
AC_TYPE_SIGNAL
])
