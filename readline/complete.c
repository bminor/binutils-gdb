/* complete.c -- filename completion for readline. */

/* Copyright (C) 1987, 1989, 1992 Free Software Foundation, Inc.

   This file is part of the GNU Readline Library, a library for
   reading lines of text with interactive input and history editing.

   The GNU Readline Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 1, or
   (at your option) any later version.

   The GNU Readline Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   The GNU General Public License is often shipped with GNU software, and
   is generally kept in a file called COPYING or LICENSE.  If you do not
   have a copy of the license, write to the Free Software Foundation,
   675 Mass Ave, Cambridge, MA 02139, USA. */

#include "sysdep.h"
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#if !defined (NO_SYS_FILE)
#  include <sys/file.h>
#endif /* !NO_SYS_FILE */

#include <errno.h>
/* Not all systems declare ERRNO in errno.h... and some systems #define it! */
#if !defined (errno)
extern int errno;
#endif /* !errno */

/* These next are for filename completion.  Perhaps this belongs
   in a different place. */
#ifndef __MSDOS__
#include <pwd.h>
#endif /* __MSDOS__ */
#if defined (USG) && !defined (isc386) && !defined (sgi)
extern struct passwd *getpwuid (), *getpwent ();
#endif
#if defined (isc386) && !defined (__STDC__) && defined (_POSIX_SOURCE)
extern struct passwd *getpwent ();
#endif

/* System-specific feature definitions and include files. */
#include "rldefs.h"

/* Some standard library routines. */
#include "readline.h"

#if !defined (strchr)
extern char *strchr ();
#endif /* !strchr */
#if !defined (strrchr)
extern char *strrchr ();
#endif /* !strrchr*/

extern char *tilde_expand ();
extern char *rl_copy_text ();

extern Function *rl_last_func;
extern int rl_editing_mode;
extern int screenwidth;

static int compare_strings ();
static char *rl_strpbrk ();

#if defined (STATIC_MALLOC)
static char *xmalloc (), *xrealloc ();
#else
extern char *xmalloc (), *xrealloc ();
#endif /* STATIC_MALLOC */

/* If non-zero, then this is the address of a function to call when
   completing on a directory name.  The function is called with
   the address of a string (the current directory name) as an arg. */
Function *rl_symbolic_link_hook = (Function *)NULL;

/* Non-zero means readline completion functions perform tilde expansion. */
int rl_complete_with_tilde_expansion = 0;

#define VISIBLE_STATS

#if defined (VISIBLE_STATS)
static int stat_char ();

/* Non-zero means add an additional character to each filename displayed
   during listing completion iff rl_filename_completion_desired which helps
   to indicate the type of file being listed. */
int rl_visible_stats = 0;
#endif /* VISIBLE_STATS */

/* **************************************************************** */
/*								    */
/*	Completion matching, from readline's point of view.	    */
/*								    */
/* **************************************************************** */

/* Pointer to the generator function for completion_matches ().
   NULL means to use filename_entry_function (), the default filename
   completer. */
Function *rl_completion_entry_function = (Function *)NULL;

/* Pointer to alternative function to create matches.
   Function is called with TEXT, START, and END.
   START and END are indices in RL_LINE_BUFFER saying what the boundaries
   of TEXT are.
   If this function exists and returns NULL then call the value of
   rl_completion_entry_function to try to match, otherwise use the
   array of strings returned. */
Function *rl_attempted_completion_function = (Function *)NULL;

/* Local variable states what happened during the last completion attempt. */
static int completion_changed_buffer = 0;

/* Complete the word at or before point.  You have supplied the function
   that does the initial simple matching selection algorithm (see
   completion_matches ()).  The default is to do filename completion. */

rl_complete (ignore, invoking_key)
     int ignore, invoking_key;
{
  if (rl_last_func == rl_complete && !completion_changed_buffer)
    rl_complete_internal ('?');
  else
    rl_complete_internal (TAB);
}

/* List the possible completions.  See description of rl_complete (). */
rl_possible_completions (ignore, invoking_key)
{
  rl_complete_internal ('?');
}

