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
static boolean ieee_read_type_index
  PARAMS ((PTR, bfd *, struct ieee_types *, const bfd_byte *,
	   const bfd_byte **, const bfd_byte *, debug_type *));
static int ieee_regno_to_gen PARAMS ((bfd *, int));
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
  boolean ptr;
  debug_type type;
  const char *name;

  if (indx < BUILTIN_TYPE_COUNT
      && types->builtins[indx] != DEBUG_TYPE_NULL)
    return types->builtins[indx];

  ptr = false;
  switch (indx)
    {
    default:
      ieee_error (abfd, bytes, p, "unknown builtin type");
      return NULL;

    case 32:
      ptr = true;
      /* Fall through.  */
    case 0:
      type = debug_make_void_type (dhandle);
      name = NULL;
      break;

    case 33:
      ptr = true;
      /* Fall through.  */
    case 1:
      type = debug_make_void_type (dhandle);
      name = "void";
      break;

    case 34:
      ptr = true;
      /* Fall through.  */
    case 2:
      type = debug_make_int_type (dhandle, 1, false);
      name = "signed char";
      break;

    case 35:
      ptr = true;
      /* Fall through.  */
    case 3:
      type = debug_make_int_type (dhandle, 1, true);
      name = "unsigned char";
      break;

    case 36:
      ptr = true;
      /* Fall through.  */
    case 4:
      type = debug_make_int_type (dhandle, 2, false);
      name = "signed short int";
      break;

    case 37:
      ptr = true;
      /* Fall through.  */
    case 5:
      type = debug_make_int_type (dhandle, 2, true);
      name = "unsigned short int";
      break;

    case 38:
      ptr = true;
      /* Fall through.  */
    case 6:
      type = debug_make_int_type (dhandle, 4, false);
      name = "signed long";
      break;

    case 39:
      ptr = true;
      /* Fall through.  */
    case 7:
      type = debug_make_int_type (dhandle, 4, true);
      name = "unsigned long";
      break;

    case 40:
      ptr = true;
      /* Fall through.  */
    case 8:
      type = debug_make_int_type (dhandle, 8, false);
      name = "signed long long";
      break;

    case 41:
      ptr = true;
      /* Fall through.  */
    case 9:
      type = debug_make_int_type (dhandle, 8, true);
      name = "unsigned long long";
      break;

    case 42:
      ptr = true;
      /* Fall through.  */
    case 10:
      type = debug_make_float_type (dhandle, 4);
      name = "float";
      break;

    case 43:
      ptr = true;
      /* Fall through.  */
    case 11:
      type = debug_make_float_type (dhandle, 8);
      name = "double";
      break;

    case 44:
      ptr = true;
      /* Fall through.  */
    case 12:
      /* FIXME: The size for this type should depend upon the
         processor.  */
      type = debug_make_float_type (dhandle, 12);
      name = "long double";
      break;

    case 45:
      ptr = true;
      /* Fall through.  */
    case 13:
      type = debug_make_float_type (dhandle, 16);
      name = "long long double";
      break;

    case 46:
      ptr = true;
      /* Fall through.  */
    case 14:
      type = debug_make_array_type (dhandle,
				    ieee_builtin_type (dhandle, abfd, types,
						       bytes, p, 19),
				    ieee_builtin_type (dhandle, abfd, types,
						       bytes, p, 16),
				    0, -1, true);
      name = "QUOTED STRING";
      break;

    case 47:
      ptr = true;
      /* Fall through.  */
    case 15:
      /* FIXME: This should be a code address.  */
      type = debug_make_int_type (dhandle, 4, true);
      name = "instruction address";
      break;

    case 48:
      ptr = true;
      /* Fall through.  */
    case 16:
      /* FIXME: The size for this type should depend upon the
         processor.  */
      type = debug_make_int_type (dhandle, 4, false);
      name = "int";
      break;

    case 49:
      ptr = true;
      /* Fall through.  */
    case 17:
      /* FIXME: The size for this type should depend upon the
         processor.  */
      type = debug_make_int_type (dhandle, 4, true);
      name = "unsigned";
      break;

    case 50:
      ptr = true;
      /* Fall through.  */
    case 18:
      /* FIXME: The size for this type should depend upon the
         processor.  */
      type = debug_make_int_type (dhandle, 4, true);
      name = "unsigned int";
      break;

    case 51:
      ptr = true;
      /* Fall through.  */
    case 19:
      type = debug_make_int_type (dhandle, 1, false);
      name = "char";
      break;

    case 52:
      ptr = true;
      /* Fall through.  */
    case 20:
      type = debug_make_int_type (dhandle, 4, false);
      name = "long";
      break;

    case 53:
      ptr = true;
      /* Fall through.  */
    case 21:
      type = debug_make_int_type (dhandle, 2, false);
      name = "short";
      break;

    case 54:
      ptr = true;
      /* Fall through.  */
    case 22:
      type = debug_make_int_type (dhandle, 2, true);
      name = "unsigned short";
      break;

    case 55:
      ptr = true;
      /* Fall through.  */
    case 23:
      type = debug_make_int_type (dhandle, 2, false);
      name = "short int";
      break;

    case 56:
      ptr = true;
      /* Fall through.  */
    case 24:
      type = debug_make_int_type (dhandle, 2, false);
      name = "signed short";
      break;

    case 57:
      ptr = true;
      /* Fall through.  */
    case 25:
      ieee_error (abfd, bytes, p, "BCD float type not supported");
      return false;
    }

  if (ptr)
    type = debug_make_pointer_type (dhandle, type);
  else if (name != NULL)
    type = debug_name_type (dhandle, name, type);

  assert (indx < BUILTIN_TYPE_COUNT);

  types->builtins[indx] = type;

  return type;
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
  if (indx >= types->alloc
      || types->types[indx].type == DEBUG_TYPE_NULL)
    {
      ieee_error (abfd, bytes, start, "undefined type");
      return false;
    }

  *ptype = types->types[indx].type;

  return true;
}

