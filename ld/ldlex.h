/* ldlex.h -
   Copyright 1991, 1992 Free Software Foundation, Inc.

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

#ifndef LDLEX_H
#define LDLEX_H

#include <stdio.h>

/* The initial parser states.  */
typedef enum input_enum {
  input_selected,		/* We've set the initial state.  */
  input_script = INPUT_SCRIPT,
  input_mri_script = INPUT_MRI_SCRIPT,
  input_defsym = INPUT_DEFSYM
} input_type;

extern input_type parser_input;

extern int hex_mode;
extern unsigned int lineno;

/* In ldlex.l.  */
extern int yylex PARAMS ((void));
extern void lex_push_file PARAMS ((FILE *, char *));
extern void lex_redirect PARAMS ((const char *));
extern void ldlex_script PARAMS ((void));
extern void ldlex_mri_script PARAMS ((void));
extern void ldlex_defsym PARAMS ((void));
extern void ldlex_expression PARAMS ((void));
extern void ldlex_both PARAMS ((void));
extern void ldlex_command PARAMS ((void));
extern void ldlex_popstate PARAMS ((void));

/* In lexsup.c.  */
extern int lex_input PARAMS ((void));
extern void lex_unput PARAMS ((int));
#ifndef yywrap
extern int yywrap PARAMS ((void));
#endif
extern void parse_args PARAMS ((int, char **));

#endif
