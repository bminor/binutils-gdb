/* ieee.c -- Write out IEEE-695 debugging information.
   Copyright (C) 1996 Free Software Foundation, Inc.
   Written by Ian Lance Taylor <ian@cygnus.com>.

   This file is part of GNU Binutils.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

/* This file reads and writes IEEE-695 debugging information.  */

#include <stdio.h>
#include <assert.h>

#include "bfd.h"
#include "ieee.h"
#include "bucomm.h"
#include "libiberty.h"
#include "debug.h"
#include "budbg.h"

/* This structure holds an entry on the block stack.  */

struct ieee_block
{
  /* The kind of block.  */
  int kind;
  /* The source file name, for a BB5 block.  */
  const char *filename;
};

/* This structure is the block stack.  */

#define BLOCKSTACK_SIZE (16)

struct ieee_blockstack
{
  /* The stack pointer.  */
  struct ieee_block *bsp;
  /* The stack.  */
  struct ieee_block stack[BLOCKSTACK_SIZE];
};

/* This structure holds information for a variable.  */

struct ieee_var
{
  /* Start of name.  */
  const char *name;
  /* Length of name.  */
  unsigned long namlen;
  /* Type.  */
  debug_type type;
};

/* This structure holds all the variables.  */

struct ieee_vars
{
  /* Number of slots allocated.  */
  unsigned int alloc;
  /* Variables.  */
  struct ieee_var *vars;
};

/* This structure holds information for a type.  We need this because
   we don't want to represent bitfields as real types.  */

struct ieee_type
{
  /* Type.  */
  debug_type type;
  /* Slot if this is type is referenced before it is defined.  */
  debug_type *pslot;
  /* If this is a bitfield, this is the size in bits.  If this is not
     a bitfield, this is zero.  */
  unsigned long bitsize;
  /* If this is a function type ('x' or 'X') this is the return type.  */
  debug_type return_type;
};

/* This structure holds all the type information.  */

struct ieee_types
{
  /* Number of slots allocated.  */
  unsigned int alloc;
  /* Types.  */
  struct ieee_type *types;
  /* Builtin types.  */
#define BUILTIN_TYPE_COUNT (60)
  debug_type builtins[BUILTIN_TYPE_COUNT];
};

/* Basic builtin types, not including the pointers.  */

enum builtin_types
{
  builtin_unknown = 0,
  builtin_void = 1,
  builtin_signed_char = 2,
  builtin_unsigned_char = 3,
  builtin_signed_short_int = 4,
  builtin_unsigned_short_int = 5,
  builtin_signed_long = 6,
  builtin_unsigned_long = 7,
  builtin_signed_long_long = 8,
  builtin_unsigned_long_long = 9,
  builtin_float = 10,
  builtin_double = 11,
  builtin_long_double = 12,
  builtin_long_long_double = 13,
  builtin_quoted_string = 14,
  builtin_instruction_address = 15,
  builtin_int = 16,
  builtin_unsigned = 17,
  builtin_unsigned_int = 18,
  builtin_char = 19,
  builtin_long = 20,
  builtin_short = 21,
  builtin_unsigned_short = 22,
  builtin_short_int = 23,
  builtin_signed_short = 24,
  builtin_bcd_float = 25
};

static void ieee_error
  PARAMS ((bfd *, const bfd_byte *, const bfd_byte *, const char *));
static void ieee_eof PARAMS ((bfd *));
static char *savestring PARAMS ((const char *, unsigned long));
static boolean ieee_read_number
  PARAMS ((bfd *, const bfd_byte *, const bfd_byte **, const bfd_byte *,
	   bfd_vma *));
static boolean ieee_read_optional_number
  PARAMS ((bfd *, const bfd_byte *, const bfd_byte **, const bfd_byte *,
	   bfd_vma *, boolean *));
static boolean ieee_read_id
  PARAMS ((bfd *, const bfd_byte *, const bfd_byte **, const bfd_byte *,
	   const char **, unsigned long *));
static boolean ieee_read_optional_id
  PARAMS ((bfd *, const bfd_byte *, const bfd_byte **, const bfd_byte *,
	   const char **, unsigned long *, boolean *));
static boolean ieee_read_expression
  PARAMS ((bfd *, const bfd_byte *, const bfd_byte **, const bfd_byte *,
	   bfd_vma *));
static debug_type ieee_builtin_type
  PARAMS ((PTR, bfd *, struct ieee_types *, const bfd_byte *,
	   const bfd_byte *, unsigned int));
static boolean ieee_alloc_type
  PARAMS ((PTR, struct ieee_types *, unsigned int, boolean));
static boolean ieee_read_type_index
  PARAMS ((PTR, bfd *, struct ieee_types *, const bfd_byte *,
	   const bfd_byte **, const bfd_byte *, debug_type *));
static int ieee_regno_to_genreg PARAMS ((bfd *, int));
static int ieee_genreg_to_regno PARAMS ((bfd *, int));
static boolean parse_ieee_bb
  PARAMS ((PTR, bfd *, struct ieee_types *, struct ieee_blockstack *,
	   const bfd_byte *, const bfd_byte **, const bfd_byte *));
static boolean parse_ieee_be
  PARAMS ((PTR, bfd *, struct ieee_blockstack *, const bfd_byte *,
	   const bfd_byte **, const bfd_byte *));
static boolean parse_ieee_nn
  PARAMS ((PTR, bfd *, struct ieee_vars *, const bfd_byte *,
	   const bfd_byte **, const bfd_byte *));
static boolean parse_ieee_ty
  PARAMS ((PTR, bfd *, struct ieee_types *, struct ieee_vars *,
	   const bfd_byte *, const bfd_byte **, const bfd_byte *));
static boolean parse_ieee_atn
  PARAMS ((PTR, bfd *, struct ieee_types *, struct ieee_vars *, int,
	   const bfd_byte *, const bfd_byte **, const bfd_byte *));
static boolean ieee_require_asn
  PARAMS ((bfd *, const bfd_byte *, const bfd_byte **, const bfd_byte *,
	   bfd_vma *));

/* Report an error in the IEEE debugging information.  */

static void
ieee_error (abfd, bytes, p, s)
     bfd *abfd;
     const bfd_byte *bytes;
     const bfd_byte *p;
     const char *s;
{
  if (p != NULL)
    fprintf (stderr, "%s: 0x%lx: %s (0x%x)\n", bfd_get_filename (abfd),
	     (unsigned long) (p - bytes), s, *p);
  else
    fprintf (stderr, "%s: %s\n", bfd_get_filename (abfd), s);
}

/* Report an unexpected EOF in the IEEE debugging information.  */

static void
ieee_eof (abfd)
     bfd *abfd;
{
  ieee_error (abfd, (const bfd_byte *) NULL, (const bfd_byte *) NULL,
	      "unexpected end of debugging information");
}

/* Save a string in memory.  */

static char *
savestring (start, len)
     const char *start;
     unsigned long len;
{
  char *ret;

  ret = (char *) xmalloc (len + 1);
  memcpy (ret, start, len);
  ret[len] = '\0';
  return ret;
}

/* Read a number which must be present in an IEEE file.  */

static boolean
ieee_read_number (abfd, bytes, pp, pend, pv)
     bfd *abfd;
     const bfd_byte *bytes;
     const bfd_byte **pp;
     const bfd_byte *pend;
     bfd_vma *pv;
{
  return ieee_read_optional_number (abfd, bytes, pp, pend, pv,
				    (boolean *) NULL);
}

/* Read a number in an IEEE file.  If ppresent is not NULL, the number
   need not be there. */

static boolean
ieee_read_optional_number (abfd, bytes, pp, pend, pv, ppresent)
     bfd *abfd;
     const bfd_byte *bytes;
     const bfd_byte **pp;
     const bfd_byte *pend;
     bfd_vma *pv;
     boolean *ppresent;
{
  ieee_record_enum_type b;

  if (*pp >= pend)
    {
      if (ppresent != NULL)
	{
	  *ppresent = false;
	  return true;
	}
      ieee_eof (abfd);
      return false;
    }

  b = (ieee_record_enum_type) **pp;
  ++*pp;

  if (b <= ieee_number_end_enum)
    {
      *pv = (bfd_vma) b;
      if (ppresent != NULL)
	*ppresent = true;
      return true;
    }

  if (b >= ieee_number_repeat_start_enum && b <= ieee_number_repeat_end_enum)
    {
      unsigned int i;

      i = (int) b - (int) ieee_number_repeat_start_enum;
      if (*pp + i - 1 >= pend)
	{
	  ieee_eof (abfd);
	  return false;
	}

      *pv = 0;
      for (; i > 0; i--)
	{
	  *pv <<= 8;
	  *pv += **pp;
	  ++*pp;
	}

      if (ppresent != NULL)
	*ppresent = true;

      return true;
    }

  if (ppresent != NULL)
    {
      --*pp;
      *ppresent = false;
      return true;
    }

  ieee_error (abfd, bytes, *pp - 1, "invalid number");
  return false;  
}

/* Read a required string from an IEEE file.  */

static boolean
ieee_read_id (abfd, bytes, pp, pend, pname, pnamlen)
     bfd *abfd;
     const bfd_byte *bytes;
     const bfd_byte **pp;
     const bfd_byte *pend;
     const char **pname;
     unsigned long *pnamlen;
{
  return ieee_read_optional_id (abfd, bytes, pp, pend, pname, pnamlen,
				(boolean *) NULL);
}

/* Read a string from an IEEE file.  If ppresent is not NULL, the
   string is optional.  */

static boolean
ieee_read_optional_id (abfd, bytes, pp, pend, pname, pnamlen, ppresent)
     bfd *abfd;
     const bfd_byte *bytes;
     const bfd_byte **pp;
     const bfd_byte *pend;
     const char **pname;
     unsigned long *pnamlen;
     boolean *ppresent;
{
  bfd_byte b;
  unsigned long len;

  if (*pp >= pend)
    {
      ieee_eof (abfd);
      return false;
    }

  b = **pp;
  ++*pp;

  if (b <= 0x7f)
    len = b;
  else if ((ieee_record_enum_type) b == ieee_extension_length_1_enum)
    {
      len = **pp;
      ++*pp;
    }
  else if ((ieee_record_enum_type) b == ieee_extension_length_2_enum)
    {
      len = (**pp << 8) + (*pp)[1];
      *pp += 2;
    }
  else
    {
      if (ppresent != NULL)
	{
	  --*pp;
	  *ppresent = false;
	  return true;
	}
      ieee_error (abfd, bytes, *pp - 1, "invalid string length");
      return false;
    }

  if ((unsigned long) (pend - *pp) < len)
    {
      ieee_eof (abfd);
      return false;
    }

  *pname = (const char *) *pp;
  *pnamlen = len;
  *pp += len;

  if (ppresent != NULL)
    *ppresent = true;

  return true;
}

/* Read an expression from an IEEE file.  Since this code is only used
   to parse debugging information, I haven't bothered to write a full
   blown IEEE expression parser.  I've only thrown in the things I've
   seen in debugging information.  This can be easily extended if
   necessary.  */

static boolean
ieee_read_expression (abfd, bytes, pp, pend, pv)
     bfd *abfd;
     const bfd_byte *bytes;
     const bfd_byte **pp;
     const bfd_byte *pend;
     bfd_vma *pv;
{
  const bfd_byte *expr_start;
#define EXPR_STACK_SIZE (10)
  bfd_vma expr_stack[EXPR_STACK_SIZE];
  bfd_vma *esp;

  expr_start = *pp;

  esp = expr_stack;

  while (1)
    {
      const bfd_byte *start;
      bfd_vma val;
      boolean present;
      ieee_record_enum_type c;

      start = *pp;

      if (! ieee_read_optional_number (abfd, bytes, pp, pend, &val, &present))
	return false;

      if (present)
	{
	  if (esp - expr_stack >= EXPR_STACK_SIZE)
	    {
	      ieee_error (abfd, bytes, start, "expression stack overflow");
	      return false;
	    }
	  *esp++ = val;
	  continue;
	}

      c = (ieee_record_enum_type) **pp;

      if (c >= ieee_module_beginning_enum)
	break;

      ++*pp;

      if (c == ieee_comma)
	break;

      switch (c)
	{
	default:
	  ieee_error (abfd, bytes, start,
		      "unsupported IEEE expression operator");
	  break;

	case ieee_variable_R_enum:
	  {
	    bfd_vma indx;
	    asection *s;

	    if (! ieee_read_number (abfd, bytes, pp, pend, &indx))
	      return false;
	    for (s = abfd->sections; s != NULL; s = s->next)
	      if ((bfd_vma) s->target_index == indx)
		break;
	    if (s == NULL)
	      {
		ieee_error (abfd, bytes, start, "unknown section");
		return false;
	      }
	    
	    if (esp - expr_stack >= EXPR_STACK_SIZE)
	      {
		ieee_error (abfd, bytes, start, "expression stack overflow");
		return false;
	      }

	    *esp++ = bfd_get_section_vma (abfd, s);
	  }
	  break;

	case ieee_function_plus_enum:
	case ieee_function_minus_enum:
	  {
	    bfd_vma v1, v2;

	    if (esp - expr_stack < 2)
	      {
		ieee_error (abfd, bytes, start, "expression stack underflow");
		return false;
	      }

	    v1 = *--esp;
	    v2 = *--esp;
	    *esp++ = v1 + v2;
	  }
	  break;
	}
    }

  if (esp - 1 != expr_stack)
    {
      ieee_error (abfd, bytes, expr_start, "expression stack mismatch");
      return false;
    }

  *pv = *--esp;

  return true;
}

/* Return an IEEE builtin type.  */

