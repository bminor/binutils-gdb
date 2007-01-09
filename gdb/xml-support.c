/* Helper routines for parsing XML using Expat.

   Copyright (C) 2006, 2007 Free Software Foundation, Inc.

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
#include "gdbcmd.h"

/* Debugging flag.  */
static int debug_xml;

/* The contents of this file are only useful if XML support is
   available.  */
#ifdef HAVE_LIBEXPAT

#include "exceptions.h"
#include "xml-support.h"

#include "gdb_expat.h"
#include "gdb_string.h"
#include "safe-ctype.h"

/* A parsing level -- used to keep track of the current element
   nesting.  */
struct scope_level
{
  /* Elements we allow at this level.  */
  const struct gdb_xml_element *elements;

  /* The element which we are within.  */
  const struct gdb_xml_element *element;

  /* Mask of which elements we've seen at this level (used for
     optional and repeatable checking).  */
  unsigned int seen;

  /* Body text accumulation.  */
  struct obstack *body;
};
typedef struct scope_level scope_level_s;
DEF_VEC_O(scope_level_s);

/* The parser itself, and our additional state.  */
struct gdb_xml_parser
{
  XML_Parser expat_parser;	/* The underlying expat parser.  */

  const char *name;		/* Name of this parser.  */
  void *user_data;		/* The user's callback data, for handlers.  */

  VEC(scope_level_s) *scopes;	/* Scoping stack.  */

  struct gdb_exception error;	/* A thrown error, if any.  */
  int last_line;		/* The line of the thrown error, or 0.  */
};

/* Process some body text.  We accumulate the text for later use; it's
   wrong to do anything with it immediately, because a single block of
   text might be broken up into multiple calls to this function.  */

static void
gdb_xml_body_text (void *data, const XML_Char *text, int length)
{
  struct gdb_xml_parser *parser = data;
  struct scope_level *scope = VEC_last (scope_level_s, parser->scopes);

  if (parser->error.reason < 0)
    return;

  if (scope->body == NULL)
    {
      scope->body = XZALLOC (struct obstack);
      obstack_init (scope->body);
    }

  obstack_grow (scope->body, text, length);
}

/* Issue a debugging message from one of PARSER's handlers.  */

void
gdb_xml_debug (struct gdb_xml_parser *parser, const char *format, ...)
{
  int line = XML_GetCurrentLineNumber (parser->expat_parser);
  va_list ap;
  char *message;

  if (!debug_xml)
    return;

  va_start (ap, format);
  message = xstrvprintf (format, ap);
  if (line)
    fprintf_unfiltered (gdb_stderr, "%s (line %d): %s\n",
			parser->name, line, message);
  else
    fprintf_unfiltered (gdb_stderr, "%s: %s\n",
			parser->name, message);
  xfree (message);
}

/* Issue an error message from one of PARSER's handlers, and stop
   parsing.  */

void
gdb_xml_error (struct gdb_xml_parser *parser, const char *format, ...)
{
  int line = XML_GetCurrentLineNumber (parser->expat_parser);
  va_list ap;

  parser->last_line = line;
  va_start (ap, format);
  throw_verror (XML_PARSE_ERROR, format, ap);
}

/* Clean up a vector of parsed attribute values.  */

static void
gdb_xml_values_cleanup (void *data)
{
  VEC(gdb_xml_value_s) **values = data;
  struct gdb_xml_value *value;
  int ix;

  for (ix = 0; VEC_iterate (gdb_xml_value_s, *values, ix, value); ix++)
    xfree (value->value);
  VEC_free (gdb_xml_value_s, *values);
}

/* Handle the start of an element.  DATA is our local XML parser, NAME
   is the element, and ATTRS are the names and values of this
   element's attributes.  */

