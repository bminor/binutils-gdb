/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

BFD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BFD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* $Id$ */

/* xoxorich. coelesced from binutils.
 *
 * Last Mod Mon Feb 18 14:49:51 PST 1991, by rich@cygint.cygnus.com
 */

#ifndef MISC_H
#define MISC_H 1

#include "ranlib.h"

#ifdef USG
#include <string.h>
#else
#include <strings.h>
#endif /* USG */

#ifdef never
#ifdef LOCKS
#undef LOCKS
#endif /* LOCKS */
#endif /* never */

 /* used for masking system io calls into stdio. */

/* the name, ie, argv[0], of this program. */

extern char *program_name;

/* Current file's name */

extern char *input_name;

/* Current member's name, or 0 if processing a non-library file.  */

extern char *input_member;

/* Report an error using the message for the last failed system call,
   followed by the string NAME.  */

#define perror_name(name)	perror(concat(program_name, ": error on ", name))
#define pfatal_with_name(name)	{perror_name(name);exit(-1);}

#ifdef __STDC__

extern char *concat(char *a, char *b, char *c);
extern void *xmalloc(unsigned int size);
extern void * xrealloc(char *ptr, int size);
extern void error(char *string, char *arg1, char *arg2, char *arg3);
extern void error_with_file(char *string);
extern void fatal(char *string, char*a1, char*a2, char*a3);
extern void print_file_name(FILE *outfile);
extern void swap_symdef_table(struct symdef *sym, int count);
#else
extern char *alloca();
extern char *concat();
extern void * xmalloc();
extern void *xrealloc();
extern void error();
extern void error_with_file();
extern void fatal();
extern void print_file_name();
extern void swap_symdef_table();
#endif /* __STDC__ */

#endif /* MISC_H */

/*
 * Local Variables:
 * comment-column: 0
 * End:
 */

/* end of misc.h */