static debug_type
ieee_builtin_type (dhandle, abfd, types, bytes, p, indx)
     PTR dhandle;
     bfd *abfd;
     struct ieee_types *types;
     const bfd_byte *bytes;
     const bfd_byte *p;
     unsigned int indx;
{
  debug_type type;
  const char *name;

  if (indx < BUILTIN_TYPE_COUNT
      && types->builtins[indx] != DEBUG_TYPE_NULL)
    return types->builtins[indx];

  if (indx >= 32 && indx < 64)
    {
      type = debug_make_pointer_type (dhandle,
				      ieee_builtin_type (dhandle, abfd,
							 types, bytes, p,
							 indx - 32));
      assert (indx < BUILTIN_TYPE_COUNT);
      types->builtins[indx] = type;
      return type;
    }

  switch ((enum builtin_types) indx)
    {
    default:
      ieee_error (abfd, bytes, p, "unknown builtin type");
      return NULL;

    case builtin_unknown:
      type = debug_make_void_type (dhandle);
      name = NULL;
      break;

    case builtin_void:
      type = debug_make_void_type (dhandle);
      name = "void";
      break;

    case builtin_signed_char:
      type = debug_make_int_type (dhandle, 1, false);
      name = "signed char";
      break;

    case builtin_unsigned_char:
      type = debug_make_int_type (dhandle, 1, true);
      name = "unsigned char";
      break;

    case builtin_signed_short_int:
      type = debug_make_int_type (dhandle, 2, false);
      name = "signed short int";
      break;

    case builtin_unsigned_short_int:
      type = debug_make_int_type (dhandle, 2, true);
      name = "unsigned short int";
      break;

    case builtin_signed_long:
      type = debug_make_int_type (dhandle, 4, false);
      name = "signed long";
      break;

    case builtin_unsigned_long:
      type = debug_make_int_type (dhandle, 4, true);
      name = "unsigned long";
      break;

    case builtin_signed_long_long:
      type = debug_make_int_type (dhandle, 8, false);
      name = "signed long long";
      break;

    case builtin_unsigned_long_long:
      type = debug_make_int_type (dhandle, 8, true);
      name = "unsigned long long";
      break;

    case builtin_float:
      type = debug_make_float_type (dhandle, 4);
      name = "float";
      break;

    case builtin_double:
      type = debug_make_float_type (dhandle, 8);
      name = "double";
      break;

    case builtin_long_double:
      /* FIXME: The size for this type should depend upon the
         processor.  */
      type = debug_make_float_type (dhandle, 12);
      name = "long double";
      break;

    case builtin_long_long_double:
      type = debug_make_float_type (dhandle, 16);
      name = "long long double";
      break;

    case builtin_quoted_string:
      type = debug_make_array_type (dhandle,
				    ieee_builtin_type (dhandle, abfd, types,
						       bytes, p,
						       ((unsigned int)
							builtin_char)),
				    ieee_builtin_type (dhandle, abfd, types,
						       bytes, p,
						       ((unsigned int)
							builtin_int)),
				    0, -1, true);
      name = "QUOTED STRING";
      break;

    case builtin_instruction_address:
      /* FIXME: This should be a code address.  */
      type = debug_make_int_type (dhandle, 4, true);
      name = "instruction address";
      break;

    case builtin_int:
      /* FIXME: The size for this type should depend upon the
         processor.  */
      type = debug_make_int_type (dhandle, 4, false);
      name = "int";
      break;

    case builtin_unsigned:
      /* FIXME: The size for this type should depend upon the
         processor.  */
      type = debug_make_int_type (dhandle, 4, true);
      name = "unsigned";
      break;

    case builtin_unsigned_int:
      /* FIXME: The size for this type should depend upon the
         processor.  */
      type = debug_make_int_type (dhandle, 4, true);
      name = "unsigned int";
      break;

    case builtin_char:
      type = debug_make_int_type (dhandle, 1, false);
      name = "char";
      break;

    case builtin_long:
      type = debug_make_int_type (dhandle, 4, false);
      name = "long";
      break;

    case builtin_short:
      type = debug_make_int_type (dhandle, 2, false);
      name = "short";
      break;

    case builtin_unsigned_short:
      type = debug_make_int_type (dhandle, 2, true);
      name = "unsigned short";
      break;

    case builtin_short_int:
      type = debug_make_int_type (dhandle, 2, false);
      name = "short int";
      break;

    case builtin_signed_short:
      type = debug_make_int_type (dhandle, 2, false);
      name = "signed short";
      break;

    case builtin_bcd_float:
      ieee_error (abfd, bytes, p, "BCD float type not supported");
      return false;
    }

  if (name != NULL)
    type = debug_name_type (dhandle, name, type);

  assert (indx < BUILTIN_TYPE_COUNT);

  types->builtins[indx] = type;

  return type;
}

/* Allocate more space in the type table.  If ref is true, this is a
   reference to the type; if it is not already defined, we should set
   up an indirect type.  */

static boolean
ieee_alloc_type (dhandle, types, indx, ref)
     PTR dhandle;
     struct ieee_types *types;
     unsigned int indx;
     boolean ref;
{
  unsigned int nalloc;
  register struct ieee_type *t;
  struct ieee_type *tend;

  if (indx >= types->alloc)
    {
      nalloc = types->alloc;
      if (nalloc == 0)
	nalloc = 4;
      while (indx >= nalloc)
	nalloc *= 2;

      types->types = ((struct ieee_type *)
		      xrealloc (types->types, nalloc * sizeof *types->types));

      memset (types->types + types->alloc, 0,
	      (nalloc - types->alloc) * sizeof *types->types);

      tend = types->types + nalloc;
      for (t = types->types + types->alloc; t < tend; t++)
	{
	  t->type = DEBUG_TYPE_NULL;
	  t->return_type = DEBUG_TYPE_NULL;
	}

      types->alloc = nalloc;
    }

  if (ref)
    {
      t = types->types + indx;
      if (t->type == NULL)
	{
	  t->pslot = (debug_type *) xmalloc (sizeof *t->pslot);
	  *t->pslot = DEBUG_TYPE_NULL;
	  t->type = debug_make_indirect_type (dhandle, t->pslot,
					      (const char *) NULL);
	  if (t->type == NULL)
	    return false;
	}
    }

  return true;
}

/* Read a type index and return the corresponding type.  */

static boolean
ieee_read_type_index (dhandle, abfd, types, bytes, pp, pend, ptype)
     PTR dhandle;
     bfd *abfd;
     struct ieee_types *types;
     const bfd_byte *bytes;
     const bfd_byte **pp;
     const bfd_byte *pend;
     debug_type *ptype;
{
  const bfd_byte *start;
  bfd_vma indx;

  start = *pp;

  if (! ieee_read_number (abfd, bytes, pp, pend, &indx))
    return false;

  if (indx < 256)
    {
      *ptype = ieee_builtin_type (dhandle, abfd, types, bytes, start, indx);
      if (*ptype == NULL)
	return false;
      return true;
    }

  indx -= 256;
  if (! ieee_alloc_type (dhandle, types, indx, true))
    return false;

  *ptype = types->types[indx].type;

  return true;
}

/* Parse IEEE debugging information for a file.  This is passed the
   bytes which compose the Debug Information Part of an IEEE file.  */

boolean
parse_ieee (dhandle, abfd, bytes, len)
     PTR dhandle;
     bfd *abfd;
     const bfd_byte *bytes;
     bfd_size_type len;
{
  struct ieee_blockstack blockstack;
  struct ieee_vars vars;
  struct ieee_types types;
  unsigned int i;
  const bfd_byte *p, *pend;

  blockstack.bsp = blockstack.stack;
  vars.alloc = 0;
  vars.vars = NULL;
  types.alloc = 0;
  types.types = NULL;
  for (i = 0; i < BUILTIN_TYPE_COUNT; i++)
    types.builtins[i] = DEBUG_TYPE_NULL;

  p = bytes;
  pend = bytes + len;
  while (p < pend)
    {
      const bfd_byte *record_start;
      ieee_record_enum_type c;

      record_start = p;

      c = (ieee_record_enum_type) *p++;

      if (c == ieee_at_record_enum)
	c = (ieee_record_enum_type) (((unsigned int) c << 8) | *p++);

      if (c <= ieee_number_repeat_end_enum)
	{
	  ieee_error (abfd, bytes, record_start, "unexpected number");
	  return false;
	}

      switch (c)
	{
	default:
	  ieee_error (abfd, bytes, record_start, "unexpected record type");
	  return false;

	case ieee_bb_record_enum:
	  if (! parse_ieee_bb (dhandle, abfd, &types, &blockstack, bytes,
			       &p, pend))
	    return false;
	  break;

	case ieee_be_record_enum:
	  if (! parse_ieee_be (dhandle, abfd, &blockstack, bytes, &p, pend))
	    return false;
	  break;

	case ieee_nn_record:
	  if (! parse_ieee_nn (dhandle, abfd, &vars, bytes, &p, pend))
	    return false;
	  break;

	case ieee_ty_record_enum:
	  if (! parse_ieee_ty (dhandle, abfd, &types, &vars, bytes, &p, pend))
	    return false;
	  break;

	case ieee_atn_record_enum:
	  if (! parse_ieee_atn (dhandle, abfd, &types, &vars,
				(blockstack.bsp <= blockstack.stack
				 ? 0
				 : blockstack.bsp[-1].kind),
				bytes, &p, pend))
	    return false;
	  break;
	}
    }

  if (blockstack.bsp != blockstack.stack)
    {
      ieee_error (abfd, (const bfd_byte *) NULL, (const bfd_byte *) NULL,
		  "blocks left on stack at end");
      return false;
    }

  return true;
}

/* Handle an IEEE BB record.  */

static boolean
parse_ieee_bb (dhandle, abfd, types, blockstack, bytes, pp, pend)
     PTR dhandle;
     bfd *abfd;
     struct ieee_types *types;
     struct ieee_blockstack *blockstack;
     const bfd_byte *bytes;
     const bfd_byte **pp;
     const bfd_byte *pend;
{
  const bfd_byte *block_start;
  bfd_byte b;
  bfd_vma size;
  const char *name;
  unsigned long namlen;
  char *namcopy;
	    
  block_start = *pp;

  b = **pp;
  ++*pp;

  if (! ieee_read_number (abfd, bytes, pp, pend, &size)
      || ! ieee_read_id (abfd, bytes, pp, pend, &name, &namlen))
    return false;

  switch (b)
    {
    case 1:
      /* BB1: Type definitions local to a module.  */
      namcopy = savestring (name, namlen);
      if (namcopy == NULL)
	return false;
      if (! debug_set_filename (dhandle, namcopy))
	return false;
      break;

    case 2:
      /* BB2: Global type definitions.  The name is supposed to be
	 empty, but we don't check. */
      if (! debug_set_filename (dhandle, "*global*"))
	return false;
      break;

    case 3:
      /* BB3: High level module block begin.  We don't have to do
	 anything here.  The name is supposed to be the same as for
	 the BB1, but we don't check.  */
      break;

    case 4:
      /* BB4: Global function.  */
      {
	bfd_vma stackspace, typindx, offset;
	debug_type return_type;

	if (! ieee_read_number (abfd, bytes, pp, pend, &stackspace)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &typindx)
	    || ! ieee_read_expression (abfd, bytes, pp, pend, &offset))
	  return false;

	/* We have no way to record the stack space.  FIXME.  */

	if (typindx < 256)
	  {
	    return_type = ieee_builtin_type (dhandle, abfd, types, bytes,
					     block_start, typindx);
	    if (return_type == NULL)
	      return false;
	  }
	else
	  {
	    typindx -= 256;
	    if (! ieee_alloc_type (dhandle, types, typindx, true))
	      return false;
	    return_type = types->types[typindx].return_type;
	    if (return_type == NULL)
	      return_type = types->types[typindx].type;
	  }

	namcopy = savestring (name, namlen);
	if (namcopy == NULL)
	  return false;
	if (! debug_record_function (dhandle, namcopy, return_type,
				     true, offset))
	  return false;
      }
      break;

    case 5:
      /* BB5: File name for source line numbers.  */
      {
	unsigned int i;

	/* We ignore the date and time.  FIXME.  */
	for (i = 0; i < 6; i++)
	  {
	    bfd_vma ignore;
	    boolean present;

	    if (! ieee_read_optional_number (abfd, bytes, pp, pend, &ignore,
					     &present))
	      return false;
	    if (! present)
	      break;
	  }

	namcopy = savestring (name, namlen);
	if (namcopy == NULL)
	  return false;
	if (! debug_start_source (dhandle, namcopy))
	  return false;
      }
      break;

    case 6:
      /* BB6: Local function or block.  */
      {
	bfd_vma stackspace, typindx, offset;

	if (! ieee_read_number (abfd, bytes, pp, pend, &stackspace)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &typindx)
	    || ! ieee_read_expression (abfd, bytes, pp, pend, &offset))
	  return false;

	/* We have no way to record the stack space.  FIXME.  */

	if (namlen == 0)
	  {
	    if (! debug_start_block (dhandle, offset))
	      return false;
	    /* Change b to indicate that this is a block
	       rather than a function.  */
	    b = 0x86;
	  }
	else
	  {
	    debug_type return_type;

	    if (typindx < 256)
	      {
		return_type = ieee_builtin_type (dhandle, abfd, types, bytes,
						 block_start, typindx);
		if (return_type == NULL)
		  return false;
	      }
	    else
	      {
		typindx -= 256;
		if (! ieee_alloc_type (dhandle, types, typindx, true))
		      return false;
		return_type = types->types[typindx].return_type;
		if (return_type == NULL)
		  return_type = types->types[typindx].type;
	      }

	    namcopy = savestring (name, namlen);
	    if (namcopy == NULL)
	      return false;
	    if (! debug_record_function (dhandle, namcopy, return_type,
					 false, offset))
	      return false;
	  }
      }
      break;

    case 10:
      /* BB10: Assembler module scope.  We completely ignore all this
	 information.  FIXME.  */
      {
	const char *inam, *vstr;
	unsigned long inamlen, vstrlen;
	bfd_vma tool_type;
	boolean present;
	unsigned int i;

	if (! ieee_read_id (abfd, bytes, pp, pend, &inam, &inamlen)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &tool_type)
	    || ! ieee_read_optional_id (abfd, bytes, pp, pend, &vstr, &vstrlen,
					&present))
	  return false;
	for (i = 0; i < 6; i++)
	  {
	    bfd_vma ignore;

	    if (! ieee_read_optional_number (abfd, bytes, pp, pend, &ignore,
					     &present))
	      return false;
	    if (! present)
	      break;
	  }
      }
      break;

    case 11:
      /* BB11: Module section.  We completely ignore all this
	 information.  FIXME.  */
      {
	bfd_vma sectype, secindx, offset, map;
	boolean present;

	if (! ieee_read_number (abfd, bytes, pp, pend, &sectype)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &secindx)
	    || ! ieee_read_expression (abfd, bytes, pp, pend, &offset)
	    || ! ieee_read_optional_number (abfd, bytes, pp, pend, &map,
					    &present))
	  return false;
      }
      break;

    default:
      ieee_error (abfd, bytes, block_start, "unknown BB type");
      return false;
    }


  /* Push this block on the block stack.  */

  if (blockstack->bsp >= blockstack->stack + BLOCKSTACK_SIZE)
    {
      ieee_error (abfd, (const bfd_byte *) NULL, (const bfd_byte *) NULL,
		  "stack overflow");
      return false;
    }

  blockstack->bsp->kind = b;
  if (b == 5)
    blockstack->bsp->filename = namcopy;
  ++blockstack->bsp;

  return true;
}

/* Handle an IEEE BE record.  */

