/* Support routines for decoding "stabs" debugging information format.
   Copyright 1986, 1987, 1988, 1989, 1990, 1991, 1992, 1993
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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Support routines for reading and decoding debugging information in
   the "stabs" format.  This format is used with many systems that use
   the a.out object file format, as well as some systems that use
   COFF or ELF where the stabs data is placed in a special section.
   Avoid placing any object file format specific code in this file. */

#include "defs.h"
#include "bfd.h"
#include "obstack.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "symfile.h"
#include "objfiles.h"
#include "aout/stab_gnu.h"	/* We always use GNU stabs, not native */
#include "buildsym.h"
#include "complaints.h"
#include "demangle.h"

#include <ctype.h>

/* Ask stabsread.h to define the vars it normally declares `extern'.  */
#define EXTERN  /**/
#include "stabsread.h"          /* Our own declarations */
#undef  EXTERN

/* The routines that read and process a complete stabs for a C struct or 
   C++ class pass lists of data member fields and lists of member function
   fields in an instance of a field_info structure, as defined below.
   This is part of some reorganization of low level C++ support and is
   expected to eventually go away... (FIXME) */

struct field_info
{
  struct nextfield
    {
      struct nextfield *next;

      /* This is the raw visibility from the stab.  It is not checked
	 for being one of the visibilities we recognize, so code which
	 examines this field better be able to deal.  */
      int visibility;

      struct field field;
    } *list;
  struct next_fnfieldlist
    {
      struct next_fnfieldlist *next;
      struct fn_fieldlist fn_fieldlist;
    } *fnlist;
};

static struct type *
dbx_alloc_type PARAMS ((int [2], struct objfile *));

static long read_huge_number PARAMS ((char **, int, int *));

static struct type *error_type PARAMS ((char **));

static int
read_type_number PARAMS ((char **, int *));

static struct type *
read_range_type PARAMS ((char **, int [2], struct objfile *));

static struct type *
os9k_read_type PARAMS ((char **, struct objfile *));

static struct type *
os9k_read_enum_type PARAMS ((char **, struct type *, struct objfile *));

static int
read_struct_fields PARAMS ((struct field_info *, char **, struct type *,
			    struct objfile *));

static int
attach_fields_to_type PARAMS ((struct field_info *, struct type *,
			       struct objfile *));

static struct type *
os9k_read_array_type PARAMS ((char **, struct type *, struct objfile *));

extern struct complaint error_type_complaint;
extern struct complaint stabs_general_complaint;
extern struct complaint range_type_base_complaint;
extern struct complaint reg_value_complaint;

/* Define this as 1 if a pcc declaration of a char or short argument
   gives the correct address.  Otherwise assume pcc gives the
   address of the corresponding int, which is not the same on a
   big-endian machine.  */

#ifndef BELIEVE_PCC_PROMOTION
#define BELIEVE_PCC_PROMOTION 0
#endif

/* Make a list of forward references which haven't been defined.  */

static struct type **undef_types;
static int undef_types_allocated;
static int undef_types_length;

/* Check for and handle cretinous stabs symbol name continuation!  */
#define STABS_CONTINUE(pp)				\
  do {							\
    if (**(pp) == '\\') *(pp) = next_symbol_text ();	\
  } while (0)


static struct type **init_type_vector[] = {
	0,
	&builtin_type_int,
	&builtin_type_char,
	&builtin_type_long,
	&builtin_type_short,
	&builtin_type_unsigned_char,
	&builtin_type_unsigned_short,
	&builtin_type_unsigned_long,
	&builtin_type_unsigned_int,
	&builtin_type_float,
	&builtin_type_double,
	&builtin_type_void,
	&builtin_type_long_double
};

static void
os9k_init_type_vector(tv)
    struct type **tv;
{
  int i;
  for (i=0; i<sizeof(init_type_vector)/sizeof(struct type **); i++)
    tv[i] = (init_type_vector[i] == 0 ? 0 : *(init_type_vector[i]));
}

/* Look up a dbx type-number pair.  Return the address of the slot
   where the type for that number-pair is stored.
   The number-pair is in TYPENUMS.

   This can be used for finding the type associated with that pair
   or for associating a new type with the pair.  */

static struct type **
dbx_lookup_type (typenums)
     int typenums[2];
{
  register int filenum = typenums[0];
  register int index = typenums[1];
  unsigned old_len;
  register int real_filenum;
  register struct header_file *f;
  int f_orig_length;

  if (filenum == -1)		/* -1,-1 is for temporary types.  */
    return 0;

  if (filenum < 0 || filenum >= n_this_object_header_files)
    {
      static struct complaint msg = {"\
Invalid symbol data: type number (%d,%d) out of range at symtab pos %d.",
				0, 0};
      complain (&msg, filenum, index, symnum);
      goto error_return;
    }

  if (filenum == 0)
    {
      /* Type is defined outside of header files.
	 Find it in this object file's type vector.  */
      if (index >= type_vector_length)
	{
	  old_len = type_vector_length;
	  if (old_len == 0)
	    {
	      type_vector_length = INITIAL_TYPE_VECTOR_LENGTH;
	      type_vector = (struct type **)
		malloc (type_vector_length * sizeof (struct type *));
	    }
	  while (index >= type_vector_length)
	    {
	      type_vector_length *= 2;
	    }
	  type_vector = (struct type **)
	    xrealloc ((char *) type_vector,
		      (type_vector_length * sizeof (struct type *)));
	  memset (&type_vector[old_len], 0,
		  (type_vector_length - old_len) * sizeof (struct type *));
	  os9k_init_type_vector(type_vector);
	}
      return (&type_vector[index]);
    }
  else
    {
      real_filenum = this_object_header_files[filenum];

      if (real_filenum >= n_header_files)
	{
	  struct type *temp_type;
	  struct type **temp_type_p;

	  warning ("GDB internal error: bad real_filenum");

	error_return:
	  temp_type = init_type (TYPE_CODE_ERROR, 0, 0, NULL, NULL);
	  temp_type_p = (struct type **) xmalloc (sizeof (struct type *));
	  *temp_type_p = temp_type;
	  return temp_type_p;
	}

      f = &header_files[real_filenum];

      f_orig_length = f->length;
      if (index >= f_orig_length)
	{
	  while (index >= f->length)
	    {
	      f->length *= 2;
	    }
	  f->vector = (struct type **)
	    xrealloc ((char *) f->vector, f->length * sizeof (struct type *));
	  memset (&f->vector[f_orig_length], 0,
		  (f->length - f_orig_length) * sizeof (struct type *));
	}
      return (&f->vector[index]);
    }
}

/* Make sure there is a type allocated for type numbers TYPENUMS
   and return the type object.
   This can create an empty (zeroed) type object.
   TYPENUMS may be (-1, -1) to return a new type object that is not
   put into the type vector, and so may not be referred to by number. */

static struct type *
dbx_alloc_type (typenums, objfile)
     int typenums[2];
     struct objfile *objfile;
{
  register struct type **type_addr;

  if (typenums[0] == -1)
    {
      return (alloc_type (objfile));
    }

  type_addr = dbx_lookup_type (typenums);

  /* If we are referring to a type not known at all yet,
     allocate an empty type for it.
     We will fill it in later if we find out how.  */
  if (*type_addr == 0)
    {
      *type_addr = alloc_type (objfile);
    }

  return (*type_addr);
}


/* Read a number by which a type is referred to in dbx data,
   or perhaps read a pair (FILENUM, TYPENUM) in parentheses.
   Just a single number N is equivalent to (0,N).
   Return the two numbers by storing them in the vector TYPENUMS.
   TYPENUMS will then be used as an argument to dbx_lookup_type.

   Returns 0 for success, -1 for error.  */

static int
read_type_number (pp, typenums)
     register char **pp;
     register int *typenums;
{
  int nbits;
  if (**pp == '(')
    {
      (*pp)++;
      typenums[0] = read_huge_number (pp, ',', &nbits);
      if (nbits != 0) return -1;
      typenums[1] = read_huge_number (pp, ')', &nbits);
      if (nbits != 0) return -1;
    }
  else
    {
      typenums[0] = 0;
      typenums[1] = read_huge_number (pp, 0, &nbits);
      if (nbits != 0) return -1;
    }
  return 0;
}