rl_insert_completions (ignore, invoking_key)
     int ignore, invoking_key;
{
  rl_complete_internal ('*');
}

/* The user must press "y" or "n". Non-zero return means "y" pressed. */
get_y_or_n ()
{
  int c;

  for (;;)
    {
      c = rl_read_key ();
      if (c == 'y' || c == 'Y')
	return (1);
      if (c == 'n' || c == 'N')
	return (0);
      if (c == ABORT_CHAR)
	rl_abort ();
      ding ();
    }
}

/* Up to this many items will be displayed in response to a
   possible-completions call.  After that, we ask the user if
   she is sure she wants to see them all. */
int rl_completion_query_items = 100;

/* The basic list of characters that signal a break between words for the
   completer routine.  The contents of this variable is what breaks words
   in the shell, i.e. " \t\n\"\\'`@$><=" */
char *rl_basic_word_break_characters = " \t\n\"\\'`@$><=;|&{(";

/* The list of characters that signal a break between words for
   rl_complete_internal.  The default list is the contents of
   rl_basic_word_break_characters.  */
char *rl_completer_word_break_characters = (char *)NULL;

/* List of characters which can be used to quote a substring of the line.
   Completion occurs on the entire substring, and within the substring
   rl_completer_word_break_characters are treated as any other character,
   unless they also appear within this list. */
char *rl_completer_quote_characters = (char *)NULL;

/* List of characters that are word break characters, but should be left
   in TEXT when it is passed to the completion function.  The shell uses
   this to help determine what kind of completing to do. */
char *rl_special_prefixes = (char *)NULL;

/* If non-zero, then disallow duplicates in the matches. */
int rl_ignore_completion_duplicates = 1;

/* Non-zero means that the results of the matches are to be treated
   as filenames.  This is ALWAYS zero on entry, and can only be changed
   within a completion entry finder function. */
int rl_filename_completion_desired = 0;

/* This function, if defined, is called by the completer when real
   filename completion is done, after all the matching names have been
   generated. It is passed a (char**) known as matches in the code below.
   It consists of a NULL-terminated array of pointers to potential
   matching strings.  The 1st element (matches[0]) is the maximal
   substring that is common to all matches. This function can re-arrange
   the list of matches as required, but all elements of the array must be
   free()'d if they are deleted. The main intent of this function is
   to implement FIGNORE a la SunOS csh. */
Function *rl_ignore_some_completions_function = (Function *)NULL;

/* Complete the word at or before point.
   WHAT_TO_DO says what to do with the completion.
   `?' means list the possible completions.
   TAB means do standard completion.
   `*' means insert all of the possible completions. */