static boolean
parse_ieee_be (dhandle, abfd, blockstack, bytes, pp, pend)
     PTR dhandle;
     bfd *abfd;
     struct ieee_blockstack *blockstack;
     const bfd_byte *bytes;
     const bfd_byte **pp;
     const bfd_byte *pend;
{
  bfd_vma offset;

  if (blockstack->bsp <= blockstack->stack)
    {
      ieee_error (abfd, bytes, *pp, "stack underflow");
      return false;
    }
  --blockstack->bsp;

  switch (blockstack->bsp->kind)
    {
    case 4:
    case 6:
      if (! ieee_read_expression (abfd, bytes, pp, pend, &offset))
	return false;
      if (! debug_end_function (dhandle, offset))
	return false;
      break;

    case 0x86:
      /* This is BE6 when BB6 started a block rather than a local
	 function.  */
      if (! ieee_read_expression (abfd, bytes, pp, pend, &offset))
	return false;
      if (! debug_end_block (dhandle, offset))
	return false;
      break;

    case 5:
      /* When we end a BB5, we look up the stack for the last BB5, if
         there is one, so that we can call debug_start_source.  */
      if (blockstack->bsp > blockstack->stack)
	{
	  struct ieee_block *bl;

	  bl = blockstack->bsp;
	  do
	    {
	      --bl;
	      if (bl->kind == 5)
		{
		  if (! debug_start_source (dhandle, bl->filename))
		    return false;
		  break;
		}
	    }
	  while (bl != blockstack->stack);
	}
      break;

    case 11:
      if (! ieee_read_expression (abfd, bytes, pp, pend, &offset))
	return false;
      /* We just ignore the module size.  FIXME.  */
      break;

    default:
      /* Other block types do not have any trailing information.  */
      break;
    }

  return true;
}

/* Parse an NN record.  */

static boolean
parse_ieee_nn (dhandle, abfd, vars, bytes, pp, pend)
     PTR dhandle;
     bfd *abfd;
     struct ieee_vars *vars;
     const bfd_byte *bytes;
     const bfd_byte **pp;
     const bfd_byte *pend;
{
  const bfd_byte *nn_start;
  bfd_vma varindx;
  const char *name;
  unsigned long namlen;

  nn_start = *pp;

  if (! ieee_read_number (abfd, bytes, pp, pend, &varindx)
      || ! ieee_read_id (abfd, bytes, pp, pend, &name, &namlen))
    return false;

  if (varindx < 32)
    {
      ieee_error (abfd, bytes, nn_start, "illegal variable index");
      return false;
    }
  varindx -= 32;

  if (varindx >= vars->alloc)
    {
      unsigned int alloc;

      alloc = vars->alloc;
      if (alloc == 0)
	alloc = 4;
      while (varindx >= alloc)
	alloc *= 2;
      vars->vars = ((struct ieee_var *)
		    xrealloc (vars->vars, alloc * sizeof *vars->vars));
      memset (vars->vars + vars->alloc, 0,
	      (alloc - vars->alloc) * sizeof *vars->vars);
      vars->alloc = alloc;
    }

  vars->vars[varindx].name = name;
  vars->vars[varindx].namlen = namlen;

  return true;
}

/* Parse a TY record.  */

static boolean
parse_ieee_ty (dhandle, abfd, types, vars, bytes, pp, pend)
     PTR dhandle;
     bfd *abfd;
     struct ieee_types *types;
     struct ieee_vars *vars;
     const bfd_byte *bytes;
     const bfd_byte **pp;
     const bfd_byte *pend;
{
  const bfd_byte *ty_start, *ty_var_start, *ty_code_start;
  bfd_vma typeindx, varindx, tc;
  debug_type type;
  boolean tag, typdef;
  unsigned long type_bitsize;
  debug_type return_type;

  ty_start = *pp;

  if (! ieee_read_number (abfd, bytes, pp, pend, &typeindx))
    return false;

  if (typeindx < 256)
    {
      ieee_error (abfd, bytes, ty_start, "illegal type index");
      return false;
    }

  typeindx -= 256;
  if (! ieee_alloc_type (dhandle, types, typeindx, false))
    return false;

  if (**pp != 0xce)
    {
      ieee_error (abfd, bytes, *pp, "unknown TY code");
      return false;
    }
  ++*pp;

  ty_var_start = *pp;

  if (! ieee_read_number (abfd, bytes, pp, pend, &varindx))
    return false;

  if (varindx < 32)
    {
      ieee_error (abfd, bytes, ty_var_start, "illegal variable index");
      return false;
    }
  varindx -= 32;

  if (varindx >= vars->alloc || vars->vars[varindx].name == NULL)
    {
      ieee_error (abfd, bytes, ty_var_start, "undefined variable in TY");
      return false;
    }

  ty_code_start = *pp;

  if (! ieee_read_number (abfd, bytes, pp, pend, &tc))
    return false;

  tag = false;
  typdef = false;
  type_bitsize = 0;
  return_type = DEBUG_TYPE_NULL;
  switch (tc)
    {
    default:
      ieee_error (abfd, bytes, ty_code_start, "unknown TY code");
      return false;

    case '!':
      /* Unknown type, with size.  We treat it as int.  FIXME.  */
      {
	bfd_vma size;

	if (! ieee_read_number (abfd, bytes, pp, pend, &size))
	  return false;
	type = debug_make_int_type (dhandle, size, false);
      }
      break;

    case 'A': /* Array.  */
    case 'a': /* FORTRAN array in column/row order.  FIXME: Not
		 distinguished from normal array.  */
      {
	debug_type ele_type;
	bfd_vma lower, upper;

	if (! ieee_read_type_index (dhandle, abfd, types, bytes, pp, pend,
				    &ele_type)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &lower)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &upper))
	  return false;
	type = debug_make_array_type (dhandle, ele_type,
				      ieee_builtin_type (dhandle, abfd, types,
							 bytes, ty_code_start,
							 ((unsigned int)
							  builtin_int)),
				      (bfd_signed_vma) lower,
				      (bfd_signed_vma) upper,
				      false);
      }
      break;

    case 'E':
      /* Simple enumeration.  */
      {
	bfd_vma size;
	unsigned int alloc;
	const char **names;
	unsigned int c;
	bfd_signed_vma *vals;
	unsigned int i;

	if (! ieee_read_number (abfd, bytes, pp, pend, &size))
	  return false;
	/* FIXME: we ignore the enumeration size.  */

	alloc = 10;
	names = (const char **) xmalloc (alloc * sizeof *names);
	memset (names, 0, alloc * sizeof *names);
	c = 0;
	while (1)
	  {
	    const char *name;
	    unsigned long namlen;
	    boolean present;

	    if (! ieee_read_optional_id (abfd, bytes, pp, pend, &name,
					 &namlen, &present))
	      return false;
	    if (! present)
	      break;

	    if (c + 1 >= alloc)
	      {
		alloc += 10;
		names = ((const char **)
			 xrealloc (names, alloc * sizeof *names));
	      }

	    names[c] = savestring (name, namlen);
	    if (names[c] == NULL)
	      return false;
	    ++c;
	  }

	names[c] = NULL;

	vals = (bfd_signed_vma *) xmalloc (c * sizeof *vals);
	for (i = 0; i < c; i++)
	  vals[i] = i;

	type = debug_make_enum_type (dhandle, names, vals);
	tag = true;
      }
      break;

    case 'G':
      /* Struct with bit fields.  */
      {
	bfd_vma size;
	unsigned int alloc;
	debug_field *fields;
	unsigned int c;

	if (! ieee_read_number (abfd, bytes, pp, pend, &size))
	  return false;

	alloc = 10;
	fields = (debug_field *) xmalloc (alloc * sizeof *fields);
	c = 0;
	while (1)
	  {
	    const char *name;
	    unsigned long namlen;
	    boolean present;
	    debug_type ftype;
	    bfd_vma bitpos, bitsize;

	    if (! ieee_read_optional_id (abfd, bytes, pp, pend, &name,
					 &namlen, &present))
	      return false;
	    if (! present)
	      break;
	    if (! ieee_read_type_index (dhandle, abfd, types, bytes, pp, pend,
					&ftype)
		|| ! ieee_read_number (abfd, bytes, pp, pend, &bitpos)
		|| ! ieee_read_number (abfd, bytes, pp, pend, &bitsize))
	      return false;

	    if (c + 1 >= alloc)
	      {
		alloc += 10;
		fields = ((debug_field *)
			  xrealloc (fields, alloc * sizeof *fields));
	      }

	    fields[c] = debug_make_field (dhandle, savestring (name, namlen),
					  ftype, bitpos, bitsize,
					  DEBUG_VISIBILITY_PUBLIC);
	    if (fields[c] == NULL)
	      return false;
	    ++c;
	  }

	fields[c] = NULL;

	type = debug_make_struct_type (dhandle, true, size, fields);
	tag = true;
      }
      break;

    case 'N':
      /* Enumeration.  */
      {
	unsigned int alloc;
	const char **names;
	bfd_signed_vma *vals;
	unsigned int c;

	alloc = 10;
	names = (const char **) xmalloc (alloc * sizeof *names);
	vals = (bfd_signed_vma *) xmalloc (alloc * sizeof *names);
	c = 0;
	while (1)
	  {
	    const char *name;
	    unsigned long namlen;
	    boolean present;
	    bfd_vma val;

	    if (! ieee_read_optional_id (abfd, bytes, pp, pend, &name,
					 &namlen, &present))
	      return false;
	    if (! present)
	      break;
	    if (! ieee_read_number (abfd, bytes, pp, pend, &val))
	      return false;

	    /* If the length of the name is zero, then the value is
               actually the size of the enum.  We ignore this
               information.  FIXME.  */
	    if (namlen == 0)
	      continue;

	    if (c + 1 >= alloc)
	      {
		alloc += 10;
		names = ((const char **)
			 xrealloc (names, alloc * sizeof *names));
		vals = ((bfd_signed_vma *)
			xrealloc (vals, alloc * sizeof *vals));
	      }

	    names[c] = savestring (name, namlen);
	    if (names[c] == NULL)
	      return false;
	    vals[c] = (bfd_signed_vma) val;
	    ++c;
	  }

	names[c] = NULL;

	type = debug_make_enum_type (dhandle, names, vals);
	tag = true;
      }
      break;

    case 'O': /* Small pointer.  We don't distinguish small and large
		 pointers.  FIXME.  */
    case 'P': /* Large pointer.  */
      {
	debug_type t;

	if (! ieee_read_type_index (dhandle, abfd, types, bytes, pp, pend, &t))
	  return false;
	type = debug_make_pointer_type (dhandle, t);
      }
      break;

    case 'R':
      /* Range.  */
      {
	bfd_vma low, high, signedp, size;

	if (! ieee_read_number (abfd, bytes, pp, pend, &low)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &high)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &signedp)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &size))
	  return false;

	type = debug_make_range_type (dhandle,
				      debug_make_int_type (dhandle, size,
							   ! signedp),
				      (bfd_signed_vma) low,
				      (bfd_signed_vma) high);
      }
      break;

    case 'S': /* Struct.  */
    case 'U': /* Union.  */
      {
	bfd_vma size;
	unsigned int alloc;
	debug_field *fields;
	unsigned int c;

	if (! ieee_read_number (abfd, bytes, pp, pend, &size))
	  return false;

	alloc = 10;
	fields = (debug_field *) xmalloc (alloc * sizeof *fields);
	c = 0;
	while (1)
	  {
	    const char *name;
	    unsigned long namlen;
	    boolean present;
	    bfd_vma tindx;
	    bfd_vma offset;
	    debug_type ftype;
	    bfd_vma bitsize;

	    if (! ieee_read_optional_id (abfd, bytes, pp, pend, &name,
					 &namlen, &present))
	      return false;
	    if (! present)
	      break;
	    if (! ieee_read_number (abfd, bytes, pp, pend, &tindx)
		|| ! ieee_read_number (abfd, bytes, pp, pend, &offset))
	      return false;

	    if (tindx < 256)
	      {
		ftype = ieee_builtin_type (dhandle, abfd, types, bytes,
					   ty_code_start, tindx);
		bitsize = 0;
		offset *= 8;
	      }
	    else
	      {
		struct ieee_type *t;

		tindx -= 256;
		if (! ieee_alloc_type (dhandle, types, tindx, true))
		  return false;
		t = types->types + tindx;
		ftype = t->type;
		bitsize = t->bitsize;
		if (bitsize == 0)
		  offset *= 8;
	      }

	    if (c + 1 >= alloc)
	      {
		alloc += 10;
		fields = ((debug_field *)
			  xrealloc (fields, alloc * sizeof *fields));
	      }

	    fields[c] = debug_make_field (dhandle, savestring (name, namlen),
					  ftype, offset, bitsize,
					  DEBUG_VISIBILITY_PUBLIC);
	    if (fields[c] == NULL)
	      return false;
	    ++c;
	  }

	fields[c] = NULL;

	type = debug_make_struct_type (dhandle, tc == 'S', size, fields);
	tag = true;
      }
      break;

    case 'T':
      /* Typedef.  */
      if (! ieee_read_type_index (dhandle, abfd, types, bytes, pp, pend,
				  &type))
	return false;
      typdef = true;
      break;

    case 'X':
      /* Procedure.  FIXME: This is an extern declaration, which we
         have no way of representing.  */
      {
	bfd_vma attr;
	debug_type rtype;
	bfd_vma nargs;
	boolean present;

	/* FIXME: We ignore the attribute and the argument names.  */

	if (! ieee_read_number (abfd, bytes, pp, pend, &attr)
	    || ! ieee_read_type_index (dhandle, abfd, types, bytes, pp, pend,
				       &rtype)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &nargs))
	  return false;
	do
	  {
	    const char *name;
	    unsigned long namlen;

	    if (! ieee_read_optional_id (abfd, bytes, pp, pend, &name,
					 &namlen, &present))
	      return false;
	  }
	while (present);

	type = debug_make_function_type (dhandle, rtype, (debug_type *) NULL,
					 false);
	return_type = rtype;
      }
      break;

    case 'Z':
      /* Array with 0 lower bound.  */
      {
	debug_type etype;
	bfd_vma high;

	if (! ieee_read_type_index (dhandle, abfd, types, bytes, pp, pend,
				    &etype)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &high))
	  return false;

	type = debug_make_array_type (dhandle, etype,
				      ieee_builtin_type (dhandle, abfd, types,
							 bytes, ty_code_start,
							 ((unsigned int)
							  builtin_int)),
				      0, (bfd_signed_vma) high, false);
      }
      break;

    case 'c': /* Complex.  */
    case 'd': /* Double complex.  */
      {
	const char *name;
	unsigned long namlen;

	/* FIXME: I don't know what the name means.  */

	if (! ieee_read_id (abfd, bytes, pp, pend, &name, &namlen))
	  return false;

	type = debug_make_complex_type (dhandle, tc == 'c' ? 4 : 8);
      }
      break;

    case 'f':
      /* Pascal file name.  FIXME.  */
      ieee_error (abfd, bytes, ty_code_start,
		  "Pascal file name not supported");
      return false;

    case 'g':
      /* Bitfield type.  */
      {
	bfd_vma signedp, bitsize;

	if (! ieee_read_number (abfd, bytes, pp, pend, &signedp)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &bitsize)
	    || ! ieee_read_type_index (dhandle, abfd, types, bytes, pp, pend,
				       &type))
	  return false;

	/* FIXME: This is just a guess.  */
	if (! signedp)
	  type = debug_make_int_type (dhandle, 4, true);
	type_bitsize = bitsize;
      }
      break;

    case 'n':
      /* Qualifier.  */
      {
	bfd_vma kind;
	debug_type t;

	if (! ieee_read_number (abfd, bytes, pp, pend, &kind)
	    || ! ieee_read_type_index (dhandle, abfd, types, bytes, pp, pend,
				       &t))
	  return false;

	switch (kind)
	  {
	  default:
	    ieee_error (abfd, bytes, ty_start, "unsupported qualifer");
	    return false;

	  case 1:
	    type = debug_make_const_type (dhandle, t);
	    break;

	  case 2:
	    type = debug_make_volatile_type (dhandle, t);
	    break;
	  }
      }
      break;

    case 's':
      /* Set.  */
      {
	bfd_vma size;
	debug_type etype;

	if (! ieee_read_number (abfd, bytes, pp, pend, &size)
	    || ! ieee_read_type_index (dhandle, abfd, types, bytes, pp, pend,
				       &etype))
	  return false;

	/* FIXME: We ignore the size.  */

	type = debug_make_set_type (dhandle, etype, false);
      }
      break;

    case 'x':
      /* Procedure with compiler dependencies.  FIXME: This is an
         extern declaration, which we have no way of representing.  */
      {
	bfd_vma attr, frame_type, push_mask, nargs, level, father;
	debug_type rtype;
	debug_type *arg_types;
	boolean varargs;
	boolean present;

	/* FIXME: We ignore almost all this information.  */

	if (! ieee_read_number (abfd, bytes, pp, pend, &attr)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &frame_type)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &push_mask)
	    || ! ieee_read_type_index (dhandle, abfd, types, bytes, pp, pend,
				       &rtype)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &nargs))
	  return false;
	if (nargs == (bfd_vma) -1)
	  {
	    arg_types = NULL;
	    varargs = false;
	  }
	else
	  {
	    unsigned int i;

	    arg_types = ((debug_type *)
			 xmalloc ((nargs + 1) * sizeof *arg_types));
	    for (i = 0; i < nargs; i++)
	      if (! ieee_read_type_index (dhandle, abfd, types, bytes, pp,
					  pend, arg_types + i))
		return false;

	    /* If the last type is pointer to void, this is really a
               varargs function.  */
	    varargs = false;
	    if (nargs > 0)
	      {
		debug_type last;

		last = arg_types[nargs - 1];
		if (debug_get_type_kind (dhandle, last) == DEBUG_KIND_POINTER
		    && (debug_get_type_kind (dhandle,
					     debug_get_target_type (dhandle,
								    last))
			== DEBUG_KIND_VOID))
		  {
		    --nargs;
		    varargs = true;
		  }
	      }

	    arg_types[nargs] = DEBUG_TYPE_NULL;
	  }
	if (! ieee_read_number (abfd, bytes, pp, pend, &level)
	    || ! ieee_read_optional_number (abfd, bytes, pp, pend, &father,
					    &present))
	  return false;

	type = debug_make_function_type (dhandle, rtype, arg_types, varargs);
	return_type = rtype;
      }
      break;
    }

  /* Record the type in the table.  If the corresponding NN record has
     a name, name it.  FIXME: Is this always correct?  */

  if (type == NULL)
    return false;

  if ((tag || typdef)
      && vars->vars[varindx].namlen > 0)
    {
      const char *name;

      name = savestring (vars->vars[varindx].name,
			 vars->vars[varindx].namlen);
      if (tag)
	type = debug_tag_type (dhandle, name, type);
      else
	type = debug_name_type (dhandle, name, type);
      if (type == NULL)
	return false;
    }

  types->types[typeindx].type = type;
  types->types[typeindx].bitsize = type_bitsize;
  types->types[typeindx].return_type = return_type;

  /* We may have already allocated type as an indirect type pointing
     to slot.  It does no harm to replace the indirect type with the
     real type.  Filling in slot as well handles the indirect types
     which are already hanging around.  */
  if (types->types[typeindx].pslot != NULL)
    *types->types[typeindx].pslot = type;

  return true;
}