static void
gdb_xml_start_element (void *data, const XML_Char *name,
		       const XML_Char **attrs)
{
  struct gdb_xml_parser *parser = data;
  struct scope_level *scope = VEC_last (scope_level_s, parser->scopes);
  struct scope_level new_scope;
  const struct gdb_xml_element *element;
  const struct gdb_xml_attribute *attribute;
  VEC(gdb_xml_value_s) *attributes = NULL;
  unsigned int seen;
  struct cleanup *back_to;

  back_to = make_cleanup (gdb_xml_values_cleanup, &attributes);

  /* Push an error scope.  If we return or throw an exception before
     filling this in, it will tell us to ignore children of this
     element.  */
  memset (&new_scope, 0, sizeof (new_scope));
  VEC_safe_push (scope_level_s, parser->scopes, &new_scope);

  gdb_xml_debug (parser, _("Entering element <%s>"), name);

  /* Find this element in the list of the current scope's allowed
     children.  Record that we've seen it.  */

  seen = 1;
  for (element = scope->elements; element && element->name;
       element++, seen <<= 1)
    if (strcmp (element->name, name) == 0)
      break;

  if (element == NULL || element->name == NULL)
    {
      gdb_xml_debug (parser, _("Element <%s> unknown"), name);
      do_cleanups (back_to);
      return;
    }

  if (!(element->flags & GDB_XML_EF_REPEATABLE) && (seen & scope->seen))
    gdb_xml_error (parser, _("Element <%s> only expected once"), name);

  scope->seen |= seen;

  for (attribute = element->attributes;
       attribute != NULL && attribute->name != NULL;
       attribute++)
    {
      const char *val = NULL;
      const XML_Char **p;
      void *parsed_value;
      struct gdb_xml_value new_value;

      for (p = attrs; *p != NULL; p += 2)
	if (!strcmp (attribute->name, p[0]))
	  {
	    val = p[1];
	    break;
	  }

      if (*p != NULL && val == NULL)
	{
	  gdb_xml_debug (parser, _("Attribute \"%s\" missing a value"),
			 attribute->name);
	  continue;
	}

      if (*p == NULL && !(attribute->flags & GDB_XML_AF_OPTIONAL))
	{
	  gdb_xml_error (parser, _("Required attribute \"%s\" of "
				   "<%s> not specified"),
			 attribute->name, element->name);
	  continue;
	}

      if (*p == NULL)
	continue;

      gdb_xml_debug (parser, _("Parsing attribute %s=\"%s\""),
		     attribute->name, val);

      if (attribute->handler)
	parsed_value = attribute->handler (parser, attribute, val);
      else
	parsed_value = xstrdup (val);

      new_value.name = attribute->name;
      new_value.value = parsed_value;
      VEC_safe_push (gdb_xml_value_s, attributes, &new_value);
    }

  /* Check for unrecognized attributes.  */
  if (debug_xml)
    {
      const XML_Char **p;

      for (p = attrs; *p != NULL; p += 2)
	{
	  for (attribute = element->attributes;
	       attribute != NULL && attribute->name != NULL;
	       attribute++)
	    if (strcmp (attribute->name, *p) == 0)
	      break;

	  if (attribute == NULL || attribute->name == NULL)
	    gdb_xml_debug (parser, _("Ignoring unknown attribute %s"), *p);
	}
    }

  /* Call the element handler if there is one.  */
  if (element->start_handler)
    element->start_handler (parser, element, parser->user_data, attributes);

  /* Fill in a new scope level.  */
  scope = VEC_last (scope_level_s, parser->scopes);
  scope->element = element;
  scope->elements = element->children;

  do_cleanups (back_to);
}

/* Wrapper for gdb_xml_start_element, to prevent throwing exceptions
   through expat.  */

static void
gdb_xml_start_element_wrapper (void *data, const XML_Char *name,
			       const XML_Char **attrs)
{
  struct gdb_xml_parser *parser = data;
  volatile struct gdb_exception ex;

  if (parser->error.reason < 0)
    return;

  TRY_CATCH (ex, RETURN_MASK_ALL)
    {
      gdb_xml_start_element (data, name, attrs);
    }
  if (ex.reason < 0)
    {
      parser->error = ex;
#ifdef HAVE_XML_STOPPARSER
      XML_StopParser (parser->expat_parser, XML_FALSE);
#endif
    }
}

