/* Symbol table lookup for the GNU debugger, GDB.
   Copyright (C) 1986, 1987, 1988, 1989, 1990 Free Software Foundation, Inc.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include "defs.h"
#include "symtab.h"
#include "param.h"
#include "gdbcore.h"
#include "frame.h"
#include "target.h"
#include "value.h"
#include "symfile.h"
#include "gdbcmd.h"

#include <obstack.h>
#include <assert.h>

#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

extern int close ();
extern void qsort ();
extern char *getenv ();

extern char *cplus_demangle ();
extern struct value *value_of_this ();
extern void break_command ();
extern void select_source_symtab ();

/* Functions this file defines */
static int find_line_common ();
struct partial_symtab *lookup_partial_symtab ();
static struct partial_symbol *lookup_partial_symbol ();

/* These variables point to the objects
   representing the predefined C data types.  */

struct type *builtin_type_void;
struct type *builtin_type_char;
struct type *builtin_type_short;
struct type *builtin_type_int;
struct type *builtin_type_long;
#ifdef LONG_LONG
struct type *builtin_type_long_long;
#endif
struct type *builtin_type_unsigned_char;
struct type *builtin_type_unsigned_short;
struct type *builtin_type_unsigned_int;
struct type *builtin_type_unsigned_long;
#ifdef LONG_LONG
struct type *builtin_type_unsigned_long_long;
#endif
struct type *builtin_type_float;
struct type *builtin_type_double;
struct type *builtin_type_error;

/* Block in which the most recently searched-for symbol was found.
   Might be better to make this a parameter to lookup_symbol and 
   value_of_this. */
struct block *block_found;

char no_symtab_msg[] = "No symbol table is loaded.  Use the \"file\" command.";

/* Check for a symtab of a specific name; first in symtabs, then in
   psymtabs.  *If* there is no '/' in the name, a match after a '/'
   in the symtab filename will also work.  */

static struct symtab *
lookup_symtab_1 (name)
     char *name;
{
  register struct symtab *s;
  register struct partial_symtab *ps;
  register char *slash = strchr (name, '/');
  register int len = strlen (name);

  for (s = symtab_list; s; s = s->next)
    if (!strcmp (name, s->filename))
      return s;

  for (ps = partial_symtab_list; ps; ps = ps->next)
    if (!strcmp (name, ps->filename))
      {
	if (ps->readin)
	  fatal ("Internal: readin pst found when no symtab found.");
	return PSYMTAB_TO_SYMTAB (ps);
      }

  if (!slash)
    {
      for (s = symtab_list; s; s = s->next)
	{
	  int l = strlen (s->filename);

	  if (s->filename[l - len -1] == '/'
	      && !strcmp (s->filename + l - len, name))
	    return s;
	}

      for (ps = partial_symtab_list; ps; ps = ps->next)
	{
	  int l = strlen (ps->filename);

	  if (ps->filename[l - len - 1] == '/'
	      && !strcmp (ps->filename + l - len, name))
	    {
	      if (ps->readin)
		fatal ("Internal: readin pst found when no symtab found.");
	      return PSYMTAB_TO_SYMTAB (ps);
	    }
	}
    }
  return 0;
}

/* Lookup the symbol table of a source file named NAME.  Try a couple
   of variations if the first lookup doesn't work.  */

struct symtab *
lookup_symtab (name)
     char *name;
{
  register struct symtab *s;
  register char *copy;

  s = lookup_symtab_1 (name);
  if (s) return s;

  /* If name not found as specified, see if adding ".c" helps.  */

  copy = (char *) alloca (strlen (name) + 3);
  strcpy (copy, name);
  strcat (copy, ".c");
  s = lookup_symtab_1 (copy);
  if (s) return s;

  /* We didn't find anything; die.  */
  return 0;
}

/* Lookup the partial symbol table of a source file named NAME.  This
   only returns true on an exact match (ie. this semantics are
   different from lookup_symtab.  */

struct partial_symtab *
lookup_partial_symtab (name)
char *name;
{
  register struct partial_symtab *s;
  
  for (s = partial_symtab_list; s; s = s->next)
    if (!strcmp (name, s->filename))
      return s;
  
  return 0;
}

/* Return a typename for a struct/union/enum type
   without the tag qualifier.  If the type has a NULL name,
   NULL is returned.  */
char *
type_name_no_tag (type)
     register struct type *type;
{
  register char *name = TYPE_NAME (type);
  char *strchr ();
  if (name == 0)
    return 0;

#if 0
  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_STRUCT:
      return name + 7;
    case TYPE_CODE_UNION:
      return name + 6;
    case TYPE_CODE_ENUM:
      return name + 5;
    }
#endif

  name = strchr (name, ' ');
  if (name)
    return name + 1;

  return TYPE_NAME (type);
}

/* Added by Bryan Boreham, Kewill, Sun Sep 17 18:07:17 1989.

   If this is a stubbed struct (i.e. declared as struct foo *), see if
   we can find a full definition in some other file. If so, copy this
   definition, so we can use it in future.  If not, set a flag so we 
   don't waste too much time in future.

   This used to be coded as a macro, but I don't think it is called 
   often enough to merit such treatment.
*/

struct complaint stub_noname_complaint =
  {"stub type has NULL name", 0, 0};

void 
check_stub_type(type)
     struct type *type;
{
  if (TYPE_FLAGS(type) & TYPE_FLAG_STUB)
    {
      char* name= type_name_no_tag (type);
      struct symbol *sym;
      if (name == 0)
	{
	  complain (&stub_noname_complaint, 0, 0);
	  return;
	}
      if (sym = lookup_symbol (name, 0, STRUCT_NAMESPACE, 0, 
			       (struct symtab **)NULL) )
	bcopy (SYMBOL_TYPE(sym), type, sizeof (struct type));
    }
}

/* Demangle a GDB method stub type.  */
char *
gdb_mangle_typename (type)
     struct type *type;
{
  static struct type *last_type;
  static char *mangled_typename;

  if (type != last_type)
    {
      /* Need a new type prefix.  */
      char *strchr ();
      char *newname = type_name_no_tag (type);
      char buf[20];
      int len;

      if (mangled_typename)
	free (mangled_typename);

      len = strlen (newname);
      sprintf (buf, "__%d", len);
      mangled_typename = (char *)xmalloc (strlen (buf) + len + 1);
      strcpy (mangled_typename, buf);
      strcat (mangled_typename, newname);
      /* Now we have built "__#newname".  */
    }
  return mangled_typename;
}

/* Lookup a primitive type named NAME. 
   Return zero if NAME is not a primitive type.*/

struct type *
lookup_primitive_typename (name)
     char *name;
{
  if (!strcmp (name, "int"))
    return builtin_type_int;
  if (!strcmp (name, "long"))
    return builtin_type_long;
  if (!strcmp (name, "short"))
    return builtin_type_short;
  if (!strcmp (name, "char"))
    return builtin_type_char;
  if (!strcmp (name, "float"))
    return builtin_type_float;
  if (!strcmp (name, "double"))
    return builtin_type_double;
  if (!strcmp (name, "void"))
    return builtin_type_void;
  return 0;
}

/* Lookup a typedef or primitive type named NAME,
   visible in lexical block BLOCK.
   If NOERR is nonzero, return zero if NAME is not suitably defined.  */

struct type *
lookup_typename (name, block, noerr)
     char *name;
     struct block *block;
     int noerr;
{
  register struct symbol *sym =
    lookup_symbol (name, block, VAR_NAMESPACE, 0, (struct symtab **)NULL);
  if (sym == 0 || SYMBOL_CLASS (sym) != LOC_TYPEDEF)
    {
      struct type *tmp;
      tmp = lookup_primitive_typename (name);
      if (!tmp && noerr)
	return 0;
      error ("No type named %s.", name);
    }
  return SYMBOL_TYPE (sym);
}

struct type *
lookup_unsigned_typename (name)
     char *name;
{
  if (!strcmp (name, "int"))
    return builtin_type_unsigned_int;
  if (!strcmp (name, "long"))
    return builtin_type_unsigned_long;
  if (!strcmp (name, "short"))
    return builtin_type_unsigned_short;
  if (!strcmp (name, "char"))
    return builtin_type_unsigned_char;
  error ("No type named unsigned %s.", name);
  return (struct type *)-1;  /* for lint */
}

/* Lookup a structure type named "struct NAME",
   visible in lexical block BLOCK.  */

struct type *
lookup_struct (name, block)
     char *name;
     struct block *block;
{
  register struct symbol *sym 
    = lookup_symbol (name, block, STRUCT_NAMESPACE, 0, (struct symtab **)NULL);

  if (sym == 0)
    error ("No struct type named %s.", name);
  if (TYPE_CODE (SYMBOL_TYPE (sym)) != TYPE_CODE_STRUCT)
    error ("This context has class, union or enum %s, not a struct.", name);
  return SYMBOL_TYPE (sym);
}

/* Lookup a union type named "union NAME",
   visible in lexical block BLOCK.  */

struct type *
lookup_union (name, block)
     char *name;
     struct block *block;
{
  register struct symbol *sym 
    = lookup_symbol (name, block, STRUCT_NAMESPACE, 0, (struct symtab **)NULL);

  if (sym == 0)
    error ("No union type named %s.", name);
  if (TYPE_CODE (SYMBOL_TYPE (sym)) != TYPE_CODE_UNION)
    error ("This context has class, struct or enum %s, not a union.", name);
  return SYMBOL_TYPE (sym);
}

/* Lookup an enum type named "enum NAME",
   visible in lexical block BLOCK.  */

struct type *
lookup_enum (name, block)
     char *name;
     struct block *block;
{
  register struct symbol *sym 
    = lookup_symbol (name, block, STRUCT_NAMESPACE, 0, (struct symtab **)NULL);
  if (sym == 0)
    error ("No enum type named %s.", name);
  if (TYPE_CODE (SYMBOL_TYPE (sym)) != TYPE_CODE_ENUM)
    error ("This context has class, struct or union %s, not an enum.", name);
  return SYMBOL_TYPE (sym);
}

/* Given a type TYPE, lookup the type of the component of type named
   NAME.  */

