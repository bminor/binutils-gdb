/* ldmisc.h -
   Copyright 1991, 1992 Free Software Foundation, Inc.

   This file is part of GLD, the Gnu Linker.

   GLD is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GLD is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GLD; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef LDMISC_H
#define LDMISC_H

/* VARARGS*/
extern void einfo ();
/* VARARGS*/
extern void minfo ();
/* VARARGS*/
extern void info_msg ();
extern void info_assert PARAMS ((char *, unsigned int));
extern void multiple_warn PARAMS ((char *message1, asymbol *sym,
				   char *message2, asymbol *sy));
extern void yyerror PARAMS ((char *));
extern char *concat PARAMS ((CONST char *, CONST char *, CONST char *));
extern PTR ldmalloc PARAMS ((size_t));
extern PTR ldrealloc PARAMS ((PTR, size_t));
extern char *buystring PARAMS ((CONST char *CONST));

#define ASSERT(x) \
do { if (!(x)) info_assert(__FILE__,__LINE__); } while (0)

#define FAIL() \
do { info_assert(__FILE__,__LINE__); } while (0)

extern void print_space PARAMS ((void));
extern void print_nl PARAMS ((void));
extern void print_address PARAMS ((bfd_vma value));

#endif
