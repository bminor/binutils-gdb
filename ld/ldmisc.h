/* ldmisc.h -

   Copyright (C) 1991 Free Software Foundation, Inc.

   This file is part of GLD, the Gnu Linker.

   GLD is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   GLD is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GLD; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */



/* VARARGS*/
PROTO(void,info,());
PROTO(void,info_assert,(char *, unsigned int));
PROTO(void,yyerror,(char *));
PROTO(char *,concat,(char *, char *, char *));
PROTO(char *, ldmalloc,(size_t));
PROTO(char *,buystring,(char *));
#define ASSERT(x) \
{ if (!(x)) info_assert(__FILE__,__LINE__); }

#define FAIL() \
{ info_assert(__FILE__,__LINE__); }
