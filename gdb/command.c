/* Library for reading command lines and decoding commands.
   Copyright (C) 1986 Free Software Foundation, Inc.

		       NO WARRANTY

  BECAUSE THIS PROGRAM IS LICENSED FREE OF CHARGE, WE PROVIDE ABSOLUTELY
NO WARRANTY, TO THE EXTENT PERMITTED BY APPLICABLE STATE LAW.  EXCEPT
WHEN OTHERWISE STATED IN WRITING, FREE SOFTWARE FOUNDATION, INC,
RICHARD M. STALLMAN AND/OR OTHER PARTIES PROVIDE THIS PROGRAM "AS IS"
WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY
AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE
DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
CORRECTION.

 IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW WILL RICHARD M.
STALLMAN, THE FREE SOFTWARE FOUNDATION, INC., AND/OR ANY OTHER PARTY
WHO MAY MODIFY AND REDISTRIBUTE THIS PROGRAM AS PERMITTED BELOW, BE
LIABLE TO YOU FOR DAMAGES, INCLUDING ANY LOST PROFITS, LOST MONIES, OR
OTHER SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE
USE OR INABILITY TO USE (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR
DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY THIRD PARTIES OR
A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS) THIS
PROGRAM, EVEN IF YOU HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY.

		GENERAL PUBLIC LICENSE TO COPY

  1. You may copy and distribute verbatim copies of this source file
as you receive it, in any medium, provided that you conspicuously and
appropriately publish on each copy a valid copyright notice "Copyright
(C) 1986 Free Software Foundation, Inc."; and include following the
copyright notice a verbatim copy of the above disclaimer of warranty
and of this License.  You may charge a distribution fee for the
physical act of transferring a copy.

  2. You may modify your copy or copies of this source file or
any portion of it, and copy and distribute such modifications under
the terms of Paragraph 1 above, provided that you also do the following:

    a) cause the modified files to carry prominent notices stating
    that you changed the files and the date of any change; and

    b) cause the whole of any work that you distribute or publish,
    that in whole or in part contains or is a derivative of this
    program or any part thereof, to be licensed at no charge to all
    third parties on terms identical to those contained in this
    License Agreement (except that you may choose to grant more extensive
    warranty protection to some or all third parties, at your option).

    c) You may charge a distribution fee for the physical act of
    transferring a copy, and you may at your option offer warranty
    protection in exchange for a fee.

Mere aggregation of another unrelated program with this program (or its
derivative) on a volume of a storage or distribution medium does not bring
the other program under the scope of these terms.

  3. You may copy and distribute this program (or a portion or derivative
of it, under Paragraph 2) in object code or executable form under the terms
of Paragraphs 1 and 2 above provided that you also do one of the following:

    a) accompany it with the complete corresponding machine-readable
    source code, which must be distributed under the terms of
    Paragraphs 1 and 2 above; or,

    b) accompany it with a written offer, valid for at least three
    years, to give any third party free (except for a nominal
    shipping charge) a complete machine-readable copy of the
    corresponding source code, to be distributed under the terms of
    Paragraphs 1 and 2 above; or,

    c) accompany it with the information you received as to where the
    corresponding source code may be obtained.  (This alternative is
    allowed only for noncommercial distribution and only if you
    received the program in object code or executable form alone.)

For an executable file, complete source code means all the source code for
all modules it contains; but, as a special exception, it need not include
source code for modules which are standard libraries that accompany the
operating system on which the executable file runs.

  4. You may not copy, sublicense, distribute or transfer this program
except as expressly provided under this License Agreement.  Any attempt
otherwise to copy, sublicense, distribute or transfer this program is void and
your rights to use the program under this License agreement shall be
automatically terminated.  However, parties who have received computer
software programs from you with this License Agreement will not have
their licenses terminated so long as such parties remain in full compliance.

  5. If you wish to incorporate parts of this program into other free
programs whose distribution conditions are different, write to the Free
Software Foundation at 675 Mass Ave, Cambridge, MA 02139.  We have not yet
worked out a simple rule that can be stated here, but we will often permit
this.  We will be guided by the two goals of preserving the free status of
all derivatives of our free software and of promoting the sharing and reuse of
software.


In other words, you are welcome to use, share and improve this program.
You are forbidden to forbid anyone else to use, share and improve
what you give them.   Help stamp out software-hoarding!  */


#include "command.h"
#include <stdio.h>

extern char *xmalloc ();

static char *savestring ();

/* Add element named NAME to command list *LIST.
   FUN should be the function to execute the command;
   it will get a character string as argument, with leading
   and trailing blanks already eliminated.

   DOC is a documentation string for the command.
   Its first line should be a complete sentence.
   It should start with ? for a command that is an abbreviation
   or with * for a command that most users don't need to know about.  */