struct type *
lookup_struct_elt_type (type, name)
     struct type *type;
     char *name;
{
  int i;

  if (TYPE_CODE (type) != TYPE_CODE_STRUCT
      && TYPE_CODE (type) != TYPE_CODE_UNION)
    {
      target_terminal_ours ();
      fflush (stdout);
      fprintf (stderr, "Type ");
      type_print (type, "", stderr, -1);
      error (" is not a structure or union type.");
    }

  for (i = TYPE_NFIELDS (type) - 1; i >= TYPE_N_BASECLASSES (type); i--)
    if (!strcmp (TYPE_FIELD_NAME (type, i), name))
      return TYPE_FIELD_TYPE (type, i);

  target_terminal_ours ();
  fflush (stdout);
  fprintf (stderr, "Type ");
  type_print (type, "", stderr, -1);
  fprintf (stderr, " has no component named ");
  fputs_filtered (name, stderr);
  error (".");
  return (struct type *)-1;	/* For lint */
}

/* Given a type TYPE, return a type of pointers to that type.
   May need to construct such a type if this is the first use.

   C++: use TYPE_MAIN_VARIANT and TYPE_CHAIN to keep pointer
   to member types under control.  */

struct type *
lookup_pointer_type (type)
     struct type *type;
{
  register struct type *ptype = TYPE_POINTER_TYPE (type);
  if (ptype) return TYPE_MAIN_VARIANT (ptype);

  /* This is the first time anyone wanted a pointer to a TYPE.  */
  if (TYPE_FLAGS (type) & TYPE_FLAG_PERM)
    ptype  = (struct type *) xmalloc (sizeof (struct type));
  else
    ptype  = (struct type *) obstack_alloc (symbol_obstack,
					    sizeof (struct type));

  bzero (ptype, sizeof (struct type));
  TYPE_MAIN_VARIANT (ptype) = ptype;
  TYPE_TARGET_TYPE (ptype) = type;
  TYPE_POINTER_TYPE (type) = ptype;
  /* New type is permanent if type pointed to is permanent.  */
  if (TYPE_FLAGS (type) & TYPE_FLAG_PERM)
    TYPE_FLAGS (ptype) |= TYPE_FLAG_PERM;
  /* We assume the machine has only one representation for pointers!  */
  TYPE_LENGTH (ptype) = sizeof (char *);
  TYPE_CODE (ptype) = TYPE_CODE_PTR;
  return ptype;
}

struct type *
lookup_reference_type (type)
     struct type *type;
{
  register struct type *rtype = TYPE_REFERENCE_TYPE (type);
  if (rtype) return TYPE_MAIN_VARIANT (rtype);

  /* This is the first time anyone wanted a pointer to a TYPE.  */
  if (TYPE_FLAGS (type) & TYPE_FLAG_PERM)
    rtype  = (struct type *) xmalloc (sizeof (struct type));
  else
    rtype  = (struct type *) obstack_alloc (symbol_obstack,
					    sizeof (struct type));

  bzero (rtype, sizeof (struct type));
  TYPE_MAIN_VARIANT (rtype) = rtype;
  TYPE_TARGET_TYPE (rtype) = type;
  TYPE_REFERENCE_TYPE (type) = rtype;
  /* New type is permanent if type pointed to is permanent.  */
  if (TYPE_FLAGS (type) & TYPE_FLAG_PERM)
    TYPE_FLAGS (rtype) |= TYPE_FLAG_PERM;
  /* We assume the machine has only one representation for pointers!  */
  TYPE_LENGTH (rtype) = sizeof (char *);
  TYPE_CODE (rtype) = TYPE_CODE_REF;
  return rtype;
}


/* Implement direct support for MEMBER_TYPE in GNU C++.
   May need to construct such a type if this is the first use.
   The TYPE is the type of the member.  The DOMAIN is the type
   of the aggregate that the member belongs to.  */

struct type *
lookup_member_type (type, domain)
     struct type *type, *domain;
{
  register struct type *mtype = TYPE_MAIN_VARIANT (type);
  struct type *main_type;

  main_type = mtype;
  while (mtype)
    {
      if (TYPE_DOMAIN_TYPE (mtype) == domain)
	return mtype;
      mtype = TYPE_NEXT_VARIANT (mtype);
    }

  /* This is the first time anyone wanted this member type.  */
  if (TYPE_FLAGS (type) & TYPE_FLAG_PERM)
    mtype  = (struct type *) xmalloc (sizeof (struct type));
  else
    mtype  = (struct type *) obstack_alloc (symbol_obstack,
					    sizeof (struct type));

  bzero (mtype, sizeof (struct type));
  if (main_type == 0)
    main_type = mtype;
  else
    {
      TYPE_NEXT_VARIANT (mtype) = TYPE_NEXT_VARIANT (main_type);
      TYPE_NEXT_VARIANT (main_type) = mtype;
    }
  TYPE_MAIN_VARIANT (mtype) = main_type;
  TYPE_TARGET_TYPE (mtype) = type;
  TYPE_DOMAIN_TYPE (mtype) = domain;
  /* New type is permanent if type pointed to is permanent.  */
  if (TYPE_FLAGS (type) & TYPE_FLAG_PERM)
    TYPE_FLAGS (mtype) |= TYPE_FLAG_PERM;

  /* In practice, this is never used.  */
  TYPE_LENGTH (mtype) = 1;
  TYPE_CODE (mtype) = TYPE_CODE_MEMBER;

#if 0
  /* Now splice in the new member pointer type.  */
  if (main_type)
    {
      /* This type was not "smashed".  */
      TYPE_CHAIN (mtype) = TYPE_CHAIN (main_type);
      TYPE_CHAIN (main_type) = mtype;
    }
#endif

  return mtype;
}

struct type *
lookup_method_type (type, domain, args)
     struct type *type, *domain, **args;
{
  register struct type *mtype = TYPE_MAIN_VARIANT (type);
  struct type *main_type;

  main_type = mtype;
  while (mtype)
    {
      if (TYPE_DOMAIN_TYPE (mtype) == domain)
	{
	  struct type **t1 = args;
	  struct type **t2 = TYPE_ARG_TYPES (mtype);
	  if (t2)
	    {
	      int i;
	      for (i = 0; t1[i] != 0 && t1[i]->code != TYPE_CODE_VOID; i++)
		if (t1[i] != t2[i])
		  break;
	      if (t1[i] == t2[i])
		return mtype;
	    }
	}
      mtype = TYPE_NEXT_VARIANT (mtype);
    }

  /* This is the first time anyone wanted this member type.  */
  if (TYPE_FLAGS (type) & TYPE_FLAG_PERM)
    mtype  = (struct type *) xmalloc (sizeof (struct type));
  else
    mtype  = (struct type *) obstack_alloc (symbol_obstack,
					    sizeof (struct type));

  bzero (mtype, sizeof (struct type));
  if (main_type == 0)
    main_type = mtype;
  else
    {
      TYPE_NEXT_VARIANT (mtype) = TYPE_NEXT_VARIANT (main_type);
      TYPE_NEXT_VARIANT (main_type) = mtype;
    }
  TYPE_MAIN_VARIANT (mtype) = main_type;
  TYPE_TARGET_TYPE (mtype) = type;
  TYPE_DOMAIN_TYPE (mtype) = domain;
  TYPE_ARG_TYPES (mtype) = args;
  /* New type is permanent if type pointed to is permanent.  */
  if (TYPE_FLAGS (type) & TYPE_FLAG_PERM)
    TYPE_FLAGS (mtype) |= TYPE_FLAG_PERM;

  /* In practice, this is never used.  */
  TYPE_LENGTH (mtype) = 1;
  TYPE_CODE (mtype) = TYPE_CODE_METHOD;

#if 0
  /* Now splice in the new member pointer type.  */
  if (main_type)
    {
      /* This type was not "smashed".  */
      TYPE_CHAIN (mtype) = TYPE_CHAIN (main_type);
      TYPE_CHAIN (main_type) = mtype;
    }
#endif

  return mtype;
}

#if 0
/* Given a type TYPE, return a type which has offset OFFSET,
   via_virtual VIA_VIRTUAL, and via_public VIA_PUBLIC.
   May need to construct such a type if none exists.  */
struct type *
lookup_basetype_type (type, offset, via_virtual, via_public)
     struct type *type;
     int offset;
     int via_virtual, via_public;
{
  register struct type *btype = TYPE_MAIN_VARIANT (type);
  struct type *main_type;

  if (offset != 0)
    {
      printf ("Internal error: type offset non-zero in lookup_basetype_type");
      offset = 0;
    }

  main_type = btype;
  while (btype)
    {
      if (/* TYPE_OFFSET (btype) == offset
	  && */ TYPE_VIA_PUBLIC (btype) == via_public
	  && TYPE_VIA_VIRTUAL (btype) == via_virtual)
	return btype;
      btype = TYPE_NEXT_VARIANT (btype);
    }

  /* This is the first time anyone wanted this member type.  */
  if (TYPE_FLAGS (type) & TYPE_FLAG_PERM)
    btype  = (struct type *) xmalloc (sizeof (struct type));
  else
    btype  = (struct type *) obstack_alloc (symbol_obstack,
					    sizeof (struct type));

  if (main_type == 0)
    {
      main_type = btype;
      bzero (btype, sizeof (struct type));
      TYPE_MAIN_VARIANT (btype) = main_type;
    }
  else
    {
      bcopy (main_type, btype, sizeof (struct type));
      TYPE_NEXT_VARIANT (main_type) = btype;
    }
/* TYPE_OFFSET (btype) = offset; */
  if (via_public)
    TYPE_FLAGS (btype) |= TYPE_FLAG_VIA_PUBLIC;
  if (via_virtual)
    TYPE_FLAGS (btype) |= TYPE_FLAG_VIA_VIRTUAL;
  /* New type is permanent if type pointed to is permanent.  */
  if (TYPE_FLAGS (type) & TYPE_FLAG_PERM)
    TYPE_FLAGS (btype) |= TYPE_FLAG_PERM;

  /* In practice, this is never used.  */
  TYPE_LENGTH (btype) = 1;
  TYPE_CODE (btype) = TYPE_CODE_STRUCT;

  return btype;
}
#endif

/* Given a type TYPE, return a type of functions that return that type.
   May need to construct such a type if this is the first use.  */

struct type *
lookup_function_type (type)
     struct type *type;
{
  register struct type *ptype = TYPE_FUNCTION_TYPE (type);
  if (ptype) return ptype;

