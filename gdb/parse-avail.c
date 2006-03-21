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
#include "sha1.h"

#include "filenames.h"
#include "gdb_assert.h"
#include "gdb_string.h"
#include "gdb_obstack.h"

#include <expat.h>

/* From xml-builtin.c.  */
extern const char *xml_builtin[][2];

/* Prototypes.  */

static int xml_process_xincludes (char **output_p, const char *text,
				  xml_fetch_another fetcher, void *fetcher_baton,
				  int nested);

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

  struct gdb_available_feature *seen_features;

  struct gdb_available_feature *target_features;

  /* Rough count of unrecognized or invalid items found while
     processing.  These are non-fatal; however, some data from the
     input has been skipped.  */
  int unhandled;

  struct xml_state_stack *state;
};

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

  reg->protocol_number = -1;
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

      else if (strcmp (name, "regnum") == 0)
	{
	  if (xml_parse_one_integer (val, &reg->protocol_number) < 0)
	    data->unhandled++;
	}

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
  for (reg = new_feature->registers; reg != NULL; reg = reg->next)
    reg->protocol_number += new_feature->protocol_number;

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

      if (feature->registers)
	{
	  /* Reverse the list of registers.  */
	  struct gdb_available_register *reg1, *reg2, *reg3;
	  int next;

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

	  next = 0;
	  for (reg1 = feature->registers; reg1; reg1 = reg1->next)
	    if (reg1->protocol_number == -1)
	      reg1->protocol_number = next++;
	    else
	      next = reg1->protocol_number + 1;
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
  if (data)
    {
      while (data->state)
	{
	  struct xml_state_stack *prev;

	  prev = data->state->prev;
	  xfree (data->state);
	  data->state = prev;
	}

      xfree (data);
    }

  XML_ParserFree (parser);
}