rl_complete_internal (what_to_do)
     int what_to_do;
{
  char *filename_completion_function ();
  char **completion_matches (), **matches;
  Function *our_func;
  int start, scan, end, delimiter = 0;
  char *text, *saved_line_buffer;
  char quote_char = '\0';
  char *replacement;

  if (rl_line_buffer)
    saved_line_buffer = savestring (rl_line_buffer);
  else
    saved_line_buffer = (char *)NULL;

  if (rl_completion_entry_function)
    our_func = rl_completion_entry_function;
  else
    our_func = (int (*)())filename_completion_function;

  /* Only the completion entry function can change this. */
  rl_filename_completion_desired = 0;

  /* We now look backwards for the start of a filename/variable word. */
  end = rl_point;

  if (rl_point)
    {
      if (rl_completer_quote_characters)
	{
	  /* We have a list of characters which can be used in pairs to
	     quote substrings for the completer.  Try to find the start
	     of an unclosed quoted substring.
	     [FIXME: Doesn't yet handle '\' escapes to quote quotes. */
	  for (scan = 0; scan < end; scan++)
	    {
	      if (quote_char != '\0')
		{
		  /* Ignore everything until the matching close quote char. */
		  if (rl_line_buffer[scan] == quote_char)
		    {
		      /* Found matching close quote. Abandon this substring. */
		      quote_char = '\0';
		      rl_point = end;
		    }
		}
	      else if (strchr (rl_completer_quote_characters, rl_line_buffer[scan]))
		{
		  /* Found start of a quoted substring. */
		  quote_char = rl_line_buffer[scan];
		  rl_point = scan + 1;
		}
	    }
	}
      if (rl_point == end)
	{
	  /* We didn't find an unclosed quoted substring upon which to do
	     completion, so use the word break characters to find the
	     substring on which to do completion. */
	  while (--rl_point &&
		 !strchr (rl_completer_word_break_characters,
			  rl_line_buffer[rl_point])) {;}
	}

      /* If we are at a word break, then advance past it. */
      if (strchr (rl_completer_word_break_characters, rl_line_buffer[rl_point]))
	{
	  /* If the character that caused the word break was a quoting
	     character, then remember it as the delimiter. */
	  if (strchr ("\"'", rl_line_buffer[rl_point]) && (end - rl_point) > 1)
	    delimiter = rl_line_buffer[rl_point];

	  /* If the character isn't needed to determine something special
	     about what kind of completion to perform, then advance past it. */

	  if (!rl_special_prefixes ||
	      !strchr (rl_special_prefixes, rl_line_buffer[rl_point]))
	    rl_point++;
	}
    }

  start = rl_point;
  rl_point = end;
  text = rl_copy_text (start, end);

  /* If the user wants to TRY to complete, but then wants to give
     up and use the default completion function, they set the
     variable rl_attempted_completion_function. */
  if (rl_attempted_completion_function)
    {
      matches =
	(char **)(*rl_attempted_completion_function) (text, start, end);

      if (matches)
	{
	  if (matches == (char **)-1)
	    matches = (char **)NULL;
	  our_func = (Function *)NULL;
	  goto after_usual_completion;
	}
    }

  matches = completion_matches (text, our_func);

 after_usual_completion:
  free (text);

  if (!matches)
    ding ();
  else
    {
      register int i;

    some_matches:

      /* It seems to me that in all the cases we handle we would like
	 to ignore duplicate possibilities.   Scan for the text to
	 insert being identical to the other completions. */
      if (rl_ignore_completion_duplicates)
	{
	  char *lowest_common;
	  int j, newlen = 0;

	  /* Sort the items. */
	  /* It is safe to sort this array, because the lowest common
	     denominator found in matches[0] will remain in place. */
	  for (i = 0; matches[i]; i++);
	  qsort (matches, i, sizeof (char *), compare_strings);

	  /* Remember the lowest common denominator for it may be unique. */
	  lowest_common = savestring (matches[0]);

	  for (i = 0; matches[i + 1]; i++)
	    {
	      if (strcmp (matches[i], matches[i + 1]) == 0)
		{
		  free (matches[i]);
		  matches[i] = (char *)-1;
		}
	      else
		newlen++;
	    }

	  /* We have marked all the dead slots with (char *)-1.
	     Copy all the non-dead entries into a new array. */
	  {
	    char **temp_array =
	      (char **)xmalloc ((3 + newlen) * sizeof (char *));

	    for (i = 1, j = 1; matches[i]; i++)
	      {
		if (matches[i] != (char *)-1)
		  temp_array[j++] = matches[i];
	      }

	    temp_array[j] = (char *)NULL;

	    if (matches[0] != (char *)-1)
	      free (matches[0]);

	    free (matches);

	    matches = temp_array;
	  }

	  /* Place the lowest common denominator back in [0]. */
	  matches[0] = lowest_common;

	  /* If there is one string left, and it is identical to the
	     lowest common denominator, then the LCD is the string to
	     insert. */
	  if (j == 2 && strcmp (matches[0], matches[1]) == 0)
	    {
	      free (matches[1]);
	      matches[1] = (char *)NULL;
	    }
	}

      switch (what_to_do)
	{
	case TAB:
	  /* If we are matching filenames, then here is our chance to
	     do clever processing by re-examining the list.  Call the
	     ignore function with the array as a parameter.  It can
	     munge the array, deleting matches as it desires. */
	  if (rl_ignore_some_completions_function &&
	      our_func == (int (*)())filename_completion_function)
	    (void)(*rl_ignore_some_completions_function)(matches);

	  /* If we are doing completion on quoted substrings, and any matches
	     contain any of the completer_word_break_characters, then auto-
	     matically prepend the substring with a quote character (just pick
	     the first one from the list of such) if it does not already begin
	     with a quote string.  FIXME: Need to remove any such automatically
	     inserted quote character when it no longer is necessary, such as
	     if we change the string we are completing on and the new set of
	     matches don't require a quoted substring. */
	  replacement = matches[0];

	  if (matches[0] && rl_completer_quote_characters && !quote_char &&
	      rl_filename_completion_desired)
	    {
	      int do_replace;

	      do_replace = 0;

	      /* If there is only a single match, see if we need to
		 quote it. */
	      if (!matches[1] &&
	          rl_strpbrk (matches[0], rl_completer_word_break_characters))
	        do_replace = 1;

	      /* If there are multiple matches, check to see if any of them
		 require that the substring be quoted. */
	      for (i = 1; matches[i] != NULL; i++)
		if (rl_strpbrk (matches[i], rl_completer_word_break_characters))
		  {
		    do_replace = 1;
		    break;
		  }
	      if (do_replace)
		{
#if defined (SHELL)
		  /* XXX - experimental */
		  /* Single-quote the replacement, since we found an
		     embedded word break character in a potential match. */
		  char *rtext;
		  extern char *single_quote ();	/* in builtins/common.c */

		  rtext = single_quote (matches[0]);
		  replacement = (char *)alloca (strlen (rtext) + 1);
		  strcpy (replacement, rtext);
		  free (rtext);
#else /* !SHELL */
		  /* Found an embedded word break character in a potential
		     match, so we need to prepend a quote character if we
		     are replacing the completion string. */
		  replacement = (char *)alloca (strlen (matches[0]) + 2);
		  quote_char = *rl_completer_quote_characters;
		  *replacement = quote_char;
		  strcpy (replacement + 1, matches[0]);
#endif /* SHELL */
		}
	    }
	  if (replacement)
	    {
	      rl_delete_text (start, rl_point);
	      rl_point = start;
	      rl_insert_text (replacement);
	    }

	  /* If there are more matches, ring the bell to indicate.
	     If this was the only match, and we are hacking files,
	     check the file to see if it was a directory.  If so,
	     add a '/' to the name.  If not, and we are at the end
	     of the line, then add a space. */
	  if (matches[1])
	    {
	      if (rl_editing_mode != vi_mode)
		ding ();	/* There are other matches remaining. */
	    }
	  else
	    {
	      char temp_string[4];
	      int temp_string_index = 0;

	      if (quote_char)
		temp_string[temp_string_index++] = quote_char;

	      temp_string[temp_string_index++] = delimiter ? delimiter : ' ';
	      temp_string[temp_string_index++] = '\0';

	      if (rl_filename_completion_desired)
		{
		  struct stat finfo;
		  char *filename = tilde_expand (matches[0]);

		  if ((stat (filename, &finfo) == 0) &&
		      S_ISDIR (finfo.st_mode))
		    {
		      if (rl_line_buffer[rl_point] != '/')
			rl_insert_text ("/");
		    }
		  else
		    {
		      if (rl_point == rl_end)
			rl_insert_text (temp_string);
		    }
		  free (filename);
		}
	      else
		{
		  if (rl_point == rl_end)
		    rl_insert_text (temp_string);
		}
	    }
	  break;

	case '*':
	  {
	    int i = 1;

	    rl_delete_text (start, rl_point);
	    rl_point = start;
	    rl_begin_undo_group ();
	    if (matches[1])
	      {
		while (matches[i])
		  {
		    rl_insert_text (matches[i++]);
		    rl_insert_text (" ");
		  }
	      }
	    else
	      {
		rl_insert_text (matches[0]);
		rl_insert_text (" ");
	      }
	    rl_end_undo_group ();
	  }
	  break;

	case '?':
	  {
	    int len, count, limit, max = 0;
	    int j, k, l;

	    /* Handle simple case first.  What if there is only one answer? */
	    if (!matches[1])
	      {
		char *temp;

		if (rl_filename_completion_desired)
		  temp = strrchr (matches[0], '/');
		else
		  temp = (char *)NULL;

		if (!temp)
		  temp = matches[0];
		else
		  temp++;

		crlf ();
		fprintf (rl_outstream, "%s", temp);
#if defined (VISIBLE_STATS)
		if (rl_filename_completion_desired && rl_visible_stats)
		  {
		    int extension_char;

		    extension_char = stat_char (matches[0]);
		    if (extension_char)
		      putc (extension_char, rl_outstream);
		  }
#endif /* VISIBLE_STATS */
		crlf ();
		goto restart;
	      }

	    /* There is more than one answer.  Find out how many there are,
	       and find out what the maximum printed length of a single entry
	       is. */
	    for (i = 1; matches[i]; i++)
	      {
		char *temp;
		int name_length;

		/* If we are hacking filenames, then only count the characters
		   after the last slash in the pathname. */
		if (rl_filename_completion_desired)
		  temp = strrchr (matches[i], '/');
		else
		  temp = (char *)NULL;

		if (!temp)
		  temp = matches[i];
		else
		  temp++;

		name_length = strlen (temp);

		if (name_length > max)
		  max = name_length;
	      }

	    len = i - 1;

	    /* If there are many items, then ask the user if she
	       really wants to see them all. */
	    if (len >= rl_completion_query_items)
	      {
		crlf ();
		fprintf (rl_outstream,
			 "There are %d possibilities.  Do you really", len);
		crlf ();
		fprintf (rl_outstream, "wish to see them all? (y or n)");
		fflush (rl_outstream);
		if (!get_y_or_n ())
		  {
		    crlf ();
		    goto restart;
		  }
	      }
	    /* How many items of MAX length can we fit in the screen window? */
	    max += 2;
	    limit = screenwidth / max;
	    if (limit != 1 && (limit * max == screenwidth))
	      limit--;

	    /* Avoid a possible floating exception.  If max > screenwidth,
	       limit will be 0 and a divide-by-zero fault will result. */
	    if (limit == 0)
	      limit = 1;

	    /* How many iterations of the printing loop? */
	    count = (len + (limit - 1)) / limit;

	    /* Watch out for special case.  If LEN is less than LIMIT, then
	       just do the inner printing loop. */
	    if (len < limit)
	      count = 1;

	    /* Sort the items if they are not already sorted. */
	    if (!rl_ignore_completion_duplicates)
	      qsort (matches, len, sizeof (char *), compare_strings);

	    /* Print the sorted items, up-and-down alphabetically, like
	       ls might. */
	    crlf ();

	    for (i = 1; i < count + 1; i++)
	      {
		for (j = 0, l = i; j < limit; j++)
		  {
		    if (l > len || !matches[l])
		      {
			break;
		      }
		    else
		      {
			char *temp = (char *)NULL;
			int printed_length;

			if (rl_filename_completion_desired)
			  temp = strrchr (matches[l], '/');

			if (!temp)
			  temp = matches[l];
			else
			  temp++;

			printed_length = strlen (temp);
			fprintf (rl_outstream, "%s", temp);

#if defined (VISIBLE_STATS)
			if (rl_filename_completion_desired &&
			    rl_visible_stats)
			  {
			    int extension_char;

			    extension_char = stat_char (matches[l]);

			    if (extension_char)
			      {
				putc (extension_char, rl_outstream);
				printed_length++;
			      }
			  }
#endif /* VISIBLE_STATS */

			if (j + 1 < limit)
			  {
			    for (k = 0; k < max - printed_length; k++)
			      putc (' ', rl_outstream);
			  }
		      }
		    l += count;
		  }
		crlf ();
	      }
	  restart:

	    rl_on_new_line ();
	  }
	  break;

	default:
	  abort ();
	}

      for (i = 0; matches[i]; i++)
	free (matches[i]);
      free (matches);
    }

  /* Check to see if the line has changed through all of this manipulation. */
  if (saved_line_buffer)
    {
      if (strcmp (rl_line_buffer, saved_line_buffer) != 0)
	completion_changed_buffer = 1;
      else
	completion_changed_buffer = 0;

      free (saved_line_buffer);
    }
}

