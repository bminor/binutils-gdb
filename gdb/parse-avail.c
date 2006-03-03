/* Support for runtime-defined target features for GDB.

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

#include "available.h"

#include "filenames.h"
#include "gdb_assert.h"
#include "gdb_string.h"
#include "gdb_obstack.h"

#include <expat.h>

/* General notes for this file:

   Do not call error while parsing supplied features.  If something is
   unrecognizable, fail gracefully.  Calling error may lead to a
   failure to initialize a gdbarch, and internal errors down the road.
   Also, it's hard to predict with XML what's meaningless today but
   might make sense tomorrow :-)

   Warnings are OK; so are true internal errors, e.g. impossible states
   in the parsing finite state machine.  Also, we will reach
   internal_error if there is a memory allocation failure.  */

/* Parse a field VALSTR that we expect to contain an integer value.
   The integer is returned in *VALP.

   Returns 0 for success, -1 for error.  */

static int
xml_parse_one_integer (const char *valstr, long *valp)
{
  char *endptr;
  long result;

  if (*valstr == '\0')
    return -1;

  result = strtol (valstr, &endptr, 0);
  if (*endptr != '\0')
    return -1;

  *valp = result;
  return 0;
}

/* Parse a field VALSTR that we expect to contain a boolean value.
   The value is returned in *VALP.

   Returns 0 for success, -1 for error.  */

static int
xml_parse_one_boolean (const char *valstr, int *valp)
{
  if (strcmp (valstr, "yes") == 0)
    *valp = 1;
  else if (strcmp (valstr, "no") == 0)
    *valp = 0;
  else
    return -1;

  return 0;
}

/* Save STR on OBSTACK, and return the new copy.  */

static char *
obstrdup (struct obstack *obstack, const char *str)
{
  char *result;
  size_t len;

  len = strlen (str);
  result = obstack_alloc (obstack, len + 1);
  memcpy (result, str, len);
  result[len] = '\0';

  return result;
}

/* Duplicate a provided ORIG_FEATURE, allocating the new copy from
   OBSTACK.  Return the new copy.  */

static struct gdb_available_feature *
copy_feature_to_obstack (struct obstack *obstack,
			 const struct gdb_available_feature *orig_feature)
{
  struct gdb_available_feature *feature;
  struct gdb_available_register **reg_slot, *orig_reg;

  feature = OBSTACK_ZALLOC (obstack, struct gdb_available_feature);

  memcpy (feature, orig_feature, sizeof (struct gdb_available_feature));
  feature->name = obstrdup (obstack, feature->name);
  if (feature->arch_data)
    feature->arch_data = obstrdup (obstack, feature->arch_data);

  reg_slot = &feature->registers;
  for (orig_reg = orig_feature->registers;
       orig_reg;
       orig_reg = orig_reg->next)
    {
      struct gdb_available_register *reg;

      reg = OBSTACK_ZALLOC (obstack, struct gdb_available_register);
      *reg_slot = reg;
      reg_slot = &reg->next;

      memcpy (reg, orig_reg, sizeof (struct gdb_available_register));
      reg->name = obstrdup (obstack, reg->name);
      if (reg->arch_data)
	reg->arch_data = obstrdup (obstack, reg->arch_data);
      if (reg->group)
	reg->group = obstrdup (obstack, reg->group);
      if (reg->type)
	reg->type = obstrdup (obstack, reg->type);
    }

  return feature;
}



/* FIXME item: Warnings versus errors?  Where to issue warnings?  */

enum xml_phase {
  PHASE_TOP,
  PHASE_IN_TARGET,
  PHASE_IN_FEATURE,
  PHASE_IN_DESCRIPTION,
  PHASE_IN_REG,
  PHASE_IN_FEATURE_SET,
  PHASE_IN_FEATURE_REF,
  PHASE_IN_XINCLUDE,
  PHASE_UNKNOWN
};

struct xml_state_stack
{
  enum xml_phase phase;

  union
  {
    struct
    {
      int seen_target;
      int nested;
    } top;
    struct
    {
      int seen_feature_set;
      int features_only;
    } target;
    struct
    {
      struct gdb_available_feature *feature;
      int seen_reg;
    } feature;
    struct
    {
      struct gdb_available_register *reg;
    } reg;
    struct
    {
      int depth;
    } unknown;
  } u;

