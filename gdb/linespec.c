/* Parser for linespec for the GNU debugger, GDB.
   Copyright 1986, 1987, 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995,
   1996, 1997, 1998, 1999, 2000, 2001, 2002
   Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "symtab.h"
#include "frame.h"
#include "command.h"
#include "symfile.h"
#include "objfiles.h"
#include "source.h"
#include "demangle.h"
#include "value.h"
#include "completer.h"
#include "cp-abi.h"
#include "block.h"
#include "parser-defs.h"

/* Prototypes for local functions.  */

static void initialize_defaults (struct symtab **default_symtab,
				 int *default_line);

static void set_flags (char *arg, int *is_quoted, char **paren_pointer);

static struct symtabs_and_lines decode_indirect (char **argptr);

static char *locate_first_half (char **argptr, int *is_quote_enclosed);

static char *find_toplevel_char (char *s, char c);

static struct symtabs_and_lines decode_compound (char **argptr,
						 int funfirstline,
						 char ***canonical,
						 char *saved_arg,
						 char *next_component);

static int examine_compound_token (char **argptr,
				   int funfirstline,
				   char ***canonical,
				   char *saved_arg,
				   char *p,
				   struct symtabs_and_lines *values);

static struct symbol *locate_class_sym (char **argptr, char *p);

static char *find_next_token (char **argptr);

static int total_number_of_methods (struct type *type);

static struct symtabs_and_lines find_method (int funfirstline,
					     char ***canonical,
					     char *saved_arg,
					     char *method,
					     struct type *class_type,
					     struct symbol *class_sym);

static int count_methods (struct type *class_type, char *method,
			  struct symbol **sym_arr);

static int find_methods (struct type *class_type, char *method,
			 struct symbol **sym_arr);

static const char *find_method_name (struct type *class_type,
				     int method_counter,
				     char *dem_opname);

static int add_matching_methods (int method_counter, struct type *class_type,
				 struct symbol **sym_arr);

static int add_constructors (int method_counter, struct type *class_type,
			     struct symbol **sym_arr);

static struct symtabs_and_lines select_symbols (struct symbol **sym_arr,
						int nelts,
						int funfirstline,
						char ***canonical);

static struct
symtabs_and_lines select_symbols_args (struct symbol **sym_arr,
				       int nelts,
				       char *args,
				       char **canonical_arr,
				       struct symtabs_and_lines values,
				       struct cleanup *old_chain);

static NORETURN void cplusplus_error (const char *name,
				      const char *fmt, ...)
     ATTR_NORETURN ATTR_FORMAT (printf, 2, 3);

static struct symtab *handle_filename (char **argptr, char *filename_end,
				       int is_quote_enclosed);

static int is_all_digits (char *arg);

static struct
symtabs_and_lines decode_all_digits (char **argptr,
				    struct symtab *default_symtab,
				    int default_line,
				    char ***canonical,
				    struct symtab *file_symtab);

static char *skip_digits (char *arg);

static struct symtabs_and_lines decode_dollar (char **argptr,
					       int funfirstline,
					       struct symtab *default_symtab,
					       char ***canonical,
					       struct symtab *file_symtab);

static struct symtabs_and_lines decode_variable (char **argptr,
						 int funfirstline,
						 char ***canonical,
						 int is_quoted,
						 char *paren_pointer,
						 struct symtab *file_symtab);

static void build_canonical_line_spec (struct symtab_and_line *sal,
				       char *symname,
				       char ***canonical);

static struct
symtabs_and_lines symbol_found (int funfirstline,
				char ***canonical,
				char *copy,
				struct symbol *sym,
				struct symtab *file_symtab,
				struct symtab *sym_symtab);

static struct
symtabs_and_lines minsym_found (int funfirstline,
				struct minimal_symbol *msymbol);

/* The parser of linespec itself.  */

/* Parse a string that specifies a line number.
   Pass the address of a char * variable; that variable will be
   advanced over the characters actually parsed.

   The string can be:

   LINENUM -- that line number in current file.  PC returned is 0.
   FILE:LINENUM -- that line in that file.  PC returned is 0.
   FUNCTION -- line number of openbrace of that function.
   PC returned is the start of the function.
   VARIABLE -- line number of definition of that variable.
   PC returned is 0.
   FILE:FUNCTION -- likewise, but prefer functions in that file.
   *EXPR -- line in which address EXPR appears.

   This may all be followed by an "if EXPR", which we ignore.

   FUNCTION may be an undebuggable function found in minimal symbol table.

   If the argument FUNFIRSTLINE is nonzero, we want the first line
   of real code inside a function when a function is specified, and it is
   not OK to specify a variable or type to get its line number.

   DEFAULT_SYMTAB specifies the file to use if none is specified.
   It defaults to current_source_symtab.
   DEFAULT_LINE specifies the line number to use for relative
   line numbers (that start with signs).  Defaults to current_source_line.
   If CANONICAL is non-NULL, store an array of strings containing the canonical
   line specs there if necessary. Currently overloaded member functions and
   line numbers or static functions without a filename yield a canonical
   line spec. The array and the line spec strings are allocated on the heap,
   it is the callers responsibility to free them.

   Note that it is possible to return zero for the symtab
   if no file is validly specified.  Callers must check that.
   Also, the line number returned may be invalid.  */

/* We allow single quotes in various places.  This is a hideous
   kludge, which exists because the completer can't yet deal with the
   lack of single quotes.  FIXME: write a linespec_completer which we
   can use as appropriate instead of make_symbol_completion_list.  */

/* Everything else in this file is a helper function for
   decode_line_1.  */

/* FIXME: carlton/2002-11-04: It would be nice for ARGPTR to be a
   const char **.  But, alas, that can't easily be fixed right now:
   for one thing, some functions actually do temporarily modify some
   of the chars in question (e.g. is_quoted), and, for another thing,
   GCC would seem not to be to thrilled about implicit conversions
   from char ** to const char **.  */