#if defined (VISIBLE_STATS)
/* Return the character which best describes FILENAME.
     `@' for symbolic links
     `/' for directories
     `*' for executables
     `=' for sockets */
static int
stat_char (filename)
     char *filename;
{
  struct stat finfo;
  int character = 0;

  if (stat (filename, &finfo) == -1)
    return (character);

  if (S_ISDIR (finfo.st_mode))
    character = '/';
#if defined (S_ISLNK)
  else if (S_ISLNK (finfo.st_mode))
    character = '@';
#endif /* S_ISLNK */
#if defined (S_ISSOCK)
  else if (S_ISSOCK (finfo.st_mode))
    character = '=';
#endif /* S_ISSOCK */
  else if (S_ISREG (finfo.st_mode))
    {
      if (access (filename, X_OK) == 0)
	character = '*';
    }
  return (character);
}
#endif /* VISIBLE_STATS */

/* Stupid comparison routine for qsort () ing strings. */
static int
compare_strings (s1, s2)
  char **s1, **s2;
{
  return (strcmp (*s1, *s2));
}

/* A completion function for usernames.
   TEXT contains a partial username preceded by a random
   character (usually `~').  */
char *
username_completion_function (text, state)
     int state;
     char *text;
{
#ifdef __GO32__
  return (char *)NULL;
#else /* !__GO32__ */
  static char *username = (char *)NULL;
  static struct passwd *entry;
  static int namelen, first_char, first_char_loc;

  if (!state)
    {
      if (username)
	free (username);

      first_char = *text;

      if (first_char == '~')
	first_char_loc = 1;
      else
	first_char_loc = 0;

      username = savestring (&text[first_char_loc]);
      namelen = strlen (username);
      setpwent ();
    }

  while (entry = getpwent ())
    {
      if (strncmp (username, entry->pw_name, namelen) == 0)
	break;
    }

  if (!entry)
    {
      endpwent ();
      return ((char *)NULL);
    }
  else
    {
      char *value = (char *)xmalloc (2 + strlen (entry->pw_name));

      *value = *text;

      strcpy (value + first_char_loc, entry->pw_name);

      if (first_char == '~')
	rl_filename_completion_desired = 1;

      return (value);
    }
#endif /* !__GO32__ */
}