/* Skip rest of this symbol and return an error type.

   General notes on error recovery:  error_type always skips to the
   end of the symbol (modulo cretinous dbx symbol name continuation).
   Thus code like this:

   if (*(*pp)++ != ';')
     return error_type (pp);

   is wrong because if *pp starts out pointing at '\0' (typically as the
   result of an earlier error), it will be incremented to point to the
   start of the next symbol, which might produce strange results, at least
   if you run off the end of the string table.  Instead use

   if (**pp != ';')
     return error_type (pp);
   ++*pp;

   or

   if (**pp != ';')
     foo = error_type (pp);
   else
     ++*pp;

   And in case it isn't obvious, the point of all this hair is so the compiler
   can define new types and new syntaxes, and old versions of the
   debugger will be able to read the new symbol tables.  */

static struct type *
error_type (pp)
     char **pp;
{
  complain (&error_type_complaint);
  while (1)
    {
      /* Skip to end of symbol.  */
      while (**pp != '\0')
	{
	  (*pp)++;
	}

      /* Check for and handle cretinous dbx symbol name continuation!  */
      if ((*pp)[-1] == '\\')
	{
	  *pp = next_symbol_text ();
	}
      else
	{
	  break;
	}
    }
  return (builtin_type_error);
}

/* This page contains subroutines of read_type.  */

#define VISIBILITY_PRIVATE      '0'     /* Stabs character for private field */
#define VISIBILITY_PROTECTED    '1'     /* Stabs character for protected fld */
#define VISIBILITY_PUBLIC       '2'     /* Stabs character for public field */
#define VISIBILITY_IGNORE       '9'     /* Optimized out or zero length */

static void
read_one_struct_field (fip, pp, p, type, objfile)
     struct field_info *fip;
     char **pp;
     char *p;
     struct type *type;
     struct objfile *objfile;
{
  fip -> list -> field.name =
    obsavestring (*pp, p - *pp, &objfile -> type_obstack);
  *pp = p + 1;

  fip -> list -> visibility = VISIBILITY_PUBLIC;
  fip -> list -> field.type = os9k_read_type (pp, objfile);
  if (**pp == ':')
    {
      p = ++(*pp);
#if 0
      /* Possible future hook for nested types. */
      if (**pp == '!')
	{
	  fip -> list -> field.bitpos = (long)-2; /* nested type */
	  p = ++(*pp);
	}
      else
#endif
	{
	  /* Static class member.  */
	  fip -> list -> field.bitpos = (long) -1;
	}
      while (*p != ';') 
	{
	  p++;
	}
      fip -> list -> field.bitsize = (long) savestring (*pp, p - *pp);
      *pp = p + 1;
      return;
    }
  else if (**pp != ',')
    {
      /* Bad structure-type format.  */
      complain (&stabs_general_complaint, "bad structure-type format");
      return;
    }

  (*pp)++;			/* Skip the comma.  */

  {
    int nbits;
    fip -> list -> field.bitpos = read_huge_number (pp, ',', &nbits);
    if (nbits != 0)
      {
	complain (&stabs_general_complaint, "bad structure-type format");
	return;
      }
    fip -> list -> field.bitsize = read_huge_number (pp, ';', &nbits);
    if (nbits != 0)
      {
	complain (&stabs_general_complaint, "bad structure-type format");
	return;
      }
  }

  if (fip -> list -> field.bitpos == 0 && fip -> list -> field.bitsize == 0)
    {
      /* This can happen in two cases: (1) at least for gcc 2.4.5 or so,
	 it is a field which has been optimized out.  The correct stab for
	 this case is to use VISIBILITY_IGNORE, but that is a recent
	 invention.  (2) It is a 0-size array.  For example
	 union { int num; char str[0]; } foo.  Printing "<no value>" for
	 str in "p foo" is OK, since foo.str (and thus foo.str[3])
	 will continue to work, and a 0-size array as a whole doesn't
	 have any contents to print.

	 I suspect this probably could also happen with gcc -gstabs (not
	 -gstabs+) for static fields, and perhaps other C++ extensions.
	 Hopefully few people use -gstabs with gdb, since it is intended
	 for dbx compatibility.  */

      /* Ignore this field.  */
      fip -> list-> visibility = VISIBILITY_IGNORE;
    }
  else
    {
      /* Detect an unpacked field and mark it as such.
	 dbx gives a bit size for all fields.
	 Note that forward refs cannot be packed,
	 and treat enums as if they had the width of ints.  */

      if (TYPE_CODE (fip -> list -> field.type) != TYPE_CODE_INT
	  && TYPE_CODE (fip -> list -> field.type) != TYPE_CODE_ENUM)
	{
	  fip -> list -> field.bitsize = 0;
	}
      if ((fip -> list -> field.bitsize 
	   == TARGET_CHAR_BIT * TYPE_LENGTH (fip -> list -> field.type)
	   || (TYPE_CODE (fip -> list -> field.type) == TYPE_CODE_ENUM
	       && (fip -> list -> field.bitsize
		   == TARGET_INT_BIT)
	       )
	   )
	  &&
	  fip -> list -> field.bitpos % 8 == 0)
	{
	  fip -> list -> field.bitsize = 0;
	}
    }
}


/* Read struct or class data fields.  They have the form:

   	NAME : [VISIBILITY] TYPENUM , BITPOS , BITSIZE ;

   At the end, we see a semicolon instead of a field.

   In C++, this may wind up being NAME:?TYPENUM:PHYSNAME; for
   a static field.

   The optional VISIBILITY is one of:

   	'/0'	(VISIBILITY_PRIVATE)
	'/1'	(VISIBILITY_PROTECTED)
	'/2'	(VISIBILITY_PUBLIC)
	'/9'	(VISIBILITY_IGNORE)

   or nothing, for C style fields with public visibility.

   Returns 1 for success, 0 for failure.  */

static int
read_struct_fields (fip, pp, type, objfile)
     struct field_info *fip;
     char **pp;
     struct type *type;
     struct objfile *objfile;
{
  register char *p;
  struct nextfield *new;

  /* We better set p right now, in case there are no fields at all...    */

  p = *pp;

  /* Read each data member type until we find the terminating ';' at the end of
     the data member list, or break for some other reason such as finding the
     start of the member function list. */

  while (**pp != ';' && **pp != '\0')
    {
      STABS_CONTINUE (pp);
      /* Get space to record the next field's data.  */
      new = (struct nextfield *) xmalloc (sizeof (struct nextfield));
      make_cleanup (free, new);
      memset (new, 0, sizeof (struct nextfield));
      new -> next = fip -> list;
      fip -> list = new;

      /* Get the field name.  */
      p = *pp;

      /* Look for the ':' that separates the field name from the field
	 values.  Data members are delimited by a single ':', while member
	 functions are delimited by a pair of ':'s.  When we hit the member
	 functions (if any), terminate scan loop and return. */

      while (*p != ':' && *p != '\0') 
	{
	  p++;
	}
      if (*p == '\0')
	return 0;

      /* Check to see if we have hit the member functions yet.  */
      if (p[1] == ':')
	{
	  break;
	}
      read_one_struct_field (fip, pp, p, type, objfile);
    }
  if (p[0] == ':' && p[1] == ':')
    {
      /* chill the list of fields: the last entry (at the head) is a
	 partially constructed entry which we now scrub. */
      fip -> list = fip -> list -> next;
    }
  return 1;
}

/* Create the vector of fields, and record how big it is.
   We need this info to record proper virtual function table information
   for this class's virtual functions.  */

static int
attach_fields_to_type (fip, type, objfile)
     struct field_info *fip;
     register struct type *type;
     struct objfile *objfile;
{
  register int nfields = 0;
  register int non_public_fields = 0;
  register struct nextfield *scan;

