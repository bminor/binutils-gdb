/* read.h - of read.c

   Copyright (C) 1986, 1990, 1992 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

extern char *input_line_pointer;/* -> char we are parsing now. */

#define PERMIT_WHITESPACE	/* Define to make whitespace be allowed in */
/* many syntactically unnecessary places. */
/* Normally undefined. For compatibility */
/* with ancient GNU cc. */
/* #undef PERMIT_WHITESPACE */

#ifdef PERMIT_WHITESPACE
#define SKIP_WHITESPACE() {if (* input_line_pointer == ' ') ++ input_line_pointer;}
#else
#define SKIP_WHITESPACE() know(*input_line_pointer != ' ' )
#endif


#define	LEX_NAME	(1)	/* may continue a name */
#define LEX_BEGIN_NAME	(2)	/* may begin a name */

#define is_name_beginner(c)     ( lex_type[c] & LEX_BEGIN_NAME )
#define is_part_of_name(c)      ( lex_type[c] & LEX_NAME       )

#ifndef is_a_char
#define CHAR_MASK	(0xff)
#define NOT_A_CHAR	(CHAR_MASK+1)
#define is_a_char(c)	(((unsigned)(c)) <= CHAR_MASK)
#endif /* is_a_char() */

extern const char lex_type[];
extern char is_end_of_line[];

/* These are initialized by the CPU specific target files (tc-*.c).  */
extern const char comment_chars[];
extern const char line_comment_chars[];
extern const char line_separator_chars[];

char *demand_copy_C_string PARAMS ((int *len_pointer));
char get_absolute_expression_and_terminator PARAMS ((long *val_pointer));
long get_absolute_expression PARAMS ((void));
void add_include_dir PARAMS ((char *path));
void big_cons PARAMS ((int nbytes));
void cons PARAMS ((unsigned int nbytes));
void demand_empty_rest_of_line PARAMS ((void));
void emit_expr PARAMS ((expressionS *exp, unsigned int nbytes));
void equals PARAMS ((char *sym_name));
void float_cons PARAMS ((int float_type));
void ignore_rest_of_line PARAMS ((void));
void pseudo_set PARAMS ((symbolS * symbolP));
void read_a_source_file PARAMS ((char *name));
void read_begin PARAMS ((void));
void s_abort PARAMS ((void));
void s_align_bytes PARAMS ((int arg));
void s_align_ptwo PARAMS ((void));
void s_app_file PARAMS ((int));
void s_app_line PARAMS ((void));
void s_comm PARAMS ((void));
void s_data PARAMS ((void));
void s_desc PARAMS ((void));
void s_else PARAMS ((int arg));
void s_end PARAMS ((int arg));
void s_endif PARAMS ((int arg));
void s_fill PARAMS ((void));
void s_globl PARAMS ((void));
void s_if PARAMS ((int arg));
void s_ifdef PARAMS ((int arg));
void s_ifeqs PARAMS ((int arg));
void s_ignore PARAMS ((int arg));
void s_include PARAMS ((int arg));
void s_lcomm PARAMS ((int needs_align));
void s_lsym PARAMS ((void));
void s_org PARAMS ((void));
void s_set PARAMS ((void));
void s_space PARAMS ((int mult));
void s_stab PARAMS ((int what));
void s_text PARAMS ((void));
void stringer PARAMS ((int append_zero));
void s_xstab PARAMS ((int what));

/* end of read.h */