/* **************************************************************** */
/*								    */
/*			     Completion				    */
/*								    */
/* **************************************************************** */

/* Non-zero means that case is not significant in completion. */
int completion_case_fold = 0;

/* Return an array of (char *) which is a list of completions for TEXT.
   If there are no completions, return a NULL pointer.
   The first entry in the returned array is the substitution for TEXT.
   The remaining entries are the possible completions.
   The array is terminated with a NULL pointer.

   ENTRY_FUNCTION is a function of two args, and returns a (char *).
     The first argument is TEXT.
     The second is a state argument; it should be zero on the first call, and
     non-zero on subsequent calls.  It returns a NULL pointer to the caller
     when there are no more matches.
 */
char **
completion_matches (text, entry_function)
     char *text;
     char *(*entry_function) ();
{
  /* Number of slots in match_list. */
  int match_list_size;

  /* The list of matches. */
  char **match_list =
    (char **)xmalloc (((match_list_size = 10) + 1) * sizeof (char *));

  /* Number of matches actually found. */
  int matches = 0;

  /* Temporary string binder. */
  char *string;

  match_list[1] = (char *)NULL;

  while (string = (*entry_function) (text, matches))
    {
      if (matches + 1 == match_list_size)
	match_list = (char **)xrealloc
	  (match_list, ((match_list_size += 10) + 1) * sizeof (char *));

      match_list[++matches] = string;
      match_list[matches + 1] = (char *)NULL;
    }

  /* If there were any matches, then look through them finding out the
     lowest common denominator.  That then becomes match_list[0]. */
  if (matches)
    {
      register int i = 1;
      int low = 100000;		/* Count of max-matched characters. */

      /* If only one match, just use that. */
      if (matches == 1)
	{
	  match_list[0] = match_list[1];
	  match_list[1] = (char *)NULL;
	}
      else
	{
	  /* Otherwise, compare each member of the list with
	     the next, finding out where they stop matching. */

	  while (i < matches)
	    {
	      register int c1, c2, si;

	      if (completion_case_fold)
		{
		  for (si = 0;
		       (c1 = to_lower(match_list[i][si])) &&
		       (c2 = to_lower(match_list[i + 1][si]));
		       si++)
		    if (c1 != c2) break;
		}
	      else
		{
		  for (si = 0;
		       (c1 = match_list[i][si]) &&
		       (c2 = match_list[i + 1][si]);
		       si++)
		    if (c1 != c2) break;
		}

	      if (low > si) low = si;
	      i++;
	    }
	  match_list[0] = (char *)xmalloc (low + 1);
	  strncpy (match_list[0], match_list[1], low);
	  match_list[0][low] = '\0';
	}
    }
  else				/* There were no matches. */
    {
      free (match_list);
      match_list = (char **)NULL;
    }
  return (match_list);
}