struct cmd_list_element *
add_cmd (name, class, fun, doc, list)
     char *name;
     int class;
     void (*fun) ();
     char *doc;
     struct cmd_list_element **list;
{
  register struct cmd_list_element *c
    = (struct cmd_list_element *) xmalloc (sizeof (struct cmd_list_element));

  delete_cmd (name, list);
  c->next = *list;
  c->name = savestring (name, strlen (name));
  c->class = class;
  c->function = fun;
  c->doc = doc;
  c->prefixlist = 0;
  c->allow_unknown = 0;
  c->abbrev_flag = 0;
  c->aux = 0;
  *list = c;
  return c;
}

struct cmd_list_element *
add_alias_cmd (name, oldname, class, abbrev_flag, list)
     char *name;
     char *oldname;
     int class;
     int abbrev_flag;
     struct cmd_list_element **list;
{
  /* Must do this since lookup_cmd tries to side-effect its first arg */
  char *copied_name;
  register struct cmd_list_element *old;
  register struct cmd_list_element *c;
  copied_name = (char *) alloca (strlen (oldname) + 1);
  strcpy (copied_name, oldname);
  old  = lookup_cmd (&copied_name, *list, 0, 1);

  if (old == 0)
    {
      delete_cmd (name, list);
      return 0;
    }

  c = add_cmd (name, class, old->function, old->doc, list);
  c->prefixlist = old->prefixlist;
  c->prefixname = old->prefixname;
  c->allow_unknown = old->allow_unknown;
  c->abbrev_flag = abbrev_flag;
  c->aux = old->aux;
  return c;
}

/* Like add_prefix_cmd but adds an element for a command prefix:
   a name that should be followed by a subcommand to be looked up
   in another command list.  PREFIXLIST should be the address
   of the variable containing that list.  */

struct cmd_list_element *
add_prefix_cmd (name, class, fun, doc, prefixlist, prefixname,
		allow_unknown, list)
     char *name;
     int class;
     void (*fun) ();
     char *doc;
     struct cmd_list_element **prefixlist;
     char *prefixname;
     int allow_unknown;
     struct cmd_list_element **list;
{
  register struct cmd_list_element *c = add_cmd (name, class, fun, doc, list);
  c->prefixlist = prefixlist;
  c->prefixname = prefixname;
  c->allow_unknown = allow_unknown;
  return c;
}

/* Remove the command named NAME from the command list.  */

void
delete_cmd (name, list)
     char *name;
     struct cmd_list_element **list;
{
  register struct cmd_list_element *c;

  while (*list && !strcmp ((*list)->name, name))
    {
      *list = (*list)->next;
    }

  if (*list)
    for (c = *list; c->next;)
      {
	if (!strcmp (c->next->name, name))
	  c->next = c->next->next;
	else
	  c = c->next;
      }
}

/* Implement a help command on command list LIST.
   COMMAND is the argument given (a command from the list to document)
   or zero for no arg (describe briefly all the commands in the list).
   CMDTYPE is a string to use in the error message if command COMMAND
   is not found in the list.  */

/* CLASS should be -1 to list all commands in LIST,
   or a nonnegative class number value to list just commands in that class,
   or -2 to list the classes themselves.  */

void
help_cmd (command, list, cmdtype, class, stream)
     char *command;
     struct cmd_list_element *list;
     char *cmdtype;
     int class;
     FILE *stream;
{
  register struct cmd_list_element *c;
  register char *p;
  register int ncmds;
  struct cmdvec { struct cmd_list_element *cmd; int class; };
  register struct cmdvec *cmdvec;
  char *cmdtype1, *cmdtype2;
  int len;

  if (command)
    {
      c = lookup_cmd (&command, list, cmdtype, 0);
      if (c == 0)
	return;

      /* There are three cases here.
	 If c->prefixlist is nonzer, we have a prefix command.
	 Print its documentation, then list its subcommands.

	 If c->function is nonzero, we really have a command.
	 Print its documentation and return.

	 If c->function is zero, we have a class name.
	 Print its documentation (as if it were a command)
	 and then set class to he number of this class
	 so that the commands in the class will be listed.  */

      p = c->doc;
      fprintf (stream, "%s\n", p);
      if (c->function != 0 && c->prefixlist == 0)
	return;
      fputc ('\n', stream);
      if (c->prefixlist)
	{
	  list = *c->prefixlist;
	  class = 0;
	  cmdtype = c->prefixname;
	}
      else
	class = c->class;
    }

  /* If CMDTYPE is "foo ", CMDTYPE1 gets " foo" and CMDTYPE2 gets "foo sub"  */
  len = strlen (cmdtype);
  cmdtype1 = (char *) alloca (len + 1);
  cmdtype1[0] = 0;
  cmdtype2 = (char *) alloca (len + 4);
  cmdtype2[0] = 0;
  if (len)
    {
      cmdtype1[0] = ' ';
      strncpy (cmdtype1 + 1, cmdtype, len - 1);
      cmdtype1[len] = 0;
      strncpy (cmdtype2, cmdtype, len - 1);
      strcpy (cmdtype2 + len - 1, " sub");
    }

  if (class == -2)
    fprintf (stream, "List of classes of %scommands:\n\n", cmdtype2);
  else
    fprintf (stream, "List of %scommands:\n\n", cmdtype2);

  for (c = list; c; c = c->next)
    {
      if (c->abbrev_flag == 0
	  && (class == -1	/* Listing all */
	      || (c->class == class && c->function != 0) /* Listing one class */
	      || (class == -2 && c->function == 0)))	/* Listing the classes */
	{
	  fprintf (stream, "%s -- ", c->name);
	  /* Print just first line of documentation.  */
	  p = c->doc;
	  while (*p && *p != '\n') p++;
	  fwrite (c->doc, 1, p - c->doc, stream);
	  fputc ('\n', stream);
	}
    }

  if (class == -2)
    fprintf (stream, "\n\
Type \"help%s\" followed by a class name for a list of commands in that class.",
	     cmdtype1);

  fprintf (stream, "\n\
Type \"help%s\" followed by %scommand name for full documentation.\n\
Command name abbreviations are allowed if unambiguous.\n",
	   cmdtype1, cmdtype2);
}

