/* Declarations for missing functions.
   Copyright (C) 2019 Free Software Foundation, Inc.

   This file is part of libctf.

   libctf is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not see
   <http://www.gnu.org/licenses/>.  */

#ifndef _CTF_DECLS_H
#define _CTF_DECLS_H

#include "config.h"

#if !HAVE_DECL_QSORT_R
#include <stddef.h>
void qsort_r (void *base, size_t nmemb, size_t size,
	      int (*compar)(const void *, const void *, void *),
	      void *arg);
#endif /* !HAVE_DECL_QSORT_R */

#undef MAX
#undef MIN
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#endif /* _CTF_DECLS_H */