/* Okay, now we write the entry_function for filename completion.  In the
   general case.  Note that completion in the shell is a little different
   because of all the pathnames that must be followed when looking up the
   completion for a command. */
char *
filename_completion_function (text, state)
     int state;
     char *text;
{
  static DIR *directory;
  static char *filename = (char *)NULL;
  static char *dirname = (char *)NULL;
  static char *users_dirname = (char *)NULL;
  static int filename_len;

  dirent *entry = (dirent *)NULL;

  /* If we don't have any state, then do some initialization. */
  if (!state)
    {
      char *temp;

      if (dirname) free (dirname);
      if (filename) free (filename);
      if (users_dirname) free (users_dirname);

      filename = savestring (text);
      if (!*text) text = ".";
      dirname = savestring (text);

      temp = strrchr (dirname, '/');

      if (temp)
	{
	  strcpy (filename, ++temp);
	  *temp = '\0';
	}
      else
	strcpy (dirname, ".");

      /* We aren't done yet.  We also support the "~user" syntax. */

      /* Save the version of the directory that the user typed. */
      users_dirname = savestring (dirname);
      {
	char *temp_dirname;

	temp_dirname = tilde_expand (dirname);
	free (dirname);
	dirname = temp_dirname;

	if (rl_symbolic_link_hook)
	  (*rl_symbolic_link_hook) (&dirname);
      }
      directory = opendir (dirname);
      filename_len = strlen (filename);

      rl_filename_completion_desired = 1;
    }

  /* At this point we should entertain the possibility of hacking wildcarded
     filenames, like /usr/man/man<WILD>/te<TAB>.  If the directory name
     contains globbing characters, then build an array of directories, and
     then map over that list while completing. */
  /* *** UNIMPLEMENTED *** */

  /* Now that we have some state, we can read the directory. */

  while (directory && (entry = readdir (directory)))
    {
      /* Special case for no filename.
	 All entries except "." and ".." match. */
      if (!filename_len)
	{
	  if ((strcmp (entry->d_name, ".") != 0) &&
	      (strcmp (entry->d_name, "..") != 0))
	    break;
	}
      else
	{
	  /* Otherwise, if these match upto the length of filename, then
	     it is a match. */
	    if (((int)D_NAMLEN (entry)) >= filename_len &&
		(entry->d_name[0] == filename[0]) &&
		(strncmp (filename, entry->d_name, filename_len) == 0))
	      {
		break;
	      }
	}
    }

  if (!entry)
    {
      if (directory)
	{
	  closedir (directory);
	  directory = (DIR *)NULL;
	}

      if (dirname)
	{
	  free (dirname);
	  dirname = (char *)NULL;
	}
      if (filename)
	{
	  free (filename);
	  filename = (char *)NULL;
	}
      if (users_dirname)
	{
	  free (users_dirname);
	  users_dirname = (char *)NULL;
	}

      return (char *)NULL;
    }
  else
    {
      char *temp;

      if (dirname && (strcmp (dirname, ".") != 0))
	{
	  if (rl_complete_with_tilde_expansion && *users_dirname == '~')
	    {
	      int dirlen = strlen (dirname);
	      temp = (char *)xmalloc (2 + dirlen + D_NAMLEN (entry));
	      strcpy (temp, dirname);
	      /* Canonicalization cuts off any final slash present.  We need
		 to add it back. */
	      if (dirname[dirlen - 1] != '/')
	        {
	          temp[dirlen] = '/';
	          temp[dirlen + 1] = '\0';
	        }
	    }
	  else
	    {
	      temp = (char *)
		xmalloc (1 + strlen (users_dirname) + D_NAMLEN (entry));
	      strcpy (temp, users_dirname);
	    }

	  strcat (temp, entry->d_name);
	}
      else
	{
	  temp = (savestring (entry->d_name));
	}
      return (temp);
    }
}

