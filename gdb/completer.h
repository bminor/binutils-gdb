/* Header for GDB line completion.
   Copyright (C) 2000-2017 Free Software Foundation, Inc.

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

/* A list of completion candidates.  Each element is a malloc string,
   because ownership of the strings is transferred to readline, which
   calls free on each element.  */
typedef std::vector<gdb::unique_xmalloc_ptr<char>> completion_list;

/* The final result of a completion that is handed over to either
   readline or the "completion" command (which pretends to be
   readline).  Mainly a wrapper for a readline-style match list array,
   though other bits of info are included too.  */

struct completion_result
{
  /* Create an empty result.  */
  completion_result ();

  /* Create a result.  */
  completion_result (char **match_list, size_t number_matches,
		     bool completion_suppress_append);

  /* Destroy a result.  */
  ~completion_result ();

  DISABLE_COPY_AND_ASSIGN (completion_result);

  /* Move a result.  */
  completion_result (completion_result &&rhs);

  /* Release ownership of the match list array.  */
  char **release_match_list ();

  /* Sort the match list.  */
  void sort_match_list ();

private:
  /* Destroy the match list array and its contents.  */
  void reset_match_list ();

public:
  /* (There's no point in making these fields private, since the whole
     point of this wrapper is to build data in the layout expected by
     readline.  Making them private would require adding getters for
     the "complete" command, which would expose the same
     implementation details anyway.)  */

  /* The match list array, in the format that readline expects.
     match_list[0] contains the common prefix.  The real match list
     starts at index 1.  The list is NULL terminated.  If there's only
     one match, then match_list[1] is NULL.  If there are no matches,
     then this is NULL.  */
  char **match_list;
  /* The number of matched completions in MATCH_LIST.  Does not
     include the NULL terminator or the common prefix.  */
  size_t number_matches;

  /* Whether readline should suppress appending a whitespace, when
     there's only one possible completion.  */
  bool completion_suppress_append;
};

/* Object used by completers to build a completion match list to hand
   over to readline.  It tracks:

   - How many unique completions have been generated, to terminate
     completion list generation early if the list has grown to a size
     so large as to be useless.  This helps avoid GDB seeming to lock
     up in the event the user requests to complete on something vague
     that necessitates the time consuming expansion of many symbol
     tables.

   - The custom word point to hand over to readline, for completers
     that parse the input string in order to dynamically adjust
     themselves depending on exactly what they're completing.  E.g.,
     the linespec completer needs to bypass readline's too-simple word
     breaking algorithm.
*/
class completion_tracker
{
public:
  completion_tracker ();
  ~completion_tracker ();

  DISABLE_COPY_AND_ASSIGN (completion_tracker);

  /* Add the completion NAME to the list of generated completions if
     it is not there already.  If too many completions were already
     found, this throws an error.  */
  void add_completion (gdb::unique_xmalloc_ptr<char> name);

  /* Add all completions matches in LIST.  Elements are moved out of
     LIST.  */
  void add_completions (completion_list &&list);

  /* Set the quote char to be appended after a unique completion is
     added to the input line.  Set to '\0' to clear.  See
     m_quote_char's description.  */
  void set_quote_char (int quote_char)
  { m_quote_char = quote_char; }

  /* The quote char to be appended after a unique completion is added
     to the input line.  Returns '\0' if no quote char has been set.
     See m_quote_char's description.  */
  int quote_char () { return m_quote_char; }

  /* Tell the tracker that the current completer wants to provide a
     custom word point instead of a list of a break chars, in the
     handle_brkchars phase.  Such completers must also compute their
     completions then.  */
  void set_use_custom_word_point (bool enable)
  { m_use_custom_word_point = enable; }

  /* Whether the current completer computes a custom word point.  */
  bool use_custom_word_point () const
  { return m_use_custom_word_point; }

  /* The custom word point.  */
  int custom_word_point () const
  { return m_custom_word_point; }

  /* Set the custom word point to POINT.  */
  void set_custom_word_point (int point)
  { m_custom_word_point = point; }

  /* Advance the custom word point by LEN.  */
  void advance_custom_word_point_by (size_t len);

  /* Whether to tell readline to skip appending a whitespace after the
     completion.  See m_suppress_append_ws.  */
  bool suppress_append_ws () const
  { return m_suppress_append_ws; }

  /* Set whether to tell readline to skip appending a whitespace after
     the completion.  See m_suppress_append_ws.  */
  void set_suppress_append_ws (bool suppress)
  { m_suppress_append_ws = suppress; }

  /* Return true if we only have one completion, and it matches
     exactly the completion word.  I.e., completing results in what we
     already have.  */
  bool completes_to_completion_word (const char *word);

  /* True if we have any completion match recorded.  */
  bool have_completions () const
  { return !m_entries_vec.empty (); }

  /* Discard the current completion match list and the current
     LCD.  */
  void discard_completions ();

  /* Build a completion_result containing the list of completion
     matches to hand over to readline.  The parameters are as in
     rl_attempted_completion_function.  */
  completion_result build_completion_result (const char *text,
					     int start, int end);

private:

  /* Add the completion NAME to the list of generated completions if
     it is not there already.  If false is returned, too many
     completions were found.  */
  bool maybe_add_completion (gdb::unique_xmalloc_ptr<char> name);

  /* Given a new match, recompute the lowest common denominator (LCD)
     to hand over to readline.  */
  void recompute_lowest_common_denominator (const char *new_match);