struct symtabs_and_lines
decode_line_1 (char **argptr, int funfirstline, struct symtab *default_symtab,
	       int default_line, char ***canonical)
{
  /* This is NULL if there are no parens in *ARGPTR, or a pointer to
     the closing parenthesis if there are parens.  */
  char *paren_pointer;
  /* This says whether or not something in *ARGPTR is quoted with
     completer_quotes (i.e. with single quotes).  */
  int is_quoted;
  /* If a file name is specified, this is its symtab.  */
  struct symtab *file_symtab = NULL;
  /* This function advances *ARGPTR, but, for error messages, we want
     to remember what it pointed to initially.  */
  char *saved_arg = *argptr;

  /* Defaults have defaults.  */

  initialize_defaults (&default_symtab, &default_line);
  
  /* Set various flags.
   * 'paren_pointer' is important for overload checking, where
   * we allow things like: 
   *     (gdb) break c::f(int)
   */

  set_flags (*argptr, &is_quoted, &paren_pointer);

  /* See if arg is *PC.  */

  if (**argptr == '*')
    return decode_indirect (argptr);

  /* Check to see if it's a multipart linespec (with colons or
     periods).  */
  {
    char *p;
    /* Is part of *ARGPTR is enclosed in double quotes?  */
    int is_quote_enclosed;
    
    /* Locate the end of the first half of the linespec.  */

    p = locate_first_half (argptr, &is_quote_enclosed);

    /* Does it look like there actually were two parts?  */

    if ((p[0] == ':' || p[0] == '.') && paren_pointer == NULL)
      {
	if (is_quoted)
	  *argptr = *argptr + 1;
      
	/* Is it a C++ or Java compound data structure?  */
      
	if (p[0] == '.' || p[1] == ':')
	  return decode_compound (argptr, funfirstline, canonical,
				  saved_arg, p);

	/* No, the first part is a filename; set file_symtab
	   accordingly.  Also, move argptr past the filename.  */
      
	file_symtab = handle_filename (argptr, p, is_quote_enclosed);
      }
  }

  /* Check whether arg is all digits (and sign).  */

  if (is_all_digits (*argptr))
    return decode_all_digits (argptr, default_symtab, default_line,
			      canonical, file_symtab);

  /* Arg token is not digits => try it as a variable name.  */

  /* If it starts with $: may be a legitimate variable or routine name
     (e.g. HP-UX millicode routines such as $$dyncall), or it may
     be history value, or it may be a convenience variable.  */
  
  if (**argptr == '$')
    return decode_dollar (argptr, funfirstline, default_symtab, canonical,
			  file_symtab);
  
  /* Look up that token as a variable.  If file specified, use that
     file's per-file block to start with.  */

  return decode_variable (argptr, funfirstline, canonical, is_quoted,
			  paren_pointer, file_symtab);
}



/* Now, the helper functions.  */

/* NOTE: carlton/2002-11-04: Some of these have non-obvious side
   effects.  In particular, if a function is passed ARGPTR as an
   argument, it modifies what ARGPTR points to.  (Typically, it
   advances *ARGPTR past whatever substring it has just looked
   at.)  */

/* NOTE: carlton/2002-11-06: Some of these functions are a little
   longer than I would like.  Note, however, that some of the length
   comes from frequent use of alloca, leading to code that can't be
   refactored so easily: you can't return memory allocated with alloca
   from a function.  There are times when it would be nice to program
   in a language with a string datatype.  */

/* First, some functions to initialize stuff at the beggining of the
   function.  */

static void
initialize_defaults (struct symtab **default_symtab, int *default_line)
{
  if (*default_symtab == NULL)
    {
      /* Use whatever we have for the default source line.  We don't use
         get_current_or_default_symtab_and_line as it can recurse and call
	 us back!  */
      struct symtab_and_line cursal
	= get_current_source_symtab_and_line ();
      
      *default_symtab = cursal.symtab;
      *default_line = cursal.line;
    }
}

static void
set_flags (char *arg, int *is_quoted, char **paren_pointer)
{
  char *if_index;
  char *paren_start;
  int has_if = 0;
  
  /* 'has_if' is for the syntax:
   *     (gdb) break foo if (a==b)
   */
  if ((if_index = strstr (arg, " if ")) != NULL ||
      (if_index = strstr (arg, "\tif ")) != NULL ||
      (if_index = strstr (arg, " if\t")) != NULL ||
      (if_index = strstr (arg, "\tif\t")) != NULL ||
      (if_index = strstr (arg, " if(")) != NULL ||
      (if_index = strstr (arg, "\tif( ")) != NULL)
    has_if = 1;
  /* Temporarily zap out "if (condition)" to not confuse the
     parenthesis-checking code below.  This is undone below. Do not
     change if_index!!  */
  if (has_if)
    {
      *if_index = '\0';
    }

  /* Set various flags.
   * 'paren_pointer' is important for overload checking, where
   * we allow things like: 
   *     (gdb) break c::f(int)
   */

  /* Maybe arg is FILE : LINENUM or FILE : FUNCTION */

  *is_quoted = (*arg && (strchr (get_gdb_completer_quote_characters (),
				 *arg)
			 != NULL));

  paren_start = strchr (arg, '(');

  if (paren_start != NULL)
    *paren_pointer = strrchr (paren_start, ')');
  else
    *paren_pointer = NULL;

  /* Now that we're safely past the has_parens check, put back " if
     (condition)" so outer layers can see it.  */
  if (has_if)
    *if_index = ' ';
}



/* Decode arg of the form *PC.  */

static struct symtabs_and_lines
decode_indirect (char **argptr)
{
  struct symtabs_and_lines values;
  CORE_ADDR pc;

  (*argptr)++;				/* Skip the initial asterisk.  */
  pc = parse_and_eval_address_1 (argptr);

  values.sals = xmalloc (sizeof (struct symtab_and_line));

  values.nelts = 1;
  values.sals[0] = find_pc_line (pc, 0);
  values.sals[0].pc = pc;
  values.sals[0].section = find_pc_overlay (pc);

  return values;
}



/* Locate the first half of the linespec, ending in a colon, period,
   or whitespace.  (More or less.)  Also, check to see if *ARGPTR is
   enclosed in double quotes; if so, set is_quote_enclosed, advance
   ARGPTR past that and zero out the trailing double quote.  */

/* FIXME: carlton/2002-11-05: Is zeroing out the trailing double quote
   really the right thing to do?  It never seems to be restored
   anywhere: also, decode_compound ignores is_quote_enclosed entirely,
   while handle_filename seems to expect the double quote to still be
   present.  Sigh...  */