/* Parse an ATN record.  */

static boolean
parse_ieee_atn (dhandle, abfd, types, vars, blocktype, bytes, pp, pend)
     PTR dhandle;
     bfd *abfd;
     struct ieee_types *types;
     struct ieee_vars *vars;
     int blocktype;
     const bfd_byte *bytes;
     const bfd_byte **pp;
     const bfd_byte *pend;
{
  const bfd_byte *atn_start, *atn_code_start;
  bfd_vma varindx;
  boolean zeroindx;
  debug_type type;
  bfd_vma atn_code;
  bfd_vma v, v2, v3, v4, v5;
  const char *name;
  unsigned long namlen;
  char *namcopy;
  boolean present;

  atn_start = *pp;

  if (! ieee_read_number (abfd, bytes, pp, pend, &varindx)
      || ! ieee_read_type_index (dhandle, abfd, types, bytes, pp, pend, &type))
    return false;

  atn_code_start = *pp;

  if (! ieee_read_number (abfd, bytes, pp, pend, &atn_code))
    return false;

  if (varindx == 0)
    {
      zeroindx = true;
      name = "";
      namlen = 0;
    }
  else if (varindx < 32)
    {
      ieee_error (abfd, bytes, atn_start, "illegal variable index");
      return false;
    }
  else
    {
      varindx -= 32;
      zeroindx = false;
      if (varindx >= vars->alloc || vars->vars[varindx].name == NULL)
	{
	  ieee_error (abfd, bytes, atn_start, "undefined variable in ATN");
	  return false;
	}

      vars->vars[varindx].type = type;

      name = vars->vars[varindx].name;
      namlen = vars->vars[varindx].namlen;
    }

  switch (atn_code)
    {
    default:
      ieee_error (abfd, bytes, atn_code_start, "unknown ATN type");
      return false;

    case 1:
      /* Automatic variable.  */
      if (! ieee_read_number (abfd, bytes, pp, pend, &v))
	return false;
      namcopy = savestring (name, namlen);
      if (type == NULL)
	type = debug_make_void_type (dhandle);
      return debug_record_variable (dhandle, namcopy, type, DEBUG_LOCAL, v);

    case 2:
      /* Register variable.  */
      if (! ieee_read_number (abfd, bytes, pp, pend, &v))
	return false;
      namcopy = savestring (name, namlen);
      if (type == NULL)
	type = debug_make_void_type (dhandle);
      return debug_record_variable (dhandle, namcopy, type, DEBUG_REGISTER,
				    ieee_regno_to_genreg (abfd, v));

    case 3:
      /* Static variable.  */
      if (! ieee_require_asn (abfd, bytes, pp, pend, &v))
	return false;
      namcopy = savestring (name, namlen);
      if (type == NULL)
	type = debug_make_void_type (dhandle);
      return debug_record_variable (dhandle, namcopy, type,
				    (blocktype == 4 || blocktype == 6
				     ? DEBUG_LOCAL_STATIC
				     : DEBUG_STATIC),
				    v);

    case 4:
      /* External function.  We don't currently record these.  FIXME.  */
      return true;

    case 5:
      /* External variable.  We don't currently record these.  FIXME.  */
      return true;

    case 7:
      if (! ieee_read_number (abfd, bytes, pp, pend, &v)
	  || ! ieee_read_number (abfd, bytes, pp, pend, &v2)
	  || ! ieee_read_optional_number (abfd, bytes, pp, pend, &v3,
					  &present))
	return false;
      if (present)
	{
	  if (! ieee_read_optional_number (abfd, bytes, pp, pend, &v4,
					   &present))
	    return false;
	}

      /* We just ignore the two optional fields in v3 and v4, since
         they are not defined.  */

      if (! ieee_require_asn (abfd, bytes, pp, pend, &v3))
	return false;

      /* We have no way to record the column number.  FIXME.  */

      return debug_record_line (dhandle, v, v3);

    case 8:
      /* Global variable.  */
      if (! ieee_require_asn (abfd, bytes, pp, pend, &v))
	return false;
      namcopy = savestring (name, namlen);
      if (type == NULL)
	type = debug_make_void_type (dhandle);
      return debug_record_variable (dhandle, namcopy, type, DEBUG_GLOBAL, v);

    case 9:
      /* Variable lifetime information.  */
      if (! ieee_read_number (abfd, bytes, pp, pend, &v))
	return false;

      /* We have no way to record this information.  FIXME.  */
      return true;

    case 10:
      /* Locked register.  */
      if (! ieee_read_number (abfd, bytes, pp, pend, &v)
	  || ! ieee_read_number (abfd, bytes, pp, pend, &v2))
	return false;

      /* I think this means a variable that is both in a register and
         a frame slot.  We ignore the frame slot.  FIXME.  */

      namcopy = savestring (name, namlen);
      if (type == NULL)
	type = debug_make_void_type (dhandle);
      return debug_record_variable (dhandle, namcopy, type, DEBUG_REGISTER, v);

    case 11:
      /* Reserved for FORTRAN common.  */
      ieee_error (abfd, bytes, atn_code_start, "unsupported ATN11");

      /* Return true to keep going.  */
      return true;

    case 12:
      /* Based variable.  */
      v3 = 0;
      v4 = 0x80;
      v5 = 0;
      if (! ieee_read_number (abfd, bytes, pp, pend, &v)
	  || ! ieee_read_number (abfd, bytes, pp, pend, &v2)
	  || ! ieee_read_optional_number (abfd, bytes, pp, pend, &v3,
					  &present))
	return false;
      if (present)
	{
	  if (! ieee_read_optional_number (abfd, bytes, pp, pend, &v4,
					   &present))
	    return false;
	  if (present)
	    {
	      if (! ieee_read_optional_number (abfd, bytes, pp, pend, &v5,
					       &present))
		return false;
	    }
	}

      /* We have no way to record this information.  FIXME.  */

      ieee_error (abfd, bytes, atn_code_start, "unsupported ATN12");

      /* Return true to keep going.  */
      return true;

    case 16:
      /* Constant.  The description of this that I have is ambiguous,
         so I'm not going to try to implement it.  */
      ieee_error (abfd, bytes, atn_code_start, "unsupported ATN16");
      return false;

    case 19:
      /* Static variable from assembler.  */
      v2 = 0;
      if (! ieee_read_number (abfd, bytes, pp, pend, &v)
	  || ! ieee_read_optional_number (abfd, bytes, pp, pend, &v2,
					  &present)
	  || ! ieee_require_asn (abfd, bytes, pp, pend, &v3))
	return false;
      namcopy = savestring (name, namlen);
      /* We don't really handle this correctly.  FIXME.  */
      return debug_record_variable (dhandle, namcopy,
				    debug_make_void_type (dhandle),
				    v2 != 0 ? DEBUG_GLOBAL : DEBUG_STATIC,
				    v3);

    case 62:
      /* Procedure miscellaneous information.  */
    case 63:
      /* Variable miscellaneous information.  */
    case 64:
      /* Module miscellaneous information.  */
      if (! ieee_read_number (abfd, bytes, pp, pend, &v)
	  || ! ieee_read_number (abfd, bytes, pp, pend, &v2)
	  || ! ieee_read_optional_id (abfd, bytes, pp, pend, &name, &namlen,
				      &present))
	return false;

      /* We just ignore all of this stuff.  FIXME.  */

      for (; v2 > 0; --v2)
	{
	  ieee_record_enum_type c;
	  bfd_vma vindx;
	  const char *str;
	  unsigned long strlen;

	  c = (ieee_record_enum_type) **pp;
	  ++*pp;
	  if (c != ieee_at_record_enum
	      && c != ieee_e2_first_byte_enum)
	    {
	      ieee_error (abfd, bytes, *pp - 1, "bad misc record");
	      return false;
	    }

	  c = (ieee_record_enum_type) (((unsigned int) c << 8) | **pp);
	  ++*pp;
	  switch (c)
	    {
	    default:
	      ieee_error (abfd, bytes, *pp - 2, "bad misc record");
	      return false;

	    case ieee_atn_record_enum:
	      if (! ieee_read_number (abfd, bytes, pp, pend, &vindx))
		return false;
	      if ((*pp)[0] != 0 || (*pp)[1] != 65)
		{
		  ieee_error (abfd, bytes, *pp, "bad atn in misc");
		  return false;
		}
	      *pp += 2;
	      if (! ieee_read_id (abfd, bytes, pp, pend, &str, &strlen))
		return false;
	      break;

	    case ieee_asn_record_enum:
	      if (! ieee_read_number (abfd, bytes, pp, pend, &vindx)
		  || ! ieee_read_expression (abfd, bytes, pp, pend, &v3))
		return false;
	      break;
	    }
	}

      return true;
    }

  /*NOTREACHED*/
}

/* Require an ASN record.  */

static boolean
ieee_require_asn (abfd, bytes, pp, pend, pv)
     bfd *abfd;
     const bfd_byte *bytes;
     const bfd_byte **pp;
     const bfd_byte *pend;
     bfd_vma *pv;
{
  const bfd_byte *start;
  ieee_record_enum_type c;
  bfd_vma varindx;

  start = *pp;

  c = (ieee_record_enum_type) **pp;
  if (c != ieee_e2_first_byte_enum)
    {
      ieee_error (abfd, bytes, start, "missing required ASN");
      return false;
    }
  ++*pp;

  c = (ieee_record_enum_type) (((unsigned int) c << 8) | **pp);
  if (c != ieee_asn_record_enum)
    {
      ieee_error (abfd, bytes, start, "missing required ASN");
      return false;
    }
  ++*pp;

  /* Just ignore the variable index.  */
  if (! ieee_read_number (abfd, bytes, pp, pend, &varindx))
    return false;

  return ieee_read_expression (abfd, bytes, pp, pend, pv);
}

/* Convert a register number in IEEE debugging information into a
   generic register number.  */

static int
ieee_regno_to_genreg (abfd, r)
     bfd *abfd;
     int r;
{
  return r;
}

/* Convert a generic register number to an IEEE specific one.  */

static int
ieee_genreg_to_regno (abfd, r)
     bfd *abfd;
     int r;
{
  return r;
}

/* These routines build IEEE debugging information out of the generic
   debugging information.  */

/* We build the IEEE debugging information byte by byte.  Rather than
   waste time copying data around, we use a linked list of buffers to
   hold the data.  */

#define IEEE_BUFSIZE (490)

