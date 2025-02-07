/* CTF ref system.
   Copyright (C) 2019-2025 Free Software Foundation, Inc.

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

#ifndef	_CTF_REF_H
#define	_CTF_REF_H

#include "ctf-impl.h"

/* This is in a separate header because nothing but ctf-string.c and
   ctf-serialize.c should use functions herein (and ctf-util.c, which defines
   them).  */

typedef struct ctf_ref
{
  ctf_list_t cre_list;		/* List forward/back pointers.  */
  uint32_t *cre_ref;		/* A single ref to this string.  */
} ctf_ref_t;

extern uint32_t ctf_str_add_ref (ctf_dict_t *, const char *, uint32_t *ref);
extern uint32_t ctf_str_add_no_dedup_ref (ctf_dict_t *, const char *,
					  uint32_t *ref);

extern ctf_ref_t *ctf_create_ref (ctf_dict_t *, ctf_list_t *, uint32_t *ref);
extern void ctf_purge_ref_list (ctf_dict_t *, ctf_list_t *);
extern void ctf_update_refs (ctf_list_t *, uint32_t value);

#endif /* _CTF_REF_H */