  /* This is the first time anyone wanted a function returning a TYPE.  */
  if (TYPE_FLAGS (type) & TYPE_FLAG_PERM)
    ptype  = (struct type *) xmalloc (sizeof (struct type));
  else
    ptype  = (struct type *) obstack_alloc (symbol_obstack,
					    sizeof (struct type));

  bzero (ptype, sizeof (struct type));
  TYPE_TARGET_TYPE (ptype) = type;
  TYPE_FUNCTION_TYPE (type) = ptype;
  /* New type is permanent if type returned is permanent.  */
  if (TYPE_FLAGS (type) & TYPE_FLAG_PERM)
    TYPE_FLAGS (ptype) |= TYPE_FLAG_PERM;
  TYPE_LENGTH (ptype) = 1;
  TYPE_CODE (ptype) = TYPE_CODE_FUNC;
  TYPE_NFIELDS (ptype) = 0;
  return ptype;
}

/* Create an array type.  Elements will be of type TYPE, and there will
   be NUM of them.

   Eventually this should be extended to take two more arguments which
   specify the bounds of the array and the type of the index.
   It should also be changed to be a "lookup" function, with the
   appropriate data structures added to the type field.
   Then read array type should call here.  */

struct type *
create_array_type (element_type, number)
     struct type *element_type;
     int number;
{
  struct type *result_type = (struct type *)
    obstack_alloc (symbol_obstack, sizeof (struct type));

  bzero (result_type, sizeof (struct type));

  TYPE_CODE (result_type) = TYPE_CODE_ARRAY;
  TYPE_TARGET_TYPE (result_type) = element_type;
  TYPE_LENGTH (result_type) = number * TYPE_LENGTH (element_type);
  TYPE_NFIELDS (result_type) = 1;
  TYPE_FIELDS (result_type) =
    (struct field *) obstack_alloc (symbol_obstack, sizeof (struct field));
  TYPE_FIELD_TYPE (result_type, 0) = builtin_type_int;
  TYPE_VPTR_FIELDNO (result_type) = -1;

  return result_type;
}


/* Smash TYPE to be a type of members of DOMAIN with type TO_TYPE.  */

void
smash_to_member_type (type, domain, to_type)
     struct type *type, *domain, *to_type;
{
  bzero (type, sizeof (struct type));
  TYPE_TARGET_TYPE (type) = to_type;
  TYPE_DOMAIN_TYPE (type) = domain;

  /* In practice, this is never needed.  */
  TYPE_LENGTH (type) = 1;
  TYPE_CODE (type) = TYPE_CODE_MEMBER;

  TYPE_MAIN_VARIANT (type) = lookup_member_type (domain, to_type);
}

/* Smash TYPE to be a type of method of DOMAIN with type TO_TYPE.  */

void
smash_to_method_type (type, domain, to_type, args)
     struct type *type, *domain, *to_type, **args;
{
  bzero (type, sizeof (struct type));
  TYPE_TARGET_TYPE (type) = to_type;
  TYPE_DOMAIN_TYPE (type) = domain;
  TYPE_ARG_TYPES (type) = args;

  /* In practice, this is never needed.  */
  TYPE_LENGTH (type) = 1;
  TYPE_CODE (type) = TYPE_CODE_METHOD;

  TYPE_MAIN_VARIANT (type) = lookup_method_type (domain, to_type, args);
}

/* Find which partial symtab on the partial_symtab_list contains
   PC.  Return 0 if none.  */

struct partial_symtab *
find_pc_psymtab (pc)
     register CORE_ADDR pc;
{
  register struct partial_symtab *ps;

  for (ps = partial_symtab_list; ps; ps = ps->next)
    if (pc >= ps->textlow && pc < ps->texthigh)
      return ps;

  return 0;
}

/* Find which partial symbol within a psymtab contains PC.  Return 0
   if none.  Check all psymtabs if PSYMTAB is 0.  */
struct partial_symbol *
find_pc_psymbol (psymtab, pc)
     struct partial_symtab *psymtab;
     CORE_ADDR pc;
{
  struct partial_symbol *best, *p;
  CORE_ADDR best_pc;
  
  if (!psymtab)
    psymtab = find_pc_psymtab (pc);
  if (!psymtab)
    return 0;

  best_pc = psymtab->textlow - 1;

  for (p = static_psymbols.list + psymtab->statics_offset;
       (p - (static_psymbols.list + psymtab->statics_offset)
	< psymtab->n_static_syms);
       p++)
    if (SYMBOL_NAMESPACE (p) == VAR_NAMESPACE
	&& SYMBOL_CLASS (p) == LOC_BLOCK
	&& pc >= SYMBOL_VALUE_ADDRESS (p)
	&& SYMBOL_VALUE_ADDRESS (p) > best_pc)
      {
	best_pc = SYMBOL_VALUE_ADDRESS (p);
	best = p;
      }
  if (best_pc == psymtab->textlow - 1)
    return 0;
  return best;
}


/* Find the definition for a specified symbol name NAME
   in namespace NAMESPACE, visible from lexical block BLOCK.
   Returns the struct symbol pointer, or zero if no symbol is found.
   If SYMTAB is non-NULL, store the symbol table in which the
   symbol was found there, or NULL if not found.
   C++: if IS_A_FIELD_OF_THIS is nonzero on entry, check to see if
   NAME is a field of the current implied argument `this'.  If so set
   *IS_A_FIELD_OF_THIS to 1, otherwise set it to zero. 
   BLOCK_FOUND is set to the block in which NAME is found (in the case of
   a field of `this', value_of_this sets BLOCK_FOUND to the proper value.) */

struct symbol *
lookup_symbol (name, block, namespace, is_a_field_of_this, symtab)
     char *name;
     register struct block *block;
     enum namespace namespace;
     int *is_a_field_of_this;
     struct symtab **symtab;
{
  register struct symbol *sym;
  register struct symtab *s;
  register struct partial_symtab *ps;
  struct blockvector *bv;

  /* Search specified block and its superiors.  */

  while (block != 0)
    {
      sym = lookup_block_symbol (block, name, namespace);
      if (sym) 
	{
	  block_found = block;
	  if (symtab != NULL)
	    {
	      /* Search the list of symtabs for one which contains the
		 address of the start of this block.  */
	      struct block *b;
	      for (s = symtab_list; s; s = s->next)
		{
		  bv = BLOCKVECTOR (s);
		  b = BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK);
		  if (BLOCK_START (b) <= BLOCK_START (block)
		      && BLOCK_END (b) > BLOCK_START (block))
		    break;
		}
	      *symtab = s;
	    }

	  return sym;
	}
      block = BLOCK_SUPERBLOCK (block);
    }

  /* C++: If requested to do so by the caller, 
     check to see if NAME is a field of `this'. */
  if (is_a_field_of_this)
    {
      struct value *v = value_of_this (0);
      
      *is_a_field_of_this = 0;
      if (v && check_field (v, name))
	{
	  *is_a_field_of_this = 1;
	  if (symtab != NULL)
	    *symtab = NULL;
	  return 0;
	}
    }

  /* Now search all global blocks.  Do the symtab's first, then
     check the psymtab's */

  for (s = symtab_list; s; s = s->next)
    {
      bv = BLOCKVECTOR (s);
      block = BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK);
      sym = lookup_block_symbol (block, name, namespace);
      if (sym) 
	{
	  block_found = block;
	  if (symtab != NULL)
	    *symtab = s;
	  return sym;
	}
    }

  /* Check for the possibility of the symbol being a global function
     that is stored on the misc function vector.  Eventually, all
     global symbols might be resolved in this way.  */
  
  if (namespace == VAR_NAMESPACE)
    {
      int ind = lookup_misc_func (name);

      /* Look for a mangled C++ name for NAME. */
      if (ind == -1)
	{
	  int name_len = strlen (name);

	  for (ind = misc_function_count; --ind >= 0; )
	      /* Assume orginal name is prefix of mangled name. */
	      if (!strncmp (misc_function_vector[ind].name, name, name_len))
		{
		  char *demangled =
		      cplus_demangle(misc_function_vector[ind].name, -1);
		  if (demangled != NULL)
		    {
		      int cond = strcmp (demangled, name);
		      free (demangled);
		      if (!cond)
			  break;
		    }
	        }
	  /* Loop terminates on no match with ind == -1. */
        }

      if (ind != -1)
	{
	  s = find_pc_symtab (misc_function_vector[ind].address);
	  if (s)
	    {
	      bv = BLOCKVECTOR (s);
	      block = BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK);
	      sym = lookup_block_symbol (block, misc_function_vector[ind].name,
				         namespace);
	      /* sym == 0 if symbol was found in the misc_function_vector
		 but not in the symtab.
		 Return 0 to use the misc_function definition of "foo_".

		 This happens for Fortran  "foo_" symbols,
		 which are "foo" in the symtab.

		 This can also happen if "asm" is used to make a
		 regular symbol but not a debugging symbol, e.g.
		 asm(".globl _main");
		 asm("_main:");
		 */

	      if (symtab != NULL)
		*symtab = s;
	      return sym;
	    }
	}
    }
      
  for (ps = partial_symtab_list; ps; ps = ps->next)
    if (!ps->readin && lookup_partial_symbol (ps, name, 1, namespace))
      {
	s = PSYMTAB_TO_SYMTAB(ps);
	bv = BLOCKVECTOR (s);
	block = BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK);
	sym = lookup_block_symbol (block, name, namespace);
	if (!sym)
	  fatal ("Internal: global symbol found in psymtab but not in symtab");
	if (symtab != NULL)
	  *symtab = s;
	return sym;
      }

  /* Now search all per-file blocks.
     Not strictly correct, but more useful than an error.
     Do the symtabs first, then check the psymtabs */

  for (s = symtab_list; s; s = s->next)
    {
      bv = BLOCKVECTOR (s);
      block = BLOCKVECTOR_BLOCK (bv, STATIC_BLOCK);
      sym = lookup_block_symbol (block, name, namespace);
      if (sym) 
	{
	  block_found = block;
	  if (symtab != NULL)
	    *symtab = s;
	  return sym;
	}
    }

  for (ps = partial_symtab_list; ps; ps = ps->next)
    if (!ps->readin && lookup_partial_symbol (ps, name, 0, namespace))
      {
	s = PSYMTAB_TO_SYMTAB(ps);
	bv = BLOCKVECTOR (s);
	block = BLOCKVECTOR_BLOCK (bv, STATIC_BLOCK);
	sym = lookup_block_symbol (block, name, namespace);
	if (!sym)
	  fatal ("Internal: static symbol found in psymtab but not in symtab");
	if (symtab != NULL)
	  *symtab = s;
	return sym;
      }

  if (symtab != NULL)
    *symtab = NULL;
  return 0;
}

