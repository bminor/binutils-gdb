/* Testsuite-internal API to libctf.
   Copyright (C) 2025 Free Software Foundation, Inc.

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

/* This header file defines the interfaces available from the CTF debugger
   library, libctf.  This API can be used by a debugger to operate on data in
   the Compact ANSI-C Type Format (CTF).  */

#ifndef	_CTF_TEST_API_H
#define	_CTF_TEST_API_H

#include <ctf-api.h>

extern ctf_dict_t *ctf_simple_open (const char *ctfsect, size_t ctfsect_size,
				    const char *symsect, size_t symsect_size,
				    size_t symsect_entsize,
				    const char *strsect, size_t strsect_size,
				    ctf_error_t *errp);

#endif				/* _CTF_TEST_API_H */