  struct xml_state_stack *prev;
};

struct xml_feature_parse_data
{
  /* The obstack to allocate new features from.  */
  struct obstack *obstack;

  /* A function to call to obtain additional features, and its
     baton.  */
  xml_fetch_another fetcher;
  void *fetcher_baton;

  struct gdb_available_feature *seen_features;

  struct gdb_available_feature *target_features;

  /* Rough count of unrecognized or invalid items found while
     processing.  These are non-fatal; however, some data from the
     input has been skipped.  */
  int unhandled;

  struct xml_state_stack *state;
};

static void
xml_start_xinclude (struct xml_feature_parse_data *data,
		    const XML_Char **attrs)
{
  struct gdb_available_feature *features, *f;
  struct cleanup *back_to;
  char *text;
  int ret;
  const char *href, *id, **p;

  href = id = NULL;
  for (p = attrs; *p; p += 2)
    {
      const char *name = p[0], *val = p[1];

      if (strcmp (name, "href") == 0)
	href = val;
      else if (strcmp (name, "xpointer") == 0)
	id = val;
      else
	data->unhandled++;
    }

  if (id == NULL)
    {
      data->unhandled++;
      return;
    }

  for (f = data->seen_features; f; f = f->next)
    if (strcmp (f->name, id) == 0)
      /* Success.  We found a match already loaded; we don't need to
	 include the document.  */
      return;

  if (href == NULL || data->fetcher == NULL)
    {
      data->unhandled++;
      return;
    }

  text = data->fetcher (href, data->fetcher_baton);
  if (text == NULL)
    {
      data->unhandled++;
      return;
    }

  back_to = make_cleanup (xfree, text);

  ret = available_features_from_xml_string (&features, data->obstack,
					    text, data->fetcher,
					    data->fetcher_baton, 1);
  if (ret == 0)
    {
      f = features;
      while (f->next)
	f = f->next;
      f->next = data->seen_features;
      data->seen_features = f;
    }
  else
    /* Something went wrong parsing the document.  */
    data->unhandled++;

  do_cleanups (back_to);
}

static void
xml_start_feature (struct xml_feature_parse_data *data,
		   const XML_Char **attrs)
{
  struct gdb_available_feature *feature;
  const XML_Char **p;

  feature = obstack_alloc (data->obstack,
			   sizeof (struct gdb_available_feature));
  memset (feature, 0, sizeof (struct gdb_available_feature));

  feature->protocol_number = -1;

  for (p = attrs; *p; p += 2)
    {
      const char *name = p[0];
      const char *val = p[1];

      if (strcmp (name, "name") == 0)
	feature->name = obstrdup (data->obstack, val);

      else
	data->unhandled++;
    }

  /* Verify that mandatory fields were supplied.  */
  if (feature->name == NULL)
    {
      data->unhandled++;
      return;
    }

  data->state->u.feature.feature = feature;
}

static void
xml_start_reg (struct xml_feature_parse_data *data,
	       const XML_Char **attrs)
{
  struct gdb_available_register *reg;
  const XML_Char **p;

  reg = obstack_alloc (data->obstack,
		       sizeof (struct gdb_available_register));
  memset (reg, 0, sizeof (struct gdb_available_register));

  reg->bitsize = -1;
  reg->readonly = -1;
  reg->save_restore = -1;

  for (p = attrs; *p; p += 2)
    {
      const char *name = p[0];
      const char *val = p[1];

      if (*val == 0)
	data->unhandled++;

      else if (strcmp (name, "name") == 0)
	reg->name = obstrdup (data->obstack, val);

      else if (strcmp (name, "bitsize") == 0)
	{
	  if (xml_parse_one_integer (val, &reg->bitsize) < 0)
	    data->unhandled++;
	}

      else if (strcmp (name, "type") == 0)
	reg->type = obstrdup (data->obstack, val);

      else if (strcmp (name, "group") == 0)
	reg->group = obstrdup (data->obstack, val);

      else if (strcmp (name, "readonly") == 0)
	{
	  if (xml_parse_one_boolean (val, &reg->readonly) < 0)
	    data->unhandled++;
	}

      else if (strcmp (name, "save-restore") == 0)
	{
	  if (xml_parse_one_boolean (val, &reg->save_restore) < 0)
	    data->unhandled++;
	}

      else
	data->unhandled++;
    }

  /* Fill in optional fields with defaults.  */
  /* FIXME: If we always provide the DTD, we don't need to do this.  */
  if (reg->readonly == -1)
    reg->readonly = 0;
  if (reg->save_restore == -1)
    reg->save_restore = 1;
  if (reg->type == NULL)
    reg->type = obstrdup (data->obstack, "int");

  /* Verify that mandatory fields were supplied.  */
  if (reg->name == NULL || reg->bitsize == -1)
    {
      data->unhandled++;
      return;
    }

  data->state->u.reg.reg = reg;
}