  /* Count up the number of fields that we have, as well as taking note of
     whether or not there are any non-public fields, which requires us to
     allocate and build the private_field_bits and protected_field_bits
     bitfields. */

  for (scan = fip -> list; scan != NULL; scan = scan -> next)
    {
      nfields++;
      if (scan -> visibility != VISIBILITY_PUBLIC)
	{
	  non_public_fields++;
	}
    }

  /* Now we know how many fields there are, and whether or not there are any
     non-public fields.  Record the field count, allocate space for the
     array of fields, and create blank visibility bitfields if necessary. */

  TYPE_NFIELDS (type) = nfields;
  TYPE_FIELDS (type) = (struct field *)
    TYPE_ALLOC (type, sizeof (struct field) * nfields);
  memset (TYPE_FIELDS (type), 0, sizeof (struct field) * nfields);

  /* Copy the saved-up fields into the field vector.  Start from the head
     of the list, adding to the tail of the field array, so that they end
     up in the same order in the array in which they were added to the list. */

  while (nfields-- > 0)
    {
      TYPE_FIELD (type, nfields) = fip -> list -> field;
      fip -> list = fip -> list -> next;
    }
  return 1;
}

/* Read the description of a structure (or union type) and return an object
   describing the type.

   PP points to a character pointer that points to the next unconsumed token
   in the the stabs string.  For example, given stabs "A:T4=s4a:1,0,32;;",
   *PP will point to "4a:1,0,32;;".

   TYPE points to an incomplete type that needs to be filled in.

   OBJFILE points to the current objfile from which the stabs information is
   being read.  (Note that it is redundant in that TYPE also contains a pointer
   to this same objfile, so it might be a good idea to eliminate it.  FIXME). 
   */

static struct type *
read_struct_type (pp, type, objfile)
     char **pp;
     struct type *type;
     struct objfile *objfile;
{
  struct cleanup *back_to;
  struct field_info fi;

  fi.list = NULL;
  fi.fnlist = NULL;

  back_to = make_cleanup (null_cleanup, 0);

  INIT_CPLUS_SPECIFIC (type);
  TYPE_FLAGS (type) &= ~TYPE_FLAG_STUB;

  /* First comes the total size in bytes.  */

  {
    int nbits;
    TYPE_LENGTH (type) = read_huge_number (pp, 0, &nbits);
    if (nbits != 0)
      return error_type (pp);
  }

  /* Now read the baseclasses, if any, read the regular C struct or C++
     class member fields, attach the fields to the type, read the C++
     member functions, attach them to the type, and then read any tilde
     field (baseclass specifier for the class holding the main vtable). */

  if (!read_struct_fields (&fi, pp, type, objfile)
      || !attach_fields_to_type (&fi, type, objfile)
     )
    {
      do_cleanups (back_to);
      return (error_type (pp));
    }

  do_cleanups (back_to);
  return (type);
}

static long
read_huge_number (pp, end, bits)
     char **pp;
     int end;
     int *bits;
{
  char *p = *pp;
  int sign = 1;
  long n = 0;
  int radix = 10;
  char overflow = 0;
  int nbits = 0;
  int c;
  long upper_limit;
  
  if (*p == '-')
    {
      sign = -1;
      p++;
    }

  /* Leading zero means octal.  GCC uses this to output values larger
     than an int (because that would be hard in decimal).  */
  if (*p == '0')
    {
      radix = 8;
      p++;
    }

  upper_limit = LONG_MAX / radix;
  while ((c = *p++) >= '0' && c < ('0' + radix))
    {
      if (n <= upper_limit)
	{
	  n *= radix;
	  n += c - '0';		/* FIXME this overflows anyway */
	}
      else
	overflow = 1;
      
      /* This depends on large values being output in octal, which is
	 what GCC does. */
      if (radix == 8)
	{
	  if (nbits == 0)
	    {
	      if (c == '0')
		/* Ignore leading zeroes.  */
		;
	      else if (c == '1')
		nbits = 1;
	      else if (c == '2' || c == '3')
		nbits = 2;
	      else
		nbits = 3;
	    }
	  else
	    nbits += 3;
	}
    }
  if (end)
    {
      if (c && c != end)
	{
	  if (bits != NULL)
	    *bits = -1;
	  return 0;
	}
    }
  else
    --p;

  *pp = p;
  if (overflow)
    {
      if (nbits == 0)
	{
	  /* Large decimal constants are an error (because it is hard to
	     count how many bits are in them).  */
	  if (bits != NULL)
	    *bits = -1;
	  return 0;
	}
      
      /* -0x7f is the same as 0x80.  So deal with it by adding one to
	 the number of bits.  */
      if (sign == -1)
	++nbits;
      if (bits)
	*bits = nbits;
    }
  else
    {
      if (bits)
	*bits = 0;
      return n * sign;
    }
  /* It's *BITS which has the interesting information.  */
  return 0;
}

static struct type *
read_range_type (pp, typenums, objfile)
     char **pp;
     int typenums[2];
     struct objfile *objfile;
{
  int rangenums[2];
  long n2, n3;
  int n2bits, n3bits;
  int self_subrange;
  struct type *result_type;
  struct type *index_type;

  /* First comes a type we are a subrange of.
     In C it is usually 0, 1 or the type being defined.  */
  /* FIXME: according to stabs.texinfo and AIX doc, this can be a type-id
     not just a type number.  */
  if (read_type_number (pp, rangenums) != 0)
    return error_type (pp);
  self_subrange = (rangenums[0] == typenums[0] &&
		   rangenums[1] == typenums[1]);

  /* A semicolon should now follow; skip it.  */
  if (**pp == ';')
    (*pp)++;

  /* The remaining two operands are usually lower and upper bounds
     of the range.  But in some special cases they mean something else.  */
  n2 = read_huge_number (pp, ';', &n2bits);
  n3 = read_huge_number (pp, ';', &n3bits);

  if (n2bits == -1 || n3bits == -1)
    return error_type (pp);
  
  /* If limits are huge, must be large integral type.  */
  if (n2bits != 0 || n3bits != 0)
    {
      char got_signed = 0;
      char got_unsigned = 0;
      /* Number of bits in the type.  */
      int nbits = 0;

      /* Range from 0 to <large number> is an unsigned large integral type.  */
      if ((n2bits == 0 && n2 == 0) && n3bits != 0)
	{
	  got_unsigned = 1;
	  nbits = n3bits;
	}
      /* Range from <large number> to <large number>-1 is a large signed
	 integral type.  Take care of the case where <large number> doesn't
	 fit in a long but <large number>-1 does.  */
      else if ((n2bits != 0 && n3bits != 0 && n2bits == n3bits + 1)
	       || (n2bits != 0 && n3bits == 0
		   && (n2bits == sizeof (long) * HOST_CHAR_BIT)
		   && n3 == LONG_MAX))
	{
	  got_signed = 1;
	  nbits = n2bits;
	}

      if (got_signed || got_unsigned)
	{
	  return init_type (TYPE_CODE_INT, nbits / TARGET_CHAR_BIT,
			    got_unsigned ? TYPE_FLAG_UNSIGNED : 0, NULL,
			    objfile);
	}
      else
	return error_type (pp);
    }

  /* A type defined as a subrange of itself, with bounds both 0, is void.  */
  if (self_subrange && n2 == 0 && n3 == 0)
    return init_type (TYPE_CODE_VOID, 0, 0, NULL, objfile);

  /* If n3 is zero and n2 is not, we want a floating type,
     and n2 is the width in bytes.

     Fortran programs appear to use this for complex types also,
     and they give no way to distinguish between double and single-complex!

     GDB does not have complex types.

     Just return the complex as a float of that size.  It won't work right
     for the complex values, but at least it makes the file loadable.  */

  if (n3 == 0 && n2 > 0)
    {
      return init_type (TYPE_CODE_FLT, n2, 0, NULL, objfile);
    }

  /* If the upper bound is -1, it must really be an unsigned int.  */