struct ieee_buf
{
  /* Next buffer.  */
  struct ieee_buf *next;
  /* Number of data bytes in this buffer.  */
  unsigned int c;
  /* Bytes.  */
  bfd_byte buf[IEEE_BUFSIZE];
};

/* In order to generate the BB11 blocks required by the HP emulator,
   we keep track of ranges of addresses which correspond to a given
   compilation unit.  */

struct ieee_range
{
  /* Next range.  */
  struct ieee_range *next;
  /* Low address.  */
  bfd_vma low;
  /* High address.  */
  bfd_vma high;
};

/* This is how we store types for the writing routines.  Most types
   are simply represented by a type index.  */

struct ieee_write_type
{
  /* Type index.  */
  unsigned int indx;
  /* The size of the type, if known.  */
  unsigned int size;
  /* If this is a struct, this is where the struct definition is
     built.  */
  struct ieee_buf *strdef;
  /* Whether the type is unsigned.  */
  unsigned int unsignedp : 1;
  /* Whether this is a reference type.  */
  unsigned int referencep : 1;
};

/* This is the type stack used by the debug writing routines.  FIXME:
   We could generate more efficient output if we remembered when we
   have output a particular type before.  */

struct ieee_type_stack
{
  /* Next entry on stack.  */
  struct ieee_type_stack *next;
  /* Type information.  */
  struct ieee_write_type type;
};

/* This is a list of associations between names and types.  This could
   be more efficiently implemented as a hash table.  */

struct ieee_name_type
{
  /* Next name/type assocation.  */
  struct ieee_name_type *next;
  /* Name.  */
  const char *name;
  /* Type.  */
  struct ieee_write_type type;
  /* If this is a tag which has not yet been defined, this is the
     kind.  If the tag has been defined, this is DEBUG_KIND_ILLEGAL.  */
  enum debug_type_kind kind;
};

/* This is a list of pending function parameter information.  We don't
   output them until we see the first block.  */

struct ieee_pending_parm
{
  /* Next pending parameter.  */
  struct ieee_pending_parm *next;
  /* Name.  */
  const char *name;
  /* Type index.  */
  unsigned int type;
  /* Kind.  */
  enum debug_parm_kind kind;
  /* Value.  */
  bfd_vma val;
};

/* This is the handle passed down by debug_write.  */

struct ieee_handle
{
  /* BFD we are writing to.  */
  bfd *abfd;
  /* Current data buffer.  */
  struct ieee_buf *current;
  /* Filename of current compilation unit.  */
  const char *filename;
  /* Module name of current compilation unit.  */
  const char *modname;
  /* List of finished data buffers.  */
  struct ieee_buf *data;
  /* List of buffers for typedefs in the current compilation unit.  */
  struct ieee_buf *types;
  /* List of buffers for variables and functions in the current
     compilation unit.  */
  struct ieee_buf *vars;
  /* List of buffers for line numbers in the current compilation unit.  */
  struct ieee_buf *linenos;
  /* Ranges for the current compilation unit.  */
  struct ieee_range *ranges;
  /* Nested pending ranges.  */
  struct ieee_range *pending_ranges;
  /* Type stack.  */
  struct ieee_type_stack *type_stack;
  /* Next unallocated type index.  */
  unsigned int type_indx;
  /* Next unallocated name index.  */
  unsigned int name_indx;
  /* Typedefs.  */
  struct ieee_name_type *typedefs;
  /* Tags.  */
  struct ieee_name_type *tags;
  /* The depth of block nesting.  This is 0 outside a function, and 1
     just after start_function is called.  */
  unsigned int block_depth;
  /* Pending function parameters.  */
  struct ieee_pending_parm *pending_parms;
  /* Current line number filename.  */
  const char *lineno_filename;
  /* Line number name index.  */
  unsigned int lineno_name_indx;
};

static boolean ieee_change_buffer
  PARAMS ((struct ieee_handle *, struct ieee_buf **));
static boolean ieee_push_type
  PARAMS ((struct ieee_handle *, unsigned int, unsigned int, boolean));
static unsigned int ieee_pop_type PARAMS ((struct ieee_handle *));
static boolean ieee_add_range
  PARAMS ((struct ieee_handle *, bfd_vma, bfd_vma));
static boolean ieee_start_range PARAMS ((struct ieee_handle *, bfd_vma));
static boolean ieee_end_range PARAMS ((struct ieee_handle *, bfd_vma));
static boolean ieee_real_write_byte PARAMS ((struct ieee_handle *, int));
static boolean ieee_write_2bytes PARAMS ((struct ieee_handle *, int));
static boolean ieee_write_number PARAMS ((struct ieee_handle *, bfd_vma));
static boolean ieee_write_id PARAMS ((struct ieee_handle *, const char *));
static boolean ieee_define_type
  PARAMS ((struct ieee_handle *, unsigned int, boolean));
static boolean ieee_define_named_type
  PARAMS ((struct ieee_handle *, const char *, boolean, unsigned int, boolean,
	   struct ieee_buf **));
static boolean ieee_finish_compilation_unit PARAMS ((struct ieee_handle *));
static boolean ieee_output_pending_parms PARAMS ((struct ieee_handle *));

static boolean ieee_start_compilation_unit PARAMS ((PTR, const char *));
static boolean ieee_start_source PARAMS ((PTR, const char *));
static boolean ieee_empty_type PARAMS ((PTR));
static boolean ieee_void_type PARAMS ((PTR));
static boolean ieee_int_type PARAMS ((PTR, unsigned int, boolean));
static boolean ieee_float_type PARAMS ((PTR, unsigned int));
static boolean ieee_complex_type PARAMS ((PTR, unsigned int));
static boolean ieee_bool_type PARAMS ((PTR, unsigned int));
static boolean ieee_enum_type
  PARAMS ((PTR, const char *, const char **, bfd_signed_vma *));
static boolean ieee_pointer_type PARAMS ((PTR));
static boolean ieee_function_type PARAMS ((PTR, int, boolean));
static boolean ieee_reference_type PARAMS ((PTR));
static boolean ieee_range_type PARAMS ((PTR, bfd_signed_vma, bfd_signed_vma));
static boolean ieee_array_type
  PARAMS ((PTR, bfd_signed_vma, bfd_signed_vma, boolean));
static boolean ieee_set_type PARAMS ((PTR, boolean));
static boolean ieee_offset_type PARAMS ((PTR));
static boolean ieee_method_type PARAMS ((PTR, boolean, int, boolean));
static boolean ieee_const_type PARAMS ((PTR));
static boolean ieee_volatile_type PARAMS ((PTR));
static boolean ieee_start_struct_type
  PARAMS ((PTR, const char *, unsigned int, boolean, unsigned int));
static boolean ieee_struct_field
  PARAMS ((PTR, const char *, bfd_vma, bfd_vma, enum debug_visibility));
static boolean ieee_end_struct_type PARAMS ((PTR));
static boolean ieee_start_class_type
  PARAMS ((PTR, const char *, unsigned int, boolean, unsigned int, boolean,
	   boolean));
static boolean ieee_class_static_member
  PARAMS ((PTR, const char *, const char *, enum debug_visibility));
static boolean ieee_class_baseclass
  PARAMS ((PTR, bfd_vma, boolean, enum debug_visibility));
static boolean ieee_class_start_method PARAMS ((PTR, const char *));
static boolean ieee_class_method_variant
  PARAMS ((PTR, const char *, enum debug_visibility, boolean, boolean,
	   bfd_vma, boolean));
static boolean ieee_class_static_method_variant
  PARAMS ((PTR, const char *, enum debug_visibility, boolean, boolean));
static boolean ieee_class_end_method PARAMS ((PTR));
static boolean ieee_end_class_type PARAMS ((PTR));
static boolean ieee_typedef_type PARAMS ((PTR, const char *));
static boolean ieee_tag_type
  PARAMS ((PTR, const char *, unsigned int, enum debug_type_kind));
static boolean ieee_typdef PARAMS ((PTR, const char *));
static boolean ieee_tag PARAMS ((PTR, const char *));
static boolean ieee_int_constant PARAMS ((PTR, const char *, bfd_vma));
static boolean ieee_float_constant PARAMS ((PTR, const char *, double));
static boolean ieee_typed_constant PARAMS ((PTR, const char *, bfd_vma));
static boolean ieee_variable
  PARAMS ((PTR, const char *, enum debug_var_kind, bfd_vma));
static boolean ieee_start_function PARAMS ((PTR, const char *, boolean));
static boolean ieee_function_parameter
  PARAMS ((PTR, const char *, enum debug_parm_kind, bfd_vma));
static boolean ieee_start_block PARAMS ((PTR, bfd_vma));
static boolean ieee_end_block PARAMS ((PTR, bfd_vma));
static boolean ieee_end_function PARAMS ((PTR));
static boolean ieee_lineno
  PARAMS ((PTR, const char *, unsigned long, bfd_vma));

static const struct debug_write_fns ieee_fns =
{
  ieee_start_compilation_unit,
  ieee_start_source,
  ieee_empty_type,
  ieee_void_type,
  ieee_int_type,
  ieee_float_type,
  ieee_complex_type,
  ieee_bool_type,
  ieee_enum_type,
  ieee_pointer_type,
  ieee_function_type,
  ieee_reference_type,
  ieee_range_type,
  ieee_array_type,
  ieee_set_type,
  ieee_offset_type,
  ieee_method_type,
  ieee_const_type,
  ieee_volatile_type,
  ieee_start_struct_type,
  ieee_struct_field,
  ieee_end_struct_type,
  ieee_start_class_type,
  ieee_class_static_member,
  ieee_class_baseclass,
  ieee_class_start_method,
  ieee_class_method_variant,
  ieee_class_static_method_variant,
  ieee_class_end_method,
  ieee_end_class_type,
  ieee_typedef_type,
  ieee_tag_type,
  ieee_typdef,
  ieee_tag,
  ieee_int_constant,
  ieee_float_constant,
  ieee_typed_constant,
  ieee_variable,
  ieee_start_function,
  ieee_function_parameter,
  ieee_start_block,
  ieee_end_block,
  ieee_end_function,
  ieee_lineno
};

/* Change the current buffer to a specified buffer chain.  */

static boolean
ieee_change_buffer (info, ppbuf)
     struct ieee_handle *info;
     struct ieee_buf **ppbuf;
{
  struct ieee_buf *buf;

  if (*ppbuf != NULL)
    {
      for (buf = *ppbuf; buf->next != NULL; buf = buf->next)
	;
    }
  else
    {
      buf = (struct ieee_buf *) xmalloc (sizeof *buf);
      buf->next = NULL;
      buf->c = 0;
      *ppbuf = buf;
    }

  info->current = buf;
  return true;
}

/* Push a type index onto the type stack.  */

static boolean
ieee_push_type (info, indx, size, unsignedp)
     struct ieee_handle *info;
     unsigned int indx;
     unsigned int size;
     boolean unsignedp;
{
  struct ieee_type_stack *ts;

  ts = (struct ieee_type_stack *) xmalloc (sizeof *ts);
  memset (ts, 0, sizeof *ts);

  ts->type.indx = indx;
  ts->type.size = size;
  ts->type.unsignedp = unsignedp;

  ts->next = info->type_stack;
  info->type_stack = ts;

  return true;
}

/* Pop a type index off the type stack.  */

static unsigned int
ieee_pop_type (info)
     struct ieee_handle *info;
{
  struct ieee_type_stack *ts;
  unsigned int ret;

  ts = info->type_stack;
  assert (ts != NULL);
  ret = ts->type.indx;
  info->type_stack = ts->next;
  free (ts);
  return ret;
}

/* Add a range of bytes included in the current compilation unit.  */

static boolean
ieee_add_range (info, low, high)
     struct ieee_handle *info;
     bfd_vma low;
     bfd_vma high;
{
  struct ieee_range *r, **pr;

  if (low == (bfd_vma) -1 || high == (bfd_vma) -1)
    return true;

  for (r = info->ranges; r != NULL; r = r->next)
    {
      if (high >= r->low && low <= r->high)
	{
	  /* The new range overlaps r.  */
	  if (low < r->low)
	    r->low = low;
	  if (high > r->high)
	    r->high = high;
	  pr = &r->next;
	  while (*pr != NULL && (*pr)->low <= r->high)
	    {
	      struct ieee_range *n;

	      if ((*pr)->high > r->high)
		r->high = (*pr)->high;
	      n = (*pr)->next;
	      free (*pr);
	      *pr = n;
	    }
	  return true;
	}
    }

  r = (struct ieee_range *) xmalloc (sizeof *r);
  memset (r, 0, sizeof *r);

  r->low = low;
  r->high = high;

  /* Store the ranges sorted by address.  */
  for (pr = &info->ranges; *pr != NULL; pr = &(*pr)->next)
    if ((*pr)->next != NULL && (*pr)->next->low > high)
      break;
  r->next = *pr;
  *pr = r;

  return true;
}

/* Start a new range for which we only have the low address.  */

static boolean
ieee_start_range (info, low)
     struct ieee_handle *info;
     bfd_vma low;
{
  struct ieee_range *r;

  r = (struct ieee_range *) xmalloc (sizeof *r);
  memset (r, 0, sizeof *r);
  r->low = low;
  r->next = info->pending_ranges;
  info->pending_ranges = r;
  return true;
}  

/* Finish a range started by ieee_start_range.  */

static boolean
ieee_end_range (info, high)
     struct ieee_handle *info;
     bfd_vma high;
{
  struct ieee_range *r;
  bfd_vma low;

  assert (info->pending_ranges != NULL);
  r = info->pending_ranges;
  low = r->low;
  info->pending_ranges = r->next;
  free (r);
  return ieee_add_range (info, low, high);
}

/* Write a byte into the buffer.  We use a macro for speed and a
   function for the complex cases.  */

#define ieee_write_byte(info, b)				\
  ((info)->current->c < IEEE_BUFSIZE				\
   ? ((info)->current->buf[(info)->current->c++] = (b), true)	\
   : ieee_real_write_byte ((info), (b)))

static boolean
ieee_real_write_byte (info, b)
     struct ieee_handle *info;
     int b;
{
  if (info->current->c >= IEEE_BUFSIZE)
    {
      struct ieee_buf *n;

      n = (struct ieee_buf *) xmalloc (sizeof *n);
      n->next = NULL;
      n->c = 0;
      info->current->next = n;
      info->current = n;
    }

  info->current->buf[info->current->c] = b;
  ++info->current->c;

  return true;
}

/* Write out two bytes.  */

static boolean
ieee_write_2bytes (info, i)
     struct ieee_handle *info;
     int i;
{
  return (ieee_write_byte (info, i >> 8)
	  && ieee_write_byte (info, i & 0xff));
}

/* Write out an integer.  */