static char *
locate_first_half (char **argptr, int *is_quote_enclosed)
{
  char *comma_index;
  char *p, *p1;
  int has_comma;

  /* Maybe we were called with a line range
     FILENAME:LINENUM,FILENAME:LINENUM and we must isolate the first
     half.  Outer layers will call again later for the second half.

     Don't count commas that appear in argument lists of overloaded
     functions, or in quoted strings.  It's stupid to go to this much
     trouble when the rest of the function is such an obvious roach
     hotel.  */
  
  comma_index = find_toplevel_char (*argptr, ',');
  has_comma = (comma_index != NULL);

  /* Temporarily zap out second half to not confuse the code below.
     This is undone below. Do not change comma_index!!  */
  if (has_comma)
    {
      *comma_index = '\0';
    }

  /* Maybe arg is FILE : LINENUM or FILE : FUNCTION */
  /* May also be CLASS::MEMBER, or NAMESPACE::NAME */
  /* Look for ':', but ignore inside of <> */

  p = *argptr;
  if (p[0] == '"')
    {
      *is_quote_enclosed = 1;
      (*argptr)++;
      p++;
    }
  else
    *is_quote_enclosed = 0;
  
  for (; *p; p++)
    {
      if (p[0] == '<')
	{
	  char *temp_end = find_template_name_end (p);
	  if (!temp_end)
	    error ("malformed template specification in command");
	  p = temp_end;
	}
      
      /* Check for the end of the first half of the linespec.  End of
         line, a tab, a double colon or the last single colon, or a
         space.  But if enclosed in double quotes we do not break on
         enclosed spaces.  */
      if (!*p
	  || p[0] == '\t'
	  || ((p[0] == ':')
	      && ((p[1] == ':') || (strchr (p + 1, ':') == NULL)))
	  || ((p[0] == ' ') && !*is_quote_enclosed))
	break;

      /* Java qualified method.  */
      if (p[0] == '.' && strchr (p, ':') == NULL)
	{
	  /* Find the *last* '.', since the others are package
	     qualifiers.  */
	  for (p1 = p; *p1; p1++)
	    {
	      if (*p1 == '.')
		p = p1;
	    }
	  break;
	}
    }
  
  while (p[0] == ' ' || p[0] == '\t')
    p++;

  /* If the closing double quote was left at the end, remove it.  */
  if (*is_quote_enclosed)
    {
      char *closing_quote = strchr (p - 1, '"');
      if (closing_quote != NULL && closing_quote[1] == '\0')
	*closing_quote = '\0';
    }

  /* Now that we've safely parsed the first half, put back ',' so
     outer layers can see it.  */
  if (has_comma)
    *comma_index = ',';

  return p;
}

/* Find an instance of the character C in the string S that is outside
   of all parenthesis pairs, single-quoted strings, and double-quoted
   strings.  Also, ignore the char within a template name, like a ','
   within foo<int, int>.  */

static char *
find_toplevel_char (char *s, char c)
{
  char quoted = 0;		/* Zero if we're not in quotes;
				   '"' if we're in a double-quoted string;
				   '\'' if we're in a single-quoted string.  */
  int depth = 0;		/* Number of unclosed parens we've seen.  */
  char *scan;

  for (scan = s; *scan; scan++)
    {
      if (quoted)
	{
	  if (*scan == quoted)
	    quoted = 0;
	  else if (*scan == '\\' && *(scan + 1))
	    scan++;
	}
      else if (*scan == c && !quoted && depth == 0)
	return scan;
      else if (*scan == '"' || *scan == '\'')
	quoted = *scan;
      else if (*scan == '(' || *scan == '<')
	depth++;
      else if ((*scan == ')' || *scan == '>') && depth > 0)
	depth--;
    }

  return NULL;
}



/* This handles C++ and Java compound data structures.  After this
   function come a bunch of its helper functions.  (And helper
   functions for the helper functions, and so forth: this is by far
   the messiest part of decode_line_1.)  */

/* NEXT_COMPONENT should point at the first component separator,
   i.e. double-colon or period.  */

static struct symtabs_and_lines
decode_compound (char **argptr, int funfirstline, char ***canonical,
		 char *saved_arg, char *next_component)
{
  char *saved_arg2 = *argptr;
  char *temp_end;
  char *copy;
  struct symtab *sym_symtab;
  struct symbol *sym;
  
  /* First check for "global" namespace specification, of the form
     "::foo".  If found, skip over the colons and jump to normal
     symbol processing.  */
  if (next_component[0] == ':' 
      && ((*argptr == next_component)
	  || (next_component[-1] == ' ')
	  || (next_component[-1] == '\t')))
    saved_arg2 += 2;
      
  /* We have what looks like a class or namespace
     scope specification (A::B), possibly with many
     levels of namespaces or classes (A::B::C::D).

     Some versions of the HP ANSI C++ compiler (as also possibly
     other compilers) generate class/function/member names with
     embedded double-colons if they are inside namespaces. To
     handle this, we loop a few times, considering larger and
     larger prefixes of the string as though they were single
     symbols.  So, if the initially supplied string is
     A::B::C::D::foo, we have to look up "A", then "A::B",
     then "A::B::C", then "A::B::C::D", and finally
     "A::B::C::D::foo" as single, monolithic symbols, because
     A, B, C or D may be namespaces.

     Note that namespaces can nest only inside other
     namespaces, and not inside classes.  So we need only
     consider *prefixes* of the string; there is no need to look up
     "B::C" separately as a symbol in the previous example. */

  while (1)
    {
      struct symtabs_and_lines values;
      char *current_component = next_component;

      /* See if we can get a useful response starting from the current
	 prefix.  */
      if (examine_compound_token (argptr, funfirstline, canonical,
				  saved_arg, next_component, &values))
	return values;
      
      /* Move pointer up to next possible class/namespace token: look
	 for double colons starting after the one we just dealt
	 with.  */
      next_component = current_component + 1;
      while (*next_component
	     && (next_component[0] != ' ')
	     && (next_component[0] != '\t')
	     && (next_component[0] != '\''))
	{
	  if (next_component[0] == '<')
	    {
	      temp_end = find_template_name_end (next_component);
	      if (!temp_end)
		error ("malformed template specification in command");
	      next_component = temp_end;
	    }
	  else if ((next_component[0] == ':')
		   && (next_component[1] == ':'))
	    break;	/* Found double-colon.  */
	  else
	    next_component++;
	}

      if (*next_component != ':')
	break;
	  
      *argptr = saved_arg2;	/* Restore argptr.  */
    }

  /* Last chance attempt -- check entire name as a symbol.  */
  copy = alloca (next_component - saved_arg2 + 1);
  memcpy (copy, saved_arg2, next_component - saved_arg2);
  /* Note: if is_quoted should be true, we snuff out quote here anyway.  */
  copy[next_component - saved_arg2] = '\0';
  /* Set argptr to skip over the name.  */
  *argptr = (*next_component == '\'') ? next_component + 1 : next_component;
  /* Look up entire name.  */
  sym = lookup_symbol (copy, 0, VAR_NAMESPACE, 0, &sym_symtab);

  if (sym != NULL)
    return symbol_found (funfirstline, canonical, copy,
			 sym, NULL, sym_symtab);

  /* Couldn't find any interpretation as classes/namespaces, so give up.  */
  /* The quotes are important if copy is empty.  */
  cplusplus_error (saved_arg,
		   "Can't find member of namespace, class, struct, or union named \"%s\"\n",
		   copy);
}

