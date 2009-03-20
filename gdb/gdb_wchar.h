/* Wide characters for gdb
   Copyright (C) 2009 Free Software Foundation, Inc.

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

#ifndef GDB_WCHAR_H
#define GDB_WCHAR_H

/* If this host has wchar_t and if iconv is available (perhaps via GNU
   libiconv), then we arrange to use those.  Otherwise, we provide a
   phony iconv which only handles a single character set, and we
   provide wrappers for the wchar_t functionality we use.  */
#if defined(HAVE_ICONV) && defined(HAVE_WCHAR_H)

#include <iconv.h>
#include <wchar.h>
#include <wctype.h>

typedef wchar_t gdb_wchar_t;
typedef wint_t gdb_wint_t;

#define gdb_wcslen wcslen
#define gdb_iswprint iswprint
#define gdb_iswdigit iswdigit
#define gdb_btowc btowc
#define gdb_WEOF WEOF

#define LCST(X) L ## X

#else

typedef char gdb_wchar_t;
typedef int gdb_wint_t;

#define gdb_wcslen strlen
#define gdb_iswprint isprint
#define gdb_iswdigit isdigit
#define gdb_btowc /* empty */
#define gdb_WEOF EOF

#define LCST(X) X

/* This define is used elsewhere so we don't need to duplicate the
   same checking logic in multiple places.  */
#define PHONY_ICONV

#endif /* defined(HAVE_ICONV) && defined(HAVE_WCHAR_H) */

#endif /* GDB_WCHAR_H */