static void
xml_start_feature_ref (struct xml_feature_parse_data *data,
		       const XML_Char **attrs)
{
  struct gdb_available_feature *feature, *new_feature;
  struct gdb_available_register *reg;
  const XML_Char **p;
  const char *feature_name = NULL;
  int i;

  for (p = attrs; *p; p += 2)
    {
      const char *name = p[0];
      const char *val = p[1];

      if (strcmp (name, "name") == 0)
	{
	  feature_name = val;
	  break;
	}
    }

  if (feature_name == NULL)
    {
      data->unhandled++;
      return;
    }

  for (feature = data->seen_features; feature; feature = feature->next)
    if (strcmp (feature->name, feature_name) == 0)
      break;

  /* TODO: Look for the feature elsewhere - on the target and in our
     database.  */

  /* If we couldn't find the feature, stop.  */
  if (feature == NULL)
    {
      data->unhandled++;
      return;
    }

  new_feature = copy_feature_to_obstack (data->obstack, feature);

  new_feature->protocol_number = -1;

  for (p = attrs; *p; p += 2)
    {
      const char *name = p[0];
      const char *val = p[1];

      if (strcmp (name, "name") == 0)
	continue;
      else if (strcmp (name, "base-regnum") == 0)
	new_feature->protocol_number = strtol (val, NULL, 0);
      else
	data->unhandled++;
    }

  /* The protocol number is only optional if there are no registers.  */
  if (new_feature->protocol_number == -1 && new_feature->registers != NULL)
    {
      data->unhandled++;
      return;
    }

  /* Set register numbers in the new feature.  */
  for (i = new_feature->protocol_number, reg = new_feature->registers;
       reg != NULL;
       i++, reg = reg->next)
    reg->protocol_number = i;

  new_feature->next = data->target_features;
  data->target_features = new_feature;
}

