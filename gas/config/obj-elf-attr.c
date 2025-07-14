/* Object attributes parsing.
   Copyright (C) 2025 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#include "obj-elf-attr.h"

#ifdef TC_OBJ_ATTR

#include "safe-ctype.h"

#define skip_whitespace(str)  do { if (is_whitespace (*(str))) ++(str); } while (0)

static inline bool
skip_past_char (char **str, char c)
{
  if (**str == c)
    {
      (*str)++;
      return true;
    }
  return false;
}
#define skip_past_comma(str) skip_past_char (str, ',')

/* A list of attributes that have been explicitly set by the assembly code.
   VENDOR is the vendor id, BASE is the tag shifted right by the number
   of bits in MASK, and bit N of MASK is set if tag BASE+N has been set.  */
typedef struct recorded_attribute_info {
  struct recorded_attribute_info *next;
  obj_attr_vendor_t vendor;
  unsigned int base;
  unsigned long mask;
} recorded_attribute_info_t;
static recorded_attribute_info_t *recorded_attributes;

static void
oav1_attr_info_free (recorded_attribute_info_t *node)
{
  while (node != NULL)
    {
      recorded_attribute_info_t *next = node->next;
      free (node);
      node = next;
    }
}

void
oav1_attr_info_init ()
{
  /* Note: this "constructor" was added for symmetry with oav1_attr_info_exit.
     recorded_attributes is a static variable which is automatically initialized
     to NULL.  There is no need to initialize it another time except for a
     cosmetic reason and to possibly help fuzzing.  */
  recorded_attributes = NULL;
}

void
oav1_attr_info_exit ()
{
  oav1_attr_info_free (recorded_attributes);
}

/* Record that we have seen an explicit specification of attribute TAG
   for vendor VENDOR.  */

static void
oav1_attr_record_seen (obj_attr_vendor_t vendor, obj_attr_tag_t tag)
{
  unsigned int base;
  unsigned long mask;
  recorded_attribute_info_t *rai;

  base = tag / (8 * sizeof (rai->mask));
  mask = 1UL << (tag % (8 * sizeof (rai->mask)));
  for (rai = recorded_attributes; rai; rai = rai->next)
    if (rai->vendor == vendor && rai->base == base)
      {
	rai->mask |= mask;
	return;
      }

  rai = XNEW (recorded_attribute_info_t);
  rai->next = recorded_attributes;
  rai->vendor = vendor;
  rai->base = base;
  rai->mask = mask;
  recorded_attributes = rai;
}

/* Return true if we have seen an explicit specification of attribute TAG
   for vendor VENDOR.  */

bool
oav1_attr_seen (obj_attr_vendor_t vendor, obj_attr_tag_t tag)
{
  unsigned int base;
  unsigned long mask;
  recorded_attribute_info_t *rai;

  base = tag / (8 * sizeof (rai->mask));
  mask = 1UL << (tag % (8 * sizeof (rai->mask)));
  for (rai = recorded_attributes; rai; rai = rai->next)
    if (rai->vendor == vendor && rai->base == base)
      return (rai->mask & mask) != 0;
  return false;
}

/* Parse an attribute directive for VENDOR.
   Returns the attribute number read, or zero on error.  */

obj_attr_tag_t
obj_attr_v1_process_attribute (obj_attr_vendor_t vendor)
{
  expressionS exp;
  int type;
  int tag;
  unsigned int i = 0;
  char *s = NULL;

  /* Read the first number or name.  */
  skip_whitespace (input_line_pointer);
  s = input_line_pointer;
  if (ISDIGIT (*input_line_pointer))
    {
      expression (& exp);
      if (exp.X_op != O_constant)
	goto bad;
      tag = exp.X_add_number;
    }
  else
    {
      char *name;

      /* A name may contain '_', but no other punctuation.  */
      for (; ISALNUM (*input_line_pointer) || *input_line_pointer == '_';
	   ++input_line_pointer)
	i++;
      if (i == 0)
	goto bad;

      name = xmemdup0 (s, i);

#ifndef CONVERT_SYMBOLIC_ATTRIBUTE
#define CONVERT_SYMBOLIC_ATTRIBUTE(a) -1
#endif

      tag = CONVERT_SYMBOLIC_ATTRIBUTE (name);
      if (tag == -1)
	{
	  as_bad (_("Attribute name not recognised: %s"), name);
	  ignore_rest_of_line ();
	  free (name);
	  return 0;
	}
      free (name);
    }

  type = bfd_elf_obj_attrs_arg_type (stdoutput, vendor, tag);

  if (! skip_past_comma (&input_line_pointer))
    goto bad;
  if (type & 1)
    {
      expression (& exp);
      if (exp.X_op != O_constant)
	{
	  as_bad (_("expected numeric constant"));
	  ignore_rest_of_line ();
	  return 0;
	}
      i = exp.X_add_number;
    }
  if ((type & 3) == 3
      && ! skip_past_comma (&input_line_pointer))
    {
      as_bad (_("expected comma"));
      ignore_rest_of_line ();
      return 0;
    }
  if (type & 2)
    {
      int len;

      skip_whitespace (input_line_pointer);
      if (*input_line_pointer != '"')
	goto bad_string;
      s = demand_copy_C_string (&len);
    }

  oav1_attr_record_seen (vendor, tag);
  bool ok = false;
  switch (type & 3)
    {
    case 3:
      ok = bfd_elf_add_obj_attr_int_string (stdoutput, vendor, tag, i, s);
      break;
    case 2:
      ok = bfd_elf_add_obj_attr_string (stdoutput, vendor, tag, s);
      break;
    case 1:
      ok = bfd_elf_add_obj_attr_int (stdoutput, vendor, tag, i);
      break;
    default:
      abort ();
    }
  if (!ok)
    as_fatal (_("error adding attribute: %s"),
	      bfd_errmsg (bfd_get_error ()));

  demand_empty_rest_of_line ();
  return tag;
 bad_string:
  as_bad (_("bad string constant"));
  ignore_rest_of_line ();
  return 0;
 bad:
  as_bad (_("expected <tag> , <value>"));
  ignore_rest_of_line ();
  return 0;
}

/* Parse a .gnu_attribute directive.  */

void
obj_elf_gnu_attribute (int ignored ATTRIBUTE_UNUSED)
{
  obj_attr_v1_process_attribute (OBJ_ATTR_GNU);
}

#endif /* TC_OBJ_ATTR */
