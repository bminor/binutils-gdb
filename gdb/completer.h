/* Header for GDB line completion.
   Copyright (C) 2000-2015 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#if !defined (COMPLETER_H)
#define COMPLETER_H 1

#include "gdb_vecs.h"
#include "command.h"

/* Types of functions in struct match_list_displayer.  */

struct match_list_displayer;

typedef void mld_crlf_ftype (const struct match_list_displayer *);
typedef void mld_putch_ftype (const struct match_list_displayer *, int);
typedef void mld_puts_ftype (const struct match_list_displayer *,
			     const char *);
typedef void mld_flush_ftype (const struct match_list_displayer *);
typedef void mld_erase_entire_line_ftype (const struct match_list_displayer *);
typedef void mld_beep_ftype (const struct match_list_displayer *);
typedef int mld_read_key_ftype (const struct match_list_displayer *);

/* Interface between CLI/TUI and gdb_match_list_displayer.  */

struct match_list_displayer
{
  /* The screen dimensions to work with when displaying matches.  */
  int height, width;

  /* Print cr,lf.  */
  mld_crlf_ftype *crlf;

  /* Not "putc" to avoid issues where it is a stdio macro.  Sigh.  */
  mld_putch_ftype *putch;

  /* Print a string.  */
  mld_puts_ftype *puts;

  /* Flush all accumulated output.  */
  mld_flush_ftype *flush;

  /* Erase the currently line on the terminal (but don't discard any text the
     user has entered, readline may shortly re-print it).  */
  mld_erase_entire_line_ftype *erase_entire_line;

  /* Ring the bell.  */
  mld_beep_ftype *beep;

  /* Read one key.  */
  mld_read_key_ftype *read_key;
};

extern void gdb_display_match_list (char **matches, int len, int max,
				    const struct match_list_displayer *);

extern VEC (char_ptr) *complete_line (const char *text,
				      const char *line_buffer,
				      int point);

extern char *readline_line_completion_function (const char *text,
						int matches);

extern VEC (char_ptr) *noop_completer (struct cmd_list_element *,
				       const char *, const char *);

extern VEC (char_ptr) *filename_completer (struct cmd_list_element *,
					   const char *, const char *);

extern VEC (char_ptr) *expression_completer (struct cmd_list_element *,
					     const char *, const char *);

extern VEC (char_ptr) *location_completer (struct cmd_list_element *,
					   const char *, const char *);

extern VEC (char_ptr) *command_completer (struct cmd_list_element *,
					  const char *, const char *);

extern VEC (char_ptr) *signal_completer (struct cmd_list_element *,
					 const char *, const char *);

extern VEC (char_ptr) *reg_or_group_completer (struct cmd_list_element *,
					       const char *, const char *);

extern char *get_gdb_completer_quote_characters (void);

extern char *gdb_completion_word_break_characters (void);

/* Set the word break characters array to the corresponding set of
   chars, based on FN.  This function is useful for cases when the
   completer doesn't know the type of the completion until some
   calculation is done (e.g., for Python functions).  */

extern void set_gdb_completion_word_break_characters (completer_ftype *fn);

/* Exported to linespec.c */

extern const char *skip_quoted_chars (const char *, const char *,
				      const char *);

extern const char *skip_quoted (const char *);

#endif /* defined (COMPLETER_H) */