/* This checks to see if we can get a useful response by treating the
   substring of *ARGPTR ending at CURRENT_COMPONENT as a class name,
   and the rest as a method name.  If so, it puts a return value for
   decode_line_1 in *VALUES and returns 1; otherwise, it returns
   0.  */

static int
examine_compound_token (char **argptr, int funfirstline,
			char ***canonical, 
			char *saved_arg, char *current_component,
			struct symtabs_and_lines *values)
{
  char *copy;
  struct symbol *class_sym;
  struct type *t;
  
  /* If CURRENT_COMPONENT points at the end of a name of a class, find
     the corresponding symbol and advance ARGPTR past the end of the
     class.  */

  class_sym = locate_class_sym (argptr, current_component);

  if (class_sym == NULL)
    return 0;

  t = check_typedef (SYMBOL_TYPE (class_sym));

  switch (TYPE_CODE (t))
    {
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      /* Find the next token (everything up to end or next blank).  */

      current_component = find_next_token (argptr);
      copy = alloca (current_component - *argptr + 1);
      memcpy (copy, *argptr, current_component - *argptr);
      copy[current_component - *argptr] = '\0';
      if (current_component != *argptr
	  && copy[current_component - *argptr - 1]
	  && (strchr (get_gdb_completer_quote_characters (),
		      copy[current_component - *argptr - 1])
	      != NULL))
	copy[current_component - *argptr - 1] = '\0';
	      
      while (*current_component == ' ' || *current_component == '\t')
	current_component++;
      *argptr = current_component;

      *values = find_method (funfirstline, canonical, saved_arg, copy,
			     t, class_sym);
      
      return 1;
    case TYPE_CODE_NAMESPACE:
      return 0;
    default:
      /* FIXME: carlton/2002-11-19: Once this all settles down, this
	 case should be an error rather than a return 0; that will
	 allow us to make VALUES the return value rather than an
	 argument.  */
      return 0;
    }
}

static struct symbol *
locate_class_sym (char **argptr, char *p)
{
  char *p1;
  char *copy;
      
  /* Extract the class name.  */
  p1 = p;
  while (p != *argptr && p[-1] == ' ')
    --p;
  copy = alloca (p - *argptr + 1);
  memcpy (copy, *argptr, p - *argptr);
  copy[p - *argptr] = 0;
	  
  /* Discard the class name from the arg.  */
  p = p1 + (p1[0] == ':' ? 2 : 1);
  while (*p == ' ' || *p == '\t')
    p++;
  *argptr = p;

  return lookup_symbol (copy, NULL, STRUCT_NAMESPACE, NULL, NULL);
}

/* Find the next token (presumably a method name); if some of it is
   quoted, advance ARGPTR past the opening quote.  */

static char *
find_next_token (char **argptr)
{
  char *p;
  
  if (**argptr
      && (strchr (get_gdb_completer_quote_characters (),
		  **argptr)
	  != NULL))
    {
      p = skip_quoted (*argptr);
      *argptr = *argptr + 1;
    }
  else
    {
      p = *argptr;
      while (*p && *p != ' ' && *p != '\t' && *p != ',' && *p != ':')
	p++;
    }

  return p;
}

/* Return the number of methods described for TYPE, including the
   methods from types it derives from. This can't be done in the symbol
   reader because the type of the baseclass might still be stubbed
   when the definition of the derived class is parsed.  */

static int
total_number_of_methods (struct type *type)
{
  int n;
  int count;

  CHECK_TYPEDEF (type);
  if (TYPE_CPLUS_SPECIFIC (type) == NULL)
    return 0;
  count = TYPE_NFN_FIELDS_TOTAL (type);

  for (n = 0; n < TYPE_N_BASECLASSES (type); n++)
    count += total_number_of_methods (TYPE_BASECLASS (type, n));

  return count;
}

/* This finds the method METHOD in the class whose type is CLASS_TYPE
   and whose symbol is CLASS_SYM.  */

/* FIXME: carlton/2002-11-06: we only use CLASS_SYM to get the name of
   the class; probably we should just get that from CLASS_TYPE
   instead.  */

static struct symtabs_and_lines
find_method (int funfirstline, char ***canonical, char *saved_arg,
	     char *method, struct type *class_type,
	     struct symbol *class_sym)
{
  struct symtabs_and_lines values;
  struct symbol **sym_arr;
  int method_count;

  sym_arr = alloca (total_number_of_methods (class_type)
		    * sizeof (struct symbol *));

  /* Find all methods with a matching name, and put them in
     sym_arr.  */
  method_count = count_methods (class_type, method, sym_arr);

  if (method_count == 1)
    {
      /* There is exactly one field with that name.  */
      struct symbol *sym = sym_arr[0];

      if (sym != NULL && SYMBOL_CLASS (sym) == LOC_BLOCK)
	{
	  values.sals = xmalloc (sizeof (struct symtab_and_line));
	  values.nelts = 1;
	  values.sals[0] = find_function_start_sal (sym, funfirstline);
	}
      else
	{
	  values.nelts = 0;
	}
    }
  else if (method_count > 0)
    {
      /* There is more than one field with that name
	 (overloaded).  Ask the user which one to use.  */
      values = select_symbols (sym_arr, method_count, funfirstline, canonical);
    }
  else
    {
      char *tmp;
	  
      if (is_operator_name (method))
	{
	  tmp = alloca (strlen (method + 3) + 9);
	  strcpy (tmp, "operator ");
	  strcat (tmp, method + 3);
	}
      else
	tmp = method;
      if (tmp[0] == '~')
	cplusplus_error (saved_arg,
			 "the class `%s' does not have destructor defined\n",
			 SYMBOL_PRINT_NAME (class_sym));
      else
	cplusplus_error (saved_arg,
			 "the class %s does not have any method named %s\n",
			 SYMBOL_PRINT_NAME (class_sym), tmp);
    }