  /* The completion matches found so far, in a vector.  */
  completion_list m_entries_vec;

  /* The completion matches found so far, in a hash table, for
     duplicate elimination as entries are added.  Otherwise the user
     is left scratching his/her head: readline and complete_command
     will remove duplicates, and if removal of duplicates there brings
     the total under max_completions the user may think gdb quit
     searching too early.  */
  htab_t m_entries_hash;

  /* If non-zero, then this is the quote char that needs to be
     appended after completion (iff we have a unique completion).  We
     don't rely on readline appending the quote char as delimiter as
     then readline wouldn't append the ' ' after the completion.
     I.e., we want this:

      before tab: "b 'function("
      after tab:  "b 'function()' "
  */
  int m_quote_char = '\0';

  /* If true, the completer has its own idea of "word" point, and
     doesn't want to rely on readline computing it based on brkchars.
     Set in the handle_brkchars phase.  */
  bool m_use_custom_word_point = false;

  /* The completer's idea of where the "word" we were looking at is
     relative to RL_LINE_BUFFER.  This is advanced in the
     handle_brkchars phase as the completer discovers potential
     completable words.  */
  int m_custom_word_point = 0;

  /* If true, tell readline to skip appending a whitespace after the
     completion.  Automatically set if we have a unique completion
     that already has a space at the end.  A completer may also
     explicitly set this.  E.g., the linespec completer sets this when
     the completion ends with the ":" separator between filename and
     function name.  */
  bool m_suppress_append_ws = false;

  /* Our idea of lowest common denominator to hand over to readline.
     See intro.  */
  char *m_lowest_common_denominator = NULL;

  /* If true, the LCD is unique.  I.e., all completion candidates had
     the same string.  */
  bool m_lowest_common_denominator_unique = false;
};

extern void gdb_display_match_list (char **matches, int len, int max,
				    const struct match_list_displayer *);

extern const char *get_max_completions_reached_message (void);

extern void complete_line (completion_tracker &tracker,
			   const char *text,
			   const char *line_buffer,
			   int point);

/* Find the bounds of the word in TEXT for completion purposes, and
   return a pointer to the end of the word.  Calls the completion
   machinery for a handle_brkchars phase (using TRACKER) to figure out
   the right work break characters for the command in TEXT.
   QUOTE_CHAR, if non-null, is set to the opening quote character if
   we found an unclosed quoted substring, '\0' otherwise.  */
extern const char *completion_find_completion_word (completion_tracker &tracker,
						    const char *text,
						    int *quote_char);


/* Assuming TEXT is an expression in the current language, find the
   completion word point for TEXT, emulating the algorithm readline
   uses to find the word point, using the current language's word
   break characters.  */

const char *advance_to_expression_complete_word_point
  (completion_tracker &tracker, const char *text);

extern char **gdb_rl_attempted_completion_function (const char *text,
						    int start, int end);

extern void noop_completer (struct cmd_list_element *,
			    completion_tracker &tracker,
			    const char *, const char *);

extern void filename_completer (struct cmd_list_element *,
				completion_tracker &tracker,
				const char *, const char *);

extern void expression_completer (struct cmd_list_element *,
				  completion_tracker &tracker,
				  const char *, const char *);

extern void location_completer (struct cmd_list_element *,
				completion_tracker &tracker,
				const char *, const char *);

extern void symbol_completer (struct cmd_list_element *,
			      completion_tracker &tracker,
			      const char *, const char *);

extern void command_completer (struct cmd_list_element *,
			       completion_tracker &tracker,
			       const char *, const char *);

extern void signal_completer (struct cmd_list_element *,
			      completion_tracker &tracker,
			      const char *, const char *);

extern void reg_or_group_completer (struct cmd_list_element *,
				    completion_tracker &tracker,
				    const char *, const char *);

extern void reggroup_completer (struct cmd_list_element *,
				completion_tracker &tracker,
				const char *, const char *);

extern const char *get_gdb_completer_quote_characters (void);

extern char *gdb_completion_word_break_characters (void);

/* Set the word break characters array to BREAK_CHARS.  This function
   is useful as const-correct alternative to direct assignment to
   rl_completer_word_break_characters, which is "char *",
   not "const char *".  */
extern void set_rl_completer_word_break_characters (const char *break_chars);

/* Get the matching completer_handle_brkchars_ftype function for FN.
   FN is one of the core completer functions above (filename,
   location, symbol, etc.).  This function is useful for cases when
   the completer doesn't know the type of the completion until some
   calculation is done (e.g., for Python functions).  */

extern completer_handle_brkchars_ftype *
  completer_handle_brkchars_func_for_completer (completer_ftype *fn);

/* Exported to linespec.c */

/* Return a list of all source files whose names begin with matching
   TEXT.  */
extern completion_list complete_source_filenames (const char *text);

/* Complete on expressions.  Often this means completing on symbol
   names, but some language parsers also have support for completing
   field names.  */
extern void complete_expression (completion_tracker &tracker,
				 const char *text, const char *word);

extern const char *skip_quoted_chars (const char *, const char *,
				      const char *);

extern const char *skip_quoted (const char *);

/* Maximum number of candidates to consider before the completer
   bails by throwing MAX_COMPLETIONS_REACHED_ERROR.  Negative values
   disable limiting.  */

extern int max_completions;

#endif /* defined (COMPLETER_H) */
