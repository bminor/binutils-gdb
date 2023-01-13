/* Common definitions.

   Copyright (C) 1986-2020 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef COMMON_COMMON_DEFS_H
#define COMMON_COMMON_DEFS_H

#ifdef GDBSERVER

#include <build-gnulib-gdbserver/config.h>

#undef PACKAGE_NAME
#undef PACKAGE
#undef PACKAGE_VERSION
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME

#include <config.h>

#else  /* GDBSERVER */

#include <gdbsupport/config.h>

#undef PACKAGE_NAME
#undef PACKAGE
#undef PACKAGE_VERSION
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME

#include "gnulib/config.h"

#endif	/* GDBSERVER */

/* From:
    https://www.gnu.org/software/gnulib/manual/html_node/stdint_002eh.html

   "On some hosts that predate C++11, when using C++ one must define
   __STDC_CONSTANT_MACROS to make visible the definitions of constant
   macros such as INTMAX_C, and one must define __STDC_LIMIT_MACROS to
   make visible the definitions of limit macros such as INTMAX_MAX.".

   And:
    https://www.gnu.org/software/gnulib/manual/html_node/inttypes_002eh.html

   "On some hosts that predate C++11, when using C++ one must define
   __STDC_FORMAT_MACROS to make visible the declarations of format
   macros such as PRIdMAX."

   Must do this before including any system header, since other system
   headers may include stdint.h/inttypes.h.  */
#define __STDC_CONSTANT_MACROS 1
#define __STDC_LIMIT_MACROS 1
#define __STDC_FORMAT_MACROS 1

/* Some distros enable _FORTIFY_SOURCE by default, which on occasion
   has caused build failures with -Wunused-result when a patch is
   developed on a distro that does not enable _FORTIFY_SOURCE.  We
   enable it here in order to try to catch these problems earlier;
   plus this seems like a reasonable safety measure.  The check for
   optimization is required because _FORTIFY_SOURCE only works when
   optimization is enabled.  If _FORTIFY_SOURCE is already defined,
   then we don't do anything.  Also, on MinGW, fortify requires
   linking to -lssp, and to avoid the hassle of checking for
   that and linking to it statically, we just don't define
   _FORTIFY_SOURCE there.  */

#if (!defined _FORTIFY_SOURCE && defined __OPTIMIZE__ && __OPTIMIZE__ > 0 \
     && !defined(__MINGW32__))
#define _FORTIFY_SOURCE 2
#endif

/* We don't support Windows versions before XP, so we define
   _WIN32_WINNT correspondingly to ensure the Windows API headers
   expose the required symbols.  */
#if defined (__MINGW32__) || defined (__CYGWIN__)
# ifdef _WIN32_WINNT
#  if _WIN32_WINNT < 0x0501
#   undef _WIN32_WINNT
#   define _WIN32_WINNT 0x0501
#  endif
# else
#  define _WIN32_WINNT 0x0501
# endif
#endif	/* __MINGW32__ || __CYGWIN__ */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>	/* for strcasecmp and strncasecmp */
#endif
#include <errno.h>
#include <alloca.h>

#include "ansidecl.h"
/* This is defined by ansidecl.h, but we prefer gnulib's version.  On
   MinGW, gnulib might enable __USE_MINGW_ANSI_STDIO, which may or not
   require use of attribute gnu_printf instead of printf.  gnulib
   checks that at configure time.  Since _GL_ATTRIBUTE_FORMAT_PRINTF
   is compatible with ATTRIBUTE_PRINTF, simply use it.  */
#undef ATTRIBUTE_PRINTF
#define ATTRIBUTE_PRINTF _GL_ATTRIBUTE_FORMAT_PRINTF

#if GCC_VERSION >= 3004
#define ATTRIBUTE_UNUSED_RESULT __attribute__ ((__warn_unused_result__))
#else
#define ATTRIBUTE_UNUSED_RESULT
#endif

#include "libiberty.h"
#include "pathmax.h"
#include "gdb/signals.h"
#include "gdb_locale.h"
#include "ptid.h"
#include "common-types.h"
#include "common-utils.h"
#include "gdb_assert.h"
#include "errors.h"
#include "print-utils.h"
#include "common-debug.h"
#include "cleanups.h"
#include "common-exceptions.h"
#include "gdbsupport/poison.h"

#define EXTERN_C extern "C"
#define EXTERN_C_PUSH extern "C" {
#define EXTERN_C_POP }

/* Pull in gdb::unique_xmalloc_ptr.  */
#include "gdbsupport/gdb_unique_ptr.h"

/* String containing the current directory (what getwd would return).  */
extern char *current_directory;

/* sbrk on macOS is not useful for our purposes, since sbrk(0) always
   returns the same value.  brk/sbrk on macOS is just an emulation
   that always returns a pointer to a 4MB section reserved for
   that.  */

#if defined (HAVE_SBRK) && !__APPLE__
#define HAVE_USEFUL_SBRK 1
#endif

#endif /* COMMON_COMMON_DEFS_H */