/* Handle the end of an element.  DATA is our local XML parser, and
   NAME is the current element.  */

static void
gdb_xml_end_element (void *data, const XML_Char *name)
{
  struct gdb_xml_parser *parser = data;
  struct scope_level *scope = VEC_last (scope_level_s, parser->scopes);
  const struct gdb_xml_element *element;
  unsigned int seen;
  char *body;

  gdb_xml_debug (parser, _("Leaving element <%s>"), name);

  for (element = scope->elements, seen = 1;
       element != NULL && element->name != NULL;
       element++, seen <<= 1)
    if ((scope->seen & seen) == 0
	&& (element->flags & GDB_XML_EF_OPTIONAL) == 0)
      gdb_xml_error (parser, _("Required element <%s> is missing"),
		     element->name);

  /* Call the element processor. */
  if (scope->body == NULL)
    body = "";
  else
    {
      int length;

      length = obstack_object_size (scope->body);
      obstack_1grow (scope->body, '\0');
      body = obstack_finish (scope->body);

      /* Strip leading and trailing whitespace.  */
      while (length > 0 && ISSPACE (body[length-1]))
	body[--length] = '\0';
      while (*body && ISSPACE (*body))
	body++;
    }

  if (scope->element != NULL && scope->element->end_handler)
    scope->element->end_handler (parser, scope->element, parser->user_data,
				 body);

  /* Pop the scope level.  */
  if (scope->body)
    {
      obstack_free (scope->body, NULL);
      xfree (scope->body);
    }
  VEC_pop (scope_level_s, parser->scopes);
}

/* Wrapper for gdb_xml_end_element, to prevent throwing exceptions
   through expat.  */

static void
gdb_xml_end_element_wrapper (void *data, const XML_Char *name)
{
  struct gdb_xml_parser *parser = data;
  volatile struct gdb_exception ex;

  if (parser->error.reason < 0)
    return;

  TRY_CATCH (ex, RETURN_MASK_ALL)
    {
      gdb_xml_end_element (data, name);
    }
  if (ex.reason < 0)
    {
      parser->error = ex;
#ifdef HAVE_XML_STOPPARSER
      XML_StopParser (parser->expat_parser, XML_FALSE);
#endif
    }
}

/* Free a parser and all its associated state.  */

static void
gdb_xml_cleanup (void *arg)
{
  struct gdb_xml_parser *parser = arg;
  struct scope_level *scope;
  int ix;

  XML_ParserFree (parser->expat_parser);

  /* Clean up the scopes.  */
  for (ix = 0; VEC_iterate (scope_level_s, parser->scopes, ix, scope); ix++)
    if (scope->body)
      {
	obstack_free (scope->body, NULL);
	xfree (scope->body);
      }
  VEC_free (scope_level_s, parser->scopes);

  xfree (parser);
}

/* Initialize and return a parser.  Register a cleanup to destroy the
   parser.  */

struct gdb_xml_parser *
gdb_xml_create_parser_and_cleanup (const char *name,
				   const struct gdb_xml_element *elements,
				   void *user_data)
{
  struct gdb_xml_parser *parser;
  struct scope_level start_scope;

  /* Initialize the parser.  */
  parser = XZALLOC (struct gdb_xml_parser);
  parser->expat_parser = XML_ParserCreateNS (NULL, '!');
  if (parser->expat_parser == NULL)
    {
      xfree (parser);
      nomem (0);
    }

  parser->name = name;

  parser->user_data = user_data;
  XML_SetUserData (parser->expat_parser, parser);

  /* Set the callbacks.  */
  XML_SetElementHandler (parser->expat_parser, gdb_xml_start_element_wrapper,
			 gdb_xml_end_element_wrapper);
  XML_SetCharacterDataHandler (parser->expat_parser, gdb_xml_body_text);

  /* Initialize the outer scope.  */
  memset (&start_scope, 0, sizeof (start_scope));
  start_scope.elements = elements;
  VEC_safe_push (scope_level_s, parser->scopes, &start_scope);

  make_cleanup (gdb_xml_cleanup, parser);

  return parser;
}

