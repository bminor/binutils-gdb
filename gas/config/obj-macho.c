/* Mach-O object file format
   Copyright 2009 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 3,
   or (at your option) any later version.

   GAS is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#define OBJ_HEADER "obj-macho.h"

#include "as.h"
#include "subsegs.h"
#include "symbols.h"
#include "write.h"
#include "mach-o.h"
#include "mach-o/loader.h"

static void
obj_mach_o_weak (int ignore ATTRIBUTE_UNUSED)
{
  char *name;
  int c;
  symbolS *symbolP;

  do
    {
      /* Get symbol name.  */
      name = input_line_pointer;
      c = get_symbol_end ();
      symbolP = symbol_find_or_make (name);
      S_SET_WEAK (symbolP);
      *input_line_pointer = c;
      SKIP_WHITESPACE ();

      if (c != ',')
        break;
      input_line_pointer++;
      SKIP_WHITESPACE ();
    }
  while (*input_line_pointer != '\n');
  demand_empty_rest_of_line ();
}

/* Parse:
   .section segname,sectname[,type[,attribute[,sizeof_stub]]]
*/

static void
obj_mach_o_section (int ignore ATTRIBUTE_UNUSED)
{
  char *p;
  char *segname;
  char *sectname;
  char c;
  int sectype = BFD_MACH_O_S_REGULAR;
  unsigned int secattr = 0;
  offsetT sizeof_stub = 0;
  const char *name;
  flagword oldflags, flags;
  asection *sec;

  /* Parse segment name.  */
  if (!is_name_beginner (*input_line_pointer))
    {
      as_bad (_("missing segment name"));
      ignore_rest_of_line ();
      return;
    }
  p = input_line_pointer;
  c = get_symbol_end ();
  segname = alloca (input_line_pointer - p + 1);
  strcpy (segname, p);
  *input_line_pointer = c;

  if (*input_line_pointer != ',')
    {
      as_bad (_("missing comma after segment name"));
      ignore_rest_of_line ();
      return;
    }
  input_line_pointer++;

  /* Parse section name.  */
  if (!is_name_beginner (*input_line_pointer))
    {
      as_bad (_("missing section name"));
      ignore_rest_of_line ();
      return;
    }
  p = input_line_pointer;
  c = get_symbol_end ();
  sectname = alloca (input_line_pointer - p + 1);
  strcpy (sectname, p);
  *input_line_pointer = c;

  /* Parse type.  */
  if (*input_line_pointer == ',')
    {
      input_line_pointer++;
      if (!is_name_beginner (*input_line_pointer))
        {
          as_bad (_("missing section type name"));
          ignore_rest_of_line ();
          return;
        }
      p = input_line_pointer;
      c = get_symbol_end ();

      sectype = bfd_mach_o_get_section_type_from_name (p);
      if (sectype == -1)
        {
          as_bad (_("unknown or invalid section type '%s'"), p);
          sectype = BFD_MACH_O_S_REGULAR;
        }
      *input_line_pointer = c;

      /* Parse attributes.  */
      if (*input_line_pointer == ',')
        {
          do
            {
              int attr;

              input_line_pointer++;

              if (!is_name_beginner (*input_line_pointer))
                {
                  as_bad (_("missing section attribute identifier"));
                  ignore_rest_of_line ();
                  break;
                }
              p = input_line_pointer;
              c = get_symbol_end ();

              attr = bfd_mach_o_get_section_attribute_from_name (p);
              if (attr == -1)
                as_bad (_("unknown or invalid section attribute '%s'"), p);
              else
                secattr |= attr;

              *input_line_pointer = c;
            }
          while (*input_line_pointer == '+');

          /* Parse sizeof_stub.  */
          if (*input_line_pointer == ',')
            {
              if (sectype != BFD_MACH_O_S_SYMBOL_STUBS)
                as_bad (_("unexpected sizeof_stub expression"));

              sizeof_stub = get_absolute_expression ();
            }
          else if (sectype == BFD_MACH_O_S_SYMBOL_STUBS)
            as_bad (_("missing sizeof_stub expression"));
        }
    }
  demand_empty_rest_of_line ();

  bfd_mach_o_normalize_section_name (segname, sectname, &name, &flags);
  if (name == NULL)
    {
      /* There is no normal BFD section name for this section.  Create one.
         The name created doesn't really matter as it will never be written
         on disk.  */
      size_t seglen = strlen (segname);
      size_t sectlen = strlen (sectname);
      char *n;

      n = xmalloc (seglen + 1 + sectlen + 1);
      memcpy (n, segname, seglen);
      n[seglen] = '.';
      memcpy (n + seglen + 1, sectname, sectlen);
      n[seglen + 1 + sectlen] = 0;
      name = n;
    }

#ifdef md_flush_pending_output
  md_flush_pending_output ();
#endif

  /* Sub-segments don't exists as is on Mach-O.  */
  sec = subseg_new (name, 0);

  oldflags = bfd_get_section_flags (stdoutput, sec);
  if (oldflags == SEC_NO_FLAGS)
    {
      bfd_mach_o_section *msect;

      if (! bfd_set_section_flags (stdoutput, sec, flags))
	as_warn (_("error setting flags for \"%s\": %s"),
		 bfd_section_name (stdoutput, sec),
		 bfd_errmsg (bfd_get_error ()));
      msect = bfd_mach_o_get_mach_o_section (sec);
      strncpy (msect->segname, segname, sizeof (msect->segname));
      msect->segname[16] = 0;
      strncpy (msect->sectname, sectname, sizeof (msect->sectname));
      msect->sectname[16] = 0;
      msect->flags = secattr | sectype;
      msect->reserved2 = sizeof_stub;
    }
  else if (flags != SEC_NO_FLAGS)
    {
      if (flags != oldflags)
	as_warn (_("Ignoring changed section attributes for %s"), name);
    }
}