static boolean
ieee_write_number (info, v)
     struct ieee_handle *info;
     bfd_vma v;
{
  bfd_vma t;
  bfd_byte ab[20];
  bfd_byte *p;
  unsigned int c;

  if (v <= (bfd_vma) ieee_number_end_enum)
    return ieee_write_byte (info, (int) v);

  t = v;
  p = ab + sizeof ab;
  while (t != 0)
    {
      *--p = t & 0xff;
      t >>= 8;
    }
  c = (ab + 20) - p;

  if (c > (unsigned int) (ieee_number_repeat_end_enum
			  - ieee_number_repeat_start_enum))
    {
      fprintf (stderr, "IEEE numeric overflow: 0x");
      fprintf_vma (stderr, v);
      fprintf (stderr, "\n");
      return false;
    }

  if (! ieee_write_byte (info, (int) ieee_number_repeat_start_enum + c))
    return false;
  for (; c > 0; --c, ++p)
    {
      if (! ieee_write_byte (info, *p))
	return false;
    }

  return true;
}

/* Write out a string.  */

static boolean
ieee_write_id (info, s)
     struct ieee_handle *info;
     const char *s;
{
  unsigned int len;

  len = strlen (s);
  if (len <= 0x7f)
    {
      if (! ieee_write_byte (info, len))
	return false;
    }
  else if (len <= 0xff)
    {
      if (! ieee_write_byte (info, (int) ieee_extension_length_1_enum)
	  || ! ieee_write_byte (info, len))
	return false;
    }
  else if (len <= 0xffff)
    {
      if (! ieee_write_byte (info, (int) ieee_extension_length_2_enum)
	  || ! ieee_write_2bytes (info, len))
	return false;
    }
  else
    {
      fprintf (stderr, "IEEE string length overflow: %u\n", len);
      return false;
    }

  for (; *s != '\0'; s++)
    if (! ieee_write_byte (info, *s))
      return false;

  return true;
}

/* Start defining a type.  */

static boolean
ieee_define_type (info, size, unsignedp)
     struct ieee_handle *info;
     unsigned int size;
     boolean unsignedp;
{
  return ieee_define_named_type (info, (const char *) NULL, false, size,
				 unsignedp, (struct ieee_buf **) NULL);
}

/* Start defining a named type.  */

static boolean
ieee_define_named_type (info, name, tagp, size, unsignedp, ppbuf)
     struct ieee_handle *info;
     const char *name;
     boolean tagp;
     unsigned int size;
     boolean unsignedp;
     struct ieee_buf **ppbuf;
{
  unsigned int type_indx;
  unsigned int name_indx;

  if (! tagp || name == NULL || *name == '\0')
    {
      type_indx = info->type_indx;
      ++info->type_indx;
    }
  else
    {
      struct ieee_name_type *nt;

      /* The name is a tag.  If we have already defined the tag, we
         must use the existing type index.  */
      for (nt = info->tags; nt != NULL; nt = nt->next)
	if (nt->name[0] == name[0]
	    && strcmp (nt->name, name) == 0)
	  break;

      if (nt == NULL)
	{
	  nt = (struct ieee_name_type *) xmalloc (sizeof *nt);
	  memset (nt, 0, sizeof *nt);
	  nt->name = name;
	  nt->next = info->tags;
	  info->tags = nt;
	  nt->type.indx = info->type_indx;
	  ++info->type_indx;
	}

      nt->type.size = size;
      nt->type.unsignedp = unsignedp;
      nt->kind = DEBUG_KIND_ILLEGAL;

      type_indx = nt->type.indx;
    }

  name_indx = info->name_indx;
  ++info->name_indx;

  if (name == NULL)
    name = "";

  /* If we were given a buffer, use it; otherwise, use the general
     type information, and make sure that the type block is started.  */
  if (ppbuf != NULL)
    {
      if (! ieee_change_buffer (info, ppbuf))
	return false;
    }
  else if (info->types != NULL)
    {
      if (! ieee_change_buffer (info, &info->types))
	return false;
    }
  else
    {
      if (! ieee_change_buffer (info, &info->types)
	  || ! ieee_write_byte (info, (int) ieee_bb_record_enum)
	  || ! ieee_write_byte (info, 1)
	  || ! ieee_write_number (info, 0)
	  || ! ieee_write_id (info, info->modname))
	return false;
    }

  /* Push the new type on the type stack, write out an NN record, and
     write out the start of a TY record.  The caller will then finish
     the TY record.  */
  return (ieee_push_type (info, type_indx, size, unsignedp)
	  && ieee_write_byte (info, (int) ieee_nn_record)
	  && ieee_write_number (info, name_indx)
	  && ieee_write_id (info, name)
	  && ieee_write_byte (info, (int) ieee_ty_record_enum)
	  && ieee_write_number (info, type_indx)
	  && ieee_write_byte (info, 0xce)
	  && ieee_write_number (info, name_indx));
}

/* The general routine to write out IEEE debugging information.  */

boolean
write_ieee_debugging_info (abfd, dhandle)
     bfd *abfd;
     PTR dhandle;
{
  struct ieee_handle info;
  struct ieee_buf *tags;
  struct ieee_name_type *nt;
  asection *s;
  const char *err;
  struct ieee_buf *b;

  memset (&info, 0, sizeof info);
  info.abfd = abfd;
  info.type_indx = 256;
  info.name_indx = 32;

  if (! debug_write (dhandle, &ieee_fns, (PTR) &info))
    return false;

  if (info.filename != NULL)
    {
      if (! ieee_finish_compilation_unit (&info))
	return false;
    }

  /* Put any undefined tags in the global typedef information.  */
  tags = NULL;
  for (nt = info.tags; nt != NULL; nt = nt->next)
    {
      unsigned int name_indx;
      char code;

      if (nt->kind == DEBUG_KIND_ILLEGAL)
	continue;
      if (tags == NULL)
	{
	  if (! ieee_change_buffer (&info, &tags)
	      || ! ieee_write_byte (&info, (int) ieee_bb_record_enum)
	      || ! ieee_write_byte (&info, 2)
	      || ! ieee_write_number (&info, 0)
	      || ! ieee_write_id (&info, ""))
	    return false;
	}
      name_indx = info.name_indx;
      ++info.name_indx;
      if (! ieee_write_byte (&info, (int) ieee_nn_record)
	  || ! ieee_write_number (&info, name_indx)
	  || ! ieee_write_id (&info, nt->name)
	  || ! ieee_write_byte (&info, (int) ieee_ty_record_enum)
	  || ! ieee_write_number (&info, nt->type.indx)
	  || ! ieee_write_byte (&info, 0xce)
	  || ! ieee_write_number (&info, name_indx))
	return false;
      switch (nt->kind)
	{
	default:
	  abort ();
	  return false;
	case DEBUG_KIND_STRUCT:
	case DEBUG_KIND_CLASS:
	  code = 'S';
	  break;
	case DEBUG_KIND_UNION:
	case DEBUG_KIND_UNION_CLASS:
	  code = 'U';
	  break;
	case DEBUG_KIND_ENUM:
	  code = 'E';
	  break;
	}
      if (! ieee_write_number (&info, code)
	  || ! ieee_write_number (&info, 0))
	return false;
    }
  if (tags != NULL)
    {
      struct ieee_buf **pb;

      if (! ieee_write_byte (&info, (int) ieee_be_record_enum))
	return false;

      for (pb = &tags; *pb != NULL; pb = &(*pb)->next)
	;
      *pb = info.data;
      info.data = tags;
    }

  /* Now all the data is in info.data.  Write it out to the BFD.  We
     normally would need to worry about whether all the other sections
     are set up yet, but the IEEE backend will handle this particular
     case correctly regardless.  */
  if (info.data == NULL)
    {
      /* There is no debugging information.  */
      return true;
    }
  err = NULL;
  s = bfd_make_section (abfd, ".debug");
  if (s == NULL)
    err = "bfd_make_section";
  if (err == NULL)
    {
      if (! bfd_set_section_flags (abfd, s, SEC_DEBUGGING | SEC_HAS_CONTENTS))
	err = "bfd_set_section_flags";
    }
  if (err == NULL)
    {
      bfd_size_type size;

      size = 0;
      for (b = info.data; b != NULL; b = b->next)
	size += b->c;
      if (! bfd_set_section_size (abfd, s, size))
	err = "bfd_set_section_size";
    }
  if (err == NULL)
    {
      file_ptr offset;

      offset = 0;
      for (b = info.data; b != NULL; b = b->next)
	{
	  if (! bfd_set_section_contents (abfd, s, b->buf, offset, b->c))
	    {
	      err = "bfd_set_section_contents";
	      break;
	    }
	  offset += b->c;
	}
    }

  if (err != NULL)
    {
      fprintf (stderr, "%s: %s: %s\n", bfd_get_filename (abfd), err,
	       bfd_errmsg (bfd_get_error ()));
      return false;
    }

  return true;
}

/* Start writing out information for a compilation unit.  */

static boolean
ieee_start_compilation_unit (p, filename)
     PTR p;
     const char *filename;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  const char *modname;
  char *c, *s;

  if (info->filename != NULL)
    {
      if (! ieee_finish_compilation_unit (info))
	return false;
    }

  info->filename = filename;
  modname = strrchr (filename, '/');
  if (modname != NULL)
    ++modname;
  else
    {
      modname = strrchr (filename, '\\');
      if (modname != NULL)
	++modname;
      else
	modname = filename;
    }
  c = xstrdup (modname);
  s = strrchr (c, '.');
  if (s != NULL)
    *s = '\0';
  info->modname = c;

  info->types = NULL;
  info->vars = NULL;
  info->linenos = NULL;
  info->ranges = NULL;

  return true;
}

/* Finish up a compilation unit.  */

static boolean
ieee_finish_compilation_unit (info)
     struct ieee_handle *info;
{
  struct ieee_buf **pp;
  struct ieee_range *r;

  if (info->types != NULL)
    {
      if (! ieee_change_buffer (info, &info->types)
	  || ! ieee_write_byte (info, (int) ieee_be_record_enum))
	return false;
    }

  if (info->vars != NULL)
    {
      if (! ieee_change_buffer (info, &info->vars)
	  || ! ieee_write_byte (info, (int) ieee_be_record_enum))
	return false;
    }

  if (info->linenos != NULL)
    {
      if (! ieee_change_buffer (info, &info->linenos)
	  || ! ieee_write_byte (info, (int) ieee_be_record_enum))
	return false;
    }

  for (pp = &info->data; *pp != NULL; pp = &(*pp)->next)
    ;
  *pp = info->types;
  for (; *pp != NULL; pp = &(*pp)->next)
    ;
  *pp = info->vars;
  for (; *pp != NULL; pp = &(*pp)->next)
    ;
  *pp = info->linenos;

  /* Build BB10/BB11 blocks based on the ranges we recorded.  */
  if (! ieee_change_buffer (info, &info->data))
    return false;

  if (! ieee_write_byte (info, (int) ieee_bb_record_enum)
      || ! ieee_write_byte (info, 10)
      || ! ieee_write_number (info, 0)
      || ! ieee_write_id (info, info->modname)
      || ! ieee_write_id (info, "")
      || ! ieee_write_number (info, 0)
      || ! ieee_write_id (info, "GNU objcopy"))
    return false;

  for (r = info->ranges; r != NULL; r = r->next)
    {
      bfd_vma low, high;
      asection *s;
      int kind;

      low = r->low;
      high = r->high;

      /* Find the section corresponding to this range.  */
      for (s = info->abfd->sections; s != NULL; s = s->next)
	{
	  if (bfd_get_section_vma (info->abfd, s) <= low
	      && high <= (bfd_get_section_vma (info->abfd, s)
			  + bfd_section_size (info->abfd, s)))
	    break;
	}

      if (s == NULL)
	{
	  /* Just ignore this range.  */
	  continue;
	}

      /* Coalesce ranges if it seems reasonable.  */
      while (r->next != NULL
	     && high + 64 >= r->next->low
	     && (r->next->high
		 <= (bfd_get_section_vma (info->abfd, s)
		     + bfd_section_size (info->abfd, s))))
	{
	  r = r->next;
	  high = r->next->high;
	}

      if ((s->flags & SEC_CODE) != 0)
	kind = 1;
      else if ((s->flags & SEC_READONLY) != 0)
	kind = 3;
      else
	kind = 2;

      if (! ieee_write_byte (info, (int) ieee_bb_record_enum)
	  || ! ieee_write_byte (info, 11)
	  || ! ieee_write_number (info, 0)
	  || ! ieee_write_id (info, "")
	  || ! ieee_write_number (info, kind)
	  || ! ieee_write_number (info, s->index)
	  || ! ieee_write_number (info, low)
	  || ! ieee_write_byte (info, (int) ieee_be_record_enum)
	  || ! ieee_write_number (info, high - low))
	return false;
    }

  if (! ieee_write_byte (info, (int) ieee_be_record_enum))
    return false;

  return true;
}

/* Start recording information from a particular source file.  This is
   used to record which file defined which types, variables, etc.  It
   is not used for line numbers, since the lineno entry point passes
   down the file name anyhow.  IEEE debugging information doesn't seem
   to store this information anywhere.  */

/*ARGSUSED*/
static boolean
ieee_start_source (p, filename)
     PTR p;
     const char *filename;
{
  return true;
}

/* Make an empty type.  */

static boolean
ieee_empty_type (p)
     PTR p;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  return ieee_push_type (info, 0, 0, false);
}

/* Make a void type.  */

static boolean
ieee_void_type (p)
     PTR p;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  return ieee_push_type (info, 1, 0, false);
}

/* Make an integer type.  */

static boolean
ieee_int_type (p, size, unsignedp)
     PTR p;
     unsigned int size;
     boolean unsignedp;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  unsigned int indx;

  switch (size)
    {
    case 1:
      indx = (int) builtin_signed_char;
      break;
    case 2:
      indx = (int) builtin_signed_short_int;
      break;
    case 4:
      indx = (int) builtin_signed_long;
      break;
    case 8:
      indx = (int) builtin_signed_long_long;
      break;
    default:
      fprintf (stderr, "IEEE unsupported integer type size %u\n", size);
      return false;
    }

  if (unsignedp)
    ++indx;

  return ieee_push_type (info, indx, size, unsignedp);
}

/* Make a floating point type.  */

static boolean
ieee_float_type (p, size)
     PTR p;
     unsigned int size;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  unsigned int indx;

  switch (size)
    {
    case 4:
      indx = (int) builtin_float;
      break;
    case 8:
      indx = (int) builtin_double;
      break;
    case 12:
      /* FIXME: This size really depends upon the processor.  */
      indx = (int) builtin_long_double;
      break;
    case 16:
      indx = (int) builtin_long_long_double;
      break;
    default:
      fprintf (stderr, "IEEE unsupported float type size %u\n", size);
      return false;
    }

  return ieee_push_type (info, indx, size, false);
}

/* Make a complex type.  */

