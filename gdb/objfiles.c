/* GDB routines for manipulating objfiles.
   Copyright 1992 Free Software Foundation, Inc.
   Contributed by Cygnus Support, using pieces from other GDB modules.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* This file contains support routines for creating, manipulating, and
   destroying objfile structures. */

#include <stdio.h>
#include "defs.h"
#include "bfd.h"		/* Binary File Description */
#include "symtab.h"
#include "symfile.h"

#include <obstack.h>

/* Externally visible variables that are owned by this module. */

struct objfile *object_files;		/* Linked list of all objfiles */

/* Allocate a new objfile struct, fill it in as best we can, and return it.
   It is also linked into the list of all known object files. */

struct objfile *
allocate_objfile (abfd, filename, dumpable)
     bfd *abfd;
     char *filename;
    int dumpable;
{
  struct objfile *objfile;

  /* First, if the objfile is to be dumpable, we must malloc the structure
     itself using the mmap version, and arrange that all memory allocation
     for the objfile uses the mmap routines.  Otherwise, just use the
     old sbrk'd malloc routines. */

  if (dumpable)
    {
      objfile = (struct objfile *) mmap_xmalloc (sizeof (struct objfile));
      (void) memset (objfile, 0, sizeof (struct objfile));
      objfile -> malloc = mmap_malloc;
      objfile -> realloc = mmap_realloc;
      objfile -> xmalloc = mmap_xmalloc;
      objfile -> xrealloc = mmap_xrealloc;
      objfile -> free = mmap_free;
      objfile -> flags |= OBJF_DUMPABLE;
    }
  else
    {
      objfile = (struct objfile *) xmalloc (sizeof (struct objfile));
      (void) memset (objfile, 0, sizeof (struct objfile));
      objfile -> malloc = (PTR (*) PARAMS ((long))) malloc;
      objfile -> realloc = (PTR (*) PARAMS ((PTR, long))) realloc;
      objfile -> xmalloc = xmalloc;
      objfile -> xrealloc = xrealloc;
      objfile -> free = free;
    }

  /* Now, malloc a fresh copy of the filename string using the malloc
     specified as appropriate for the objfile. */

  objfile -> name = (*objfile -> xmalloc) (strlen (filename) + 1);
  strcpy (objfile -> name, filename);

  objfile -> obfd = abfd;

  objfile -> mtime = bfd_get_mtime (abfd);

  /* Set up the various obstacks to use the memory allocation/free
     functions that are appropriate for this objfile. */

  obstack_full_begin (&objfile -> psymbol_obstack, 0, 0,
		      objfile -> xmalloc, objfile -> free);
  obstack_full_begin (&objfile -> symbol_obstack, 0, 0,
		      objfile -> xmalloc, objfile -> free);
  obstack_full_begin (&objfile -> type_obstack, 0, 0,
		      objfile -> xmalloc, objfile -> free);

  /* Push this file onto the head of the linked list of other such files. */

  objfile -> next = object_files;
  object_files = objfile;

  return (objfile);
}


/* Destroy an objfile and all the symtabs and psymtabs under it.  Note
   that as much as possible is allocated on the symbol_obstack and
   psymbol_obstack, so that the memory can be efficiently freed. */

void
free_objfile (objfile)
     struct objfile *objfile;
{
  struct objfile *ofp;

  if (objfile -> name)
    {
      (*objfile -> free) (objfile -> name);
    }
  if (objfile -> obfd)
    {
      bfd_close (objfile -> obfd);
    }

  /* Remove it from the chain of all objfiles.  */

  if (object_files == objfile)
    {
      object_files = objfile -> next;
    }
  else
    {
      for (ofp = object_files; ofp; ofp = ofp -> next)
	{
	  if (ofp -> next == objfile)
	    {
	      ofp -> next = objfile -> next;
	    }
	}
    }

  obstack_free (&objfile -> psymbol_obstack, 0);
  obstack_free (&objfile -> symbol_obstack, 0);
  obstack_free (&objfile -> type_obstack, 0);

#if 0	/* FIXME!! */

  /* Before the symbol table code was redone to make it easier to
     selectively load and remove information particular to a specific
     linkage unit, gdb used to do these things whenever the monolithic
     symbol table was blown away.  How much still needs to be done
     is unknown, but we play it safe for now and keep each action until
     it is shown to be no longer needed. */
     
  clear_symtab_users_once ();
#if defined (CLEAR_SOLIB)
  CLEAR_SOLIB ();
#endif
  clear_pc_function_cache ();

#endif

  /* The last thing we do is free the objfile struct itself, using the
     free() that is appropriate for the objfile. */

  (*objfile -> free) (objfile);
}


/* Free all the object files at once.  */