  else if (n2 == 0 && n3 == -1)
    {
      /* It is unsigned int or unsigned long.  */
      /* GCC 2.3.3 uses this for long long too, but that is just a GDB 3.5
	 compatibility hack.  */
      return init_type (TYPE_CODE_INT, TARGET_INT_BIT / TARGET_CHAR_BIT,
			TYPE_FLAG_UNSIGNED, NULL, objfile);
    }

  /* Special case: char is defined (Who knows why) as a subrange of
     itself with range 0-127.  */
  else if (self_subrange && n2 == 0 && n3 == 127)
    return init_type (TYPE_CODE_INT, 1, 0, NULL, objfile);

  /* We used to do this only for subrange of self or subrange of int.  */
  else if (n2 == 0)
    {
      if (n3 < 0)
	/* n3 actually gives the size.  */
	return init_type (TYPE_CODE_INT, - n3, TYPE_FLAG_UNSIGNED,
			  NULL, objfile);
      if (n3 == 0xff)
	return init_type (TYPE_CODE_INT, 1, TYPE_FLAG_UNSIGNED, NULL, objfile);
      if (n3 == 0xffff)
	return init_type (TYPE_CODE_INT, 2, TYPE_FLAG_UNSIGNED, NULL, objfile);

      /* -1 is used for the upper bound of (4 byte) "unsigned int" and
	 "unsigned long", and we already checked for that,
	 so don't need to test for it here.  */
    }
  /* I think this is for Convex "long long".  Since I don't know whether
     Convex sets self_subrange, I also accept that particular size regardless
     of self_subrange.  */
  else if (n3 == 0 && n2 < 0
	   && (self_subrange
	       || n2 == - TARGET_LONG_LONG_BIT / TARGET_CHAR_BIT))
    return init_type (TYPE_CODE_INT, - n2, 0, NULL, objfile);
  else if (n2 == -n3 -1)
    {
      if (n3 == 0x7f)
	return init_type (TYPE_CODE_INT, 1, 0, NULL, objfile);
      if (n3 == 0x7fff)
	return init_type (TYPE_CODE_INT, 2, 0, NULL, objfile);
      if (n3 == 0x7fffffff)
	return init_type (TYPE_CODE_INT, 4, 0, NULL, objfile);
    }

  /* We have a real range type on our hands.  Allocate space and
     return a real pointer.  */

  /* At this point I don't have the faintest idea how to deal with
     a self_subrange type; I'm going to assume that this is used
     as an idiom, and that all of them are special cases.  So . . .  */
  if (self_subrange)
    return error_type (pp);

  index_type = *dbx_lookup_type (rangenums);
  if (index_type == NULL)
    {
      /* Does this actually ever happen?  Is that why we are worrying
         about dealing with it rather than just calling error_type?  */

      static struct type *range_type_index;

      complain (&range_type_base_complaint, rangenums[1]);
      if (range_type_index == NULL)
	range_type_index =
	  init_type (TYPE_CODE_INT, TARGET_INT_BIT / TARGET_CHAR_BIT,
		     0, "range type index type", NULL);
      index_type = range_type_index;
    }

  result_type = create_range_type ((struct type *) NULL, index_type, n2, n3);
  return (result_type);
}

#if !defined (REG_STRUCT_HAS_ADDR)
#define REG_STRUCT_HAS_ADDR(gcc_p) 0
#endif

static char *type_synonym_name = NULL;

struct symbol *
os9k_define_symbol (valu, string, desc, type, objfile)
     CORE_ADDR valu;
     char *string;
     int desc;
     int type;
     struct objfile *objfile;
{
  register struct symbol *sym;
  char *p = (char *) strchr (string, ':');
  int deftype;
  int synonym = 0;
  register int i;

  /* We would like to eliminate nameless symbols, but keep their types.
     E.g. stab entry ":t10=*2" should produce a type 10, which is a pointer
     to type 2, but, should not create a symbol to address that type. Since
     the symbol will be nameless, there is no way any user can refer to it. */

  int nameless;

  /* Ignore syms with empty names.  */
  if (string[0] == 0)
    return 0;

  /* Ignore old-style symbols from cc -go  */
  if (p == 0)
    return 0;

  while (p[1] == ':')
    {
       p += 2;
       p = strchr(p, ':');
    }

  /* If a nameless stab entry, all we need is the type, not the symbol.
     e.g. ":t10=*2" or a nameless enum like " :T16=ered:0,green:1,blue:2,;" */
  nameless = (p == string || ((string[0] == ' ') && (string[1] == ':')));

  sym = (struct symbol *) 
    obstack_alloc (&objfile -> symbol_obstack, sizeof (struct symbol));
  memset (sym, 0, sizeof (struct symbol));

  if (processing_gcc_compilation)
    {
      /* GCC 2.x puts the line number in desc.  SunOS apparently puts in the
	 number of bytes occupied by a type or object, which we ignore.  */
      SYMBOL_LINE(sym) = desc;
    }
  else
    {
      SYMBOL_LINE(sym) = 0;			/* unknown */
    }

  {
      SYMBOL_LANGUAGE (sym) = current_subfile -> language;
      SYMBOL_NAME (sym)	= (char *)
	obstack_alloc (&objfile -> symbol_obstack, ((p - string) + 1));
      /* Open-coded memcpy--saves function call time.  */
      /* FIXME:  Does it really?  Try replacing with simple strcpy and
	 try it on an executable with a large symbol table. */
      /* FIXME: considering that gcc can open code memcpy anyway, I
	 doubt it.  xoxorich. */
      {
	register char *p1 = string;
	register char *p2 = SYMBOL_NAME (sym);
	while (p1 != p)
	  {
	    *p2++ = *p1++;
	  }
	*p2++ = '\0';
      }

      /* If this symbol is from a C++ compilation, then attempt to cache the
	 demangled form for future reference.  This is a typical time versus
	 space tradeoff, that was decided in favor of time because it sped up
	 C++ symbol lookups by a factor of about 20. */

      SYMBOL_INIT_DEMANGLED_NAME (sym, &objfile->symbol_obstack);
  }
  p++;

  /* Determine the type of name being defined.  */
#if 0
  /* Getting GDB to correctly skip the symbol on an undefined symbol
     descriptor and not ever dump core is a very dodgy proposition if
     we do things this way.  I say the acorn RISC machine can just
     fix their compiler.  */
  /* The Acorn RISC machine's compiler can put out locals that don't
     start with "234=" or "(3,4)=", so assume anything other than the
     deftypes we know how to handle is a local.  */
  if (!strchr ("cfFGpPrStTvVXCR", *p))
#else
  if (isdigit (*p) || *p == '(' || *p == '-')
#endif
    deftype = 'l';
  else
    deftype = *p++;

