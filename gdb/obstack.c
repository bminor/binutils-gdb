/* obstack.c - subroutines used implicitly by object stack macros
   Copyright (C) 1988 Free Software Foundation, Inc.

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
 (C) 1988 Free Software Foundation, Inc."; and include following the
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

  3. You may copy and distribute this program or any portion of it in
compiled, executable or object code form under the terms of Paragraphs
1 and 2 above provided that you do the following:

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
all derivatives our free software and of promoting the sharing and reuse of
software.


In other words, you are welcome to use, share and improve this program.
You are forbidden to forbid anyone else to use, share and improve
what you give them.   Help stamp out software-hoarding!  */


#include "obstack.h"

#ifdef __STDC__
#define POINTER void *
#else
#define POINTER char *
#endif

/* Determine default alignment.  */
struct fooalign {char x; double d;};
#define DEFAULT_ALIGNMENT ((char *)&((struct fooalign *) 0)->d - (char *)0)
/* If malloc were really smart, it would round addresses to DEFAULT_ALIGNMENT.
   But in fact it might be less smart and round addresses to as much as
   DEFAULT_ROUNDING.  So we prepare for it to do that.  */
union fooround {long x; double d;};
#define DEFAULT_ROUNDING (sizeof (union fooround))

/* When we copy a long block of data, this is the unit to do it with.
   On some machines, copying successive ints does not work;
   in such a case, redefine COPYING_UNIT to `long' (if that works)
   or `char' as a last resort.  */
#ifndef COPYING_UNIT
#define COPYING_UNIT int
#endif

/* The non-GNU-C macros copy the obstack into this global variable
   to avoid multiple evaluation.  */

struct obstack *_obstack;

/* Initialize an obstack H for use.  Specify chunk size SIZE (0 means default).
   Objects start on multiples of ALIGNMENT (0 means use default).
   CHUNKFUN is the function to use to allocate chunks,
   and FREEFUN the function to free them.  */

void
_obstack_begin (h, size, alignment, chunkfun, freefun)
     struct obstack *h;
     int size;
     int alignment;
     POINTER (*chunkfun) ();
     void (*freefun) ();
{
  register struct _obstack_chunk* chunk; /* points to new chunk */

  if (alignment == 0)
    alignment = DEFAULT_ALIGNMENT;
  if (size == 0)
    /* Default size is what GNU malloc can fit in a 4096-byte block.
       Pick a number small enough that when rounded up to DEFAULT_ROUNDING
       it is still smaller than 4096 - 4.  */
    {
      int extra = 4;
      if (extra < DEFAULT_ROUNDING)
	extra = DEFAULT_ROUNDING;
      size = 4096 - extra;
    }

  h->chunkfun = (struct _obstack_chunk * (*)()) chunkfun;
  h->freefun = freefun;
  h->chunk_size = size;
  h->alignment_mask = alignment - 1;

  chunk	= h->chunk = (*h->chunkfun) (h->chunk_size);
  h->next_free = h->object_base = chunk->contents;
  h->chunk_limit = chunk->limit
   = (char *) chunk + h->chunk_size;
  chunk->prev = 0;
}

/* Allocate a new current chunk for the obstack *H
   on the assumption that LENGTH bytes need to be added
   to the current object, or a new object of length LENGTH allocated.
   Copies any partial object from the end of the old chunk
   to the beginning of the new one.  */

void
_obstack_newchunk (h, length)
     struct obstack *h;
     int length;
{
  register struct _obstack_chunk*	old_chunk = h->chunk;
  register struct _obstack_chunk*	new_chunk;
  register long	new_size;
  register int obj_size = h->next_free - h->object_base;
  register int i;

  /* Compute size for new chunk.  */
  new_size = (obj_size + length) << 1;
  if (new_size < h->chunk_size)
    new_size = h->chunk_size;

  /* Allocate and initialize the new chunk.  */
  new_chunk = h->chunk = (*h->chunkfun) (new_size);
  new_chunk->prev = old_chunk;
  new_chunk->limit = h->chunk_limit = (char *) new_chunk + new_size;

  /* Move the existing object to the new chunk.
     Word at a time is fast and is safe because these
     structures are aligned at least that much.  */
  for (i = (obj_size + sizeof (COPYING_UNIT) - 1) / sizeof (COPYING_UNIT) - 1;
       i >= 0; i--)
    ((COPYING_UNIT *)new_chunk->contents)[i]
      = ((COPYING_UNIT *)h->object_base)[i];

  h->object_base = new_chunk->contents;
  h->next_free = h->object_base + obj_size;
}