/* Look, in partial_symtab PST, for symbol NAME.  Check the global
   symbols if GLOBAL, the static symbols if not */

static struct partial_symbol *
lookup_partial_symbol (pst, name, global, namespace)
     struct partial_symtab *pst;
     char *name;
     int global;
     enum namespace namespace;
{
  struct partial_symbol *start, *psym;
  int length = (global ? pst->n_global_syms : pst->n_static_syms);

  if (!length)
    return (struct partial_symbol *) 0;
  
  start = (global ?
	   global_psymbols.list + pst->globals_offset :
	   static_psymbols.list + pst->statics_offset  );

  if (global)			/* This means we can use a binary */
				/* search.  */
    {
      struct partial_symbol *top, *bottom, *center;

      /* Binary search.  This search is guaranteed to end with center
         pointing at the earliest partial symbol with the correct
	 name.  At that point *all* partial symbols with that name
	 will be checked against the correct namespace. */
      bottom = start;
      top = start + length - 1;
      while (top > bottom)
	{
	  center = bottom + (top - bottom) / 2;

	  assert (center < top);
	  
	  if (strcmp (SYMBOL_NAME (center), name) >= 0)
	    top = center;
	  else
	    bottom = center + 1;
	}
      assert (top == bottom);
      
      while (!strcmp (SYMBOL_NAME (top), name))
	{
	  if (SYMBOL_NAMESPACE (top) == namespace)
	    return top;
	  top ++;
	}
    }
  else
    {
      /* Can't use a binary search */
      for (psym = start; psym < start + length; psym++)
	if (namespace == SYMBOL_NAMESPACE (psym)
	    && !strcmp (name, SYMBOL_NAME (psym)))
	  return psym;
    }

  return (struct partial_symbol *) 0;
}

/* Look for a symbol in block BLOCK.  */

struct symbol *
lookup_block_symbol (block, name, namespace)
     register struct block *block;
     char *name;
     enum namespace namespace;
{
  register int bot, top, inc;
  register struct symbol *sym, *parameter_sym;

  top = BLOCK_NSYMS (block);
  bot = 0;

  /* If the blocks's symbols were sorted, start with a binary search.  */

  if (BLOCK_SHOULD_SORT (block))
    {
      /* First, advance BOT to not far before
	 the first symbol whose name is NAME.  */

      while (1)
	{
	  inc = (top - bot + 1);
	  /* No need to keep binary searching for the last few bits worth.  */
	  if (inc < 4)
	    break;
	  inc = (inc >> 1) + bot;
	  sym = BLOCK_SYM (block, inc);
	  if (SYMBOL_NAME (sym)[0] < name[0])
	    bot = inc;
	  else if (SYMBOL_NAME (sym)[0] > name[0])
	    top = inc;
	  else if (strcmp (SYMBOL_NAME (sym), name) < 0)
	    bot = inc;
	  else
	    top = inc;
	}

      /* Now scan forward until we run out of symbols,
	 find one whose name is greater than NAME,
	 or find one we want.
	 If there is more than one symbol with the right name and namespace,
	 we return the first one.  dbxread.c is careful to make sure
	 that if one is a register then it comes first.  */

      top = BLOCK_NSYMS (block);
      while (bot < top)
	{
	  sym = BLOCK_SYM (block, bot);
	  inc = SYMBOL_NAME (sym)[0] - name[0];
	  if (inc == 0)
	    inc = strcmp (SYMBOL_NAME (sym), name);
	  if (inc == 0 && SYMBOL_NAMESPACE (sym) == namespace)
	    return sym;
	  if (inc > 0)
	    return 0;
	  bot++;
	}
      return 0;
    }

  /* Here if block isn't sorted.
     This loop is equivalent to the loop above,
     but hacked greatly for speed.

     Note that parameter symbols do not always show up last in the
     list; this loop makes sure to take anything else other than
     parameter symbols first; it only uses parameter symbols as a
     last resort.  Note that this only takes up extra computation
     time on a match.  */

  parameter_sym = (struct symbol *) 0;
  top = BLOCK_NSYMS (block);
  inc = name[0];
  while (bot < top)
    {
      sym = BLOCK_SYM (block, bot);
      if (SYMBOL_NAME (sym)[0] == inc
	  && !strcmp (SYMBOL_NAME (sym), name)
	  && SYMBOL_NAMESPACE (sym) == namespace)
	{
	  if (SYMBOL_CLASS (sym) == LOC_ARG
	      || SYMBOL_CLASS (sym) == LOC_LOCAL_ARG
	      || SYMBOL_CLASS (sym) == LOC_REF_ARG
	      || SYMBOL_CLASS (sym) == LOC_REGPARM)
	    parameter_sym = sym;
	  else
	    return sym;
	}
      bot++;
    }
  return parameter_sym;		/* Will be 0 if not found. */
}

/* Return the symbol for the function which contains a specified
   lexical block, described by a struct block BL.  */

struct symbol *
block_function (bl)
     struct block *bl;
{
  while (BLOCK_FUNCTION (bl) == 0 && BLOCK_SUPERBLOCK (bl) != 0)
    bl = BLOCK_SUPERBLOCK (bl);

  return BLOCK_FUNCTION (bl);
}

/* Subroutine of find_pc_line */

struct symtab *
find_pc_symtab (pc)
     register CORE_ADDR pc;
{
  register struct block *b;
  struct blockvector *bv;
  register struct symtab *s;
  register struct partial_symtab *ps;

  /* Search all symtabs for one whose file contains our pc */

  for (s = symtab_list; s; s = s->next)
    {
      bv = BLOCKVECTOR (s);
      b = BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK);
      if (BLOCK_START (b) <= pc
	  && BLOCK_END (b) > pc)
	break;
    }

  if (!s)
    {
      ps = find_pc_psymtab (pc);
      if (ps && ps->readin)
	fatal ("Internal error: pc in read in psymtab, but not in symtab.");

      if (ps)
	s = PSYMTAB_TO_SYMTAB (ps);
    }

  return s;
}

/* Find the source file and line number for a given PC value.
   Return a structure containing a symtab pointer, a line number,
   and a pc range for the entire source line.
   The value's .pc field is NOT the specified pc.
   NOTCURRENT nonzero means, if specified pc is on a line boundary,
   use the line that ends there.  Otherwise, in that case, the line
   that begins there is used.  */

struct symtab_and_line
find_pc_line (pc, notcurrent)
     CORE_ADDR pc;
     int notcurrent;
{
  struct symtab *s;
  register struct linetable *l;
  register int len;
  register int i;
  register struct linetable_entry *item;
  struct symtab_and_line val;
  struct blockvector *bv;

  /* Info on best line seen so far, and where it starts, and its file.  */

  int best_line = 0;
  CORE_ADDR best_pc = 0;
  CORE_ADDR best_end = 0;
  struct symtab *best_symtab = 0;

  /* Store here the first line number
     of a file which contains the line at the smallest pc after PC.
     If we don't find a line whose range contains PC,
     we will use a line one less than this,
     with a range from the start of that file to the first line's pc.  */
  int alt_line = 0;
  CORE_ADDR alt_pc = 0;
  struct symtab *alt_symtab = 0;

  /* Info on best line seen in this file.  */

  int prev_line;
  CORE_ADDR prev_pc;

  /* Info on first line of this file.  */

  int first_line;
  CORE_ADDR first_pc;

  /* If this pc is not from the current frame,
     it is the address of the end of a call instruction.
     Quite likely that is the start of the following statement.
     But what we want is the statement containing the instruction.
     Fudge the pc to make sure we get that.  */

  if (notcurrent) pc -= 1;

  s = find_pc_symtab (pc);
  if (s == 0)
    {
      val.symtab = 0;
      val.line = 0;
      val.pc = pc;
      val.end = 0;
      return val;
    }

  bv = BLOCKVECTOR (s);

  /* Look at all the symtabs that share this blockvector.
     They all have the same apriori range, that we found was right;
     but they have different line tables.  */

  for (; s && BLOCKVECTOR (s) == bv; s = s->next)
    {
      /* Find the best line in this symtab.  */
      l = LINETABLE (s);
      len = l->nitems;
      prev_line = -1;
      first_line = -1;
      for (i = 0; i < len; i++)
	{
	  item = &(l->item[i]);
	  
	  if (first_line < 0)
	    {
	      first_line = item->line;
	      first_pc = item->pc;
	    }
	  /* Return the last line that did not start after PC.  */
	  if (pc >= item->pc)
	    {
	      prev_line = item->line;
	      prev_pc = item->pc;
	    }
	  else
	    break;
	}

      /* Is this file's best line closer than the best in the other files?
	 If so, record this file, and its best line, as best so far.  */
      if (prev_line >= 0 && prev_pc > best_pc)
	{
	  best_pc = prev_pc;
	  best_line = prev_line;
	  best_symtab = s;
	  if (i < len)
	    best_end = item->pc;
	  else
	    best_end = 0;
	}
      /* Is this file's first line closer than the first lines of other files?
	 If so, record this file, and its first line, as best alternate.  */
      if (first_line >= 0 && first_pc > pc
	  && (alt_pc == 0 || first_pc < alt_pc))
	{
	  alt_pc = first_pc;
	  alt_line = first_line;
	  alt_symtab = s;
	}
    }
  if (best_symtab == 0)
    {
      val.symtab = alt_symtab;
      val.line = alt_line - 1;
      val.pc = BLOCK_END (BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK));
      val.end = alt_pc;
    }
  else
    {
      val.symtab = best_symtab;
      val.line = best_line;
      val.pc = best_pc;
      val.end = (best_end ? best_end
		   : (alt_pc ? alt_pc
		      : BLOCK_END (BLOCKVECTOR_BLOCK (bv, GLOBAL_BLOCK))));
    }
  return val;
}

