/* environ.c -- library for manipulating environments for GNU.
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

In other words, feel free to share this program, but don't try to
stop anyone else from sharing it.  */

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#include "environ.h"

/* Return a new environment object.  */

struct environ *
make_environ ()
{
  register struct environ *e;

  e = (struct environ *) xmalloc (sizeof (struct environ));

  e->allocated = 10;
  e->vector = (char **) xmalloc ((e->allocated + 1) * sizeof (char *));
  e->vector[0] = 0;
  return e;
}

/* Free an environment and all the strings in it.  */

void
free_environ (e)
     register struct environ *e;
{
  register char **vector = e->vector;

  while (*vector)
    free (*vector++);

  free (e);
}

/* Copy the environment given to this process into E.
   Also copies all the strings in it, so we can be sure
   that all strings in these environments are safe to free.  */

void
init_environ (e)
     register struct environ *e;
{
  extern char **environ;
  register int i;

  for (i = 0; environ[i]; i++);

  if (e->allocated < i)
    {
      e->allocated = max (i, e->allocated + 10);
      e->vector = (char **) xrealloc (e->vector,
				      (e->allocated + 1) * sizeof (char *));
    }

  bcopy (environ, e->vector, (i + 1) * sizeof (char *));

  while (--i >= 0)
    {
      register int len = strlen (e->vector[i]);
      register char *new = (char *) xmalloc (len + 1);
      bcopy (e->vector[i], new, len);
      e->vector[i] = new;
    }
}

/* Return the vector of environment E.
   This is used to get something to pass to execve.  */

char **
environ_vector (e)
     struct environ *e;
{
  return e->vector;
}

/* Return the value in environment E of variable VAR.  */

char *
get_in_environ (e, var)
     struct environ *e;
     char *var;
{
  register int len = strlen (var);
  register char **vector = e->vector;
  register char *s;

  for (; s = *vector; vector++)
    if (!strncmp (s, var, len)
	&& s[len] == '=')
      return &s[len + 1];

  return 0;
}

/* Store the value in E of VAR as VALUE.  */

void
set_in_environ (e, var, value)
     struct environ *e;
     char *var;
     char *value;
{
  register int i;
  register int len = strlen (var);
  register char **vector = e->vector;
  register char *s;

  for (i = 0; s = vector[i]; i++)
    if (!strncmp (s, var, len)
	&& s[len] == '=')
      break;

  if (s == 0)
    {
      if (i == e->allocated)
	{
	  e->allocated += 10;
	  vector = (char **) xrealloc (vector,
				       (e->allocated + 1) * sizeof (char *));
	  e->vector = vector;
	}
      vector[i + 1] = 0;
    }
  else
    free (s);

  s = (char *) xmalloc (len + strlen (value) + 2);
  strcpy (s, var);
  strcat (s, "=");
  strcat (s, value);
  vector[i] = s;
  return;
}

/* Remove the setting for variable VAR from environment E.  */

void
unset_in_environ (e, var)
     struct environ *e;
     char *var;
{
  register int len = strlen (var);
  register char **vector = e->vector;
  register char *s;

  for (; s = *vector; vector++)
    if (!strncmp (s, var, len)
	&& s[len] == '=')
      {
	free (s);
	bcopy (vector + 1, vector,
	       (e->allocated - (vector - e->vector)) * sizeof (char *));
	e->vector[e->allocated - 1] = 0;
	return;
      }
}