/* Free objects in obstack H, including OBJ and everything allocate
   more recently than OBJ.  If OBJ is zero, free everything in H.  */

void
#ifdef __STDC__
#undef obstack_free
obstack_free (h, obj)

#else
_obstack_free (h, obj)
#endif
     struct obstack *h;
     POINTER obj;
{
  register struct _obstack_chunk*  lp;	/* below addr of any objects in this chunk */
  register struct _obstack_chunk*  plp;	/* point to previous chunk if any */

  lp = (h)->chunk;
  while (lp != 0 && ((POINTER)lp > obj || (POINTER)(lp)->limit < obj))
    {
      plp = lp -> prev;
      (*h->freefun) (lp);
      lp = plp;
    }
  if (lp)
    {
      (h)->object_base = (h)->next_free = (char *)(obj);
      (h)->chunk_limit = lp->limit;
      (h)->chunk = lp;
    }
  else if (obj != 0)
    /* obj is not in any of the chunks! */
    abort ();
}

#if 0
/* These are now turned off because the applications do not use it
   and it uses bcopy via obstack_grow, which causes trouble on sysV.  */

/* Now define the functional versions of the obstack macros.
   Define them to simply use the corresponding macros to do the job.  */

#ifdef __STDC__
/* These function definitions do not work with non-ANSI preprocessors;
   they won't pass through the macro names in parentheses.  */

/* The function names appear in parentheses in order to prevent
   the macro-definitions of the names from being expanded there.  */

POINTER (obstack_base) (obstack)
     struct obstack *obstack;
{
  return obstack_base (obstack);
}

POINTER (obstack_next_free) (obstack)
     struct obstack *obstack;
{
  return obstack_next_free (obstack);
}

int (obstack_object_size) (obstack)
     struct obstack *obstack;
{
  return obstack_object_size (obstack);
}

int (obstack_room) (obstack)
     struct obstack *obstack;
{
  return obstack_room (obstack);
}

void (obstack_grow) (obstack, pointer, length)
     struct obstack *obstack;
     POINTER pointer;
     int length;
{
  obstack_grow (obstack, pointer, length);
}

void (obstack_grow0) (obstack, pointer, length)
     struct obstack *obstack;
     POINTER pointer;
     int length;
{
  obstack_grow0 (obstack, pointer, length);
}

void (obstack_1grow) (obstack, character)
     struct obstack *obstack;
     int character;
{
  obstack_1grow (obstack, character);
}

void (obstack_blank) (obstack, length)
     struct obstack *obstack;
     int length;
{
  obstack_blank (obstack, length);
}

void (obstack_1grow_fast) (obstack, character)
     struct obstack *obstack;
     int character;
{
  obstack_1grow_fast (obstack, character);
}

void (obstack_blank_fast) (obstack, length)
     struct obstack *obstack;
     int length;
{
  obstack_blank_fast (obstack, length);
}

POINTER (obstack_finish) (obstack)
     struct obstack *obstack;
{
  return obstack_finish (obstack);
}

POINTER (obstack_alloc) (obstack, length)
     struct obstack *obstack;
     int length;
{
  return obstack_alloc (obstack, length);
}

POINTER (obstack_copy) (obstack, pointer, length)
     struct obstack *obstack;
     POINTER pointer;
     int length;
{
  return obstack_copy (obstack, pointer, length);
}

POINTER (obstack_copy0) (obstack, pointer, length)
     struct obstack *obstack;
     POINTER pointer;
     int length;
{
  return obstack_copy0 (obstack, pointer, length);
}

#endif /* __STDC__ */

#endif /* 0 */