/* Find the PC value for a given source file and line number.
   Returns zero for invalid line number.
   The source file is specified with a struct symtab.  */

CORE_ADDR
find_line_pc (symtab, line)
     struct symtab *symtab;
     int line;
{
  register struct linetable *l;
  register int ind;
  int dummy;

  if (symtab == 0)
    return 0;
  l = LINETABLE (symtab);
  ind = find_line_common(l, line, &dummy);
  return ind ? l->item[ind].pc : 0;
}

/* Find the range of pc values in a line.
   Store the starting pc of the line into *STARTPTR
   and the ending pc (start of next line) into *ENDPTR.
   Returns 1 to indicate success.
   Returns 0 if could not find the specified line.  */

int
find_line_pc_range (symtab, thisline, startptr, endptr)
     struct symtab *symtab;
     int thisline;
     CORE_ADDR *startptr, *endptr;
{
  register struct linetable *l;
  register int ind;
  int exact_match;		/* did we get an exact linenumber match */

  if (symtab == 0)
    return 0;

  l = LINETABLE (symtab);
  ind = find_line_common (l, thisline, &exact_match);
  if (ind)
    {
      *startptr = l->item[ind].pc;
      /* If we have not seen an entry for the specified line,
	 assume that means the specified line has zero bytes.  */
      if (!exact_match || ind == l->nitems-1)
	*endptr = *startptr;
      else
	/* Perhaps the following entry is for the following line.
	   It's worth a try.  */
	if (ind+1 < l->nitems
	 && l->item[ind+1].line == thisline + 1)
	  *endptr = l->item[ind+1].pc;
	else
	  *endptr = find_line_pc (symtab, thisline+1);
      return 1;
    }

  return 0;
}

/* Given a line table and a line number, return the index into the line
   table for the pc of the nearest line whose number is >= the specified one.
   Return 0 if none is found.  The value is never zero is it is an index.

   Set *EXACT_MATCH nonzero if the value returned is an exact match.  */

static int
find_line_common (l, lineno, exact_match)
     register struct linetable *l;
     register int lineno;
     int *exact_match;
{
  register int i;
  register int len;

  /* BEST is the smallest linenumber > LINENO so far seen,
     or 0 if none has been seen so far.
     BEST_INDEX identifies the item for it.  */

  int best_index = 0;
  int best = 0;

  if (lineno <= 0)
    return 0;

  len = l->nitems;
  for (i = 0; i < len; i++)
    {
      register struct linetable_entry *item = &(l->item[i]);

      if (item->line == lineno)
	{
	  *exact_match = 1;
	  return i;
	}

      if (item->line > lineno && (best == 0 || item->line < best))
	{
	  best = item->line;
	  best_index = i;
	}
    }

  /* If we got here, we didn't get an exact match.  */

  *exact_match = 0;
  return best_index;
}

int
find_pc_line_pc_range (pc, startptr, endptr)
     CORE_ADDR pc;
     CORE_ADDR *startptr, *endptr;
{
  struct symtab_and_line sal;
  sal = find_pc_line (pc, 0);
  *startptr = sal.pc;
  *endptr = sal.end;
  return sal.symtab != 0;
}

/* Recursive helper function for decode_line_1.
 * Look for methods named NAME in type T.
 * Return number of matches.
 * Put matches in PHYSNAMES and SYM_ARR (which better be big enough!).
 * These allocations seem to define "big enough":
 * sym_arr = (struct symbol **) alloca(TYPE_NFN_FIELDS_TOTAL (t) * sizeof(struct symbol*));
 * physnames = (char **) alloca (TYPE_NFN_FIELDS_TOTAL (t) * sizeof(char*));
 */

int
find_methods(t, name, physnames, sym_arr)
     struct type *t;
     char *name;
     char **physnames;
     struct symbol **sym_arr;
{
  int i1 = 0;
  int ibase;
  struct symbol *sym_class;
  char *class_name = type_name_no_tag (t);
  /* Ignore this class if it doesn't have a name.
     This prevents core dumps, but is just a workaround
     because we might not find the function in
     certain cases, such as
     struct D {virtual int f();}
     struct C : D {virtual int g();}
     (in this case g++ 1.35.1- does not put out a name
     for D as such, it defines type 19 (for example) in
     the same stab as C, and then does a
     .stabs "D:T19" and a .stabs "D:t19".
     Thus
     "break C::f" should not be looking for field f in
     the class named D, 
     but just for the field f in the baseclasses of C
     (no matter what their names).
     
     However, I don't know how to replace the code below
     that depends on knowing the name of D.  */
  if (class_name
      && (sym_class = lookup_symbol (class_name,
				     (struct block *)NULL,
				     STRUCT_NAMESPACE,
				     (int *)NULL,
				     (struct symtab **)NULL)))
    {
      int method_counter;
      t = SYMBOL_TYPE (sym_class);
      for (method_counter = TYPE_NFN_FIELDS (t) - 1;
	   method_counter >= 0;
	   --method_counter)
	{
	  int field_counter;
	  struct fn_field *f = TYPE_FN_FIELDLIST1 (t, method_counter);

	  char *method_name = TYPE_FN_FIELDLIST_NAME (t, method_counter);
	  if (!strcmp (name, method_name))
	    /* Find all the fields with that name.  */
	    for (field_counter = TYPE_FN_FIELDLIST_LENGTH (t, method_counter) - 1;
		 field_counter >= 0;
		 --field_counter)
	      {
		char *phys_name;
		if (TYPE_FLAGS (TYPE_FN_FIELD_TYPE (f, field_counter)) & TYPE_FLAG_STUB)
		  check_stub_method (t, method_counter, field_counter);
		phys_name = TYPE_FN_FIELD_PHYSNAME (f, field_counter);
		physnames[i1] = (char*) alloca (strlen (phys_name) + 1);
		strcpy (physnames[i1], phys_name);
		sym_arr[i1] = lookup_symbol (phys_name,
					     SYMBOL_BLOCK_VALUE (sym_class),
					     VAR_NAMESPACE,
					     (int *) NULL,
					     (struct symtab **) NULL);
		if (sym_arr[i1]) i1++;
	      }
	}
    }
  /* Only search baseclasses if there is no match yet,
   * since names in derived classes override those in baseclasses.
   */
  if (i1)
    return i1;
  for (ibase = 0; ibase < TYPE_N_BASECLASSES (t); ibase++)
    i1 += find_methods(TYPE_BASECLASS(t, ibase), name,
		       physnames + i1, sym_arr + i1);
  return i1;
}

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

   FUNCTION may be an undebuggable function found in misc_function_vector.

   If the argument FUNFIRSTLINE is nonzero, we want the first line
   of real code inside a function when a function is specified.

   DEFAULT_SYMTAB specifies the file to use if none is specified.
   It defaults to current_source_symtab.
   DEFAULT_LINE specifies the line number to use for relative
   line numbers (that start with signs).  Defaults to current_source_line.

   Note that it is possible to return zero for the symtab
   if no file is validly specified.  Callers must check that.
   Also, the line number returned may be invalid.  */