/* A function for simple tilde expansion. */
int
rl_tilde_expand (ignore, key)
     int ignore, key;
{
  register int start, end;
  char *homedir;

  end = rl_point;
  start = end - 1;

  if (rl_point == rl_end && rl_line_buffer[rl_point] == '~')
    {
      homedir = tilde_expand ("~");
      goto insert;
    }
  else if (rl_line_buffer[start] != '~')
    {
      for (; !whitespace (rl_line_buffer[start]) && start >= 0; start--);
      start++;
    }

  end = start;
  do
    {
      end++;
    }
  while (!whitespace (rl_line_buffer[end]) && end < rl_end);

  if (whitespace (rl_line_buffer[end]) || end >= rl_end)
    end--;

  /* If the first character of the current word is a tilde, perform
     tilde expansion and insert the result.  If not a tilde, do
     nothing. */
  if (rl_line_buffer[start] == '~')
    {
      char *temp;
      int len;

      len = end - start + 1;
      temp = (char *)alloca (len + 1);
      strncpy (temp, rl_line_buffer + start, len);
      temp[len] = '\0';
      homedir = tilde_expand (temp);

    insert:
      rl_begin_undo_group ();
      rl_delete_text (start, end + 1);
      rl_point = start;
      rl_insert_text (homedir);
      rl_end_undo_group ();
    }

  return (0);
}