  return values;
}

/* Find all methods in the class whose type is CLASS_TYPE whose name
   is METHOD, and put them in SYM_ARR.  Return the number of methods
   found.  */

static int
count_methods (struct type *class_type, char *method,
	       struct symbol **sym_arr)
{
  int count = 0;	/* Counter for the symbol array.  */
  
  if (destructor_name_p (method, class_type))
    {
      /* Destructors are a special case.  */
      int m_index, f_index;
      
      if (get_destructor_fn_field (class_type, &m_index, &f_index))
	{
	  struct fn_field *f = TYPE_FN_FIELDLIST1 (class_type, m_index);
	  
	  sym_arr[count] =
	    lookup_symbol_linkage (TYPE_FN_FIELD_PHYSNAME (f, f_index));
	  if (sym_arr[count])
	    count++;
	}
    }
  else
    count = find_methods (class_type, method, sym_arr);

  return count;
}

/* This does most of the legwork for count_methods, and the arguments
   are the same.  */

static int
find_methods (struct type *class_type, char *method,
	      struct symbol **sym_arr)
{
  int count = 0;
  int ibase;
  const char *class_name = type_name_no_tag (class_type);

  /* Ignore this class if it doesn't have a name.  This is ugly, but
     unless we figure out how to get the physname without the name of
     the class, then the loop can't do any good.  */
  if (class_name != NULL
      && (lookup_symbol (class_name, NULL, STRUCT_NAMESPACE, NULL,
			 NULL) != NULL))
    {
      int method_counter;
      int name_len = strlen (method);

      check_typedef (class_type);

      /* Loop over each method name.  At this level, all overloads of a name
         are counted as a single name.  There is an inner loop which loops over
         each overload.  */

      for (method_counter = TYPE_NFN_FIELDS (class_type) - 1;
	   method_counter >= 0;
	   --method_counter)
	{
	  int field_counter;
	  char dem_opname[64];
	  const char *current_method_name
	    = find_method_name (class_type, method_counter, dem_opname);

	  if (strcmp_iw (method, current_method_name) == 0)
	    /* Find all the overloaded methods with that name.  */
	    count += add_matching_methods (method_counter, class_type,
					   sym_arr + count);
	  else if (strncmp (class_name, method, name_len) == 0
		   && (class_name[name_len] == '\0'
		       || class_name[name_len] == '<'))
	    count += add_constructors (method_counter, class_type,
				       sym_arr + count);
	}
    }

  /* Only search baseclasses if there is no match yet, since names in
     derived classes override those in baseclasses.

     FIXME: The above is not true; it is only true of member functions
     if they have the same number of arguments (??? - section 13.1 of the
     ARM says the function members are not in the same scope but doesn't
     really spell out the rules in a way I understand.  In any case, if
     the number of arguments differ this is a case in which we can overload
     rather than hiding without any problem, and gcc 2.4.5 does overload
     rather than hiding in this case).  */

  if (count == 0)
    for (ibase = 0; ibase < TYPE_N_BASECLASSES (class_type); ibase++)
      count += find_methods (TYPE_BASECLASS (class_type, ibase), method,
			     sym_arr + count);

  return count;
}

/* Return the name of the METHOD_COUNTER'th method of the class whose
   type is CLASS_TYPE.  If demangling is necessary, DEM_OPNAME will be
   used as storage space.  Note that this function is g++
   specific.  */

static const char *
find_method_name (struct type *class_type, int method_counter,
		  char *dem_opname)
{
  const char *name = TYPE_FN_FIELDLIST_NAME (class_type, method_counter);
  if (strncmp (name, "__", 2) == 0
      || strncmp (name, "op", 2) == 0
      || strncmp (name, "type", 4) == 0)
    {
      if (cplus_demangle_opname (name, dem_opname, DMGL_ANSI))
	name = dem_opname;
      else if (cplus_demangle_opname (name, dem_opname, 0))
	name = dem_opname;
    }

  return name;
}

/* Add the symbols associated to methods of the class whose type is
   CLASS_TYPE and whose name matches the method indexed by
   METHOD_COUNTER to the array SYM_ARR.  Return the number of methods
   added.  */
static int
add_matching_methods (int method_counter, struct type *class_type,
		      struct symbol **sym_arr)
{
  int field_counter;
  int count = 0;
  
  for (field_counter
	 = TYPE_FN_FIELDLIST_LENGTH (class_type, method_counter) - 1;
       field_counter >= 0;
       --field_counter)
    {
      struct fn_field *f;
      char *phys_name;

      f = TYPE_FN_FIELDLIST1 (class_type, method_counter);

      if (TYPE_FN_FIELD_STUB (f, field_counter))
	{
	  char *tmp_name;

	  tmp_name = gdb_mangle_name (class_type, method_counter,
				      field_counter);
	  phys_name = alloca (strlen (tmp_name) + 1);
	  strcpy (phys_name, tmp_name);
	  xfree (tmp_name);
	}
      else
	phys_name = TYPE_FN_FIELD_PHYSNAME (f, field_counter);
		
      /* Destructor is handled by caller; don't add it to the list.  */
      if (is_destructor_name (phys_name) != 0)
	continue;

      sym_arr[count] = lookup_symbol_linkage (phys_name);
      if (sym_arr[count])
	count++;
      else
	{
	  /* This error message gets printed, but the method
	     still seems to be found
	     fputs_filtered("(Cannot find method ", gdb_stdout);
	     fprintf_symbol_filtered (gdb_stdout, phys_name,
	     language_cplus,
	     DMGL_PARAMS | DMGL_ANSI);
	     fputs_filtered(" - possibly inlined.)\n", gdb_stdout);
	  */
	}
    }

  return count;
}

/* If we're looking for a constructor, add the symbols associated to
   methods of the class whose type is CLASS_TYPE and which are indexed
   by by METHOD_COUNTER to the array SYM_ARR.  Return the number of
   methods added.  */

static int
add_constructors (int method_counter, struct type *class_type,
		  struct symbol **sym_arr)
{
  int field_counter;
  int count = 0;