  switch (deftype)
    {
    case 'c':
      /* c is a special case, not followed by a type-number.
	 SYMBOL:c=iVALUE for an integer constant symbol.
	 SYMBOL:c=rVALUE for a floating constant symbol.
	 SYMBOL:c=eTYPE,INTVALUE for an enum constant symbol.
	 e.g. "b:c=e6,0" for "const b = blob1"
	 (where type 6 is defined by "blobs:t6=eblob1:0,blob2:1,;").  */
      if (*p != '=')
	{
	  SYMBOL_CLASS (sym) = LOC_CONST;
	  SYMBOL_TYPE (sym) = error_type (&p);
	  SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
	  add_symbol_to_list (sym, &file_symbols);
	  return sym;
	}
      ++p;
      switch (*p++)
	{
	case 'r':
	  {
	    double d = atof (p);
	    char *dbl_valu;

	    /* FIXME-if-picky-about-floating-accuracy: Should be using
	       target arithmetic to get the value.  real.c in GCC
	       probably has the necessary code.  */

	    /* FIXME: lookup_fundamental_type is a hack.  We should be
	       creating a type especially for the type of float constants.
	       Problem is, what type should it be?

	       Also, what should the name of this type be?  Should we
	       be using 'S' constants (see stabs.texinfo) instead?  */

	    SYMBOL_TYPE (sym) = lookup_fundamental_type (objfile,
							 FT_DBL_PREC_FLOAT);
	    dbl_valu = (char *)
	      obstack_alloc (&objfile -> symbol_obstack,
			     TYPE_LENGTH (SYMBOL_TYPE (sym)));
	    store_floating (dbl_valu, TYPE_LENGTH (SYMBOL_TYPE (sym)), d);
	    SYMBOL_VALUE_BYTES (sym) = dbl_valu;
	    SYMBOL_CLASS (sym) = LOC_CONST_BYTES;
	  }
	  break;
	case 'i':
	  {
	    /* Defining integer constants this way is kind of silly,
	       since 'e' constants allows the compiler to give not
	       only the value, but the type as well.  C has at least
	       int, long, unsigned int, and long long as constant
	       types; other languages probably should have at least
	       unsigned as well as signed constants.  */

	    /* We just need one int constant type for all objfiles.
	       It doesn't depend on languages or anything (arguably its
	       name should be a language-specific name for a type of
	       that size, but I'm inclined to say that if the compiler
	       wants a nice name for the type, it can use 'e').  */
	    static struct type *int_const_type;

	    /* Yes, this is as long as a *host* int.  That is because we
	       use atoi.  */
	    if (int_const_type == NULL)
	      int_const_type =
		init_type (TYPE_CODE_INT,
			   sizeof (int) * HOST_CHAR_BIT / TARGET_CHAR_BIT, 0,
			   "integer constant",
			   (struct objfile *)NULL);
	    SYMBOL_TYPE (sym) = int_const_type;
	    SYMBOL_VALUE (sym) = atoi (p);
	    SYMBOL_CLASS (sym) = LOC_CONST;
	  }
	  break;
	case 'e':
	  /* SYMBOL:c=eTYPE,INTVALUE for a constant symbol whose value
	     can be represented as integral.
	     e.g. "b:c=e6,0" for "const b = blob1"
	     (where type 6 is defined by "blobs:t6=eblob1:0,blob2:1,;").  */
	  {
	    SYMBOL_CLASS (sym) = LOC_CONST;
	    SYMBOL_TYPE (sym) = os9k_read_type (&p, objfile);

	    if (*p != ',')
	      {
		SYMBOL_TYPE (sym) = error_type (&p);
		break;
	      }
	    ++p;

	    /* If the value is too big to fit in an int (perhaps because
	       it is unsigned), or something like that, we silently get
	       a bogus value.  The type and everything else about it is
	       correct.  Ideally, we should be using whatever we have
	       available for parsing unsigned and long long values,
	       however.  */
	    SYMBOL_VALUE (sym) = atoi (p);
	  }
	  break;
	default:
	  {
	    SYMBOL_CLASS (sym) = LOC_CONST;
	    SYMBOL_TYPE (sym) = error_type (&p);
	  }
	}
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &file_symbols);
      return sym;

    case 'C':
      /* The name of a caught exception.  */
      SYMBOL_TYPE (sym) = os9k_read_type (&p, objfile);
      SYMBOL_CLASS (sym) = LOC_LABEL;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      SYMBOL_VALUE_ADDRESS (sym) = valu;
      add_symbol_to_list (sym, &local_symbols);
      break;

    case 'f':
      /* A static function definition.  */
      SYMBOL_TYPE (sym) = os9k_read_type (&p, objfile);
      SYMBOL_CLASS (sym) = LOC_BLOCK;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &file_symbols);
      /* fall into process_function_types.  */

    process_function_types:
      /* Function result types are described as the result type in stabs.
	 We need to convert this to the function-returning-type-X type
	 in GDB.  E.g. "int" is converted to "function returning int".  */
      if (TYPE_CODE (SYMBOL_TYPE (sym)) != TYPE_CODE_FUNC)
	{
#if 0
	  /* This code doesn't work -- it needs to realloc and can't.  */
	  /* Attempt to set up to record a function prototype... */
	  struct type *new = alloc_type (objfile);

	  /* Generate a template for the type of this function.  The 
	     types of the arguments will be added as we read the symbol 
	     table. */
	  *new = *lookup_function_type (SYMBOL_TYPE(sym));
	  SYMBOL_TYPE(sym) = new;
	  TYPE_OBJFILE (new) = objfile;
	  in_function_type = new;
#else
	  SYMBOL_TYPE (sym) = lookup_function_type (SYMBOL_TYPE (sym));
#endif
	}
      /* fall into process_prototype_types */

    process_prototype_types:
      /* Sun acc puts declared types of arguments here.  We don't care
	 about their actual types (FIXME -- we should remember the whole
	 function prototype), but the list may define some new types
	 that we have to remember, so we must scan it now.  */
      while (*p == ';') {
	p++;
	os9k_read_type (&p, objfile);
      }
      break;

    case 'F':
      /* A global function definition.  */
      SYMBOL_TYPE (sym) = os9k_read_type (&p, objfile);
      SYMBOL_CLASS (sym) = LOC_BLOCK;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &global_symbols);
      goto process_function_types;

    case 'V':
      SYMBOL_TYPE (sym) = os9k_read_type (&p, objfile);
      SYMBOL_VALUE_ADDRESS (sym) = valu;
/*
      i = hashname (SYMBOL_NAME (sym));
      SYMBOL_VALUE_CHAIN (sym) = global_sym_chain[i];
      global_sym_chain[i] = sym;
*/
      SYMBOL_CLASS (sym) = LOC_STATIC;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &global_symbols);
      break;

      /* This case is faked by a conditional above,
	 when there is no code letter in the dbx data.
	 Dbx data never actually contains 'l'.  */
    case 'l':
      SYMBOL_TYPE (sym) = os9k_read_type (&p, objfile);
      SYMBOL_CLASS (sym) = LOC_LOCAL;
      SYMBOL_VALUE (sym) = valu;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &local_symbols);
      break;

    case 'p':
      SYMBOL_TYPE (sym) = os9k_read_type (&p, objfile);

      /* Normally this is a parameter, a LOC_ARG.  On the i960, it
	 can also be a LOC_LOCAL_ARG depending on symbol type.  */
#ifndef DBX_PARM_SYMBOL_CLASS
#define	DBX_PARM_SYMBOL_CLASS(type)	LOC_ARG
#endif

      SYMBOL_CLASS (sym) = DBX_PARM_SYMBOL_CLASS (type);
      SYMBOL_VALUE (sym) = valu;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
#if 0
      /* This doesn't work yet.  */
      add_param_to_type (&in_function_type, sym);
#endif
      add_symbol_to_list (sym, &local_symbols);
      break;

    case 'P':
      /* acc seems to use P to delare the prototypes of functions that
         are referenced by this file.  gdb is not prepared to deal
         with this extra information.  FIXME, it ought to.  */
      if (type == N_FUN)
	{
	  os9k_read_type (&p, objfile);
	  goto process_prototype_types;
	}
      /*FALLTHROUGH*/

    case 'R':
#if 0
      /* Parameter which is in a register.  */
      SYMBOL_TYPE (sym) = os9k_read_type (&p, objfile);
      SYMBOL_CLASS (sym) = LOC_REGPARM;
      SYMBOL_VALUE (sym) = STAB_REG_TO_REGNUM (valu);
      if (SYMBOL_VALUE (sym) >= NUM_REGS)
	{
	  complain (&reg_value_complaint, SYMBOL_SOURCE_NAME (sym));
	  SYMBOL_VALUE (sym) = SP_REGNUM;  /* Known safe, though useless */
	}
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &local_symbols);
      break;