static boolean
ieee_complex_type (p, size)
     PTR p;
     unsigned int size;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  char code;

  switch (size)
    {
    case 4:
      code = 'c';
      break;
    case 8:
      code = 'd';
      break;
    default:
      fprintf (stderr, "IEEE unsupported complex type size %u\n", size);
      return false;
    }

  /* FIXME: I don't know what the string is for.  */
  return (ieee_define_type (info, size, false)
	  && ieee_write_number (info, code)
	  && ieee_write_id (info, ""));
}

/* Make a boolean type.  IEEE doesn't support these, so we just make
   an integer type instead.  */

static boolean
ieee_bool_type (p, size)
     PTR p;
     unsigned int size;
{
  return ieee_int_type (p, size, true);
}

/* Make an enumeration.  */

static boolean
ieee_enum_type (p, tag, names, vals)
     PTR p;
     const char *tag;
     const char **names;
     bfd_signed_vma *vals;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  boolean simple;
  int i;

  /* If this is a simple enumeration, in which the values start at 0
     and always increment by 1, we can use type E.  Otherwise we must
     use type N.  */

  simple = true;
  if (names != NULL)
    {
      for (i = 0; names[i] != NULL; i++)
	{
	  if (vals[i] != i)
	    {
	      simple = false;
	      break;
	    }
	}
    }

  if (! ieee_define_named_type (info, tag, true, 0, true,
				(struct ieee_buf **) NULL)
      || ! ieee_write_number (info, simple ? 'E' : 'N'))
    return false;
  if (simple)
    {
      /* FIXME: This is supposed to be the enumeration size, but we
         don't store that.  */
      if (! ieee_write_number (info, 4))
	return false;
    }
  if (names != NULL)
    {
      for (i = 0; names[i] != NULL; i++)
	{
	  if (! ieee_write_id (info, names[i]))
	    return false;
	  if (! simple)
	    {
	      if (! ieee_write_number (info, vals[i]))
		return false;
	    }
	}
    }

  return true;
}

/* Make a pointer type.  */

static boolean
ieee_pointer_type (p)
     PTR p;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  unsigned int indx;

  indx = ieee_pop_type (info);

  /* A pointer to a simple builtin type can be obtained by adding 32.  */
  if (indx < 32)
    return ieee_push_type (info, indx + 32, 0, true);

  return (ieee_define_type (info, 0, true)
	  && ieee_write_number (info, 'P')
	  && ieee_write_number (info, indx));
}

/* Make a function type.  */

static boolean
ieee_function_type (p, argcount, varargs)
     PTR p;
     int argcount;
     boolean varargs;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  unsigned int *args = NULL;
  int i;
  unsigned int retindx;

  if (argcount > 0)
    {
      args = (unsigned int *) xmalloc (argcount * sizeof *args);
      for (i = argcount - 1; i >= 0; i--)
	args[i] = ieee_pop_type (info);
    }
  else if (argcount < 0)
    varargs = false;

  retindx = ieee_pop_type (info);

  /* An attribute of 0x41 means that the frame and push mask are
     unknown.  */
  if (! ieee_define_type (info, 0, true)
      || ! ieee_write_number (info, 'x')
      || ! ieee_write_number (info, 0x41)
      || ! ieee_write_number (info, 0)
      || ! ieee_write_number (info, 0)
      || ! ieee_write_number (info, retindx)
      || ! ieee_write_number (info, (bfd_vma) argcount + (varargs ? 1 : 0)))
    return false;
  if (argcount > 0)
    {
      for (i = 0; i < argcount; i++)
	if (! ieee_write_number (info, args[i]))
	  return false;
      free (args);
    }
  if (varargs)
    {
      /* A varargs function is represented by writing out the last
         argument as type void *, although this makes little sense.  */
      if (! ieee_write_number (info, (bfd_vma) builtin_void + 32))
	return false;
    }

  return ieee_write_number (info, 0);
}

/* Make a reference type.  */

static boolean
ieee_reference_type (p)
     PTR p;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  /* IEEE appears to record a normal pointer type, and then use a
     pmisc record to indicate that it is really a reference.  */

  if (! ieee_pointer_type (p))
    return false;
  info->type_stack->type.referencep = true;
  return true;
}

/* Make a range type.  */

static boolean
ieee_range_type (p, low, high)
     PTR p;
     bfd_signed_vma low;
     bfd_signed_vma high;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  unsigned int size;
  boolean unsignedp;

  size = info->type_stack->type.size;
  unsignedp = info->type_stack->type.unsignedp;
  (void) ieee_pop_type (info);
  return (ieee_define_type (info, size, unsignedp)
	  && ieee_write_number (info, 'R')
	  && ieee_write_number (info, (bfd_vma) low)
	  && ieee_write_number (info, (bfd_vma) high)
	  && ieee_write_number (info, unsignedp ? 0 : 1)
	  && ieee_write_number (info, size));
}

/* Make an array type.  */

/*ARGSUSED*/
static boolean
ieee_array_type (p, low, high, stringp)
     PTR p;
     bfd_signed_vma low;
     bfd_signed_vma high;
     boolean stringp;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  unsigned int eleindx;

  /* IEEE does not store the range, so we just ignore it.  */
  (void) ieee_pop_type (info);
  eleindx = ieee_pop_type (info);

  if (! ieee_define_type (info, 0, false)
      || ! ieee_write_number (info, low == 0 ? 'Z' : 'C')
      || ! ieee_write_number (info, eleindx))
    return false;
  if (low != 0)
    {
      if (! ieee_write_number (info, low))
	return false;
    }

  return ieee_write_number (info, high);
}

/* Make a set type.  */

static boolean
ieee_set_type (p, bitstringp)
     PTR p;
     boolean bitstringp;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  unsigned int eleindx;

  eleindx = ieee_pop_type (info);

  /* FIXME: We don't know the size, so we just use 4.  */

  return (ieee_define_type (info, 0, true)
	  && ieee_write_number (info, 's')
	  && ieee_write_number (info, 4)
	  && ieee_write_number (info, eleindx));
}

/* Make an offset type.  */

static boolean
ieee_offset_type (p)
     PTR p;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  unsigned int targetindx, baseindx;

  targetindx = ieee_pop_type (info);
  baseindx = ieee_pop_type (info);

  /* FIXME: The MRI C++ compiler does not appear to generate any
     useful type information about an offset type.  It just records a
     pointer to member as an integer.  The MRI/HP IEEE spec does
     describe a pmisc record which can be used for a pointer to
     member.  Unfortunately, it does not describe the target type,
     which seems pretty important.  I'm going to punt this for now.  */

  return ieee_int_type (p, 4, true);
}  

/* Make a method type.  */

static boolean
ieee_method_type (p, domain, argcount, varargs)
     PTR p;
     boolean domain;
     int argcount;
     boolean varargs;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  /* FIXME: The MRI/HP IEEE spec defines a pmisc record to use for a
     method, but the definition is incomplete.  We just output an 'x'
     type.  */

  if (domain)
    (void) ieee_pop_type (info);

  return ieee_function_type (p, argcount, varargs);
}

/* Make a const qualified type.  */

static boolean
ieee_const_type (p)
     PTR p;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  unsigned int size;
  boolean unsignedp;
  unsigned int indx;

  size = info->type_stack->type.size;
  unsignedp = info->type_stack->type.unsignedp;
  indx = ieee_pop_type (info);
  return (ieee_define_type (info, size, unsignedp)
	  && ieee_write_number (info, 'n')
	  && ieee_write_number (info, 1)
	  && ieee_write_number (info, indx));
}

/* Make a volatile qualified type.  */

static boolean
ieee_volatile_type (p)
     PTR p;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  unsigned int size;
  boolean unsignedp;
  unsigned int indx;

  size = info->type_stack->type.size;
  unsignedp = info->type_stack->type.unsignedp;
  indx = ieee_pop_type (info);
  return (ieee_define_type (info, size, unsignedp)
	  && ieee_write_number (info, 'n')
	  && ieee_write_number (info, 2)
	  && ieee_write_number (info, indx));
}

/* Start defining a struct type.  We build it in the strdef field on
   the stack, to avoid confusing type definitions required by the
   fields with the struct type itself.  */

static boolean
ieee_start_struct_type (p, tag, id, structp, size)
     PTR p;
     const char *tag;
     unsigned int id;
     boolean structp;
     unsigned int size;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  struct ieee_buf *strdef;

  strdef = NULL;
  if (! ieee_define_named_type (info, tag, true, size, true, &strdef)
      || ! ieee_write_number (info, structp ? 'S' : 'U')
      || ! ieee_write_number (info, size))
    return false;

  info->type_stack->type.strdef = strdef;

  return true;
}

/* Add a field to a struct.  */

static boolean
ieee_struct_field (p, name, bitpos, bitsize, visibility)
     PTR p;
     const char *name;
     bfd_vma bitpos;
     bfd_vma bitsize;
     enum debug_visibility visibility;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  unsigned int size;
  boolean unsignedp;
  unsigned int indx;
  bfd_vma offset;

  size = info->type_stack->type.size;
  unsignedp = info->type_stack->type.unsignedp;
  indx = ieee_pop_type (info);

  assert (info->type_stack != NULL && info->type_stack->type.strdef != NULL);

  /* If the bitsize doesn't match the expected size, we need to output
     a bitfield type.  */
  if (size == 0 || bitsize == size * 8)
    offset = bitpos / 8;
  else
    {
      if (! ieee_define_type (info, 0, unsignedp)
	  || ! ieee_write_number (info, 'g')
	  || ! ieee_write_number (info, unsignedp ? 0 : 1)
	  || ! ieee_write_number (info, indx))
	return false;
      indx = ieee_pop_type (info);
      offset = bitpos;
    }

  /* Switch to the struct we are building in order to output this
     field definition.  */
  return (ieee_change_buffer (info, &info->type_stack->type.strdef)
	  && ieee_write_id (info, name)
	  && ieee_write_number (info, indx)
	  && ieee_write_number (info, offset));
}

/* Finish up a struct type.  */

static boolean
ieee_end_struct_type (p)
     PTR p;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  struct ieee_buf **pb;

  assert (info->type_stack != NULL && info->type_stack->type.strdef != NULL);

  /* Make sure we have started the types block.  */
  if (info->types == NULL)
    {
      if (! ieee_change_buffer (info, &info->types)
	  || ! ieee_write_byte (info, (int) ieee_bb_record_enum)
	  || ! ieee_write_byte (info, 1)
	  || ! ieee_write_number (info, 0)
	  || ! ieee_write_id (info, info->modname))
	return false;
    }

  /* Append the struct definition to the types.  */
  for (pb = &info->types; *pb != NULL; pb = &(*pb)->next)
    ;
  *pb = info->type_stack->type.strdef;
  info->type_stack->type.strdef = NULL;

  /* Leave the struct on the type stack.  */

  return true;
}

/* Start a class type.  */

static boolean
ieee_start_class_type (p, tag, id, structp, size, vptr, ownvptr)
     PTR p;
     const char *tag;
     unsigned int id;
     boolean structp;
     unsigned int size;
     boolean vptr;
     boolean ownvptr;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  /* FIXME.  */
  if (vptr && ! ownvptr)
    (void) ieee_pop_type (info);
  return ieee_start_struct_type (p, tag, id, structp, size);
}

/* Add a static member to a class.  */

static boolean
ieee_class_static_member (p, name, physname, visibility)
     PTR p;
     const char *name;
     const char *physname;
     enum debug_visibility visibility;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  /* FIXME.  */
  (void) ieee_pop_type (info);
  return true;
}

/* Add a base class to a class.  */

static boolean
ieee_class_baseclass (p, bitpos, virtual, visibility)
     PTR p;
     bfd_vma bitpos;
     boolean virtual;
     enum debug_visibility visibility;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  /* FIXME.  */
  (void) ieee_pop_type (info);
  return true;
}

/* Start building a method for a class.  */

static boolean
ieee_class_start_method (p, name)
     PTR p;
     const char *name;
{
  /* FIXME.  */
  return true;
}

/* Define a new method variant.  */

static boolean
ieee_class_method_variant (p, physname, visibility, constp, volatilep,
			   voffset, context)
     PTR p;
     const char *physname;
     enum debug_visibility visibility;
     boolean constp;
     boolean volatilep;
     bfd_vma voffset;
     boolean context;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  /* FIXME.  */
  (void) ieee_pop_type (info);
  if (context)
    (void) ieee_pop_type (info);
  return true;
}

/* Define a new static method variant.  */

static boolean
ieee_class_static_method_variant (p, physname, visibility, constp, volatilep)
     PTR p;
     const char *physname;
     enum debug_visibility visibility;
     boolean constp;
     boolean volatilep;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  /* FIXME.  */
  (void) ieee_pop_type (info);
  return true;
}

/* Finish up a method.  */

static boolean
ieee_class_end_method (p)
     PTR p;
{
  /* FIXME.  */
  return true;
}

/* Finish up a class.  */

static boolean
ieee_end_class_type (p)
     PTR p;
{
  return ieee_end_struct_type (p);
}

/* Push a previously seen typedef onto the type stack.  */

static boolean
ieee_typedef_type (p, name)
     PTR p;
     const char *name;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  register struct ieee_name_type *nt;

  for (nt = info->typedefs; nt != NULL; nt = nt->next)
    {
      if (nt->name[0] == name[0]
	  && strcmp (nt->name, name) == 0)
	{
	  if (! ieee_push_type (info, nt->type.indx, nt->type.size,
				nt->type.unsignedp))
	    return false;
	  /* Copy over any other type information we may have.  */
	  info->type_stack->type = nt->type;
	  return true;
	}
    }

  abort ();
}

/* Push a tagged type onto the type stack.  */

static boolean
ieee_tag_type (p, name, id, kind)
     PTR p;
     const char *name;
     unsigned int id;
     enum debug_type_kind kind;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  register struct ieee_name_type *nt;

  if (name == NULL)
    return true;

  for (nt = info->tags; nt != NULL; nt = nt->next)
    {
      if (nt->name[0] == name[0]
	  && strcmp (nt->name, name) == 0)
	{
	  if (! ieee_push_type (info, nt->type.indx, nt->type.size,
				nt->type.unsignedp))
	    return false;
	  /* Copy over any other type information we may have.  */
	  info->type_stack->type = nt->type;
	  return true;
	}
    }

  nt = (struct ieee_name_type *) xmalloc (sizeof *nt);
  memset (nt, 0, sizeof *nt);

  nt->name = name;
  nt->type.indx = info->type_indx;
  ++info->type_indx;
  nt->kind = kind;

  nt->next = info->tags;
  info->tags = nt;

  return ieee_push_type (info, nt->type.indx, 0, false);
}

/* Output a typedef.  */