  /* For GCC 3.x and stabs, constructors and destructors have names
     like __base_ctor and __complete_dtor.  Check the physname for now
     if we're looking for a constructor.  */
  for (field_counter
	 = TYPE_FN_FIELDLIST_LENGTH (class_type, method_counter) - 1;
       field_counter >= 0;
       --field_counter)
    {
      struct fn_field *f;
      char *phys_name;
		  
      f = TYPE_FN_FIELDLIST1 (class_type, method_counter);

      /* GCC 3.x will never produce stabs stub methods, so we don't need
	 to handle this case.  */
      if (TYPE_FN_FIELD_STUB (f, field_counter))
	continue;
      phys_name = TYPE_FN_FIELD_PHYSNAME (f, field_counter);
      if (! is_constructor_name (phys_name))
	continue;

      /* If this method is actually defined, include it in the
	 list.  */
      sym_arr[count] = lookup_symbol_linkage (phys_name);
      if (sym_arr[count])
	count++;
    }

  return count;
}

/* Given a list of NELTS symbols in SYM_ARR, return a list of lines to
   operate on (ask user if necessary).
   If CANONICAL is non-NULL return a corresponding array of mangled names
   as canonical line specs there.  */

static struct symtabs_and_lines
select_symbols (struct symbol **sym_arr, int nelts, int funfirstline,
		char ***canonical)
{
  struct symtabs_and_lines values, return_values;
  char *args, *arg1;
  int i;
  char *prompt;
  char *symname;
  struct cleanup *old_chain = NULL;
  char **canonical_arr = NULL;

  values.sals = alloca (nelts * sizeof (struct symtab_and_line));

  if (canonical)
    {
      canonical_arr = xmalloc (nelts * sizeof (char *));
      old_chain = make_cleanup (xfree, canonical_arr);
      memset (canonical_arr, 0, nelts * sizeof (char *));
      *canonical = canonical_arr;
    }

  i = 0;
  printf_unfiltered ("[0] cancel\n[1] all\n");
  while (i < nelts)
    {
      init_sal (&values.sals[i]);	/* Initialize to zeroes.  */
      if (sym_arr[i] && SYMBOL_CLASS (sym_arr[i]) == LOC_BLOCK)
	{
	  values.sals[i] = find_function_start_sal (sym_arr[i], funfirstline);
	  printf_unfiltered ("[%d] %s at %s:%d\n",
			     (i + 2),
			     SYMBOL_PRINT_NAME (sym_arr[i]),
			     values.sals[i].symtab->filename,
			     values.sals[i].line);
	}
      else
	printf_unfiltered ("?HERE\n");
      i++;
    }

  if ((prompt = getenv ("PS2")) == NULL)
    {
      prompt = "> ";
    }
  args = command_line_input (prompt, 0, "overload-choice");

  if (args == 0 || *args == 0)
    error_no_arg ("one or more choice numbers");

  return select_symbols_args (sym_arr, nelts, args, canonical_arr,
			      values, old_chain);
}

/* This is the part of select_symbols that actually handles the user's
   response.  */

static struct symtabs_and_lines
select_symbols_args (struct symbol **sym_arr, int nelts,
		     char *args, char **canonical_arr,
		     struct symtabs_and_lines values,
		     struct cleanup *old_chain)
{
  struct symtabs_and_lines return_values;
  struct cleanup *new_chain;
  int i;
  
  return_values.sals = xmalloc (nelts * sizeof (struct symtab_and_line));
  new_chain = make_cleanup (xfree, return_values.sals);
  if (old_chain == NULL)
    old_chain = new_chain;

  for (i = 0; i < nelts; ++i)
    init_sal (&return_values.sals[i]);

  i = 0;
  while (*args)
    {
      int num;
      char *arg1;

      arg1 = args;
      while (*arg1 >= '0' && *arg1 <= '9')
	arg1++;
      if (*arg1 && *arg1 != ' ' && *arg1 != '\t')
	error ("Arguments must be choice numbers.");

      num = atoi (args);

      if (num == 0)
	error ("canceled");
      else if (num == 1)
	{
	  if (canonical_arr != NULL)
	    {
	      int j;
	      
	      for (j = 0; j < nelts; j++)
		{
		  if (canonical_arr[j] == NULL)
		    {
		      char *symname = SYMBOL_NAME (sym_arr[j]);
		      canonical_arr[j]
			= savestring (symname, strlen (symname));
		    }
		}
	    }
	  memcpy (return_values.sals, values.sals,
		  (nelts * sizeof (struct symtab_and_line)));
	  return_values.nelts = nelts;
	  discard_cleanups (old_chain);
	  return return_values;
	}

      if (num >= nelts + 2)
	{
	  printf_unfiltered ("No choice number %d.\n", num);
	}
      else
	{
	  num -= 2;
	  if (values.sals[num].pc)
	    {
	      if (canonical_arr != NULL)
		{
		  char *symname = SYMBOL_NAME (sym_arr[num]);
		  make_cleanup (xfree, symname);
		  canonical_arr[i] = savestring (symname, strlen (symname));
		}
	      return_values.sals[i++] = values.sals[num];
	      values.sals[num].pc = 0;
	    }
	  else
	    {
	      printf_unfiltered ("duplicate request for %d ignored.\n", num);
	    }
	}

      args = arg1;
      while (*args == ' ' || *args == '\t')
	args++;
    }
  return_values.nelts = i;
  discard_cleanups (old_chain);
  return return_values;
}

/* Issue a helpful hint on using the command completion feature on
   single quoted demangled C++ symbols as part of the completion
   error.  */

static NORETURN void
cplusplus_error (const char *name, const char *fmt, ...)
{
  struct ui_file *tmp_stream;
  tmp_stream = mem_fileopen ();
  make_cleanup_ui_file_delete (tmp_stream);

  {
    va_list args;
    va_start (args, fmt);
    vfprintf_unfiltered (tmp_stream, fmt, args);
    va_end (args);
  }

  while (*name == '\'')
    name++;
  fprintf_unfiltered (tmp_stream,
		      ("Hint: try '%s<TAB> or '%s<ESC-?>\n"
		       "(Note leading single quote.)"),
		      name, name);
  error_stream (tmp_stream);
}



/* Return the symtab associated to the filename given by the substring
   of *ARGPTR ending at FILE_NAME_END.  */