#endif 0
    case 'r':
      /* Register variable (either global or local).  */
      SYMBOL_TYPE (sym) = os9k_read_type (&p, objfile);
      SYMBOL_CLASS (sym) = LOC_REGISTER;
      SYMBOL_VALUE (sym) = STAB_REG_TO_REGNUM (valu);
      if (SYMBOL_VALUE (sym) >= NUM_REGS)
	{
	  complain (&reg_value_complaint, SYMBOL_SOURCE_NAME (sym));
	  SYMBOL_VALUE (sym) = SP_REGNUM;  /* Known safe, though useless */
	}
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      if (within_function)
	{
	  /* Sun cc uses a pair of symbols, one 'p' and one 'r' with the same
	     name to represent an argument passed in a register.
	     GCC uses 'P' for the same case.  So if we find such a symbol pair
	     we combine it into one 'P' symbol.

	     But we only do this in the REG_STRUCT_HAS_ADDR case, so that
	     we can still get information about what is going on with the
	     stack (VAX for computing args_printed, using stack slots instead
	     of saved registers in backtraces, etc.).

	     Note that this code illegally combines
	       main(argc) struct foo argc; { register struct foo argc; }
	     but this case is considered pathological and causes a warning
	     from a decent compiler.  */

	  if (local_symbols
	      && local_symbols->nsyms > 0
	      && REG_STRUCT_HAS_ADDR (processing_gcc_compilation)
	      && (TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_STRUCT
		  || TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_UNION))
	    {
	      struct symbol *prev_sym;
	      prev_sym = local_symbols->symbol[local_symbols->nsyms - 1];
	      if (SYMBOL_CLASS (prev_sym) == LOC_ARG
		  && STREQ (SYMBOL_NAME (prev_sym), SYMBOL_NAME(sym)))
		{
		  SYMBOL_CLASS (prev_sym) = LOC_REGPARM;
		  /* Use the type from the LOC_REGISTER; that is the type
		     that is actually in that register.  */
		  SYMBOL_TYPE (prev_sym) = SYMBOL_TYPE (sym);
		  SYMBOL_VALUE (prev_sym) = SYMBOL_VALUE (sym);
		  sym = prev_sym;
		  break;
		}
	    }
          add_symbol_to_list (sym, &local_symbols);
	}
      else
        add_symbol_to_list (sym, &file_symbols);
      break;

    case 't':
      SYMBOL_TYPE (sym) = os9k_read_type (&p, objfile);

      /* For a nameless type, we don't want a create a symbol, thus we
	 did not use `sym'. Return without further processing. */
      if (nameless) return NULL;

      SYMBOL_CLASS (sym) = LOC_TYPEDEF;
      SYMBOL_VALUE (sym) = valu;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;

      if (TYPE_NAME (SYMBOL_TYPE (sym)) == NULL)
	{
	  if (TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_PTR
	      || TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_FUNC)
	    {
	      /* If we are giving a name to a type such as "pointer to
		 foo" or "function returning foo", we better not set
		 the TYPE_NAME.  If the program contains "typedef char
		 *caddr_t;", we don't want all variables of type char
		 * to print as caddr_t.  This is not just a
		 consequence of GDB's type management; PCC and GCC (at
		 least through version 2.4) both output variables of
		 either type char * or caddr_t with the type number
		 defined in the 't' symbol for caddr_t.  If a future
		 compiler cleans this up it GDB is not ready for it
		 yet, but if it becomes ready we somehow need to
		 disable this check (without breaking the PCC/GCC2.4
		 case).

		 Sigh.

		 Fortunately, this check seems not to be necessary
		 for anything except pointers or functions.  */
	    }
	  else
	    TYPE_NAME (SYMBOL_TYPE (sym)) = SYMBOL_NAME (sym);
	}

      add_symbol_to_list (sym, &file_symbols);
      break;

    case 'T':
      /* Struct, union, or enum tag.  For GNU C++, this can be be followed
	 by 't' which means we are typedef'ing it as well.  */
      synonym = *p == 't';

      if (synonym)
	{
	  p++;
	  type_synonym_name = obsavestring (SYMBOL_NAME (sym),
					    strlen (SYMBOL_NAME (sym)),
					    &objfile -> symbol_obstack);
	}
      /* The semantics of C++ state that "struct foo { ... }" also defines 
	 a typedef for "foo".  Unfortunately, cfront never makes the typedef
	 when translating C++ into C.  We make the typedef here so that
	 "ptype foo" works as expected for cfront translated code.  */
      else if (current_subfile->language == language_cplus)
	{
	  synonym = 1;
	  type_synonym_name = obsavestring (SYMBOL_NAME (sym),
					    strlen (SYMBOL_NAME (sym)),
					    &objfile -> symbol_obstack);
	}

      SYMBOL_TYPE (sym) = os9k_read_type (&p, objfile);

      /* For a nameless type, we don't want a create a symbol, thus we
	 did not use `sym'. Return without further processing. */
      if (nameless) return NULL;

      SYMBOL_CLASS (sym) = LOC_TYPEDEF;
      SYMBOL_VALUE (sym) = valu;
     SYMBOL_NAMESPACE (sym) = STRUCT_NAMESPACE;
      if (TYPE_TAG_NAME (SYMBOL_TYPE (sym)) == 0)
	TYPE_TAG_NAME (SYMBOL_TYPE (sym))
	  = obconcat (&objfile -> type_obstack, "", "", SYMBOL_NAME (sym));
      add_symbol_to_list (sym, &file_symbols);

      if (synonym)
	{
	  /* Clone the sym and then modify it. */
	  register struct symbol *typedef_sym = (struct symbol *)
	    obstack_alloc (&objfile -> symbol_obstack, sizeof (struct symbol));
	  *typedef_sym = *sym;
	  SYMBOL_CLASS (typedef_sym) = LOC_TYPEDEF;
	  SYMBOL_VALUE (typedef_sym) = valu;
	  SYMBOL_NAMESPACE (typedef_sym) = VAR_NAMESPACE;
	  if (TYPE_NAME (SYMBOL_TYPE (sym)) == 0)
	    TYPE_NAME (SYMBOL_TYPE (sym))
	      = obconcat (&objfile -> type_obstack, "", "", SYMBOL_NAME (sym));
	  add_symbol_to_list (typedef_sym, &file_symbols);
	}
      break;

    case 'v':
      /* Static symbol at top level of file */
      SYMBOL_TYPE (sym) = os9k_read_type (&p, objfile);
      SYMBOL_CLASS (sym) = LOC_STATIC;
      SYMBOL_VALUE_ADDRESS (sym) = valu;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &file_symbols);
      break;

    default:
      SYMBOL_TYPE (sym) = error_type (&p);
      SYMBOL_CLASS (sym) = LOC_CONST;
      SYMBOL_VALUE (sym) = 0;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      add_symbol_to_list (sym, &file_symbols);
      break;
    }

  /* When passing structures to a function, some systems sometimes pass
     the address in a register, not the structure itself. 

     If REG_STRUCT_HAS_ADDR yields non-zero we have to convert LOC_REGPARM
     to LOC_REGPARM_ADDR for structures and unions.  */

  if (SYMBOL_CLASS (sym) == LOC_REGPARM
      && REG_STRUCT_HAS_ADDR (processing_gcc_compilation)
      && ((TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_STRUCT)
	  || (TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_UNION)))
    SYMBOL_CLASS (sym) = LOC_REGPARM_ADDR;

  return sym;
}


/* Read type information or a type definition; return the type.  Even
   though this routine accepts either type information or a type
   definition, the distinction is relevant--some parts of stabsread.c
   assume that type information starts with a digit, '-', or '(' in
   deciding whether to call read_type.  */

static struct type *
os9k_read_type (pp, objfile)
     register char **pp;
     struct objfile *objfile;
{
  register struct type *type = 0;
  struct type *type1;
  int typenums[2];
  int xtypenums[2];
  char type_descriptor;

  /* Size in bits of type if specified by a type attribute, or -1 if
     there is no size attribute.  */
  int type_size = -1;

  /* Used to distinguish string and bitstring from char-array and set. */
  int is_string = 0;