void
free_all_objfiles ()
{
  struct objfile *objfile, *temp;

  ALL_OBJFILES_SAFE (objfile, temp)
    {
      free_objfile (objfile);
    }
}

/* Many places in gdb want to test just to see if we have any partial
   symbols available.  This function returns zero if none are currently
   available, nonzero otherwise. */

int
have_partial_symbols ()
{
  struct objfile *ofp;
  int havethem = 0;

  for (ofp = object_files; ofp; ofp = ofp -> next)
    {
      if (ofp -> psymtabs != NULL)
	{
	  havethem++;
	  break;
	}
    }
  return (havethem);
}

/* Many places in gdb want to test just to see if we have any full
   symbols available.  This function returns zero if none are currently
   available, nonzero otherwise. */

int
have_full_symbols ()
{
  struct objfile *ofp;
  int havethem = 0;

  for (ofp = object_files; ofp; ofp = ofp -> next)
    {
      if (ofp -> symtabs != NULL)
	{
	  havethem++;
	  break;
	}
    }
  return (havethem);
}

/* Many places in gdb want to test just to see if we have any minimal
   symbols available.  This function returns zero if none are currently
   available, nonzero otherwise. */

int
have_minimal_symbols ()
{
  struct objfile *ofp;
  int havethem = 0;

  for (ofp = object_files; ofp; ofp = ofp -> next)
    {
      if (ofp -> msymbols != NULL)
	{
	  havethem++;
	  break;
	}
    }
  return (havethem);
}

/* Call the function specified by FUNC for each currently available objfile,
   for as long as this function continues to return NULL.  If the function
   ever returns non-NULL, then the iteration over the objfiles is terminated,
   and the result is returned to the caller.  The function called has full
   control over the form and content of the information returned via the
   non-NULL result, which may be as simple as a pointer to the objfile that
   the iteration terminated on, or as complex as a pointer to a private
   structure containing multiple results. */

PTR
iterate_over_objfiles (func, arg1, arg2, arg3)
     PTR (*func) PARAMS ((struct objfile *, PTR, PTR, PTR));
     PTR arg1;
     PTR arg2;
     PTR arg3;
{
  register struct objfile *objfile;
  PTR result = NULL;

  for (objfile = object_files;
       objfile != NULL && result == NULL;
       objfile = objfile -> next)
    {
      result = (*func)(objfile, arg1, arg2, arg3);
    }
  return (result);
}

/* Call the function specified by FUNC for each currently available symbol
   table, for as long as this function continues to return NULL.  If the
   function ever returns non-NULL, then the iteration over the symbol tables
   is terminated, and the result is returned to the caller.  The function
   called has full control over the form and content of the information
   returned via the non-NULL result, which may be as simple as a pointer
   to the symtab that the iteration terminated on, or as complex as a
   pointer to a private structure containing multiple results. */

PTR 
iterate_over_symtabs (func, arg1, arg2, arg3)
     PTR (*func) PARAMS ((struct objfile *, struct symtab *, PTR, PTR, PTR));
     PTR arg1;
     PTR arg2;
     PTR arg3;
{
  register struct objfile *objfile;
  register struct symtab *symtab;
  PTR result = NULL;

  for (objfile = object_files;
       objfile != NULL && result == NULL;
       objfile = objfile -> next)
    {
      for (symtab = objfile -> symtabs;
	   symtab != NULL && result == NULL;
	   symtab = symtab -> next)
	{
	  result = (*func)(objfile, symtab, arg1, arg2, arg3);
	}
    }
  return (result);
}

/* Call the function specified by FUNC for each currently available partial
   symbol table, for as long as this function continues to return NULL.  If
   the function ever returns non-NULL, then the iteration over the partial
   symbol tables is terminated, and the result is returned to the caller.

   The function called has full control over the form and content of the
   information returned via the non-NULL result, which may be as simple as a
   pointer to the partial symbol table that the iteration terminated on, or
   as complex as a pointer to a private structure containing multiple
   results. */

PTR 
iterate_over_psymtabs (func, arg1, arg2, arg3)
     PTR (*func) PARAMS ((struct objfile *, struct partial_symtab *,
			  PTR, PTR, PTR));
     PTR arg1;
     PTR arg2;
     PTR arg3;
{
  register struct objfile *objfile;
  register struct partial_symtab *psymtab;
  PTR result = NULL;

  for (objfile = object_files;
       objfile != NULL && result == NULL;
       objfile = objfile -> next)
    {
      for (psymtab = objfile -> psymtabs;
	   psymtab != NULL && result == NULL;
	   psymtab = psymtab -> next)
	{
	  result = (*func)(objfile, psymtab, arg1, arg2, arg3);
	}
    }
  return (result);
}
