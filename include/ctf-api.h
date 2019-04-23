/* Public API to libctf.
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

/* This header file defines the interfaces available from the CTF debugger
   library, libctf.  This API can be used by a debugger to operate on data in
   the Compact ANSI-C Type Format (CTF).  */

#ifndef	_CTF_API_H
#define	_CTF_API_H

#include <sys/param.h>
#include <sys/types.h>
#include <ctf.h>

#ifdef	__cplusplus
extern "C"
  {
#endif

/* Clients can open one or more CTF containers and obtain a pointer to an
   opaque ctf_file_t.  Types are identified by an opaque ctf_id_t token.
   They can also open or create read-only archives of CTF containers in a
   ctf_archive_t.

   These opaque definitions allow libctf to evolve without breaking clients.  */

typedef struct ctf_file ctf_file_t;
typedef struct ctf_archive_internal ctf_archive_t;
typedef long ctf_id_t;

/* Functions that return integer status or a ctf_id_t use the following value
   to indicate failure.  ctf_errno() can be used to obtain an error code.  */
#define	CTF_ERR	(-1L)

#define	ECTF_BASE	1000	/* Base value for libctf errnos.  */


enum
  {
   ECTF_FMT = ECTF_BASE,	/* File is not in CTF or ELF format.  */
   ECTF_BFDERR,			/* BFD error.  */
   ECTF_CTFVERS,		/* CTF version is more recent than libctf.  */
   ECTF_BFD_AMBIGUOUS,		/* Ambiguous BFD target.  */
   ECTF_SYMTAB,			/* Symbol table uses invalid entry size.  */
   ECTF_SYMBAD,			/* Symbol table data buffer invalid.  */
   ECTF_STRBAD,			/* String table data buffer invalid.  */
   ECTF_CORRUPT,		/* File data corruption detected.  */
   ECTF_NOCTFDATA,		/* ELF file does not contain CTF data.  */
   ECTF_NOCTFBUF,		/* Buffer does not contain CTF data.  */
   ECTF_NOSYMTAB,		/* Symbol table data is not available.  */
   ECTF_NOPARENT,		/* Parent CTF container is not available.  */
   ECTF_DMODEL,			/* Data model mismatch.  */
   ECTF_UNUSED,			/* Unused error.  */
   ECTF_ZALLOC,			/* Failed to allocate (de)compression buffer.  */
   ECTF_DECOMPRESS,		/* Failed to decompress CTF data.  */
   ECTF_STRTAB,			/* String table for this string is missing.  */
   ECTF_BADNAME,		/* String offset is corrupt w.r.t. strtab.  */
   ECTF_BADID,			/* Invalid type ID number.  */
   ECTF_NOTSOU,			/* Type is not a struct or union.  */
   ECTF_NOTENUM,		/* Type is not an enum.  */
   ECTF_NOTSUE,			/* Type is not a struct, union, or enum.  */
   ECTF_NOTINTFP,		/* Type is not an integer, float, or enum.  */
   ECTF_NOTARRAY,		/* Type is not an array.  */
   ECTF_NOTREF,			/* Type does not reference another type.  */
   ECTF_NAMELEN,		/* Buffer is too small to hold type name.  */
   ECTF_NOTYPE,			/* No type found corresponding to name.  */
   ECTF_SYNTAX,			/* Syntax error in type name.  */
   ECTF_NOTFUNC,		/* Symtab entry does not refer to a function.  */
   ECTF_NOFUNCDAT,		/* No func info available for function.  */
   ECTF_NOTDATA,		/* Symtab entry does not refer to a data obj.  */
   ECTF_NOTYPEDAT,		/* No type info available for object.  */
   ECTF_NOLABEL,		/* No label found corresponding to name.  */
   ECTF_NOLABELDATA,		/* File does not contain any labels.  */
   ECTF_NOTSUP,			/* Feature not supported.  */
   ECTF_NOENUMNAM,		/* Enum element name not found.  */
   ECTF_NOMEMBNAM,		/* Member name not found.  */
   ECTF_RDONLY,			/* CTF container is read-only.  */
   ECTF_DTFULL,			/* CTF type is full (no more members allowed).  */
   ECTF_FULL,			/* CTF container is full.  */
   ECTF_DUPLICATE,		/* Duplicate member or variable name.  */
   ECTF_CONFLICT,		/* Conflicting type definition present.  */
   ECTF_OVERROLLBACK,		/* Attempt to roll back past a ctf_update.  */
   ECTF_COMPRESS,		/* Failed to compress CTF data.  */
   ECTF_ARCREATE,		/* Error creating CTF archive.  */
   ECTF_ARNNAME,		/* Name not found in CTF archive.  */
   ECTF_SLICEOVERFLOW,		/* Overflow of type bitness or offset in slice.  */
   ECTF_DUMPSECTUNKNOWN,	/* Unknown section number in dump.  */
   ECTF_DUMPSECTCHANGED		/* Section changed in middle of dump.  */
  };

/* The CTF data model is inferred to be the caller's data model or the data
   model of the given object, unless ctf_setmodel() is explicitly called.  */
#define	CTF_MODEL_ILP32 1	/* Object data model is ILP32.  */
#define	CTF_MODEL_LP64  2	/* Object data model is LP64.  */
#ifdef _LP64
# define CTF_MODEL_NATIVE CTF_MODEL_LP64
#else
# define CTF_MODEL_NATIVE CTF_MODEL_ILP32
#endif

/* Dynamic CTF containers can be created using ctf_create().  The ctf_add_*
   routines can be used to add new definitions to the dynamic container.
   New types are labeled as root or non-root to determine whether they are
   visible at the top-level program scope when subsequently doing a lookup.  */

#define	CTF_ADD_NONROOT	0	/* Type only visible in nested scope.  */
#define	CTF_ADD_ROOT	1	/* Type visible at top-level scope.  */

extern int ctf_errno (ctf_file_t *);
extern const char *ctf_errmsg (int);

extern void ctf_setdebug (int debug);
extern int ctf_getdebug (void);

#ifdef	__cplusplus
}
#endif

#endif				/* _CTF_API_H */
