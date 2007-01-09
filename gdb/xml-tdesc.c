/* XML target description support for GDB.

   Copyright (C) 2006
   Free Software Foundation, Inc.

   Contributed by CodeSourcery.

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
#include "target.h"
#include "target-descriptions.h"
#include "xml-support.h"
#include "xml-tdesc.h"

#include "gdb_assert.h"

#if !defined(HAVE_LIBEXPAT)

/* Parse DOCUMENT into a target description.  Or don't, since we don't have
   an XML parser.  */

static struct target_desc *
tdesc_parse_xml (const char *document)
{
  static int have_warned;

  if (!have_warned)
    {
      have_warned = 1;
      warning (_("Can not parse XML target description; XML support was "
		 "disabled at compile time"));
    }

  return NULL;
}

#else /* HAVE_LIBEXPAT */

/* Callback data for target description parsing.  */

struct tdesc_parsing_data
{
  /* The target description we are building.  */
  struct target_desc *tdesc;
};

/* Handle the end of an <architecture> element and its value.  */

static void
tdesc_end_arch (struct gdb_xml_parser *parser,
		const struct gdb_xml_element *element,
		void *user_data, const char *body_text)
{
  struct tdesc_parsing_data *data = user_data;
  const struct bfd_arch_info *arch;

  arch = bfd_scan_arch (body_text);
  if (arch == NULL)
    gdb_xml_error (parser, _("Target description specified unknown "
			     "architecture \"%s\""), body_text);
  set_tdesc_architecture (data->tdesc, arch);
}

/* The elements and attributes of an XML target description.  */

const struct gdb_xml_element target_children[] = {
  { "architecture", NULL, NULL, GDB_XML_EF_OPTIONAL,
    NULL, tdesc_end_arch },
  { NULL, NULL, NULL, GDB_XML_EF_NONE, NULL, NULL }
};

const struct gdb_xml_element tdesc_elements[] = {
  { "target", NULL, target_children, GDB_XML_EF_NONE,
    NULL, NULL },
  { NULL, NULL, NULL, GDB_XML_EF_NONE, NULL, NULL }
};

/* Parse DOCUMENT into a target description and return it.  */

static struct target_desc *
tdesc_parse_xml (const char *document)
{
  struct cleanup *back_to, *result_cleanup;
  struct gdb_xml_parser *parser;
  struct tdesc_parsing_data data;

  memset (&data, 0, sizeof (struct tdesc_parsing_data));

  back_to = make_cleanup (null_cleanup, NULL);
  parser = gdb_xml_create_parser_and_cleanup (_("target description"),
					      tdesc_elements, &data);

  data.tdesc = allocate_target_description ();
  result_cleanup = make_cleanup_free_target_description (data.tdesc);

  if (gdb_xml_parse (parser, document) == 0)
    {
      /* Parsed successfully.  */
      discard_cleanups (result_cleanup);
      do_cleanups (back_to);
      return data.tdesc;
    }
  else
    {
      warning (_("Could not load XML target description; ignoring"));
      do_cleanups (back_to);
      return NULL;
    }
}

#endif /* HAVE_LIBEXPAT */


/* Close FILE.  */

static void
do_cleanup_fclose (void *file)
{
  fclose (file);
}

/* Open FILENAME, read all its text into memory, close it, and return
   the text.  If something goes wrong, return NULL and warn.  */

static char *
fetch_xml_from_file (const char *filename)
{
  FILE *file;
  struct cleanup *back_to;
  char *text;
  size_t len, offset;

  file = fopen (filename, FOPEN_RT);
  if (file == NULL)
    {
      warning (_("Could not open \"%s\""), filename);
      return NULL;
    }
  back_to = make_cleanup (do_cleanup_fclose, file);

  /* Read in the whole file, one chunk at a time.  */
  len = 4096;
  offset = 0;
  text = xmalloc (len);
  make_cleanup (free_current_contents, &text);
  while (1)
    {
      size_t bytes_read;

      /* Continue reading where the last read left off.  Leave at least
	 one byte so that we can NUL-terminate the result.  */
      bytes_read = fread (text + offset, 1, len - offset - 1, file);
      if (ferror (file))
	{
	  warning (_("Read error from \"%s\""), filename);
	  do_cleanups (back_to);
	  return NULL;
	}

      offset += bytes_read;

      if (feof (file))
	break;

      len = len * 2;
      text = xrealloc (text, len);
    }

  fclose (file);
  discard_cleanups (back_to);

  text[offset] = '\0';
  return text;
}

/* Read an XML target description from FILENAME.  Parse it, and return
   the parsed description.  */

const struct target_desc *
file_read_description_xml (const char *filename)
{
  struct target_desc *tdesc;
  char *tdesc_str;
  struct cleanup *back_to;

  tdesc_str = fetch_xml_from_file (filename);
  if (tdesc_str == NULL)
    return NULL;

  back_to = make_cleanup (xfree, tdesc_str);
  tdesc = tdesc_parse_xml (tdesc_str);
  do_cleanups (back_to);

  return tdesc;
}

/* Read an XML target description using OPS.  Parse it, and return the
   parsed description.  */

const struct target_desc *
target_read_description_xml (struct target_ops *ops)
{
  struct target_desc *tdesc;
  char *tdesc_str;
  struct cleanup *back_to;

  tdesc_str = target_read_stralloc (ops, TARGET_OBJECT_AVAILABLE_FEATURES,
				    "target.xml");
  if (tdesc_str == NULL)
    return NULL;

  back_to = make_cleanup (xfree, tdesc_str);
  tdesc = tdesc_parse_xml (tdesc_str);
  do_cleanups (back_to);

  return tdesc;
}