static void XMLCALL
xml_feature_start_element (void *data_, const XML_Char *name,
			   const XML_Char **attrs)
{
  struct xml_feature_parse_data *data = data_;
  const XML_Char **p;
  struct xml_state_stack *next_state;
  enum xml_phase next_phase;

  if (data->state->phase == PHASE_UNKNOWN)
    {
#if 1
      fprintf_unfiltered (gdb_stderr, "skipping, name %s\n", name);
#endif
      data->state->u.unknown.depth++;
      return;
    }

#if 1
  fprintf_unfiltered (gdb_stderr, "entering, name %s\n", name);
  for (p = attrs; *p; p += 2)
    fprintf_unfiltered (gdb_stderr, "  attr %s=\"%s\"\n", p[0], p[1]);
#endif

  next_state = xmalloc (sizeof (struct xml_state_stack));
  memset (next_state, 0, sizeof (struct xml_state_stack));

  /* Map the element we're entering to our known elements.  */
  if (strcmp (name, "target") == 0)
    next_phase = PHASE_IN_TARGET;
  else if (strcmp (name, "feature") == 0)
    next_phase = PHASE_IN_FEATURE;
  else if (strcmp (name, "description") == 0)
    next_phase = PHASE_IN_DESCRIPTION;
  else if (strcmp (name, "reg") == 0)
    next_phase = PHASE_IN_REG;
  else if (strcmp (name, "feature-set") == 0)
    next_phase = PHASE_IN_FEATURE_SET;
  else if (strcmp (name, "feature-ref") == 0)
    next_phase = PHASE_IN_FEATURE_REF;
  else if (strcmp (name, "http://www.w3.org/2001/XInclude!include") == 0)
    next_phase = PHASE_IN_XINCLUDE;
  else
    next_phase = PHASE_UNKNOWN;

  /* Make sure that the next phase is sensible.  */
  switch (data->state->phase)
    {
    case PHASE_TOP:
      if (next_phase == PHASE_IN_TARGET
	  && !data->state->u.top.seen_target)
	{
	  data->state->u.top.seen_target = 1;
	  break;
	}
      if (next_phase == PHASE_IN_FEATURE
	  && !data->state->u.top.seen_target
	  && data->state->u.top.nested)
	{
	  data->state->u.top.seen_target = 1;
	  break;
	}
      next_phase = PHASE_UNKNOWN;
      break;

    case PHASE_IN_TARGET:
      if (next_phase == PHASE_IN_FEATURE
	  && !data->state->u.target.seen_feature_set)
	break;
      if (next_phase == PHASE_IN_XINCLUDE
	  && !data->state->u.target.seen_feature_set)
	break;
      if (next_phase == PHASE_IN_FEATURE_SET
	  && !data->state->u.target.seen_feature_set)
	{
	  if (data->state->u.target.features_only)
	    {
	      /* Skip parsing the feature set.  This is not an
		 error, so counteract the increment of data->unhandled
		 below.  */
	      next_phase = PHASE_UNKNOWN;
	      data->unhandled--;
	    }

	  data->state->u.target.seen_feature_set = 1;
	  break;
	}
      next_phase = PHASE_UNKNOWN;
      break;

    case PHASE_IN_FEATURE:
      if (next_phase == PHASE_IN_DESCRIPTION
	  && !data->state->u.feature.seen_reg)
	break;
      if (next_phase == PHASE_IN_REG)
	{
	  data->state->u.feature.seen_reg = 1;
	  break;
	}
      next_phase = PHASE_UNKNOWN;
      break;

    case PHASE_IN_DESCRIPTION:
      next_phase = PHASE_UNKNOWN;
      break;

    case PHASE_IN_REG:
      if (next_phase == PHASE_IN_DESCRIPTION)
	break;
      next_phase = PHASE_UNKNOWN;
      break;

    case PHASE_IN_FEATURE_SET:
      if (next_phase == PHASE_IN_FEATURE_REF)
	break;
      next_phase = PHASE_UNKNOWN;
      break;

    case PHASE_IN_FEATURE_REF:
    case PHASE_IN_XINCLUDE:
      next_phase = PHASE_UNKNOWN;
      break;

    case PHASE_UNKNOWN:
    default:
      internal_error (__FILE__, __LINE__, "unexpected current state");
      break;
    }

  next_state->phase = next_phase;
  next_state->prev = data->state;
  data->state = next_state;

  /* FIXME: Check for unknown attributes in more elements, e.g. target.  */
  /* FIXME: Check for missing mandatory elements more thoroughly.  */

  switch (next_phase)
    {
    case PHASE_TOP:
      internal_error (__FILE__, __LINE__, "unexpected next state");
      break;

    case PHASE_IN_TARGET:
      if (data->state->prev->u.top.nested)
	data->state->u.target.features_only = 1;
      break;

    case PHASE_IN_FEATURE:
      xml_start_feature (data, attrs);
      break;

    case PHASE_IN_DESCRIPTION:
      /* FIXME: Currently, descriptions are ignored.  */
      break;

    case PHASE_IN_REG:
      xml_start_reg (data, attrs);
      break;

    case PHASE_IN_FEATURE_SET:
      /* Nothing needed.  */
      break;

    case PHASE_IN_FEATURE_REF:
      xml_start_feature_ref (data, attrs);
      break;

    case PHASE_IN_XINCLUDE:
      xml_start_xinclude (data, attrs);
      break;

    case PHASE_UNKNOWN:
      data->unhandled++;
      next_state->u.unknown.depth++;
      break;

    default:
      internal_error (__FILE__, __LINE__, "unexpected next state");
      break;
    }
}