/* Convert a register number in IEEE debugging information into a
   generic register number.  */

static int
ieee_regno_to_gen (abfd, r)
     bfd *abfd;
     int r;
{
  return r;
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
	    if (typindx >= types->alloc
		|| types->types[typindx].type == DEBUG_TYPE_NULL)
	      {
		ieee_error (abfd, bytes, block_start, "undefined type index");
		return false;
	      }
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
		if (typindx >= types->alloc
		    || types->types[typindx].type == DEBUG_TYPE_NULL)
		  {
		    ieee_error (abfd, bytes, block_start,
				"undefined type index");
		    return false;
		  }
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

  if (typeindx >= types->alloc)
    {
      unsigned int nalloc;
      struct ieee_type *t, *tend;

      nalloc = types->alloc;
      if (nalloc == 0)
	nalloc = 4;
      while (typeindx >= nalloc)
	nalloc *= 2;
      types->types = ((struct ieee_type *)
		      xrealloc (types->types, nalloc * sizeof *types->types));
      tend = types->types + nalloc;
      for (t = types->types + types->alloc; t < tend; t++)
	{
	  t->bitsize = 0;
	  t->type = DEBUG_TYPE_NULL;
	}
      types->alloc = nalloc;
    }

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
				      debug_make_int_type (dhandle, 4, false),
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
		if (tindx >= types->alloc
		    || types->types[tindx].type == DEBUG_TYPE_NULL)
		  {
		    ieee_error (abfd, bytes, ty_start, "undefined type index");
		    return false;
		  }
		t = &types->types[tindx];
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

	type = debug_make_function_type (dhandle, rtype);
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
				      debug_make_int_type (dhandle, 4, false),
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
	boolean present;

	/* FIXME: We ignore almost all this information.  */

	if (! ieee_read_number (abfd, bytes, pp, pend, &attr)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &frame_type)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &push_mask)
	    || ! ieee_read_type_index (dhandle, abfd, types, bytes, pp, pend,
				       &rtype)
	    || ! ieee_read_number (abfd, bytes, pp, pend, &nargs))
	  return false;
	if (nargs != (bfd_vma) -1)
	  {
	    for (; nargs > 0; nargs--)
	      {
		debug_type atype;

		if (! ieee_read_type_index (dhandle, abfd, types, bytes, pp,
					    pend, &atype))
		  return false;
	      }
	  }
	if (! ieee_read_number (abfd, bytes, pp, pend, &level)
	    || ! ieee_read_optional_number (abfd, bytes, pp, pend, &father,
					    &present))
	  return false;

	type = debug_make_function_type (dhandle, rtype);
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
				    ieee_regno_to_gen (abfd, v));

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

      /* I don't know what this means.  FIXME.  */

      ieee_error (abfd, bytes, atn_code_start, "unsupported ATN10");

      /* Return true to keep going.  */
      return true;

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