static boolean
ieee_typdef (p, name)
     PTR p;
     const char *name;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  struct ieee_name_type *nt;
  unsigned int size;
  boolean unsignedp;
  unsigned int indx;

  nt = (struct ieee_name_type *) xmalloc (sizeof *nt);
  memset (nt, 0, sizeof *nt);
  nt->name = name;
  nt->type = info->type_stack->type;
  nt->kind = DEBUG_KIND_ILLEGAL;

  nt->next = info->typedefs;
  info->typedefs = nt;

  size = info->type_stack->type.size;
  unsignedp = info->type_stack->type.unsignedp;
  indx = ieee_pop_type (info);

  /* If this is a simple builtin type using a builtin name, we don't
     want to output the typedef itself.  We also want to change the
     type index to correspond to the name being used.  We recognize
     names used in stabs debugging output even if they don't exactly
     correspond to the names used for the IEEE builtin types.  */
  if (indx <= (unsigned int) builtin_bcd_float)
    {
      boolean found;

      found = false;
      switch ((enum builtin_types) indx)
	{
	default:
	  break;

	case builtin_void:
	  if (strcmp (name, "void") == 0)
	    found = true;
	  break;

	case builtin_signed_char:
	case builtin_char:
	  if (strcmp (name, "signed char") == 0)
	    {
	      indx = (unsigned int) builtin_signed_char;
	      found = true;
	    }
	  else if (strcmp (name, "char") == 0)
	    {
	      indx = (unsigned int) builtin_char;
	      found = true;
	    }
	  break;

	case builtin_unsigned_char:
	  if (strcmp (name, "unsigned char") == 0)
	    found = true;
	  break;

	case builtin_signed_short_int:
	case builtin_short:
	case builtin_short_int:
	case builtin_signed_short:
	  if (strcmp (name, "signed short int") == 0)
	    {
	      indx = (unsigned int) builtin_signed_short_int;
	      found = true;
	    }
	  else if (strcmp (name, "short") == 0)
	    {
	      indx = (unsigned int) builtin_short;
	      found = true;
	    }
	  else if (strcmp (name, "short int") == 0)
	    {
	      indx = (unsigned int) builtin_short_int;
	      found = true;
	    }
	  else if (strcmp (name, "signed short") == 0)
	    {
	      indx = (unsigned int) builtin_signed_short;
	      found = true;
	    }
	  break;

	case builtin_unsigned_short_int:
	case builtin_unsigned_short:
	  if (strcmp (name, "unsigned short int") == 0
	      || strcmp (name, "short unsigned int") == 0)
	    {
	      indx = builtin_unsigned_short_int;
	      found = true;
	    }
	  else if (strcmp (name, "unsigned short") == 0)
	    {
	      indx = builtin_unsigned_short;
	      found = true;
	    }
	  break;

	case builtin_signed_long:
	case builtin_int: /* FIXME: Size depends upon architecture.  */
	case builtin_long:
	  if (strcmp (name, "signed long") == 0)
	    {
	      indx = builtin_signed_long;
	      found = true;
	    }
	  else if (strcmp (name, "int") == 0)
	    {
	      indx = builtin_int;
	      found = true;
	    }
	  else if (strcmp (name, "long") == 0
		   || strcmp (name, "long int") == 0)
	    {
	      indx = builtin_long;
	      found = true;
	    }
	  break;

	case builtin_unsigned_long:
	case builtin_unsigned: /* FIXME: Size depends upon architecture.  */
	case builtin_unsigned_int: /* FIXME: Like builtin_unsigned.  */
	  if (strcmp (name, "unsigned long") == 0
	      || strcmp (name, "long unsigned int") == 0)
	    {
	      indx = builtin_unsigned_long;
	      found = true;
	    }
	  else if (strcmp (name, "unsigned") == 0)
	    {
	      indx = builtin_unsigned;
	      found = true;
	    }
	  else if (strcmp (name, "unsigned int") == 0)
	    {
	      indx = builtin_unsigned_int;
	      found = true;
	    }
	  break;

	case builtin_signed_long_long:
	  if (strcmp (name, "signed long long") == 0
	      || strcmp (name, "long long int") == 0)
	    found = true;
	  break;

	case builtin_unsigned_long_long:
	  if (strcmp (name, "unsigned long long") == 0
	      || strcmp (name, "long long unsigned int") == 0)
	    found = true;
	  break;

	case builtin_float:
	  if (strcmp (name, "float") == 0)
	    found = true;
	  break;

	case builtin_double:
	  if (strcmp (name, "double") == 0)
	    found = true;
	  break;

	case builtin_long_double:
	  if (strcmp (name, "long double") == 0)
	    found = true;
	  break;

	case builtin_long_long_double:
	  if (strcmp (name, "long long double") == 0)
	    found = true;
	  break;
	}

      if (found)
	{
	  nt->type.indx = indx;
	  return true;
	}
    }

  if (! ieee_define_named_type (info, name, false, size, unsignedp,
				(struct ieee_buf **) NULL)
      || ! ieee_write_number (info, 'T')
      || ! ieee_write_number (info, indx))
    return false;

  /* Remove the type we just added to the type stack.  */
  (void) ieee_pop_type (info);

  return true;
}

/* Output a tag for a type.  We don't have to do anything here.  */

static boolean
ieee_tag (p, name)
     PTR p;
     const char *name;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  (void) ieee_pop_type (info);
  return true;
}

/* Output an integer constant.  */

static boolean
ieee_int_constant (p, name, val)
     PTR p;
     const char *name;
     bfd_vma val;
{
  /* FIXME.  */
  return true;
}

/* Output a floating point constant.  */

static boolean
ieee_float_constant (p, name, val)
     PTR p;
     const char *name;
     double val;
{
  /* FIXME.  */
  return true;
}

/* Output a typed constant.  */

static boolean
ieee_typed_constant (p, name, val)
     PTR p;
     const char *name;
     bfd_vma val;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  /* FIXME.  */
  (void) ieee_pop_type (info);
  return true;
}

/* Output a variable.  */

static boolean
ieee_variable (p, name, kind, val)
     PTR p;
     const char *name;
     enum debug_var_kind kind;
     bfd_vma val;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  unsigned int name_indx;
  unsigned int size;
  unsigned int type_indx;
  boolean asn;

  /* Make sure the variable section is started.  */
  if (info->vars != NULL)
    {
      if (! ieee_change_buffer (info, &info->vars))
	return false;
    }
  else
    {
      if (! ieee_change_buffer (info, &info->vars)
	  || ! ieee_write_byte (info, (int) ieee_bb_record_enum)
	  || ! ieee_write_byte (info, 3)
	  || ! ieee_write_number (info, 0)
	  || ! ieee_write_id (info, info->modname))
	return false;
    }

  name_indx = info->name_indx;
  ++info->name_indx;

  size = info->type_stack->type.size;
  type_indx = ieee_pop_type (info);

  /* Write out an NN and an ATN record for this variable.  */
  if (! ieee_write_byte (info, (int) ieee_nn_record)
      || ! ieee_write_number (info, name_indx)
      || ! ieee_write_id (info, name)
      || ! ieee_write_2bytes (info, (int) ieee_atn_record_enum)
      || ! ieee_write_number (info, name_indx)
      || ! ieee_write_number (info, type_indx))
    return false;
  switch (kind)
    {
    default:
      abort ();
      return false;
    case DEBUG_GLOBAL:
      if (! ieee_write_number (info, 8)
	  || ! ieee_add_range (info, val, val + size))
	return false;
      asn = true;
      break;
    case DEBUG_STATIC:
    case DEBUG_LOCAL_STATIC:
      if (! ieee_write_number (info, 3)
	  || ! ieee_add_range (info, val, val + size))
	return false;
      asn = true;
      break;
    case DEBUG_LOCAL:
      if (! ieee_write_number (info, 1)
	  || ! ieee_write_number (info, val))
	return false;
      asn = false;
      break;
    case DEBUG_REGISTER:
      if (! ieee_write_number (info, 2)
	  || ! ieee_write_number (info,
				  ieee_genreg_to_regno (info->abfd, val)))
	return false;
      asn = false;
      break;
    }

  if (asn)
    {
      if (! ieee_write_2bytes (info, (int) ieee_asn_record_enum)
	  || ! ieee_write_number (info, name_indx)
	  || ! ieee_write_number (info, val))
	return false;
    }

  return true;
}

/* Start outputting information for a function.  */

static boolean
ieee_start_function (p, name, global)
     PTR p;
     const char *name;
     boolean global;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  unsigned int indx;

  /* Make sure the variable section is started.  */
  if (info->vars != NULL)
    {
      if (! ieee_change_buffer (info, &info->vars))
	return false;
    }
  else
    {
      if (! ieee_change_buffer (info, &info->vars)
	  || ! ieee_write_byte (info, (int) ieee_bb_record_enum)
	  || ! ieee_write_byte (info, 3)
	  || ! ieee_write_number (info, 0)
	  || ! ieee_write_id (info, info->modname))
	return false;
    }

  indx = ieee_pop_type (info);

  /* The address is written out as the first block.  */

  ++info->block_depth;

  return (ieee_write_byte (info, (int) ieee_bb_record_enum)
	  && ieee_write_byte (info, global ? 4 : 6)
	  && ieee_write_number (info, 0)
	  && ieee_write_id (info, name)
	  && ieee_write_number (info, 0)
	  && ieee_write_number (info, indx));
}

/* Add a function parameter.  This will normally be called before the
   first block, so we postpone them until we see the block.  */

static boolean
ieee_function_parameter (p, name, kind, val)
     PTR p;
     const char *name;
     enum debug_parm_kind kind;
     bfd_vma val;
{
  struct ieee_handle *info = (struct ieee_handle *) p;
  struct ieee_pending_parm *m, **pm;

  assert (info->block_depth == 1);

  m = (struct ieee_pending_parm *) xmalloc (sizeof *m);
  memset (m, 0, sizeof *m);

  m->next = NULL;
  m->name = name;
  m->type = ieee_pop_type (info);
  m->kind = kind;
  m->val = val;

  for (pm = &info->pending_parms; *pm != NULL; pm = &(*pm)->next)
    ;
  *pm = m;

  return true;  
}

/* Output pending function parameters.  */

static boolean
ieee_output_pending_parms (info)
     struct ieee_handle *info;
{
  struct ieee_pending_parm *m;

  m = info->pending_parms;
  while (m != NULL)
    {
      struct ieee_pending_parm *next;
      enum debug_var_kind vkind;

      switch (m->kind)
	{
	default:
	  abort ();
	  return false;
	case DEBUG_PARM_STACK:
	case DEBUG_PARM_REFERENCE:
	  vkind = DEBUG_LOCAL;
	  break;
	case DEBUG_PARM_REG:
	case DEBUG_PARM_REF_REG:
	  vkind = DEBUG_REGISTER;
	  break;
	}

      if (! ieee_push_type (info, m->type, 0, false)
	  || ! ieee_variable ((PTR) info, m->name, vkind, m->val))
	return false;

      /* FIXME: We should output a pmisc note here for reference
         parameters.  */

      next = m->next;
      free (m);
      m = next;
    }
  info->pending_parms = NULL;

  return true;
}

/* Start a block.  If this is the first block, we output the address
   to finish the BB4 or BB6, and then output the function parameters.  */

static boolean
ieee_start_block (p, addr)
     PTR p;
     bfd_vma addr;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  if (! ieee_change_buffer (info, &info->vars))
    return false;

  if (info->block_depth == 1)
    {
      if (! ieee_write_number (info, addr)
	  || ! ieee_output_pending_parms (info))
	return false;
    }
  else
    {
      if (! ieee_write_byte (info, (int) ieee_bb_record_enum)
	  || ! ieee_write_byte (info, 6)
	  || ! ieee_write_byte (info, 0)
	  || ! ieee_write_id (info, "")
	  || ! ieee_write_number (info, 0)
	  || ! ieee_write_number (info, 0)
	  || ! ieee_write_number (info, addr))
	return false;
    }

  if (! ieee_start_range (info, addr))
    return false;

  ++info->block_depth;

  return true;
}

/* End a block.  */

static boolean
ieee_end_block (p, addr)
     PTR p;
     bfd_vma addr;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  if (! ieee_change_buffer (info, &info->vars)
      || ! ieee_write_byte (info, (int) ieee_be_record_enum)
      || ! ieee_write_number (info, addr))
    return false;

  if (! ieee_end_range (info, addr))
    return false;

  --info->block_depth;

  return true;
}

/* End a function.  */

static boolean
ieee_end_function (p)
     PTR p;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  assert (info->block_depth == 1);

  --info->block_depth;

  return true;
}

/* Record line number information.  */

static boolean
ieee_lineno (p, filename, lineno, addr)
     PTR p;
     const char *filename;
     unsigned long lineno;
     bfd_vma addr;
{
  struct ieee_handle *info = (struct ieee_handle *) p;

  assert (info->filename != NULL);

  /* Make sure we have a line number block.  */
  if (info->linenos != NULL)
    {
      if (! ieee_change_buffer (info, &info->linenos))
	return false;
    }
  else
    {
      info->lineno_name_indx = info->name_indx;
      ++info->name_indx;
      if (! ieee_change_buffer (info, &info->linenos)
	  || ! ieee_write_byte (info, (int) ieee_bb_record_enum)
	  || ! ieee_write_byte (info, 5)
	  || ! ieee_write_number (info, 0)
	  || ! ieee_write_id (info, info->filename)
	  || ! ieee_write_byte (info, (int) ieee_nn_record)
	  || ! ieee_write_number (info, info->lineno_name_indx)
	  || ! ieee_write_id (info, ""))
	return false;
      info->lineno_filename = info->filename;
    }

  if (strcmp (filename, info->lineno_filename) != 0)
    {
      if (strcmp (info->filename, info->lineno_filename) != 0)
	{
	  /* We were not in the main file.  Close the block for the
             included file.  */
	  if (! ieee_write_byte (info, (int) ieee_be_record_enum))
	    return false;
	}
      if (strcmp (info->filename, filename) != 0)
	{
	  /* We are not changing to the main file.  Open a block for
             the new included file.  */
	  if (! ieee_write_byte (info, (int) ieee_bb_record_enum)
	      || ! ieee_write_byte (info, 5)
	      || ! ieee_write_number (info, 0)
	      || ! ieee_write_id (info, filename))
	    return false;
	}
      info->lineno_filename = filename;
    }

  return (ieee_write_2bytes (info, (int) ieee_atn_record_enum)
	  && ieee_write_number (info, info->lineno_name_indx)
	  && ieee_write_number (info, 0)
	  && ieee_write_number (info, 7)
	  && ieee_write_number (info, lineno)
	  && ieee_write_number (info, 0)
	  && ieee_write_2bytes (info, (int) ieee_asn_record_enum)
	  && ieee_write_number (info, info->lineno_name_indx)
	  && ieee_write_number (info, addr));
}