static void XMLCALL
xml_feature_end_element (void *data_, const XML_Char *name)
{
  struct xml_feature_parse_data *data = data_;
  struct xml_state_stack *state;
  struct gdb_available_feature *feature;
  struct gdb_available_register *reg;

#if 1
  fprintf_unfiltered (gdb_stderr, "leaving, name %s\n", name);
#endif

  switch (data->state->phase)
    {
    case PHASE_UNKNOWN:
      data->state->u.unknown.depth--;
      if (data->state->u.unknown.depth)
	return;
      break;

    case PHASE_IN_TARGET:
      break;

    case PHASE_IN_FEATURE:
      feature = data->state->u.feature.feature;

      /* Do nothing if a fatal error occured.  */
      if (feature == NULL)
	break;

      /* Record the feature.  */
      feature->next = data->seen_features;
      data->seen_features = feature;

      /* Reverse the list of registers.  */
      if (feature->registers)
	{
	  struct gdb_available_register *reg1, *reg2, *reg3;

	  reg1 = NULL;
	  reg2 = feature->registers;

	  while (reg2)
	    {
	      reg3 = reg2->next;
	      reg2->next = reg1;
	      reg1 = reg2;
	      reg2 = reg3;
	    }

	  feature->registers = reg1;
	}

      break;

    case PHASE_IN_DESCRIPTION:
      /* FIXME: Currently, descriptions are ignored.  */
      break;

    case PHASE_IN_REG:
      gdb_assert (data->state->prev->phase == PHASE_IN_FEATURE);

      feature = data->state->prev->u.feature.feature;
      reg = data->state->u.reg.reg;

      /* Do nothing if a fatal error occured, either here
	 or in the containing feature.  */
      if (reg == NULL || feature == NULL)
	break;

      reg->next = feature->registers;
      feature->registers = reg;
      break;

    case PHASE_IN_FEATURE_SET:
      /* Reverse the list of features.  */
      if (data->target_features)
	{
	  struct gdb_available_feature *feat1, *feat2, *feat3;

	  feat1 = NULL;
	  feat2 = data->target_features;

	  while (feat2)
	    {
	      feat3 = feat2->next;
	      feat2->next = feat1;
	      feat1 = feat2;
	      feat2 = feat3;
	    }

	  data->target_features = feat1;
	}
      break;

    case PHASE_IN_FEATURE_REF:
    case PHASE_IN_XINCLUDE:
      /* Nothing needed.  */
      break;

    case PHASE_TOP:
    default:
      internal_error (__FILE__, __LINE__, "unexpected ending state");
      break;
    }

  state = data->state;
  data->state = data->state->prev;
  xfree (state);
}

static void
xml_parser_cleanup (void *parser)
{
  struct xml_feature_parse_data *data;

  data = XML_GetUserData (parser);
  while (data->state)
    {
      struct xml_state_stack *prev;

      prev = data->state->prev;
      xfree (data->state);
      data->state = prev;
    }

  XML_ParserFree (parser);
}

static int XMLCALL
xml_feature_external_entity (XML_Parser parser,
			     const XML_Char *context,
			     const XML_Char *base,
			     const XML_Char *systemId,
			     const XML_Char *publicId)
{
  struct xml_feature_parse_data *data;
  XML_Parser entity_parser;
  char *text;
  struct cleanup *back_to;

  data = XML_GetUserData (parser);
  if (data->fetcher == NULL)
    return XML_STATUS_ERROR;

  text = data->fetcher (systemId, data->fetcher_baton);
  if (text == NULL)
    return XML_STATUS_ERROR;

  entity_parser = XML_ExternalEntityParserCreate (parser, context, NULL);
  back_to = make_cleanup (xml_parser_cleanup, entity_parser);

  XML_SetElementHandler (entity_parser, NULL, NULL);

  if (XML_Parse (entity_parser, text, strlen (text), 1) != XML_STATUS_OK)
    {
      do_cleanups (back_to);
      return XML_STATUS_ERROR;
    }

  do_cleanups (back_to);
  return XML_STATUS_OK;
}

/* FIXME: Error check more XML_* calls.  */

