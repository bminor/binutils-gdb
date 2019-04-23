/* Implementation header.
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

#ifndef	_CTF_IMPL_H
#define	_CTF_IMPL_H

#include "config.h"
#include <sys/errno.h>
#include <ctf-api.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>
#include <elf.h>

#ifdef	__cplusplus
extern "C"
  {
#endif

/* Compiler attributes.  */

#if defined (__GNUC__)

/* GCC.  We assume that all compilers claiming to be GCC support sufficiently
   many GCC attributes that the code below works.  If some non-GCC compilers
   masquerading as GCC in fact do not implement these attributes, version checks
   may be required.  */

/* We use the _libctf_*_ pattern to avoid clashes with any future attribute
   macros glibc may introduce, which have names of the pattern
   __attribute_blah__.  */

#define _libctf_printflike_(string_index,first_to_check) \
    __attribute__ ((__format__ (__printf__, (string_index), (first_to_check))))
#define _libctf_unlikely_(x) __builtin_expect ((x), 0)
#define _libctf_unused_ __attribute__ ((__unused__))
#define _libctf_malloc_ __attribute__((__malloc__))

#endif

typedef struct ctf_list
{
  struct ctf_list *l_prev;	/* Previous pointer or tail pointer.  */
  struct ctf_list *l_next;	/* Next pointer or head pointer.  */
} ctf_list_t;

#define	ctf_list_prev(elem)	((void *)(((ctf_list_t *)(elem))->l_prev))
#define	ctf_list_next(elem)	((void *)(((ctf_list_t *)(elem))->l_next))

extern void ctf_list_append (ctf_list_t *, void *);
extern void ctf_list_prepend (ctf_list_t *, void *);
extern void ctf_list_delete (ctf_list_t *, void *);

extern const char *ctf_strraw (ctf_file_t *, uint32_t);
extern const char *ctf_strptr (ctf_file_t *, uint32_t);

extern void *ctf_set_open_errno (int *, int);
extern long ctf_set_errno (ctf_file_t *, int);

_libctf_malloc_
extern void *ctf_data_alloc (size_t);
extern void ctf_data_free (void *, size_t);
extern void ctf_data_protect (void *, size_t);

_libctf_malloc_
extern void *ctf_mmap (size_t length, size_t offset, int fd);
extern void ctf_munmap (void *, size_t);
extern ssize_t ctf_pread (int fd, void *buf, ssize_t count, off_t offset);

_libctf_malloc_
extern void *ctf_alloc (size_t);
extern void ctf_free (void *);

_libctf_malloc_
extern char *ctf_strdup (const char *);
extern char *ctf_str_append (char *, const char *);
extern const char *ctf_strerror (int);

_libctf_printflike_ (1, 2)
extern void ctf_dprintf (const char *, ...);
extern void libctf_init_debug (void);

extern Elf64_Sym *ctf_sym_to_elf64 (const Elf32_Sym *src, Elf64_Sym *dst);

extern int _libctf_debug;	/* debugging messages enabled */

#ifdef	__cplusplus
}
#endif

#endif /* _CTF_IMPL_H */