/* Invoke PARSER on BUFFER.  BUFFER is the data to parse, which
   should be NUL-terminated.

   The return value is 0 for success or -1 for error.  It may throw,
   but only if something unexpected goes wrong during parsing; parse
   errors will be caught, warned about, and reported as failure.  */

int
gdb_xml_parse (struct gdb_xml_parser *parser, const char *buffer)
{
  enum XML_Status status;
  const char *error_string;

  status = XML_Parse (parser->expat_parser, buffer, strlen (buffer), 1);

  if (status == XML_STATUS_OK && parser->error.reason == 0)
    return 0;

  if (parser->error.reason == RETURN_ERROR
      && parser->error.error == XML_PARSE_ERROR)
    {
      gdb_assert (parser->error.message != NULL);
      error_string = parser->error.message;
    }
  else if (status == XML_STATUS_ERROR)
    {
      enum XML_Error err = XML_GetErrorCode (parser->expat_parser);
      error_string = XML_ErrorString (err);
    }
  else
    {
      gdb_assert (parser->error.reason < 0);
      throw_exception (parser->error);
    }

  if (parser->last_line != 0)
    warning (_("while parsing %s (at line %d): %s"), parser->name,
	     parser->last_line, error_string);
  else
    warning (_("while parsing %s: %s"), parser->name, error_string);

  return -1;
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

/* Parse an integer string into a ULONGEST and return it, or call
   gdb_xml_error if it could not be parsed.  */

ULONGEST
gdb_xml_parse_ulongest (struct gdb_xml_parser *parser, const char *value)
{
  ULONGEST result;

  if (xml_parse_unsigned_integer (value, &result) != 0)
    gdb_xml_error (parser, _("Can't convert \"%s\" to an integer"), value);

  return result;
}

/* Parse an integer attribute into a ULONGEST.  */

void *
gdb_xml_parse_attr_ulongest (struct gdb_xml_parser *parser,
			     const struct gdb_xml_attribute *attribute,
			     const char *value)
{
  ULONGEST result;
  void *ret;

  if (xml_parse_unsigned_integer (value, &result) != 0)
    gdb_xml_error (parser, _("Can't convert %s=\"%s\" to an integer"),
		   attribute->name, value);

  ret = xmalloc (sizeof (result));
  memcpy (ret, &result, sizeof (result));
  return ret;
}

/* Map NAME to VALUE.  A struct gdb_xml_enum * should be saved as the
   value of handler_data when using gdb_xml_parse_attr_enum to parse a
   fixed list of possible strings.  The list is terminated by an entry
   with NAME == NULL.  */

void *
gdb_xml_parse_attr_enum (struct gdb_xml_parser *parser,
			 const struct gdb_xml_attribute *attribute,
			 const char *value)
{
  const struct gdb_xml_enum *enums = attribute->handler_data;
  void *ret;

  for (enums = attribute->handler_data; enums->name != NULL; enums++)
    if (strcmp (enums->name, value) == 0)
      break;

  if (enums->name == NULL)
    gdb_xml_error (parser, _("Unknown attribute value %s=\"%s\""),
		 attribute->name, value);

  ret = xmalloc (sizeof (enums->value));
  memcpy (ret, &enums->value, sizeof (enums->value));
  return ret;
}

#endif /* HAVE_LIBEXPAT */

static void
show_debug_xml (struct ui_file *file, int from_tty,
		struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("XML debugging is %s.\n"), value);
}

void _initialize_xml_support (void);

void
_initialize_xml_support (void)
{
  add_setshow_boolean_cmd ("xml", class_maintenance, &debug_xml,
			   _("Set XML parser debugging."),
			   _("Show XML parser debugging."),
			   _("When set, debugging messages for XML parsers "
			     "are displayed."),
			   NULL, show_debug_xml,
			   &setdebuglist, &showdebuglist);
}