static struct symtab *
handle_filename (char **argptr, char *filename_end, int is_quote_enclosed)
{
  char *saved_filename_end;
  char *copy;
  struct symtab *file_symtab;
  
  /* Extract the file name.  */
  saved_filename_end = filename_end;
  while (filename_end != *argptr && filename_end[-1] == ' ')
    --filename_end;
  if ((*filename_end == '"') && is_quote_enclosed)
    --filename_end;
  copy = alloca (filename_end - *argptr + 1);
  memcpy (copy, *argptr, filename_end - *argptr);
  /* It may have the ending quote right after the file name.  */
  if (is_quote_enclosed && copy[filename_end - *argptr - 1] == '"')
    copy[filename_end - *argptr - 1] = 0;
  else
    copy[filename_end - *argptr] = 0;
      
  /* Find that file's data.  */
  file_symtab = lookup_symtab (copy);
  if (file_symtab == NULL)
    {
      if (!have_full_symbols () && !have_partial_symbols ())
	error ("No symbol table is loaded.  Use the \"file\" command.");
      error ("No source file named %s.", copy);
    }
  
  /* Discard the file name from the arg.  */
  filename_end = saved_filename_end + 1;
  while (*filename_end == ' ' || *filename_end == '\t')
    filename_end++;
  *argptr = filename_end;

  return file_symtab;
}



/* Functions related to cases where the arg is all digits.  */

static int
is_all_digits (char *arg)
{
  char *q = skip_digits (arg);

  return q != arg && (*q == 0 || *q == ' ' || *q == '\t' || *q == ',');
}

static struct symtabs_and_lines
decode_all_digits (char **argptr, struct symtab *default_symtab,
		   int default_line, char ***canonical,
		   struct symtab *file_symtab)
{
  struct symtabs_and_lines values;
  struct symtab_and_line val;
  char *end_of_digits = skip_digits (*argptr);

  enum sign
    {
      none, plus, minus
    }
  sign = none;

  /* We might need a canonical line spec if no file was specified.  */
  int need_canonical = (file_symtab == NULL) ? 1 : 0;

  init_sal (&val);

  /* This is where we need to make sure that we have good defaults.
     We must guarantee that this section of code is never executed
     when we are called with just a function name, since
     set_default_source_symtab_and_line uses
     select_source_symtab that calls us with such an argument.  */

  if (file_symtab == NULL && default_symtab == NULL)
    {
      /* Make sure we have at least a default source file.  */
      set_default_source_symtab_and_line ();
      initialize_defaults (&default_symtab, &default_line);
    }

  if (**argptr == '+')
    sign = plus, (*argptr)++;
  else if (**argptr == '-')
    sign = minus, (*argptr)++;
  val.line = atoi (*argptr);
  switch (sign)
    {
    case plus:
      if (end_of_digits == *argptr)
	val.line = 5;
      if (file_symtab == NULL)
	val.line = default_line + val.line;
      break;
    case minus:
      if (end_of_digits == *argptr)
	val.line = 15;
      if (file_symtab == NULL)
	val.line = default_line - val.line;
      else
	val.line = 1;
      break;
    case none:
      break;		/* No need to adjust val.line.  */
    }

  while (*end_of_digits == ' ' || *end_of_digits == '\t')
    end_of_digits++;
  *argptr = end_of_digits;
  if (file_symtab == NULL)
    file_symtab = default_symtab;

  /* It is possible that this source file has more than one symtab, 
     and that the new line number specification has moved us from the
     default (in file_symtab) to a new one.  */
  val.symtab = find_line_symtab (file_symtab, val.line, NULL, NULL);
  if (val.symtab == NULL)
    val.symtab = file_symtab;

  val.pc = 0;
  values.sals = xmalloc (sizeof (struct symtab_and_line));
  values.sals[0] = val;
  values.nelts = 1;
  if (need_canonical)
    build_canonical_line_spec (values.sals, NULL, canonical);
  return values;
}

static char *
skip_digits (char *arg)
{
  if (*arg == '-' || *arg == '+')
    arg++;
  while (*arg >= '0' && *arg <= '9')
    arg++;

  return arg;
}



/* Decode a linespec starting with a dollar sign.  */

static struct symtabs_and_lines
decode_dollar (char **argptr, int funfirstline, struct symtab *default_symtab,
	       char ***canonical, struct symtab *file_symtab)
{
  struct value *valx;
  int index = 0;
  int need_canonical = 0;
  char *p;
  struct symtabs_and_lines values;
  char *copy;

  /* One or two $ chars possible.  */
  p = skip_quoted (*argptr + (((*argptr)[1] == '$') ? 2 : 1));

  copy = alloca (p - *argptr + 1);
  memcpy (copy, *argptr, p - *argptr);
  copy[p - *argptr] = '\0';

  while (*p == ' ' || *p == '\t')
    p++;
  *argptr = p;
  
  p = (copy[1] == '$') ? copy + 2 : copy + 1;
  while (*p >= '0' && *p <= '9')
    p++;
  if (!*p)		/* Reached end of token without hitting non-digit.  */
    {
      /* We have a value history reference.  */
      sscanf ((copy[1] == '$') ? copy + 2 : copy + 1, "%d", &index);
      valx = access_value_history ((copy[1] == '$') ? -index : index);
      if (TYPE_CODE (VALUE_TYPE (valx)) != TYPE_CODE_INT)
	error ("History values used in line specs must have integer values.");
    }
  else
    {
      /* Not all digits -- may be user variable/function or a
	 convenience variable.  */

      struct symbol *sym;
      struct symtab *sym_symtab;
      struct minimal_symbol *msymbol;
      
      /* Look up entire name as a symbol first.  */
      sym = lookup_symbol (copy, 0, VAR_NAMESPACE, 0, &sym_symtab);
      if (sym != NULL)
	return symbol_found (funfirstline, canonical, copy,
			     sym, NULL, sym_symtab);

      /* If symbol was not found, look in minimal symbol tables.  */
      msymbol = lookup_minimal_symbol (copy, NULL, NULL);
      if (msymbol != NULL)
	return minsym_found (funfirstline, msymbol);

      /* Not a user variable or function -- must be convenience variable.  */
      file_symtab = NULL;
      need_canonical = 1;
      valx = value_of_internalvar (lookup_internalvar (copy + 1));
      if (TYPE_CODE (VALUE_TYPE (valx)) != TYPE_CODE_INT)
	error ("Convenience variables used in line specs must have integer values.");
    }

  /* Either history value or convenience value from above, in valx.  */
  values.sals = xmalloc (sizeof (struct symtab_and_line));
  init_sal (values.sals);
  values.sals[0].symtab  = file_symtab != NULL ? file_symtab : default_symtab;
  values.sals[0].line = value_as_long (valx);
  values.sals[0].pc = 0;
  values.nelts = 1;

  if (need_canonical)
    build_canonical_line_spec (values.sals, NULL, canonical);

  return values;
}