  /* Read type number if present.  The type number may be omitted.
     for instance in a two-dimensional array declared with type
     "ar1;1;10;ar1;1;10;4".  */
  if ((**pp >= '0' && **pp <= '9')
      || **pp == '('
      || **pp == '-')
    {
      if (read_type_number (pp, typenums) != 0)
	return error_type (pp);
      
      /* Type is not being defined here.  Either it already exists,
	 or this is a forward reference to it.  dbx_alloc_type handles
	 both cases.  */
      if (**pp != '=')
	return dbx_alloc_type (typenums, objfile);

      /* Type is being defined here.  */
      /* Skip the '='.  */
      ++(*pp);

      while (**pp == '@')
	{
	  char *p = *pp + 1;
	  /* It might be a type attribute or a member type.  */
	  if (isdigit (*p) || *p ==  '(' || *p == '-')
	    /* Member type.  */
	    break;
	  else
	    {
	      /* Type attributes.  */
	      char *attr = p;

	      /* Skip to the semicolon.  */
	      while (*p != ';' && *p != '\0')
		++p;
	      *pp = p;
	      if (*p == '\0')
		return error_type (pp);
	      else
		/* Skip the semicolon.  */
		++*pp;

	      switch (*attr)
		{
		case 's':
		  type_size = atoi (attr + 1);
		  if (type_size <= 0)
		    type_size = -1;
		  break;

		case 'S':
		  is_string = 1;
		  break;

		default:
		  /* Ignore unrecognized type attributes, so future compilers
		     can invent new ones.  */
		  break;
		}
	    }
	}
      /* Skip the type descriptor, we get it below with (*pp)[-1].  */
      ++(*pp);
    }
  else
    {
      /* 'typenums=' not present, type is anonymous.  Read and return
	 the definition, but don't put it in the type vector.  */
      typenums[0] = typenums[1] = -1;
      (*pp)++;
    }

  type_descriptor = (*pp)[-1];
  switch (type_descriptor)
    {
    case 'x':
      {
	enum type_code code;

	/* Used to index through file_symbols.  */
	struct pending *ppt;
	int i;
	
	/* Name including "struct", etc.  */
	char *type_name;
	
	{
	  char *from, *to, *p, *q1, *q2;
	  
	  /* Set the type code according to the following letter.  */
	  switch ((*pp)[0])
	    {
	    case 's':
	      code = TYPE_CODE_STRUCT;
	      break;
	    case 'u':
	      code = TYPE_CODE_UNION;
	      break;
	    case 'e':
	      code = TYPE_CODE_ENUM;
	      break;
	    default:
	      {
		/* Complain and keep going, so compilers can invent new
		   cross-reference types.  */
		static struct complaint msg =
		  {"Unrecognized cross-reference type `%c'", 0, 0};
		complain (&msg, (*pp)[0]);
		code = TYPE_CODE_STRUCT;
		break;
	      }
	    }
	   
	  q1 = strchr(*pp, '<');
	  p = strchr(*pp, ':');
	  if (p == NULL)
	    return error_type (pp);
	  while (q1 && p > q1 && p[1] == ':')
	    {
	       q2 = strchr(q1, '>');
	       if (!q2 || q2 < p)
		 break;
	       p += 2;
	       p = strchr(p, ':');
	       if (p == NULL)
		 return error_type (pp);
	    }
	  to = type_name = 
		(char *)obstack_alloc (&objfile->type_obstack, p - *pp + 1);
	
	  /* Copy the name.  */
	  from = *pp + 1;
	  while (from < p) 
	    *to++ = *from++;
	  *to = '\0';
	  
	  /* Set the pointer ahead of the name which we just read, and
	     the colon.  */
	  *pp = from + 1;
	}

	/* Now check to see whether the type has already been
	   declared.  This was written for arrays of cross-referenced
	   types before we had TYPE_CODE_TARGET_STUBBED, so I'm pretty
	   sure it is not necessary anymore.  But it might be a good
	   idea, to save a little memory.  */

	for (ppt = file_symbols; ppt; ppt = ppt->next)
	  for (i = 0; i < ppt->nsyms; i++)
	    {
	      struct symbol *sym = ppt->symbol[i];

	      if (SYMBOL_CLASS (sym) == LOC_TYPEDEF
		  && SYMBOL_NAMESPACE (sym) == STRUCT_NAMESPACE
		  && (TYPE_CODE (SYMBOL_TYPE (sym)) == code)
		  && STREQ (SYMBOL_NAME (sym), type_name))
		{
		  obstack_free (&objfile -> type_obstack, type_name);
		  type = SYMBOL_TYPE (sym);
		  return type;
		}
	    }

	/* Didn't find the type to which this refers, so we must
	   be dealing with a forward reference.  Allocate a type
	   structure for it, and keep track of it so we can
	   fill in the rest of the fields when we get the full
	   type.  */
	type = dbx_alloc_type (typenums, objfile);
	TYPE_CODE (type) = code;
	TYPE_TAG_NAME (type) = type_name;
	TYPE_NAME(type) = NULL;
	INIT_CPLUS_SPECIFIC(type);
	TYPE_FLAGS (type) |= TYPE_FLAG_STUB;

	add_undefined_type (type);
	return type;
      }

    case '-':				/* RS/6000 built-in type */
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '(':

      {
	char *pp_saved;

	(*pp)--;
	pp_saved = *pp;

	/* Peek ahead at the number to detect void.  */
	if (read_type_number (pp, xtypenums) != 0)
	  return error_type (pp);

	if (typenums[0] == xtypenums[0] && typenums[1] == xtypenums[1])
	  /* It's being defined as itself.  That means it is "void".  */
	  type = init_type (TYPE_CODE_VOID, 0, 0, NULL, objfile);
	else
	  {
	    struct type *xtype;

	    /* Go back to the number and have read_type get it.  This means
	       that we can deal with something like t(1,2)=(3,4)=... which
	       the Lucid compiler uses.  */
	    *pp = pp_saved;
	    xtype = os9k_read_type (pp, objfile);

	    /* The type is being defined to another type.  So we copy the type.
	       This loses if we copy a C++ class and so we lose track of how
	       the names are mangled (but g++ doesn't output stabs like this
	       now anyway).  */

	    type = alloc_type (objfile);
	    memcpy (type, xtype, sizeof (struct type));

	    /* The idea behind clearing the names is that the only purpose
	       for defining a type to another type is so that the name of
	       one can be different.  So we probably don't need to worry much
	       about the case where the compiler doesn't give a name to the
	       new type.  */
	    TYPE_NAME (type) = NULL;
	    TYPE_TAG_NAME (type) = NULL;
	  }
	if (typenums[0] != -1)
	  *dbx_lookup_type (typenums) = type;
	break;
      }

    /* In the following types, we must be sure to overwrite any existing
       type that the typenums refer to, rather than allocating a new one
       and making the typenums point to the new one.  This is because there
       may already be pointers to the existing type (if it had been
       forward-referenced), and we must change it to a pointer, function,
       reference, or whatever, *in-place*.  */

    case '*':
      type1 = os9k_read_type (pp, objfile);
      type = make_pointer_type (type1, dbx_lookup_type (typenums));
      break;

    case '&':				/* Reference to another type */
      type1 = os9k_read_type (pp, objfile);
      type = make_reference_type (type1, dbx_lookup_type (typenums));
      break;

    case 'f':				/* Function returning another type */
      if (**pp == '(') {
	while (**pp != ')') ++*pp;	/* skip os9000 function prototype */
        ++*pp;
      }
      type1 = os9k_read_type (pp, objfile);
      type = make_function_type (type1, dbx_lookup_type (typenums));
      break;

    case 'c':				/* Const qualifier on some type (Sun) */
      type = os9k_read_type (pp, objfile);
      /* FIXME! For now, we ignore const and volatile qualifiers.  */
      break;

    case 'b':                           /* Const volatile */
    case 'i':				/* Volatile qual on some type (Sun) */
      type = os9k_read_type (pp, objfile);
      /* FIXME! For now, we ignore const and volatile qualifiers.  */
      break;

/* FIXME -- we should be doing smash_to_XXX types here.  */
    case '@':				/* Member (class & variable) type */
      {
	struct type *domain = os9k_read_type (pp, objfile);
	struct type *memtype;

	if (**pp != ',')
	  /* Invalid member type data format.  */
	  return error_type (pp);
	++*pp;

	memtype = os9k_read_type (pp, objfile);
	type = dbx_alloc_type (typenums, objfile);
	smash_to_member_type (type, domain, memtype);
      }
      break;

    case 'r':				/* Range type */
      type = read_range_type (pp, typenums, objfile);
      if (typenums[0] != -1)
	*dbx_lookup_type (typenums) = type;
      break;

    case 'e':				/* Enumeration type */
      type = dbx_alloc_type (typenums, objfile);
      type = os9k_read_enum_type (pp, type, objfile);
      if (typenums[0] != -1)
	*dbx_lookup_type (typenums) = type;
      break;

    case 's':				/* Struct type */
    case 'u':				/* Union type */
      type = dbx_alloc_type (typenums, objfile);
      if (!TYPE_NAME (type))
	{
	  TYPE_NAME (type) = type_synonym_name;
	}
      type_synonym_name = NULL;
      switch (type_descriptor)
	{
	  case 's':
	    TYPE_CODE (type) = TYPE_CODE_STRUCT;
	    break;
	  case 'u':
	    TYPE_CODE (type) = TYPE_CODE_UNION;
	    break;
	}
      type = read_struct_type (pp, type, objfile);
      break;

    case 'a':				/* Array type */
      if (**pp == 'r')
        ++*pp;
      
      type = dbx_alloc_type (typenums, objfile);
      type = os9k_read_array_type (pp, type, objfile);
      if (is_string)
	TYPE_CODE (type) = TYPE_CODE_STRING;
      break;

    case 'S':
      type1 = os9k_read_type (pp, objfile);
      type = create_set_type ((struct type*) NULL, type1);
      if (is_string)
	TYPE_CODE (type) = TYPE_CODE_BITSTRING;
      if (typenums[0] != -1)
	*dbx_lookup_type (typenums) = type;
      break;

    default:
      --*pp;			/* Go back to the symbol in error */
				/* Particularly important if it was \0! */
      return error_type (pp);
    }

