/* Inline functions.
   Copyright (C) 2020 Free Software Foundation, Inc.

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

#ifndef	_CTF_INLINES_H
#define	_CTF_INLINES_H

#ifdef	__cplusplus
extern "C"
{
#endif

#include "config.h"

#ifndef _libctf_malloc_
#error "ctf-inlines.h" should not be included directly: include "ctf-impl.h".
#endif


static inline int
ctf_dynhash_cinsert (ctf_dynhash_t *h, const void *k, const void *v)
{
  return ctf_dynhash_insert (h, (void *) k, (void *) v);
}

#ifdef	__cplusplus
}
#endif

#endif /* _CTF_INLINES_H */