/* Look up the contents of *LINE as a command in the command list LIST.
   LIST is a chain of struct cmd_list_element's.
   If it is found, return the struct cmd_list_element for that command
   and update *LINE to point after the command name, at the first argument.
   If not found, call error if ALLOW_UNKNOWN is zero
   otherwise (or if error returns) return zero.
   Call error if specified command is ambiguous,
   unless ALLOW_UNKNOWN is negative.
   CMDTYPE precedes the word "command" in the error message.  */

struct cmd_list_element *
lookup_cmd (line, list, cmdtype, allow_unknown)
     char **line;
     struct cmd_list_element *list;
     char *cmdtype;
     int allow_unknown;
{
  register char *p;
  register struct cmd_list_element *c, *found;
  int nfound;
  char ambbuf[100];

  /* Skip leading whitespace.  */

  while (**line == ' ' || **line == '\t')
    (*line)++;

  /* Clear out trailing whitespace.  */

  p = *line + strlen (*line);
  while (p != *line && (p[-1] == ' ' || p[-1] == '\t'))
    p--;
  *p = 0;

  /* Find end of command name.  */

  p = *line;
  while (*p == '-'
	 || (*p >= 'a' && *p <= 'z')
	 || (*p >= 'A' && *p <= 'Z')
	 || (*p >= '0' && *p <= '9'))
    {
      if (*p >= 'A' && *p <= 'Z')
	*p += 'a' - 'A';
      p++;
    }

  /* Look up the command name.
     If exact match, keep that.
     Otherwise, take command abbreviated, if unique.  */

  found = 0;
  nfound = 0;
  for (c = list; c; c = c->next)
    {
      if (!strncmp (*line, c->name, p - *line))
	{
	  found = c;
	  nfound++;
	  if (c->name[p - *line] == 0)
	    {
	      nfound = 1;
	      break;
	    }
	}
    }

  /* Report error for undefined command name.  */

  if (nfound != 1)
    {
      if (nfound > 1 && allow_unknown >= 0)
	{
	  *p = 0;
	  ambbuf[0] = 0;
	  for (c = list; c; c = c->next)
	    if (!strncmp (*line, c->name, p - *line))
	      {
		if (strlen (ambbuf) + strlen (c->name) + 6 < sizeof ambbuf)
		  {
		    if (strlen (ambbuf))
		      strcat (ambbuf, ", ");
		    strcat (ambbuf, c->name);
		  }
		else
		  {
		    strcat (ambbuf, "..");
		    break;
		  }
	      }
	  error ("Ambiguous %scommand \"%s\": %s.", cmdtype, *line, ambbuf);
	}
      else if (!allow_unknown)
	{
	  *p = 0;
	  error ("Undefined %scommand: \"%s\".", cmdtype, *line);
	}
      return 0;
    }

  /* Skip whitespace before the argument.  */

  while (*p == ' ' || *p == '\t') p++;
  *line = p;

  if (found->prefixlist && *p)
    {
      c = lookup_cmd (line, *found->prefixlist, found->prefixname,
		      found->allow_unknown);
      if (c)
	return c;
    }

  return found;
}

/* Make a copy of the string at PTR with SIZE characters
   (and add a null character at the end in the copy).
   Uses malloc to get the space.  Returns the address of the copy.  */

static char *
savestring (ptr, size)
     char *ptr;
     int size;
{
  register char *p = (char *) xmalloc (size + 1);
  bcopy (ptr, p, size);
  p[size] = 0;
  return p;
}