struct symtabs_and_lines
decode_line_1 (argptr, funfirstline, default_symtab, default_line)
     char **argptr;
     int funfirstline;
     struct symtab *default_symtab;
     int default_line;
{
  struct symtabs_and_lines decode_line_2 ();
  struct symtabs_and_lines values;
  struct symtab_and_line val;
  register char *p, *p1;
  register struct symtab *s;

  register struct symbol *sym;
  /* The symtab that SYM was found in.  */
  struct symtab *sym_symtab;

  register CORE_ADDR pc;
  register int i;
  char *copy;
  struct symbol *sym_class;
  int i1;
  struct symbol **sym_arr;
  struct type *t;
  char **physnames;
  
  /* Defaults have defaults.  */

  if (default_symtab == 0)
    {
      default_symtab = current_source_symtab;
      default_line = current_source_line;
    }

  /* See if arg is *PC */

  if (**argptr == '*')
    {
      (*argptr)++;
      pc = parse_and_eval_address_1 (argptr);
      values.sals = (struct symtab_and_line *)
	xmalloc (sizeof (struct symtab_and_line));
      values.nelts = 1;
      values.sals[0] = find_pc_line (pc, 0);
      values.sals[0].pc = pc;
      return values;
    }

  /* Maybe arg is FILE : LINENUM or FILE : FUNCTION */

  s = 0;

  for (p = *argptr; *p; p++)
    {
      if (p[0] == ':' || p[0] == ' ' || p[0] == '\t')
	break;
    }
  while (p[0] == ' ' || p[0] == '\t') p++;

  if (p[0] == ':')
    {

      /*  C++  */
      if (p[1] ==':')
	{
	  /* Extract the class name.  */
	  p1 = p;
	  while (p != *argptr && p[-1] == ' ') --p;
	  copy = (char *) alloca (p - *argptr + 1);
	  bcopy (*argptr, copy, p - *argptr);
	  copy[p - *argptr] = 0;

	  /* Discard the class name from the arg.  */
	  p = p1 + 2;
	  while (*p == ' ' || *p == '\t') p++;
	  *argptr = p;

	  sym_class = lookup_symbol (copy, 0, STRUCT_NAMESPACE, 0, 
				     (struct symtab **)NULL);
       
	  if (sym_class &&
	      (TYPE_CODE (SYMBOL_TYPE (sym_class)) == TYPE_CODE_STRUCT
	       || TYPE_CODE (SYMBOL_TYPE (sym_class)) == TYPE_CODE_UNION))
	    {
	      /* Arg token is not digits => try it as a function name
		 Find the next token (everything up to end or next whitespace). */
	      p = *argptr;
	      while (*p && *p != ' ' && *p != '\t' && *p != ',' && *p !=':') p++;
	      copy = (char *) alloca (p - *argptr + 1);
	      bcopy (*argptr, copy, p - *argptr);
	      copy[p - *argptr] = '\0';

	      /* no line number may be specified */
	      while (*p == ' ' || *p == '\t') p++;
	      *argptr = p;

	      sym = 0;
	      i1 = 0;		/*  counter for the symbol array */
	      t = SYMBOL_TYPE (sym_class);
	      sym_arr = (struct symbol **) alloca(TYPE_NFN_FIELDS_TOTAL (t) * sizeof(struct symbol*));
	      physnames = (char **) alloca (TYPE_NFN_FIELDS_TOTAL (t) * sizeof(char*));

	      if (destructor_name_p (copy, t))
		{
		  /* destructors are a special case.  */
		  struct fn_field *f = TYPE_FN_FIELDLIST1 (t, 0);
		  int len = TYPE_FN_FIELDLIST_LENGTH (t, 0) - 1;
		  char *phys_name = TYPE_FN_FIELD_PHYSNAME (f, len);
		  physnames[i1] = (char *)alloca (strlen (phys_name) + 1);
		  strcpy (physnames[i1], phys_name);
		  sym_arr[i1] =
		    lookup_symbol (phys_name, SYMBOL_BLOCK_VALUE (sym_class),
				   VAR_NAMESPACE, 0, (struct symtab **)NULL);
		  if (sym_arr[i1]) i1++;
		}
	      else
		i1 = find_methods (t, copy, physnames, sym_arr);
	      if (i1 == 1)
		{
		  /* There is exactly one field with that name.  */
		  sym = sym_arr[0];

		  if (sym && SYMBOL_CLASS (sym) == LOC_BLOCK)
		    {
		      /* Arg is the name of a function */
		      pc = BLOCK_START (SYMBOL_BLOCK_VALUE (sym)) + FUNCTION_START_OFFSET;
		      if (funfirstline)
			SKIP_PROLOGUE (pc);
		      values.sals = (struct symtab_and_line *)xmalloc (sizeof (struct symtab_and_line));
		      values.nelts = 1;
		      values.sals[0] = find_pc_line (pc, 0);
		      values.sals[0].pc = (values.sals[0].end && values.sals[0].pc != pc) ? values.sals[0].end : pc;
		    }
		  else
		    {
		      values.nelts = 0;
		    }
		  return values;
		}
	      if (i1 > 0)
		{
		  /* There is more than one field with that name
		     (overloaded).  Ask the user which one to use.  */
		  return decode_line_2 (sym_arr, i1, funfirstline);
		}
	      else
		error ("that class does not have any method named %s",copy);
	    }
	  else
	    /* The quotes are important if copy is empty.  */
	    error("No class, struct, or union named \"%s\"", copy );
	}
      /*  end of C++  */


      /* Extract the file name.  */
      p1 = p;
      while (p != *argptr && p[-1] == ' ') --p;
      copy = (char *) alloca (p - *argptr + 1);
      bcopy (*argptr, copy, p - *argptr);
      copy[p - *argptr] = 0;

      /* Find that file's data.  */
      s = lookup_symtab (copy);
      if (s == 0)
	{
	  if (symtab_list == 0 && partial_symtab_list == 0)
	    error (no_symtab_msg);
	  error ("No source file named %s.", copy);
	}

      /* Discard the file name from the arg.  */
      p = p1 + 1;
      while (*p == ' ' || *p == '\t') p++;
      *argptr = p;
    }

  /* S is specified file's symtab, or 0 if no file specified.
     arg no longer contains the file name.  */

  /* Check whether arg is all digits (and sign) */

  p = *argptr;
  if (*p == '-' || *p == '+') p++;
  while (*p >= '0' && *p <= '9')
    p++;

  if (p != *argptr && (*p == 0 || *p == ' ' || *p == '\t' || *p == ','))
    {
      /* We found a token consisting of all digits -- at least one digit.  */
      enum sign {none, plus, minus} sign = none;

      /* This is where we need to make sure that we have good defaults.
	 We must guarantee that this section of code is never executed
	 when we are called with just a function name, since
	 select_source_symtab calls us with such an argument  */

      if (s == 0 && default_symtab == 0)
	{
	  if (symtab_list == 0 && partial_symtab_list == 0)
	    error (no_symtab_msg);
	  select_source_symtab (0);
	  default_symtab = current_source_symtab;
	  default_line = current_source_line;
	}

      if (**argptr == '+')
	sign = plus, (*argptr)++;
      else if (**argptr == '-')
	sign = minus, (*argptr)++;
      val.line = atoi (*argptr);
      switch (sign)
	{
	case plus:
	  if (p == *argptr)
	    val.line = 5;
	  if (s == 0)
	    val.line = default_line + val.line;
	  break;
	case minus:
	  if (p == *argptr)
	    val.line = 15;
	  if (s == 0)
	    val.line = default_line - val.line;
	  else
	    val.line = 1;
	  break;
	case none:
	  break;	/* No need to adjust val.line.  */
	}

      while (*p == ' ' || *p == '\t') p++;
      *argptr = p;
      if (s == 0)
	s = default_symtab;
      val.symtab = s;
      val.pc = 0;
      values.sals = (struct symtab_and_line *)xmalloc (sizeof (struct symtab_and_line));
      values.sals[0] = val;
      values.nelts = 1;
      return values;
    }

  /* Arg token is not digits => try it as a variable name
     Find the next token (everything up to end or next whitespace).  */
  p = *argptr;
  while (*p && *p != ' ' && *p != '\t' && *p != ',') p++;
  copy = (char *) alloca (p - *argptr + 1);
  bcopy (*argptr, copy, p - *argptr);
  copy[p - *argptr] = 0;
  while (*p == ' ' || *p == '\t') p++;
  *argptr = p;

  /* Look up that token as a variable.
     If file specified, use that file's per-file block to start with.  */

  sym = lookup_symbol (copy,
		       (s ? BLOCKVECTOR_BLOCK (BLOCKVECTOR (s), STATIC_BLOCK)
			: get_selected_block ()),
		       VAR_NAMESPACE, 0, &sym_symtab);

  if (sym != NULL)
    {
      if (SYMBOL_CLASS (sym) == LOC_BLOCK)
	{
	  /* Arg is the name of a function */
	  pc = BLOCK_START (SYMBOL_BLOCK_VALUE (sym)) + FUNCTION_START_OFFSET;
	  if (funfirstline)
	    SKIP_PROLOGUE (pc);
	  val = find_pc_line (pc, 0);
#ifdef PROLOGUE_FIRSTLINE_OVERLAP
	  /* Convex: no need to suppress code on first line, if any */
	  val.pc = pc;
#else
	  val.pc = (val.end && val.pc != pc) ? val.end : pc;
#endif
	  values.sals = (struct symtab_and_line *)xmalloc (sizeof (struct symtab_and_line));
	  values.sals[0] = val;
	  values.nelts = 1;
	  
	  /* I think this is always the same as the line that
	     we calculate above, but the general principle is
	     "trust the symbols more than stuff like
	     SKIP_PROLOGUE".  */
	  if (SYMBOL_LINE (sym) != 0)
	    values.sals[0].line = SYMBOL_LINE (sym);
	  
	  return values;
	}
      else if (SYMBOL_LINE (sym) != 0)
	{
	  /* We know its line number.  */
	  values.sals = (struct symtab_and_line *)
	    xmalloc (sizeof (struct symtab_and_line));
	  values.nelts = 1;
	  bzero (&values.sals[0], sizeof (values.sals[0]));
	  values.sals[0].symtab = sym_symtab;
	  values.sals[0].line = SYMBOL_LINE (sym);
	  return values;
	}
      else
	/* This can happen if it is compiled with a compiler which doesn't
	   put out line numbers for variables.  */
	error ("Line number not known for symbol \"%s\"", copy);
    }

  if (symtab_list == 0 && partial_symtab_list == 0)
    error (no_symtab_msg);

  if ((i = lookup_misc_func (copy)) >= 0)
    {
      val.symtab = 0;
      val.line = 0;
      val.pc = misc_function_vector[i].address + FUNCTION_START_OFFSET;
      if (funfirstline)
	SKIP_PROLOGUE (val.pc);
      values.sals = (struct symtab_and_line *)xmalloc (sizeof (struct symtab_and_line));
      values.sals[0] = val;
      values.nelts = 1;
      return values;
    }

  error ("Function %s not defined.", copy);
  return values;	/* for lint */
}

struct symtabs_and_lines
decode_line_spec (string, funfirstline)
     char *string;
     int funfirstline;
{
  struct symtabs_and_lines sals;
  if (string == 0)
    error ("Empty line specification.");
  sals = decode_line_1 (&string, funfirstline,
			current_source_symtab, current_source_line);
  if (*string)
    error ("Junk at end of line specification: %s", string);
  return sals;
}

/* Given a list of NELTS symbols in sym_arr (with corresponding
   mangled names in physnames), return a list of lines to operate on
   (ask user if necessary).  */
struct symtabs_and_lines
decode_line_2 (sym_arr, nelts, funfirstline)
     struct symbol *sym_arr[];
     int nelts;
     int funfirstline;
{
  char *getenv();
  struct symtabs_and_lines values, return_values;
  register CORE_ADDR pc;
  char *args, *arg1, *command_line_input ();
  int i;
  char *prompt;

  values.sals = (struct symtab_and_line *) alloca (nelts * sizeof(struct symtab_and_line));
  return_values.sals = (struct symtab_and_line *) xmalloc (nelts * sizeof(struct symtab_and_line));

  i = 0;
  printf("[0] cancel\n[1] all\n");
  while (i < nelts)
    {
      if (sym_arr[i] && SYMBOL_CLASS (sym_arr[i]) == LOC_BLOCK)
	{
	  /* Arg is the name of a function */
	  pc = BLOCK_START (SYMBOL_BLOCK_VALUE (sym_arr[i])) 
	       + FUNCTION_START_OFFSET;
	  if (funfirstline)
	    SKIP_PROLOGUE (pc);
	  values.sals[i] = find_pc_line (pc, 0);
	  values.sals[i].pc = (values.sals[i].end && values.sals[i].pc != pc) ?
			       values.sals[i].end                      :  pc;
	  printf("[%d] file:%s; line number:%d\n",
		 (i+2), values.sals[i].symtab->filename, values.sals[i].line);
	}
      else printf ("?HERE\n");
      i++;
    }
  
  if ((prompt = getenv ("PS2")) == NULL)
    {
      prompt = ">";
    }
  printf("%s ",prompt);
  fflush(stdout);

  args = command_line_input (0, 0);
  
  if (args == 0)
    error_no_arg ("one or more choice numbers");

  i = 0;
  while (*args)
    {
      int num;

      arg1 = args;
      while (*arg1 >= '0' && *arg1 <= '9') arg1++;
      if (*arg1 && *arg1 != ' ' && *arg1 != '\t')
	error ("Arguments must be choice numbers.");

      num = atoi (args);

      if (num == 0)
	error ("cancelled");
      else if (num == 1)
	{
	  bcopy (values.sals, return_values.sals, (nelts * sizeof(struct symtab_and_line)));
	  return_values.nelts = nelts;
	  return return_values;
	}

      if (num > nelts + 2)
	{
	  printf ("No choice number %d.\n", num);
	}
      else
	{
	  num -= 2;
	  if (values.sals[num].pc)
	    {
	      return_values.sals[i++] = values.sals[num];
	      values.sals[num].pc = 0;
	    }
	  else
	    {
	      printf ("duplicate request for %d ignored.\n", num);
	    }
	}

      args = arg1;
      while (*args == ' ' || *args == '\t') args++;
    }
  return_values.nelts = i;
  return return_values;
}

