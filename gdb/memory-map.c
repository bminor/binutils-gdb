/* Routines for handling XML memory maps provided by target.

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
#include "memory-map.h"
#include "gdb_assert.h"
#include "exceptions.h"

#include "gdb_string.h"

#if !defined(HAVE_LIBEXPAT)

VEC(mem_region_s) *
parse_memory_map (const char *memory_map)
{
  static int have_warned;

  if (!have_warned)
    {
      have_warned = 1;
      warning (_("Can not parse XML memory map; XML support was disabled "
		 "at compile time"));
    }

  return NULL;
}

#else /* HAVE_LIBEXPAT */

#include "xml-support.h"

#include "gdb_expat.h"

/* Internal parsing data passed to all Expat callbacks.  */
struct memory_map_parsing_data
  {
    VEC(mem_region_s) **memory_map;
    struct mem_region *currently_parsing;
    char *character_data;
    const char *property_name;
    int capture_text;
  };

static void
free_memory_map_parsing_data (void *p_)
{
  struct memory_map_parsing_data *p = p_;

  xfree (p->character_data);
}

/* Callback called by Expat on start of element.
   DATA_ is pointer to memory_map_parsing_data
   NAME is the name of element
   ATTRS is the zero-terminated array of attribute names and
   attribute values.

   This function handles the following elements:
   - 'memory' -- creates a new memory region and initializes it
     from attributes.  Sets DATA_.currently_parsing to the new region.
   - 'properties' -- sets DATA.capture_text.  */

static void
memory_map_start_element (void *data_, const XML_Char *name,
			  const XML_Char **attrs)
{
  static const XML_Char *type_names[] = {"ram", "rom", "flash", 0};
  static int type_values[] = { MEM_RW, MEM_RO, MEM_FLASH };
  struct memory_map_parsing_data *data = data_;
  struct gdb_exception ex;

  TRY_CATCH (ex, RETURN_MASK_ERROR)
    {
      if (strcmp (name, "memory") == 0)
	{
	  struct mem_region *r;

	  r = VEC_safe_push (mem_region_s, *data->memory_map, NULL);
	  mem_region_init (r);

	  r->lo = xml_get_integer_attribute (attrs, "start");
	  r->hi = r->lo + xml_get_integer_attribute (attrs, "length");
	  r->attrib.mode = xml_get_enum_value (attrs, "type", type_names,
					       type_values);
	  r->attrib.blocksize = -1;

	  data->currently_parsing = r;
	}
      else if (strcmp (name, "property") == 0)
	{
	  if (!data->currently_parsing)
	    throw_error (XML_PARSE_ERROR,
		_("memory map: found 'property' element outside 'memory'"));

	  data->capture_text = 1;

	  data->property_name = xml_get_required_attribute (attrs, "name");
	}
    }
  if (ex.reason < 0)
    throw_error
      (ex.error, _("While parsing element %s:\n%s"), name, ex.message);
}

/* Callback called by Expat on start of element.  DATA_ is a pointer
   to our memory_map_parsing_data.  NAME is the name of the element.

   This function handles the following elements:
   - 'property' -- check that the property name is 'blocksize' and
     sets DATA->currently_parsing->attrib.blocksize
   - 'memory' verifies that flash block size is set.  */

static void
memory_map_end_element (void *data_, const XML_Char *name)
{
  struct memory_map_parsing_data *data = data_;
  struct gdb_exception ex;

  TRY_CATCH (ex, RETURN_MASK_ERROR)
    {
      if (strcmp (name, "property") == 0)
	{
	  if (strcmp (data->property_name, "blocksize") == 0)
	    {
	      if (!data->character_data)
		throw_error (XML_PARSE_ERROR,
			     _("Empty content of 'property' element"));
	      char *end = NULL;
	      data->currently_parsing->attrib.blocksize
		= strtoul (data->character_data, &end, 0);
	      if (*end != '\0')
		throw_error (XML_PARSE_ERROR,
			     _("Invalid content of the 'blocksize' property"));
	    }
	  else
	    throw_error (XML_PARSE_ERROR,
			 _("Unknown memory region property: %s"), name);

	  data->capture_text = 0;
	}
      else if (strcmp (name, "memory") == 0)
	{
	  if (data->currently_parsing->attrib.mode == MEM_FLASH
	      && data->currently_parsing->attrib.blocksize == -1)
	    throw_error (XML_PARSE_ERROR,
			 _("Flash block size is not set"));

	  data->currently_parsing = 0;
	  data->character_data = 0;
	}
    }
  if (ex.reason < 0)
    throw_error
      (ex.error, _("while parsing element %s: \n%s"), name, ex.message);
}

/* Callback called by expat for all character data blocks.
   DATA_ is the pointer to memory_map_parsing_data.
   S is the point to character data.
   LEN is the length of data; the data is not zero-terminated.

   If DATA_->CAPTURE_TEXT is 1, appends this block of characters
   to DATA_->CHARACTER_DATA.  */
static void
memory_map_character_data (void *data_, const XML_Char *s,
			   int len)
{
  struct memory_map_parsing_data *data = data_;
  int current_size = 0;

  if (!data->capture_text)
    return;

  /* Expat interface does not guarantee that a single call to
     a handler will be made. Actually, one call for each line
     will be made, and character data can possibly span several
     lines.

     Take care to realloc the data if needed.  */
  if (!data->character_data)
    data->character_data = xmalloc (len + 1);
  else
    {
      current_size = strlen (data->character_data);
      data->character_data = xrealloc (data->character_data,
				       current_size + len + 1);
    }

  memcpy (data->character_data + current_size, s, len);
  data->character_data[current_size + len] = '\0';
}

static void
clear_result (void *p)
{
  VEC(mem_region_s) **result = p;
  VEC_free (mem_region_s, *result);
  *result = NULL;
}

VEC(mem_region_s) *
parse_memory_map (const char *memory_map)
{
  VEC(mem_region_s) *result = NULL;
  struct cleanup *back_to = make_cleanup (null_cleanup, NULL);
  struct cleanup *before_deleting_result;
  struct cleanup *saved;
  volatile struct gdb_exception ex;
  int ok = 0;

  struct memory_map_parsing_data data = {};

  XML_Parser parser = XML_ParserCreateNS (NULL, '!');
  if (parser == NULL)
    goto out;

  make_cleanup_free_xml_parser (parser);
  make_cleanup (free_memory_map_parsing_data, &data);
  /* Note: 'clear_result' will zero 'result'.  */
  before_deleting_result = make_cleanup (clear_result, &result);

  XML_SetElementHandler (parser, memory_map_start_element,
			 memory_map_end_element);
  XML_SetCharacterDataHandler (parser, memory_map_character_data);
  XML_SetUserData (parser, &data);
  data.memory_map = &result;

  TRY_CATCH (ex, RETURN_MASK_ERROR)
    {
      if (XML_Parse (parser, memory_map, strlen (memory_map), 1)
	  != XML_STATUS_OK)
	{
	  enum XML_Error err = XML_GetErrorCode (parser);

	  throw_error (XML_PARSE_ERROR, "%s", XML_ErrorString (err));
	}
    }
  if (ex.reason != GDB_NO_ERROR)
    {
      if (ex.error == XML_PARSE_ERROR)
	/* Just report it.  */
	warning (_("Could not parse XML memory map: %s"), ex.message);
      else
	throw_exception (ex);
    }
  else
    /* Parsed successfully, don't need to delete the result.  */
    discard_cleanups (before_deleting_result);

 out:
  do_cleanups (back_to);
  return result;
}

#endif /* HAVE_LIBEXPAT */