struct known_section
{
  const char *name;
  unsigned int flags;
};

static const struct known_section known_sections[] =
  {
    /* 0 */ { NULL, 0},
    /* 1 */ { ".cstring", BFD_MACH_O_S_CSTRING_LITERALS }
  };

static void
obj_mach_o_known_section (int sect_index)
{
  const struct known_section *sect = &known_sections[sect_index];
  asection *old_sec;
  segT sec;

#ifdef md_flush_pending_output
  md_flush_pending_output ();
#endif

  old_sec = bfd_get_section_by_name (stdoutput, sect->name);
  if (old_sec)
    {
      /* Section already present.  */
      sec = old_sec;
      subseg_set (sec, 0);
    }
  else
    {
      bfd_mach_o_section *msect;

      sec = subseg_force_new (sect->name, 0);

      /* Set default flags.  */
      msect = bfd_mach_o_get_mach_o_section (sec);
      msect->flags = sect->flags;
    }
}

/* Called from read.c:s_comm after we've parsed .comm symbol, size.
   Parse a possible alignment value.  */

static symbolS *
obj_mach_o_common_parse (int ignore ATTRIBUTE_UNUSED,
                         symbolS *symbolP, addressT size)
{
  addressT align = 0;

  if (*input_line_pointer == ',')
    {
      align = parse_align (0);
      if (align == (addressT) -1)
	return NULL;
    }

  S_SET_VALUE (symbolP, size);
  S_SET_EXTERNAL (symbolP);
  S_SET_SEGMENT (symbolP, bfd_com_section_ptr);

  symbol_get_bfdsym (symbolP)->flags |= BSF_OBJECT;

  return symbolP;
}

static void
obj_mach_o_comm (int ignore ATTRIBUTE_UNUSED)
{
  s_comm_internal (ignore, obj_mach_o_common_parse);
}

static void
obj_mach_o_subsections_via_symbols (int arg ATTRIBUTE_UNUSED)
{
  /* Currently ignore it.  */
  demand_empty_rest_of_line ();
}

const pseudo_typeS mach_o_pseudo_table[] =
{
  { "weak", obj_mach_o_weak, 0},
  { "section", obj_mach_o_section, 0},
  { "cstring", obj_mach_o_known_section, 1},
  { "lcomm", s_lcomm, 1 },
  { "comm", obj_mach_o_comm, 0 },
  { "subsections_via_symbols", obj_mach_o_subsections_via_symbols, 0 },

  {NULL, NULL, 0}
};