int
available_features_from_xml_string (struct gdb_available_feature **features_p,
				    struct obstack *obstack,
				    const char *text, xml_fetch_another fetcher,
				    void *fetcher_baton,
				    int nested)
{
  XML_Parser parser;
  struct xml_feature_parse_data *data;
  struct cleanup *back_to;

  parser = XML_ParserCreateNS (NULL, '!');
  if (parser == NULL)
    return -1;
  back_to = make_cleanup (xml_parser_cleanup, parser);

  data = XCALLOC (1, struct xml_feature_parse_data);
  make_cleanup (xfree, data);
  XML_SetUserData (parser, data);

  data->obstack = obstack;
  data->state = XCALLOC (1, struct xml_state_stack);
  data->state->phase = PHASE_TOP;
  data->state->u.top.nested = nested;

  data->fetcher = fetcher;
  data->fetcher_baton = fetcher_baton;

  XML_SetElementHandler (parser, xml_feature_start_element,
			 xml_feature_end_element);

  XML_SetParamEntityParsing (parser,
			     XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE);
  XML_SetExternalEntityRefHandler (parser, xml_feature_external_entity);

  /* FIXME: Enable this after we compile in the DTD?  */
  /* XML_UseForeignDTD (parser, XML_TRUE); */

  if (XML_Parse (parser, text, strlen (text), 1) != XML_STATUS_OK)
    {
      enum XML_Error err = XML_GetErrorCode (parser);

      warning (_("XML parsing error: %s"), XML_ErrorString (err));
      return -1;
    }

  if (nested)
    *features_p = data->seen_features;
  else
    *features_p = data->target_features;

  /* TODO: If data->unhandled, warn?  */
  /* TODO: Can other errors be fatal?  */
  /* TODO: Should *features_p == NULL be an error?  */

  do_cleanups (back_to);

  return 0;
}

static void
do_cleanup_fclose (void *file)
{
  fclose (file);
}

static char *
fetch_available_features_from_file (const char *filename, void *base_)
{
  const char *base = base_;
  FILE *file;
  struct cleanup *back_to;
  char *text;
  size_t len, offset;

  if (base && *base)
    {
      char *fullname = concat (base, "/", filename, NULL);
      if (fullname == NULL)
	nomem (0);
      file = fopen (fullname, FOPEN_RT);
      xfree (fullname);
    }
  else
    file = fopen (filename, FOPEN_RT);

  if (file == NULL)
    return NULL;
  back_to = make_cleanup (do_cleanup_fclose, file);

  /* Read in the whole file.  */
  len = 4096;
  offset = 0;
  text = xmalloc (len);
  make_cleanup (free_current_contents, &text);
  while (1)
    {
      size_t bytes_read;

      bytes_read = fread (text + offset, 1, len - offset, file);
      if (ferror (file))
	{
	  warning (_("Could not read from \"%s\""), filename);
	  do_cleanups (back_to);
	  return NULL;
	}

      if (feof (file))
	break;

      offset += bytes_read;
      len += len;
      text = xrealloc (text, len);
    }

  fclose (file);
  discard_cleanups (back_to);

  return text;
}

int
available_features_from_xml_file (struct gdb_available_feature **feature_p,
				  struct obstack *obstack,
				  const char *filename)
{
  struct cleanup *back_to = make_cleanup (null_cleanup, NULL);
  const char *p;
  char *dirname, *text;

  text = fetch_available_features_from_file (filename, NULL);
  if (text == NULL)
    return -1;

  /* Simple, portable version of dirname that does not modify its
     argument.  */
  p = lbasename (filename);
  while (p > filename && IS_DIR_SEPARATOR (p[-1]))
    --p;
  if (p > filename)
    {
      dirname = xmalloc (p - filename + 1);
      memcpy (dirname, filename, p - filename);
      dirname[p - filename] = '\0';
      make_cleanup (xfree, dirname);
    }
  else
    dirname = NULL;

  available_features_from_xml_string (feature_p, obstack, text,
				      fetch_available_features_from_file,
				      dirname, 0);

  do_cleanups (back_to);

  return 0;
}

/* For debugging.  */

int
try_available_features_from_xml_file (const char *filename)
{
  struct obstack obstack;
  struct gdb_available_feature *feature;
  int ret;

  obstack_init (&obstack);
  ret = available_features_from_xml_file (&feature, &obstack, filename);
  obstack_free (&obstack, NULL);

  return ret;
}