/* Return the index of misc function named NAME.  */

int
lookup_misc_func (name)
     register char *name;
{
  register int i;

  for (i = 0; i < misc_function_count; i++)
    if (!strcmp (misc_function_vector[i].name, name))
      return i;
  return -1;		/* not found */
}

/* Slave routine for sources_info.  Force line breaks at ,'s.
   NAME is the name to print and *FIRST is nonzero if this is the first
   name printed.  Set *FIRST to zero.  */
static void
output_source_filename (name, first)
     char *name;
     int *first;
{
  static int column;
  /* Table of files printed so far.  Since a single source file can
     result in several partial symbol tables, we need to avoid printing
     it more than once.  Note: if some of the psymtabs are read in and
     some are not, it gets printed both under "Source files for which
     symbols have been read" and "Source files for which symbols will
     be read in on demand".  I consider this a reasonable way to deal
     with the situation.  I'm not sure whether this can also happen for
     symtabs; it doesn't hurt to check.  */
  static char **tab = NULL;
  /* Allocated size of tab in elements.
     Start with one 256-byte block (when using GNU malloc.c).
     24 is the malloc overhead when range checking is in effect.  */
  static int tab_alloc_size = (256 - 24) / sizeof (char *);
  /* Current size of tab in elements.  */
  static int tab_cur_size;

  char **p;

  if (*first)
    {
      if (tab == NULL)
	tab = (char **) xmalloc (tab_alloc_size * sizeof (*tab));
      tab_cur_size = 0;
    }

  /* Is NAME in tab?  */
  for (p = tab; p < tab + tab_cur_size; p++)
    if (strcmp (*p, name) == 0)
      /* Yes; don't print it again.  */
      return;
  /* No; add it to tab.  */
  if (tab_cur_size == tab_alloc_size)
    {
      tab_alloc_size *= 2;
      tab = (char **) xrealloc (tab, tab_alloc_size * sizeof (*tab));
    }
  tab[tab_cur_size++] = name;

  if (*first)
    {
      column = 0;
      *first = 0;
    }
  else
    {
      printf_filtered (",");
      column++;
    }

  if (column != 0 && column + strlen (name) >= 70)
    {
      printf_filtered ("\n");
      column = 0;
    }
  else if (column != 0)
    {
      printf_filtered (" ");
      column++;
    }
  fputs_filtered (name, stdout);
  column += strlen (name);
}  

static void
sources_info ()
{
  register struct symtab *s;
  register struct partial_symtab *ps;
  int first;
  
  if (symtab_list == 0 && partial_symtab_list == 0)
    {
      printf (no_symtab_msg);
      return;
    }
  
  printf_filtered ("Source files for which symbols have been read in:\n\n");

  first = 1;
  for (s = symtab_list; s; s = s->next)
    output_source_filename (s->filename, &first);
  printf_filtered ("\n\n");
  
  printf_filtered ("Source files for which symbols will be read in on demand:\n\n");

  first = 1;
  for (ps = partial_symtab_list; ps; ps = ps->next)
    if (!ps->readin)
      output_source_filename (ps->filename, &first);
  printf_filtered ("\n");
}

/* List all symbols (if REGEXP is 0) or all symbols matching REGEXP.
   If CLASS is zero, list all symbols except functions and type names.
   If CLASS is 1, list only functions.
   If CLASS is 2, list only type names.

   BPT is non-zero if we should set a breakpoint at the functions
   we find.  */

static void
list_symbols (regexp, class, bpt)
     char *regexp;
     int class;
     int bpt;
{
  register struct symtab *s;
  register struct partial_symtab *ps;
  register struct blockvector *bv;
  struct blockvector *prev_bv = 0;
  register struct block *b;
  register int i, j;
  register struct symbol *sym;
  struct partial_symbol *psym;
  char *val;
  static char *classnames[]
    = {"variable", "function", "type", "method"};
  int found_in_file = 0;

  if (regexp)
    if (0 != (val = (char *) re_comp (regexp)))
      error ("Invalid regexp (%s): %s", val, regexp);

  /* Search through the partial_symtab_list *first* for all symbols
     matching the regexp.  That way we don't have to reproduce all of
     the machinery below. */
  for (ps = partial_symtab_list; ps; ps = ps->next)
    {
      struct partial_symbol *bound, *gbound, *sbound;
      int keep_going = 1;

      if (ps->readin) continue;
      
      gbound = global_psymbols.list + ps->globals_offset + ps->n_global_syms;
      sbound = static_psymbols.list + ps->statics_offset + ps->n_static_syms;
      bound = gbound;

      /* Go through all of the symbols stored in a partial
	 symtab in one loop. */
      psym = global_psymbols.list + ps->globals_offset;
      while (keep_going)
	{
	  if (psym >= bound)
	    {
	      if (bound == gbound && ps->n_static_syms != 0)
		{
		  psym = static_psymbols.list + ps->statics_offset;
		  bound = sbound;
		}
	      else
		keep_going = 0;
	      continue;
	    }
	  else
	    {
	      QUIT;

	      /* If it would match (logic taken from loop below)
		 load the file and go on to the next one */
	      if ((regexp == 0 || re_exec (SYMBOL_NAME (psym)))
		  && ((class == 0 && SYMBOL_CLASS (psym) != LOC_TYPEDEF
		       && SYMBOL_CLASS (psym) != LOC_BLOCK)
		      || (class == 1 && SYMBOL_CLASS (psym) == LOC_BLOCK)
		      || (class == 2 && SYMBOL_CLASS (psym) == LOC_TYPEDEF)
		      || (class == 3 && SYMBOL_CLASS (psym) == LOC_BLOCK)))
		{
		  (void) PSYMTAB_TO_SYMTAB(ps);
		  keep_going = 0;
		}
	    }
	  psym++;
	}
    }

  /* Here, *if* the class is correct (function only, right now), we
     search through the misc function vector for symbols that
     match, and call find_pc_symtab on them to force their symbols to
     be read.  The symbol will then be found during the scan of symtabs
     below.  */

  if (class == 1)
    {
      for (i = 0; i < misc_function_count; i++)
	if (regexp == 0 || re_exec (misc_function_vector[i].name))
	  {
	    (void) find_pc_symtab (misc_function_vector[i].address);
	  }
    }

  /* Printout here so as to get after the "Reading in symbols"
     messages which will be generated above.  */
  if (!bpt)
    printf_filtered (regexp
	  ? "All %ss matching regular expression \"%s\":\n"
	  : "All defined %ss:\n",
	  classnames[class],
	  regexp);

  for (s = symtab_list; s; s = s->next)
    {
      found_in_file = 0;
      bv = BLOCKVECTOR (s);
      /* Often many files share a blockvector.
	 Scan each blockvector only once so that
	 we don't get every symbol many times.
	 It happens that the first symtab in the list
	 for any given blockvector is the main file.  */
      if (bv != prev_bv)
	for (i = GLOBAL_BLOCK; i <= STATIC_BLOCK; i++)
	  {
	    b = BLOCKVECTOR_BLOCK (bv, i);
	    /* Skip the sort if this block is always sorted.  */
	    if (!BLOCK_SHOULD_SORT (b))
	      sort_block_syms (b);
	    for (j = 0; j < BLOCK_NSYMS (b); j++)
	      {
		QUIT;
		sym = BLOCK_SYM (b, j);
		if ((regexp == 0 || re_exec (SYMBOL_NAME (sym)))
		    && ((class == 0 && SYMBOL_CLASS (sym) != LOC_TYPEDEF
			 && SYMBOL_CLASS (sym) != LOC_BLOCK)
			|| (class == 1 && SYMBOL_CLASS (sym) == LOC_BLOCK)
			|| (class == 2 && SYMBOL_CLASS (sym) == LOC_TYPEDEF)
			|| (class == 3 && SYMBOL_CLASS (sym) == LOC_BLOCK)))
		  {
		    if (bpt)
		      {
			/* Set a breakpoint here, if it's a function */
			if (class == 1)
			  break_command (SYMBOL_NAME(sym), 0);
		      }
		    else if (!found_in_file)
		      {
			fputs_filtered ("\nFile ", stdout);
			fputs_filtered (s->filename, stdout);
			fputs_filtered (":\n", stdout);
		      }
		    found_in_file = 1;

		    if (class != 2 && i == STATIC_BLOCK)
		      printf_filtered ("static ");
		    if (class == 2
			&& SYMBOL_NAMESPACE (sym) != STRUCT_NAMESPACE)
		      printf_filtered ("typedef ");

		    if (class < 3)
		      {
			type_print (SYMBOL_TYPE (sym),
				    (SYMBOL_CLASS (sym) == LOC_TYPEDEF
				     ? "" : SYMBOL_NAME (sym)),
				    stdout, 0);

			if (class == 2
			    && SYMBOL_NAMESPACE (sym) != STRUCT_NAMESPACE
			    && (TYPE_NAME ((SYMBOL_TYPE (sym))) == 0
				|| 0 != strcmp (TYPE_NAME ((SYMBOL_TYPE (sym))),
						SYMBOL_NAME (sym))))
			  {
			    fputs_filtered (" ", stdout);
			    fprint_symbol (stdout, SYMBOL_NAME (sym));
			  }

			printf_filtered (";\n");
		      }
		    else
		      {
# if 0
			char buf[1024];
			type_print_base (TYPE_FN_FIELD_TYPE(t, i), stdout, 0, 0); 
			type_print_varspec_prefix (TYPE_FN_FIELD_TYPE(t, i), stdout, 0); 
			sprintf (buf, " %s::", type_name_no_tag (t));
			type_print_method_args (TYPE_FN_FIELD_ARGS (t, i), buf, name, stdout);
# endif
		      }
		  }
	      }
	  }
      prev_bv = bv;
    }
}

