/* Helper routines for parsing XML using Expat.

   Copyright (C) 2006
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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "defs.h"

/* The contents of this file are only useful if XML support is
   available.  */
#ifdef HAVE_LIBEXPAT

#include "exceptions.h"
#include "xml-support.h"

#include "gdb_expat.h"
#include "gdb_string.h"

/* Returns the value of attribute ATTR from expat attribute list
   ATTRLIST.  If not found, throws an exception.  */

const XML_Char *
xml_get_required_attribute (const XML_Char **attrs,
			    const XML_Char *attr)
{
  const XML_Char **p;
  for (p = attrs; *p; p += 2)
    {
      const char *name = p[0];
      const char *val = p[1];

      if (strcmp (name, attr) == 0)
	return val;
    }
  throw_error (XML_PARSE_ERROR, _("Can't find attribute %s"), attr);
}

/* Parse a field VALSTR that we expect to contain an integer value.
   The integer is returned in *VALP.  The string is parsed with an
   equivalent to strtoul.

   Returns 0 for success, -1 for error.  */

static int
xml_parse_unsigned_integer (const char *valstr, ULONGEST *valp)
{
  const char *endptr;
  ULONGEST result;

  if (*valstr == '\0')
    return -1;

  result = strtoulst (valstr, &endptr, 0);
  if (*endptr != '\0')
    return -1;

  *valp = result;
  return 0;
}

/* Gets the value of an integer attribute named ATTR, if it's present.
   If the attribute is not found, or can't be parsed as integer,
   throws an exception.  */

ULONGEST
xml_get_integer_attribute (const XML_Char **attrs,
			   const XML_Char *attr)
{
  ULONGEST result;
  const XML_Char *value = xml_get_required_attribute (attrs, attr);

  if (xml_parse_unsigned_integer (value, &result) != 0)
    {
      throw_error (XML_PARSE_ERROR,
		   _("Can't convert value of attribute %s, %s, to integer"),
		   attr, value);
    }
  return result;
}

/* Obtains a value of attribute with enumerated type. In XML, enumerated
   attributes have string as a value, and in C, they are represented as
   values of enumerated type. This function maps the attribute onto
   an integer value that can be immediately converted into enumerated
   type.

   First, obtains the string value of ATTR in ATTRS.
   Then, finds the index of that value in XML_NAMES, which is a zero-terminated
   array of strings. If found, returns the element of VALUES with that index.
   Otherwise throws.  */

int
xml_get_enum_value (const XML_Char **attrs,
		    const XML_Char *attr,
		    const XML_Char **xml_names,
		    int *values)
{
  const XML_Char *value = xml_get_required_attribute (attrs, attr);

  int i;
  for (i = 0; xml_names[i]; ++i)
    {
      if (strcmp (xml_names[i], value) == 0)
	return values[i];
    }
  throw_error (XML_PARSE_ERROR,
	       _("Invalid enumerated value in XML: %s"), value);
}

/* Cleanup wrapper for XML_ParserFree, with the correct type
   for make_cleanup.  */

static void
free_xml_parser (void *parser)
{
  XML_ParserFree (parser);
}

/* Register a cleanup to release PARSER.  Only the parser itself
   is freed; another cleanup may be necessary to discard any
   associated user data.  */

void
make_cleanup_free_xml_parser (XML_Parser parser)
{
  make_cleanup (free_xml_parser, parser);
}

#endif /* HAVE_LIBEXPAT */