/* Decode a linespec that's a variable.  If FILE_SYMTAB is non-NULL,
   look in that file's static variables first.  */

static struct symtabs_and_lines
decode_variable (char **argptr, int funfirstline, char ***canonical,
		 int is_quoted, char *paren_pointer,
		 struct symtab *file_symtab)
{
  char *p;
  char *copy;
  struct symbol *sym;
  struct symtab *sym_symtab;
  struct minimal_symbol *msymbol;

  if (is_quoted)
    {
      p = skip_quoted (*argptr);
      if (p[-1] != '\'')
	error ("Unmatched single quote.");
    }
  else if (paren_pointer != NULL)
    {
      p = paren_pointer + 1;
    }
  else
    {
      p = skip_quoted (*argptr);
    }

  copy = alloca (p - *argptr + 1);
  memcpy (copy, *argptr, p - *argptr);
  copy[p - *argptr] = '\0';

  /* If it's completer-quoted, get rid of the quotes.  */

  if (p != *argptr
      && copy[0]
      && copy[0] == copy[p - *argptr - 1]
      && strchr (get_gdb_completer_quote_characters (), copy[0]) != NULL)
    {
      copy[p - *argptr - 1] = '\0';
      copy++;
    }
  
  while (*p == ' ' || *p == '\t')
    p++;
  *argptr = p;

  /* Look up that token as a variable.
     If file specified, use that file's per-file block to start with.  */

  sym = lookup_symbol (copy,
		       (file_symtab != NULL
			? BLOCKVECTOR_BLOCK (BLOCKVECTOR (file_symtab),
					     STATIC_BLOCK)
			: get_selected_block (0)),
		       VAR_NAMESPACE, NULL, &sym_symtab);

  if (sym != NULL)
    {
      return symbol_found (funfirstline, canonical, copy,
			   sym, file_symtab, sym_symtab);
    }

  msymbol = lookup_minimal_symbol (copy, NULL, NULL);

  if (msymbol != NULL)
    return minsym_found (funfirstline, msymbol);

  if (!have_full_symbols () &&
      !have_partial_symbols () && !have_minimal_symbols ())
    error ("No symbol table is loaded.  Use the \"file\" command.");

  error ("Function \"%s\" not defined.", copy);
}



/* Build a canonical line spec in CANONICAL if it is non-NULL and if
   the SAL has a symtab.
   If SYMNAME is non-NULL the canonical line spec is `filename:symname'.
   If SYMNAME is NULL the line number from SAL is used and the canonical
   line spec is `filename:linenum'.  */

static void
build_canonical_line_spec (struct symtab_and_line *sal, char *symname,
			   char ***canonical)
{
  char **canonical_arr;
  char *canonical_name;
  char *filename;
  struct symtab *sal_symtab = sal->symtab;

  if (sal_symtab == NULL
      || sal_symtab->filename == NULL
      || canonical == NULL)
    return;

  canonical_arr = xmalloc (sizeof (char *));
  *canonical = canonical_arr;

  filename = sal_symtab->filename;
  if (symname != NULL)
    {
      canonical_name = xmalloc (strlen (filename) + strlen (symname) + 2);
      sprintf (canonical_name, "%s:%s", filename, symname);
    }
  else
    {
      canonical_name = xmalloc (strlen (filename) + 30);
      sprintf (canonical_name, "%s:%d", filename, sal->line);
    }
  canonical_arr[0] = canonical_name;
}

/* We've found a symbol SYM to associate with our linespec; build a
   corresponding struct symtabs_and_lines.  */

static struct symtabs_and_lines
symbol_found (int funfirstline, char ***canonical, char *copy,
	      struct symbol *sym, struct symtab *file_symtab,
	      struct symtab *sym_symtab)
{
  struct symtabs_and_lines values;
  
  if (SYMBOL_CLASS (sym) == LOC_BLOCK)
    {
      /* Arg is the name of a function.  */
      values.sals = xmalloc (sizeof (struct symtab_and_line));
      values.sals[0] = find_function_start_sal (sym, funfirstline);
      values.nelts = 1;

      /* Don't use the SYMBOL_LINE; if used at all it points to
	 the line containing the parameters or thereabouts, not
	 the first line of code.  */

      /* We might need a canonical line spec if it is a static
	 function.  */
      if (file_symtab == NULL)
	{
	  struct blockvector *bv = BLOCKVECTOR (sym_symtab);
	  struct block *b = BLOCKVECTOR_BLOCK (bv, STATIC_BLOCK);
	  if (lookup_block_symbol (b, copy, NULL, VAR_NAMESPACE) != NULL)
	    build_canonical_line_spec (values.sals, copy, canonical);
	}
      return values;
    }
  else
    {
      if (funfirstline)
	error ("\"%s\" is not a function", copy);
      else if (SYMBOL_LINE (sym) != 0)
	{
	  /* We know its line number.  */
	  values.sals = xmalloc (sizeof (struct symtab_and_line));
	  values.nelts = 1;
	  memset (&values.sals[0], 0, sizeof (values.sals[0]));
	  values.sals[0].symtab = sym_symtab;
	  values.sals[0].line = SYMBOL_LINE (sym);
	  return values;
	}
      else
	/* This can happen if it is compiled with a compiler which doesn't
	   put out line numbers for variables.  */
	/* FIXME: Shouldn't we just set .line and .symtab to zero
	   and return?  For example, "info line foo" could print
	   the address.  */
	error ("Line number not known for symbol \"%s\"", copy);
    }
}

/* We've found a minimal symbol MSYMBOL to associate with our
   linespec; build a corresponding struct symtabs_and_lines.  */

static struct symtabs_and_lines
minsym_found (int funfirstline, struct minimal_symbol *msymbol)
{
  struct symtabs_and_lines values;
  
  values.sals = xmalloc (sizeof (struct symtab_and_line));
  values.sals[0] = find_pc_sect_line (SYMBOL_VALUE_ADDRESS (msymbol),
				      NULL, 0);
  values.sals[0].section = SYMBOL_BFD_SECTION (msymbol);
  if (funfirstline)
    {
      values.sals[0].pc += FUNCTION_START_OFFSET;
      values.sals[0].pc = SKIP_PROLOGUE (values.sals[0].pc);
    }
  values.nelts = 1;
  return values;
}