/* Find the first occurrence in STRING1 of any character from STRING2.
   Return a pointer to the character in STRING1. */
static char *
rl_strpbrk (string1, string2)
     char *string1, *string2;
{
  register char *scan;

  for (; *string1; string1++)
    {
      for (scan = string2; *scan; scan++)
	{
	  if (*string1 == *scan)
	    {
	      return (string1);
	    }
	}
    }
  return ((char *)NULL);
}

#if defined (STATIC_MALLOC)

/* **************************************************************** */
/*								    */
/*			xmalloc and xrealloc ()		     	    */
/*								    */
/* **************************************************************** */

static void memory_error_and_abort ();

static char *
xmalloc (bytes)
     int bytes;
{
  char *temp = (char *)malloc (bytes);

  if (!temp)
    memory_error_and_abort ();
  return (temp);
}

static char *
xrealloc (pointer, bytes)
     char *pointer;
     int bytes;
{
  char *temp;

  if (!pointer)
    temp = (char *)malloc (bytes);
  else
    temp = (char *)realloc (pointer, bytes);

  if (!temp)
    memory_error_and_abort ();

  return (temp);
}

static void
memory_error_and_abort ()
{
  fprintf (stderr, "readline: Out of virtual memory!\n");
  abort ();
}
#endif /* STATIC_MALLOC */