static void
variables_info (regexp)
     char *regexp;
{
  list_symbols (regexp, 0, 0);
}

static void
functions_info (regexp)
     char *regexp;
{
  list_symbols (regexp, 1, 0);
}

static void
types_info (regexp)
     char *regexp;
{
  list_symbols (regexp, 2, 0);
}

#if 0
/* Tiemann says: "info methods was never implemented."  */
static void
methods_info (regexp)
     char *regexp;
{
  list_symbols (regexp, 3, 0);
}
#endif /* 0 */

/* Breakpoint all functions matching regular expression. */
static void
rbreak_command (regexp)
     char *regexp;
{
  list_symbols (regexp, 1, 1);
}

/* Initialize the standard C scalar types.  */

static
struct type *
init_type (code, length, uns, name)
     enum type_code code;
     int length, uns;
     char *name;
{
  register struct type *type;

  type = (struct type *) xmalloc (sizeof (struct type));
  bzero (type, sizeof *type);
  TYPE_MAIN_VARIANT (type) = type;
  TYPE_CODE (type) = code;
  TYPE_LENGTH (type) = length;
  TYPE_FLAGS (type) = uns ? TYPE_FLAG_UNSIGNED : 0;
  TYPE_FLAGS (type) |= TYPE_FLAG_PERM;
  TYPE_NFIELDS (type) = 0;
  TYPE_NAME (type) = name;

  /* C++ fancies.  */
  TYPE_NFN_FIELDS (type) = 0;
  TYPE_N_BASECLASSES (type) = 0;
  return type;
}

/* Return Nonzero if block a is lexically nested within block b,
   or if a and b have the same pc range.
   Return zero otherwise. */
int
contained_in (a, b)
     struct block *a, *b;
{
  if (!a || !b)
    return 0;
  return BLOCK_START (a) >= BLOCK_START (b)
      && BLOCK_END (a)   <= BLOCK_END (b);
}


/* Helper routine for make_symbol_completion_list.  */

int return_val_size, return_val_index;
char **return_val;

void
completion_list_add_symbol (symname)
     char *symname;
{
  if (return_val_index + 3 > return_val_size)
    return_val =
      (char **)xrealloc (return_val,
			 (return_val_size *= 2) * sizeof (char *));
  
  return_val[return_val_index] =
    (char *)xmalloc (1 + strlen (symname));
  
  strcpy (return_val[return_val_index], symname);
  
  return_val[++return_val_index] = (char *)NULL;
}

/* Return a NULL terminated array of all symbols (regardless of class) which
   begin by matching TEXT.  If the answer is no symbols, then the return value
   is an array which contains only a NULL pointer.

   Problem: All of the symbols have to be copied because readline
   frees them.  I'm not going to worry about this; hopefully there
   won't be that many.  */

char **
make_symbol_completion_list (text)
  char *text;
{
  register struct symtab *s;
  register struct partial_symtab *ps;
  register struct block *b, *surrounding_static_block = 0;
  extern struct block *get_selected_block ();
  register int i, j;
  struct partial_symbol *psym;

  int text_len = strlen (text);
  return_val_size = 100;
  return_val_index = 0;
  return_val =
    (char **)xmalloc ((1 + return_val_size) *sizeof (char *));
  return_val[0] = (char *)NULL;

  /* Look through the partial symtabs for all symbols which begin
     by matching TEXT.  Add each one that you find to the list.  */

  for (ps = partial_symtab_list; ps; ps = ps->next)
    {
      /* If the psymtab's been read in we'll get it when we search
	 through the blockvector.  */
      if (ps->readin) continue;

      for (psym = global_psymbols.list + ps->globals_offset;
	   psym < (global_psymbols.list + ps->globals_offset
		   + ps->n_global_syms);
	   psym++)
	{
	  QUIT;			/* If interrupted, then quit. */
	  if ((strncmp (SYMBOL_NAME (psym), text, text_len) == 0))
	    completion_list_add_symbol (SYMBOL_NAME (psym));
	}
      
      for (psym = static_psymbols.list + ps->statics_offset;
	   psym < (static_psymbols.list + ps->statics_offset
		   + ps->n_static_syms);
	   psym++)
	{
	  QUIT;
	  if ((strncmp (SYMBOL_NAME (psym), text, text_len) == 0))
	    completion_list_add_symbol (SYMBOL_NAME (psym));
	}
    }

  /* At this point scan through the misc function vector and add each
     symbol you find to the list.  Eventually we want to ignore
     anything that isn't a text symbol (everything else will be
     handled by the psymtab code above).  */

  for (i = 0; i < misc_function_count; i++)
    if (!strncmp (text, misc_function_vector[i].name, text_len))
      completion_list_add_symbol (misc_function_vector[i].name);

  /* Search upwards from currently selected frame (so that we can
     complete on local vars.  */
  for (b = get_selected_block (); b; b = BLOCK_SUPERBLOCK (b))
    {
      if (!BLOCK_SUPERBLOCK (b))
	surrounding_static_block = b; /* For elmin of dups */
      
      /* Also catch fields of types defined in this places which
	 match our text string.  Only complete on types visible
	 from current context.  */
      for (i = 0; i < BLOCK_NSYMS (b); i++)
	{
	  register struct symbol *sym = BLOCK_SYM (b, i);
	  
	  if (!strncmp (SYMBOL_NAME (sym), text, text_len))
	    completion_list_add_symbol (SYMBOL_NAME (sym));

	  if (SYMBOL_CLASS (sym) == LOC_TYPEDEF)
	    {
	      struct type *t = SYMBOL_TYPE (sym);
	      enum type_code c = TYPE_CODE (t);

	      if (c == TYPE_CODE_UNION || c == TYPE_CODE_STRUCT)
		for (j = TYPE_N_BASECLASSES (t); j < TYPE_NFIELDS (t); j++)
		  if (TYPE_FIELD_NAME (t, j) &&
		      !strncmp (TYPE_FIELD_NAME (t, j), text, text_len))
		    completion_list_add_symbol (TYPE_FIELD_NAME (t, j));
	    }
	}
    }

  /* Go through the symtabs and check the externs and statics for
     symbols which match.  */

  for (s = symtab_list; s; s = s->next)
    {
      b = BLOCKVECTOR_BLOCK (BLOCKVECTOR (s), GLOBAL_BLOCK);
      
      for (i = 0; i < BLOCK_NSYMS (b); i++)
	if (!strncmp (SYMBOL_NAME (BLOCK_SYM (b, i)), text, text_len))
	  completion_list_add_symbol (SYMBOL_NAME (BLOCK_SYM (b, i)));
    }

  for (s = symtab_list; s; s = s->next)
    {
      b = BLOCKVECTOR_BLOCK (BLOCKVECTOR (s), STATIC_BLOCK);

      /* Don't do this block twice.  */
      if (b == surrounding_static_block) continue;
      
      for (i = 0; i < BLOCK_NSYMS (b); i++)
	if (!strncmp (SYMBOL_NAME (BLOCK_SYM (b, i)), text, text_len))
	  completion_list_add_symbol (SYMBOL_NAME (BLOCK_SYM (b, i)));
    }

  return (return_val);
}

void
_initialize_symtab ()
{
  add_info ("variables", variables_info,
	    "All global and static variable names, or those matching REGEXP.");
  add_info ("functions", functions_info,
	    "All function names, or those matching REGEXP.");

  /* FIXME:  This command has at least the following problems:
     1.  It prints builtin types (in a very strange and confusing fashion).
     2.  It doesn't print right, e.g. with
         typedef struct foo *FOO
	 type_print prints "FOO" when we want to make it (in this situation)
	 print "struct foo *".
     I also think "ptype" or "whatis" is more likely to be useful (but if
     there is much disagreement "info types" can be fixed).  */
  add_info ("types", types_info,
	    "All types names, or those matching REGEXP.");

#if 0
  add_info ("methods", methods_info,
	    "All method names, or those matching REGEXP::REGEXP.\n\
If the class qualifier is ommited, it is assumed to be the current scope.\n\
If the first REGEXP is ommited, then all methods matching the second REGEXP\n\
are listed.");
#endif
  add_info ("sources", sources_info,
	    "Source files in the program.");

  add_com ("rbreak", no_class, rbreak_command,
	    "Set a breakpoint for all functions matching REGEXP.");

  /* FIXME:  The code below assumes that the sizes of the basic data
     types are the same on the host and target machines!!!  */

  builtin_type_void = init_type (TYPE_CODE_VOID, 1, 0, "void");

  builtin_type_float = init_type (TYPE_CODE_FLT, sizeof (float), 0, "float");
  builtin_type_double = init_type (TYPE_CODE_FLT, sizeof (double), 0, "double");

  builtin_type_char = init_type (TYPE_CODE_INT, sizeof (char), 0, "char");
  builtin_type_short = init_type (TYPE_CODE_INT, sizeof (short), 0, "short");
  builtin_type_long = init_type (TYPE_CODE_INT, sizeof (long), 0, "long");
  builtin_type_int = init_type (TYPE_CODE_INT, sizeof (int), 0, "int");

  builtin_type_unsigned_char = init_type (TYPE_CODE_INT, sizeof (char), 1, "unsigned char");
  builtin_type_unsigned_short = init_type (TYPE_CODE_INT, sizeof (short), 1, "unsigned short");
  builtin_type_unsigned_long = init_type (TYPE_CODE_INT, sizeof (long), 1, "unsigned long");
  builtin_type_unsigned_int = init_type (TYPE_CODE_INT, sizeof (int), 1, "unsigned int");
#ifdef LONG_LONG
  builtin_type_long_long =
    init_type (TYPE_CODE_INT, sizeof (long long), 0, "long long");
  builtin_type_unsigned_long_long = 
    init_type (TYPE_CODE_INT, sizeof (long long), 1, "unsigned long long");
#endif
  builtin_type_error = init_type (TYPE_CODE_ERROR, 0, 0, "<unknown type>");
}