  if (type == 0)
    {
      warning ("GDB internal error, type is NULL in stabsread.c\n");
      return error_type (pp);
    }

  /* Size specified in a type attribute overrides any other size.  */
  if (type_size != -1)
    TYPE_LENGTH (type) = type_size / TARGET_CHAR_BIT;

  return type;
}

/* Read a definition of an array type,
   and create and return a suitable type object.
   Also creates a range type which represents the bounds of that
   array.  */

static struct type *
os9k_read_array_type (pp, type, objfile)
     register char **pp;
     register struct type *type;
     struct objfile *objfile;
{
  struct type *index_type, *element_type, *range_type;
  int lower, upper;
  int adjustable = 0;
  int nbits;

  /* Format of an array type:
     "arlower,upper;<array_contents_type>".  Put code in
     to handle this.

     Fortran adjustable arrays use Adigits or Tdigits for lower or upper;
     for these, produce a type like float[][].  */

  index_type = builtin_type_int;

  if (!(**pp >= '0' && **pp <= '9') && **pp != '-')
    {
      (*pp)++;
      adjustable = 1;
    }
  lower = read_huge_number (pp, ',', &nbits);
  if (nbits != 0)
    return error_type (pp);

  if (!(**pp >= '0' && **pp <= '9') && **pp != '-')
    {
      (*pp)++;
      adjustable = 1;
    }
  upper = read_huge_number (pp, ';', &nbits);
  if (nbits != 0)
    return error_type (pp);
  
  element_type = os9k_read_type (pp, objfile);

  if (adjustable)
    {
      lower = 0;
      upper = -1;
    }

  range_type =
    create_range_type ((struct type *) NULL, index_type, lower, upper);
  type = create_array_type (type, element_type, range_type);

  /* If we have an array whose element type is not yet known, but whose
     bounds *are* known, record it to be adjusted at the end of the file.  */
  /* FIXME: Why check for zero length rather than TYPE_FLAG_STUB?  I think
     the two have the same effect except that the latter is cleaner and the
     former would be wrong for types which really are zero-length (if we
     have any).  */

  if (TYPE_LENGTH (element_type) == 0 && !adjustable)
    {
      TYPE_FLAGS (type) |= TYPE_FLAG_TARGET_STUB;
      add_undefined_type (type);
    }

  return type;
}


/* Read a definition of an enumeration type,
   and create and return a suitable type object.
   Also defines the symbols that represent the values of the type.  */

static struct type *
os9k_read_enum_type (pp, type, objfile)
     register char **pp;
     register struct type *type;
     struct objfile *objfile;
{
  register char *p;
  char *name;
  register long n;
  register struct symbol *sym;
  int nsyms = 0;
  struct pending **symlist;
  struct pending *osyms, *syms;
  int o_nsyms;
  int size;
  int nbits;

#if 0
  /* FIXME!  The stabs produced by Sun CC merrily define things that ought
     to be file-scope, between N_FN entries, using N_LSYM.  What's a mother
     to do?  For now, force all enum values to file scope.  */
  if (within_function)
    symlist = &local_symbols;
  else
#endif
    symlist = &file_symbols;
  osyms = *symlist;
  o_nsyms = osyms ? osyms->nsyms : 0;

  /* Read the value-names and their values.
     The input syntax is NAME:VALUE,NAME:VALUE, and so on.
     A semicolon or comma instead of a NAME means the end.  */

  size = read_huge_number(pp, 0, &nbits);
  if (nbits != 0)
    return error_type (pp);
  while (**pp && **pp != ';' && **pp != ',')
    {
      STABS_CONTINUE (pp);
      p = *pp;
      while (*p != ':') p++;
      name = obsavestring (*pp, p - *pp, &objfile -> symbol_obstack);
      *pp = p + 1;
      n = read_huge_number (pp, ',', &nbits);
      if (nbits != 0)
	return error_type (pp);

      sym = (struct symbol *)
	obstack_alloc (&objfile -> symbol_obstack, sizeof (struct symbol));
      memset (sym, 0, sizeof (struct symbol));
      SYMBOL_NAME (sym) = name;
      SYMBOL_LANGUAGE (sym) = current_subfile -> language;
      SYMBOL_CLASS (sym) = LOC_CONST;
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      SYMBOL_VALUE (sym) = n;
      add_symbol_to_list (sym, symlist);
      nsyms++;
    }

  if (**pp == ';')
    (*pp)++;			/* Skip the semicolon.  */

  /* Now fill in the fields of the type-structure.  */

  TYPE_LENGTH (type) = TARGET_INT_BIT / HOST_CHAR_BIT;
  TYPE_CODE (type) = TYPE_CODE_ENUM;
  TYPE_FLAGS (type) &= ~TYPE_FLAG_STUB;
  TYPE_NFIELDS (type) = nsyms;
  TYPE_FIELDS (type) = (struct field *)
    TYPE_ALLOC (type, sizeof (struct field) * nsyms);
  memset (TYPE_FIELDS (type), 0, sizeof (struct field) * nsyms);

  /* Find the symbols for the values and put them into the type.
     The symbols can be found in the symlist that we put them on
     to cause them to be defined.  osyms contains the old value
     of that symlist; everything up to there was defined by us.  */
  /* Note that we preserve the order of the enum constants, so
     that in something like "enum {FOO, LAST_THING=FOO}" we print
     FOO, not LAST_THING.  */

  for (syms = *symlist, n = 0; syms; syms = syms->next)
    {
      int j = 0;
      if (syms == osyms)
	j = o_nsyms;
      for (; j < syms->nsyms; j++,n++)
	{
	  struct symbol *xsym = syms->symbol[j];
	  SYMBOL_TYPE (xsym) = type;
	  TYPE_FIELD_NAME (type, n) = SYMBOL_NAME (xsym);
	  TYPE_FIELD_VALUE (type, n) = 0;
	  TYPE_FIELD_BITPOS (type, n) = SYMBOL_VALUE (xsym);
	  TYPE_FIELD_BITSIZE (type, n) = 0;
	}
      if (syms == osyms)
	break;
    }

  return type;
}