static int XMLCALL
xml_fetch_external_entity (XML_Parser parser,
			   const XML_Char *context,
			   const XML_Char *base,
			   const XML_Char *systemId,
			   const XML_Char *publicId)
{
  XML_Parser entity_parser;
  const char *text;
  struct cleanup *back_to;

  if (publicId != NULL)
    return XML_STATUS_ERROR;

  if (systemId != NULL && strcmp (systemId, "gdb-target.dtd") != 0)
    return XML_STATUS_ERROR;

  text = fetch_xml_builtin ("gdb-target.dtd");
  if (text == NULL)
    internal_error (__FILE__, __LINE__, "could not locate built-in DTD");

  entity_parser = XML_ExternalEntityParserCreate (parser, context, NULL);
  back_to = make_cleanup (xml_parser_cleanup, entity_parser);

  /* Don't use our handlers for the contents of the DTD.  */
  XML_SetElementHandler (entity_parser, NULL, NULL);
  XML_SetDoctypeDeclHandler (entity_parser, NULL, NULL);
  XML_SetXmlDeclHandler (entity_parser, NULL);
  XML_SetDefaultHandler (entity_parser, NULL);
  XML_SetUserData (entity_parser, NULL);

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
available_features_from_xml_string (struct gdb_feature_set *feature_set,
				    const char *text, xml_fetch_another fetcher,
				    void *fetcher_baton,
				    int nested)
{
  XML_Parser parser;
  struct xml_feature_parse_data *data;
  struct cleanup *back_to;
  char *expanded_text;
  int ret;
  union
  {
    /* We use a union to represent the checksum in order to guarantee
       sufficient alignment.  */
    uint32_t words[5];
    unsigned char bytes[20];
  } sha1sum;

  /* Expand all XInclude directives.  */
  ret = xml_process_xincludes (&expanded_text, text, fetcher, fetcher_baton,
			       0);
  if (ret != 0)
    return -1;

  /* Save the checksum.  */
  sha1_buffer (expanded_text, strlen (expanded_text), sha1sum.bytes);
  memcpy (feature_set->checksum, sha1sum.bytes, 20);

  parser = XML_ParserCreateNS (NULL, '!');
  if (parser == NULL)
    return -1;
  back_to = make_cleanup (xml_parser_cleanup, parser);

  data = XCALLOC (1, struct xml_feature_parse_data);
  XML_SetUserData (parser, data);

  data->obstack = feature_set->obstack;
  data->state = XCALLOC (1, struct xml_state_stack);
  data->state->phase = PHASE_TOP;
  data->state->u.top.nested = nested;

  XML_SetElementHandler (parser, xml_feature_start_element,
			 xml_feature_end_element);

  XML_SetParamEntityParsing (parser,
			     XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE);
  XML_SetExternalEntityRefHandler (parser, xml_fetch_external_entity);

  /* Even if no DTD is provided, use the built-in DTD anyway.  */
  XML_UseForeignDTD (parser, XML_TRUE);

  if (XML_Parse (parser, expanded_text, strlen (expanded_text), 1)
      != XML_STATUS_OK)
    {
      enum XML_Error err = XML_GetErrorCode (parser);

      warning (_("XML parsing error: %s"), XML_ErrorString (err));

      do_cleanups (back_to);
      return -1;
    }

  if (nested)
    feature_set->features = data->seen_features;
  else
    feature_set->features = data->target_features;

  /* TODO: If data->unhandled, warn?  */
  /* TODO: Can other errors be fatal?  */
  /* TODO: Should target_features == NULL be an error?  */

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
available_features_from_xml_file (struct gdb_feature_set *feature_set,
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

  available_features_from_xml_string (feature_set, text,
				      fetch_available_features_from_file,
				      dirname, 0);

  /* FIXME: We ignored the return value!  */

  do_cleanups (back_to);

  return 0;
}

/* For debugging.  */

int
try_available_features_from_xml_file (const char *filename)
{
  struct gdb_feature_set feature_set;
  struct obstack obstack;
  int ret;

  obstack_init (&obstack);
  feature_set.obstack = &obstack;
  ret = available_features_from_xml_file (&feature_set, filename);
  obstack_free (&obstack, NULL);

  return ret;
}

/* XInclude processing.  This is done as a separate step from actually
   parsing the document, so that we can produce a single combined XML
   document to hand to a front end (and to simplify comparing two
   documents).  The DTD is also processed in this stage, and default
   values for attributes are filled in.  */

struct xml_xinclude_parse_data
{
  /* The obstack to build the output in.  */
  struct obstack obstack;

  /* The current parser.  */
  XML_Parser parser;

  /* The external DTD subset, if one has been loaded.  */
  char *external_dtd;

  /* A count indicating whether we are in an element whose
     children should not be copied to the output, and if so,
     how deep we are nested.  This is used for any (unexpected)
     children of XInclude directives, and for the DTD.  */
  int skip_depth;

  /* A function to call to obtain additional features, and its
     baton.  */
  xml_fetch_another fetcher;
  void *fetcher_baton;

  /* Rough count of unrecognized or invalid items found while
     processing.  These are non-fatal; however, some data from the
     input has been skipped.  */
  int unhandled;
};

static void
xml_xinclude_start_xinclude (struct xml_xinclude_parse_data *data,
			     const XML_Char **attrs)
{
  struct cleanup *back_to;
  char *text, *output;
  int ret;
  const char *href, **p;

  href = NULL;
  for (p = attrs; *p; p += 2)
    {
      const char *name = p[0], *val = p[1];

      if (strcmp (name, "href") == 0)
	href = val;
      else
	data->unhandled++;
    }

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

  ret = xml_process_xincludes (&output, text,
			       data->fetcher, data->fetcher_baton,
			       1);

  if (ret == 0)
    obstack_grow (&data->obstack, output, strlen (output));
  else
    /* Something went wrong parsing the document.  */
    data->unhandled++;

  do_cleanups (back_to);
}

/* TODO: We output the doctype and <?xml> declaration for the first
   document unchanged, if present, and discard those for included
   documents.  Should we always generate doctype and version
   information?  Never?  Should we include the external DTD subset
   in the output document?

   Issues to consider:
   - We can not simply include the external DTD subset in the document
   as an internal subset, because <!IGNORE> and <!INCLUDE> are valid
   only in external subsets.  We may ignore this...
   - Passing through the outer <?xml> declaration is incorrect because
   we generate UTF-8, not whatever the input encoding was.
   - If we do not pass the DTD into the output, default values will not
   be filled in.  */

static void XMLCALL
xml_xinclude_start_element (void *data_, const XML_Char *name,
			    const XML_Char **attrs)
{
  struct xml_xinclude_parse_data *data = data_;

  /* If we are already skipping, keep on going.  */
  if (data->skip_depth)
    {
      data->skip_depth++;
      return;
    }

  if (strcmp (name, "http://www.w3.org/2001/XInclude!include") == 0)
    {
      xml_xinclude_start_xinclude (data, attrs);

      /* Skip any children of this element (and its end).  */
      data->skip_depth = 1;
    }
  else
    XML_DefaultCurrent (data->parser);
}

static void XMLCALL
xml_xinclude_end_element (void *data_, const XML_Char *name)
{
  struct xml_xinclude_parse_data *data = data_;

  /* If we are skipping, update the depth.  */
  if (data->skip_depth)
    {
      data->skip_depth--;
      return;
    }

  /* Otherwise just print it out.  If this is the "end" of an empty element,
     we don't need to do anything special - the default handler will get
     called with LEN == 0.  */
  XML_DefaultCurrent (data->parser);
}

static void XMLCALL
xml_xinclude_default (void *data_, const XML_Char *s, int len)
{
  struct xml_xinclude_parse_data *data = data_;

  /* If we are inside of e.g. xi:include or the DTD, don't save this
     string.  */
  if (data->skip_depth)
    return;

  obstack_grow (&data->obstack, s, len);
}

static void XMLCALL
xml_xinclude_start_doctype (void *data_, const XML_Char *doctypeName,
			    const XML_Char *sysid, const XML_Char *pubid,
			    int has_internal_subset)
{
  struct xml_xinclude_parse_data *data = data_;

  /* Don't print out the doctype, or the contents of the DTD internal
     subset, if any.  */
  data->skip_depth++;
}

static void XMLCALL
xml_xinclude_end_doctype (void *data_)
{
  struct xml_xinclude_parse_data *data = data_;

  data->skip_depth--;
}

static void XMLCALL
xml_xinclude_xml_decl (void *data_, const XML_Char *version,
		       const XML_Char *encoding, int standalone)
{
  /* Do nothing - this prevents the default handler from being called.  */
}

static void
xml_xinclude_parser_cleanup (void *parser)
{
  struct xml_xinclude_parse_data *data;

  data = XML_GetUserData (parser);
  if (data)
    {
      obstack_free (&data->obstack, NULL);

      if (data->external_dtd)
	xfree (data->external_dtd);

      xfree (data);
    }

  XML_ParserFree (parser);
}

/* FIXME: Error check more XML_* calls.  */

int
xml_process_xincludes (char **output_p, const char *text,
		       xml_fetch_another fetcher, void *fetcher_baton,
		       int nested)
{
  XML_Parser parser;
  struct xml_xinclude_parse_data *data;
  struct cleanup *back_to;

  parser = XML_ParserCreateNS (NULL, '!');
  if (parser == NULL)
    return -1;
  back_to = make_cleanup (xml_xinclude_parser_cleanup, parser);

  data = XCALLOC (1, struct xml_xinclude_parse_data);
  XML_SetUserData (parser, data);

  obstack_init (&data->obstack);

  data->parser = parser;
  data->fetcher = fetcher;
  data->fetcher_baton = fetcher_baton;

  XML_SetElementHandler (parser, xml_xinclude_start_element,
			 xml_xinclude_end_element);
  XML_SetDefaultHandler (parser, xml_xinclude_default);

  /* Always discard the XML version declarations; the only important
     thing this provides is encoding, and our result will have been
     converted to UTF-8.  */
  XML_SetXmlDeclHandler (parser, xml_xinclude_xml_decl);

  if (nested)
    /* Discard the doctype for included documents.  */
    XML_SetDoctypeDeclHandler (parser, xml_xinclude_start_doctype,
			       xml_xinclude_end_doctype);

  XML_SetParamEntityParsing (parser,
			     XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE);
  XML_SetExternalEntityRefHandler (parser, xml_fetch_external_entity);

  /* Even if no DTD is provided, use the built-in DTD anyway.  */
  XML_UseForeignDTD (parser, XML_TRUE);

  if (XML_Parse (parser, text, strlen (text), 1) != XML_STATUS_OK)
    {
      enum XML_Error err = XML_GetErrorCode (parser);

      warning (_("XML parsing error: %s"), XML_ErrorString (err));

      do_cleanups (back_to);
      return -1;
    }

  /* TODO: If data->unhandled, warn?  */
  /* TODO: Can other errors be fatal?  */

  obstack_1grow (&data->obstack, '\0');
  *output_p = xstrdup (obstack_finish (&data->obstack));

  do_cleanups (back_to);

  return 0;
}

int
xml_xinclude_from_file (char **output_p, const char *filename)
{
  struct cleanup *back_to = make_cleanup (null_cleanup, NULL);
  const char *p;
  char *dirname, *text;
  int ret;

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

  ret = xml_process_xincludes (output_p, text,
			       fetch_available_features_from_file,
			       dirname, 0);

  do_cleanups (back_to);
  return ret;
}

/* For debugging.  */

int
try_xml_xinclude_from_xml_file (const char *filename)
{
  char *output;
  int ret;

  ret = xml_xinclude_from_file (&output, filename);

  if (ret == 0)
    printf_unfiltered ("%s", output);

  return ret;
}

/* Return the XML corresponding to the given filename, if it
   was compiled in to GDB, and NULL otherwise.  */

const char *
fetch_xml_builtin (const char *name)
{
  const char *(*p)[2];

  for (p = xml_builtin; (*p)[0]; p++)
    if (strcmp ((*p)[0], name) == 0)
      return (*p)[1];

  return NULL;
}
