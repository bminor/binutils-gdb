/* Mach-O object file format
   Copyright 2009, 2011 Free Software Foundation, Inc.

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

/* Here we handle the mach-o directives that are common to all architectures.

   Most significant are mach-o named sections and a variety of symbol type
   decorations.  */

/* Mach-O supports multiple, named segments each of which may contain
   multiple named sections.  Thus the concept of subsectioning is 
   handled by (say) having a __TEXT segment with appropriate flags from
   which subsections are generated like __text, __const etc.  
   
   The well-known as short-hand section switch directives like .text, .data
   etc. are mapped onto predefined segment/section pairs using facilites
   supplied by the mach-o port of bfd.
   
   A number of additional mach-o short-hand section switch directives are
   also defined.  */

#define OBJ_HEADER "obj-macho.h"

#include "as.h"
#include "subsegs.h"
#include "symbols.h"
#include "write.h"
#include "mach-o.h"
#include "mach-o/loader.h"
#include "obj-macho.h"

/* TODO: Implement "-dynamic"/"-static" command line options.  */

static int obj_mach_o_is_static;

/* Allow for special re-ordering on output.  */

static int seen_objc_section;

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

/* This will put at most 16 characters (terminated by a ',' or newline) from
   the input stream into dest.  If there are more than 16 chars before the
   delimiter, a warning is given and the string is truncated.  On completion of
   this function, input_line_pointer will point to the char after the ',' or 
   to the newline.  
   
   It trims leading and trailing space.  */

static int
collect_16char_name (char *dest, const char *msg, int require_comma)
{
  char c, *namstart;

  SKIP_WHITESPACE ();
  namstart = input_line_pointer;

  while ( (c = *input_line_pointer) != ',' 
	 && !is_end_of_line[(unsigned char) c])
    input_line_pointer++;

  {
      int len = input_line_pointer - namstart; /* could be zero.  */
      /* lose any trailing space.  */  
      while (len > 0 && namstart[len-1] == ' ') 
        len--;
      if (len > 16)
        {
          *input_line_pointer = '\0'; /* make a temp string.  */
	  as_bad (_("the %s name '%s' is too long (maximum 16 characters)"),
		     msg, namstart);
	  *input_line_pointer = c; /* restore for printing.  */
	  len = 16;
	}
      if (len > 0)
        memcpy (dest, namstart, len);
  }

  if (c != ',' && require_comma)
    {
      as_bad (_("expected a %s name followed by a `,'"), msg);
      return 1;
    }

  return 0;
}

/* .section

   The '.section' specification syntax looks like:
   .section <segment> , <section> [, type [, attribs [, size]]]

   White space is allowed everywhere between elements.

   <segment> and <section> may be from 0 to 16 chars in length - they may
   contain spaces but leading and trailing space will be trimmed.  It is 
   mandatory that they be present (or that zero-length names are indicated
   by ",,").

   There is only a single section type for any entry.

   There may be multiple attributes, they are delimited by `+'.

   Not all section types and attributes are accepted by the Darwin system
   assemblers as user-specifiable - although, at present, we do here.  */

static void
obj_mach_o_section (int ignore ATTRIBUTE_UNUSED)
{
  char *p;
  char c;
  unsigned int sectype = BFD_MACH_O_S_REGULAR;
  unsigned int defsectype = BFD_MACH_O_S_REGULAR;
  unsigned int sectype_given = 0;
  unsigned int secattr = 0;
  unsigned int defsecattr = 0;
  int secattr_given = 0;
  unsigned int secalign = 0;
  offsetT sizeof_stub = 0;
  const mach_o_section_name_xlat * xlat;
  const char *name;
  flagword oldflags, flags;
  asection *sec;
  bfd_mach_o_section *msect;
  char segname[17];
  char sectname[17];

  /* Zero-length segment and section names are allowed.  */
  /* Parse segment name.  */
  memset (segname, 0, sizeof(segname));
  if (collect_16char_name (segname, "segment", 1))
    {
      ignore_rest_of_line ();
      return;
    }
  input_line_pointer++; /* Skip the terminating ',' */

  /* Parse section name.  */
  memset (sectname, 0, sizeof(sectname));
  collect_16char_name (sectname, "section", 0);

  /* Parse type.  */
  if (*input_line_pointer == ',')
    {
      char tmpc;
      int len;
      input_line_pointer++;
      SKIP_WHITESPACE ();
      p = input_line_pointer;
      while ((c = *input_line_pointer) != ','
	      && !is_end_of_line[(unsigned char) c])
	input_line_pointer++;

      len = input_line_pointer - p;
      /* strip trailing spaces.  */
      while (len > 0 && p[len-1] == ' ')
	len--;
      tmpc = p[len];

      /* Temporarily make a string from the token.  */
      p[len] = 0;
      sectype = bfd_mach_o_get_section_type_from_name (p);
      if (sectype > 255) /* Max Section ID == 255.  */
        {
          as_bad (_("unknown or invalid section type '%s'"), p);
          sectype = BFD_MACH_O_S_REGULAR;
        }
      else
	sectype_given = 1;
      /* Restore.  */
      tmpc = p[len];

      /* Parse attributes.
	 TODO: check validity of attributes for section type.  */
      if (sectype_given && c == ',')
        {
          do
            {
              int attr;

	      /* Skip initial `,' and subsequent `+'.  */
              input_line_pointer++;
	      SKIP_WHITESPACE ();
	      p = input_line_pointer;
	      while ((c = *input_line_pointer) != '+'
		      && c != ','
		      && !is_end_of_line[(unsigned char) c])
		input_line_pointer++;

	      len = input_line_pointer - p;
	      /* strip trailing spaces.  */
	      while (len > 0 && p[len-1] == ' ')
		len--;
	      tmpc = p[len];

	      /* Temporarily make a string from the token.  */
	      p[len] ='\0';
              attr = bfd_mach_o_get_section_attribute_from_name (p);
	      if (attr == -1)
                as_bad (_("unknown or invalid section attribute '%s'"), p);
              else
		{
		  secattr_given = 1;
                  secattr |= attr;
		}
	      /* Restore.  */
	      p[len] = tmpc;
            }
          while (*input_line_pointer == '+');

          /* Parse sizeof_stub.  */
          if (*input_line_pointer == ',')
            {
              if (sectype != BFD_MACH_O_S_SYMBOL_STUBS)
                as_bad (_("unexpected sizeof_stub expression"));

	      input_line_pointer++;
              sizeof_stub = get_absolute_expression ();
            }
          else if (sectype == BFD_MACH_O_S_SYMBOL_STUBS)
            as_bad (_("missing sizeof_stub expression"));
        }
    }
  demand_empty_rest_of_line ();

  flags = SEC_NO_FLAGS;
  /* This provides default bfd flags and default mach-o section type and
     attributes along with the canonical name.  */
  xlat = bfd_mach_o_section_data_for_mach_sect (stdoutput, segname, sectname);
  if (xlat != NULL)
    {
      name = xstrdup (xlat->bfd_name);
      flags = xlat->bfd_flags;
      defsectype = xlat->macho_sectype;
      defsecattr = xlat->macho_secattr;
      secalign = xlat->sectalign;
    }
  else
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
  msect = bfd_mach_o_get_mach_o_section (sec);
   if (oldflags == SEC_NO_FLAGS)
    {
      if (! bfd_set_section_flags (stdoutput, sec, flags))
	as_warn (_("error setting flags for \"%s\": %s"),
		 bfd_section_name (stdoutput, sec),
		 bfd_errmsg (bfd_get_error ()));
      strncpy (msect->segname, segname, sizeof (msect->segname));
      msect->segname[16] = 0;
      strncpy (msect->sectname, sectname, sizeof (msect->sectname));
      msect->sectname[16] = 0;
      msect->align = secalign;
      if (sectype_given)
	{
	  msect->flags = sectype;
	  if (secattr_given)
	    msect->flags |= secattr;
	  else
	    msect->flags |= defsecattr;
	}
      else
        msect->flags = defsectype | defsecattr;
      msect->reserved2 = sizeof_stub;
    }
  else if (flags != SEC_NO_FLAGS)
    {
      if (flags != oldflags
	  || msect->flags != (secattr | sectype))
	as_warn (_("Ignoring changed section attributes for %s"), name);
    }
}

static segT 
obj_mach_o_segT_from_bfd_name (const char *nam, int must_succeed)
{
  const mach_o_section_name_xlat *xlat;
  const char *segn;
  segT sec;

  /* BFD has tables of flags and default attributes for all the sections that
     have a 'canonical' name.  */
  xlat = bfd_mach_o_section_data_for_bfd_name (stdoutput, nam, &segn);
  if (xlat == NULL)
    {
      if (must_succeed)
	as_fatal (_("BFD is out of sync with GAS, "
		     "unhandled well-known section type `%s'"), nam);
      return NULL;
    }

  sec = bfd_get_section_by_name (stdoutput, nam);
  if (sec == NULL)
    {
      bfd_mach_o_section *msect;

      sec = subseg_force_new (xlat->bfd_name, 0);

      /* Set default type, attributes and alignment.  */
      msect = bfd_mach_o_get_mach_o_section (sec);
      msect->flags = xlat->macho_sectype | xlat->macho_secattr;
      msect->align = xlat->sectalign;

      if ((msect->flags & BFD_MACH_O_SECTION_TYPE_MASK) 
	  == BFD_MACH_O_S_ZEROFILL)
	seg_info (sec)->bss = 1;
    }

  return sec;
}

static const char * const known_sections[] =
{
  /*  0 */ NULL,
  /* __TEXT */
  /*  1 */ ".const",
  /*  2 */ ".static_const",
  /*  3 */ ".cstring",
  /*  4 */ ".literal4",
  /*  5 */ ".literal8",
  /*  6 */ ".literal16",
  /*  7 */ ".constructor",
  /*  8 */ ".destructor",
  /*  9 */ ".eh_frame",
  /* __DATA */
  /* 10 */ ".const_data",
  /* 11 */ ".static_data",
  /* 12 */ ".mod_init_func",
  /* 13 */ ".mod_term_func",
  /* 14 */ ".dyld",
  /* 15 */ ".cfstring"
};

/* Interface for a known non-optional section directive.  */

static void
obj_mach_o_known_section (int sect_index)
{
  segT section;

#ifdef md_flush_pending_output
  md_flush_pending_output ();
#endif

  section = obj_mach_o_segT_from_bfd_name (known_sections[sect_index], 1);
  if (section != NULL)
    subseg_set (section, 0);

  /* else, we leave the section as it was; there was a fatal error anyway.  */
}

static const char * const objc_sections[] =
{
  /*  0 */ NULL,
  /*  1 */ ".objc_class",
  /*  2 */ ".objc_meta_class",
  /*  3 */ ".objc_cat_cls_meth",
  /*  4 */ ".objc_cat_inst_meth",
  /*  5 */ ".objc_protocol",
  /*  6 */ ".objc_string_object",
  /*  7 */ ".objc_cls_meth",
  /*  8 */ ".objc_inst_meth",
  /*  9 */ ".objc_cls_refs",
  /* 10 */ ".objc_message_refs",
  /* 11 */ ".objc_symbols",
  /* 12 */ ".objc_category",
  /* 13 */ ".objc_class_vars",
  /* 14 */ ".objc_instance_vars",
  /* 15 */ ".objc_module_info",
  /* 16 */ ".cstring", /* objc_class_names Alias for .cstring */
  /* 17 */ ".cstring", /* Alias objc_meth_var_types for .cstring */
  /* 18 */ ".cstring", /* objc_meth_var_names Alias for .cstring */
  /* 19 */ ".objc_selector_strs",
  /* 20 */ ".objc_image_info", /* extension.  */
  /* 21 */ ".objc_selector_fixup", /* extension.  */
  /* 22 */ ".objc1_class_ext", /* ObjC-1 extension.  */
  /* 23 */ ".objc1_property_list", /* ObjC-1 extension.  */
  /* 24 */ ".objc1_protocol_ext" /* ObjC-1 extension.  */
};

/* This currently does the same as known_sections, but kept separate for
   ease of maintenance.  */

static void
obj_mach_o_objc_section (int sect_index)
{
  segT section;
  
#ifdef md_flush_pending_output
  md_flush_pending_output ();
#endif

  section = obj_mach_o_segT_from_bfd_name (objc_sections[sect_index], 1);
  if (section != NULL)
    {
      seen_objc_section = 1; /* We need to ensure that certain sections are
				present and in the right order.  */
      subseg_set (section, 0);
    }

  /* else, we leave the section as it was; there was a fatal error anyway.  */
}

/* Debug section directives.  */

static const char * const debug_sections[] =
{
  /*  0 */ NULL,
  /* __DWARF */
  /*  1 */ ".debug_frame",
  /*  2 */ ".debug_info",
  /*  3 */ ".debug_abbrev",
  /*  4 */ ".debug_aranges",
  /*  5 */ ".debug_macinfo",
  /*  6 */ ".debug_line",
  /*  7 */ ".debug_loc",
  /*  8 */ ".debug_pubnames",
  /*  9 */ ".debug_pubtypes",
  /* 10 */ ".debug_str",
  /* 11 */ ".debug_ranges",
  /* 12 */ ".debug_macro"
};

/* ??? Maybe these should be conditional on gdwarf-*.
   It`s also likely that we will need to be able to set them from the cfi
   code.  */

static void
obj_mach_o_debug_section (int sect_index)
{
  segT section;

#ifdef md_flush_pending_output
  md_flush_pending_output ();
#endif

  section = obj_mach_o_segT_from_bfd_name (debug_sections[sect_index], 1);
  if (section != NULL)
    subseg_set (section, 0);

  /* else, we leave the section as it was; there was a fatal error anyway.  */
}

/* This could be moved to the tc-xx files, but there is so little dependency
   there, that the code might as well be shared.  */

struct opt_tgt_sect 
{
 const char *name;
 unsigned x86_val;
 unsigned ppc_val;
};

/* The extensions here are for specific sections that are generated by GCC
   and Darwin system tools, but don't have directives in the `system as'.  */

static const struct opt_tgt_sect tgt_sections[] =
{
  /*  0 */ { NULL, 0, 0},
  /*  1 */ { ".lazy_symbol_pointer", 0, 0},
  /*  2 */ { ".lazy_symbol_pointer2", 0, 0}, /* X86 - extension */
  /*  3 */ { ".lazy_symbol_pointer3", 0, 0}, /* X86 - extension */
  /*  4 */ { ".non_lazy_symbol_pointer", 0, 0},
  /*  5 */ { ".non_lazy_symbol_pointer_x86", 0, 0}, /* X86 - extension */
  /*  6 */ { ".symbol_stub", 16, 20},
  /*  7 */ { ".symbol_stub1", 0, 16}, /* PPC - extension */
  /*  8 */ { ".picsymbol_stub", 26, 36},
  /*  9 */ { ".picsymbol_stub1", 0, 32}, /* PPC - extension */
  /* 10 */ { ".picsymbol_stub2", 25, 0}, /* X86 - extension */
  /* 11 */ { ".picsymbol_stub3", 5, 0}, /* X86 - extension  */
};

/* Interface for an optional section directive.  */

static void
obj_mach_o_opt_tgt_section (int sect_index)
{
  const struct opt_tgt_sect *tgtsct = &tgt_sections[sect_index];
  segT section;

#ifdef md_flush_pending_output
  md_flush_pending_output ();
#endif

  section = obj_mach_o_segT_from_bfd_name (tgtsct->name, 0);
  if (section == NULL)
    {
      as_bad (_("%s is not used for the selected target"), tgtsct->name);
      /* Leave the section as it is.  */
    }
  else
    {
      bfd_mach_o_section *mo_sec = bfd_mach_o_get_mach_o_section (section);
      subseg_set (section, 0);
#if defined (TC_I386)
      mo_sec->reserved2 = tgtsct->x86_val;
#elif defined (TC_PPC)
      mo_sec->reserved2 = tgtsct->ppc_val;
#else
      mo_sec->reserved2 = 0;
#endif
    }
}

/* We don't necessarily have the three 'base' sections on mach-o.
   Normally, we would start up with only the 'text' section defined.
   However, even that can be suppressed with (TODO) c/l option "-n".
   Thus, we have to be able to create all three sections on-demand.  */

static void
obj_mach_o_base_section (int sect_index)
{
  segT section;

#ifdef md_flush_pending_output
  md_flush_pending_output ();
#endif

  /* We don't support numeric (or any other) qualifications on the
     well-known section shorthands.  */
  demand_empty_rest_of_line ();

  switch (sect_index)
    {
      /* Handle the three sections that are globally known within GAS.
	 For Mach-O, these are created on demand rather than at startup.  */
      case 1:
	if (text_section == NULL)
	  text_section = obj_mach_o_segT_from_bfd_name (TEXT_SECTION_NAME, 1);
	if (obj_mach_o_is_static)
	  {
	    bfd_mach_o_section *mo_sec
		= bfd_mach_o_get_mach_o_section (text_section);
	    mo_sec->flags &= ~BFD_MACH_O_S_ATTR_PURE_INSTRUCTIONS;
	  }
	section = text_section;
	break;
      case 2:
	if (data_section == NULL)
	  data_section = obj_mach_o_segT_from_bfd_name (DATA_SECTION_NAME, 1);
	section = data_section;
	break;
      case 3:
        /* ??? maybe this achieves very little, as an addition.  */
	if (bss_section == NULL)
	  {
	    bss_section = obj_mach_o_segT_from_bfd_name (BSS_SECTION_NAME, 1);
	    seg_info (bss_section)->bss = 1;
	  }
	section = bss_section;
	break;
      default:
        as_fatal (_("internal error: base section index out of range"));
        return;
	break;
    }
  subseg_set (section, 0);
}

/* This finishes off parsing a .comm or .lcomm statement, which both can have
   an (optional) alignment field.  It also allows us to create the bss section
   on demand.  */

static symbolS *
obj_mach_o_common_parse (int is_local, symbolS *symbolP,
			 addressT size)
{
  addressT align = 0;

  /* Both comm and lcomm take an optional alignment, as a power
     of two between 1 and 15.  */
  if (*input_line_pointer == ',')
    {
      /* We expect a power of 2.  */
      align = parse_align (0);
      if (align == (addressT) -1)
	return NULL;
      if (align > 15)
	{
	  as_warn (_("Alignment (%lu) too large: 15 assumed."),
		  (unsigned long)align);
	  align = 15;
	}
    }

  if (is_local)
    {
      /* Create the BSS section on demand.  */
      if (bss_section == NULL)
	{
	  bss_section = obj_mach_o_segT_from_bfd_name (BSS_SECTION_NAME, 1);
	  seg_info (bss_section)->bss = 1;	  
	}
      bss_alloc (symbolP, size, align);
      S_CLEAR_EXTERNAL (symbolP);
    }
  else
    {
      S_SET_VALUE (symbolP, size);
      S_SET_ALIGN (symbolP, align);
      S_SET_EXTERNAL (symbolP);
      S_SET_SEGMENT (symbolP, bfd_com_section_ptr);
    }

  symbol_get_bfdsym (symbolP)->flags |= BSF_OBJECT;

  return symbolP;
}

static void
obj_mach_o_comm (int is_local)
{
  s_comm_internal (is_local, obj_mach_o_common_parse);
}

static void
obj_mach_o_subsections_via_symbols (int arg ATTRIBUTE_UNUSED)
{
  /* Currently ignore it.  */
  demand_empty_rest_of_line ();
}

/* Dummy function to allow test-code to work while we are working
   on things.  */

static void
obj_mach_o_placeholder (int arg ATTRIBUTE_UNUSED)
{
  ignore_rest_of_line ();
}

const pseudo_typeS mach_o_pseudo_table[] =
{
  /* Section directives.  */
  { "comm", obj_mach_o_comm, 0 },
  { "lcomm", obj_mach_o_comm, 1 },

  { "text", obj_mach_o_base_section, 1},
  { "data", obj_mach_o_base_section, 2},
  { "bss", obj_mach_o_base_section, 3},   /* extension */

  { "const", obj_mach_o_known_section, 1},
  { "static_const", obj_mach_o_known_section, 2},
  { "cstring", obj_mach_o_known_section, 3},
  { "literal4", obj_mach_o_known_section, 4},
  { "literal8", obj_mach_o_known_section, 5},
  { "literal16", obj_mach_o_known_section, 6},
  { "constructor", obj_mach_o_known_section, 7},
  { "destructor", obj_mach_o_known_section, 8},
  { "eh_frame", obj_mach_o_known_section, 9},

  { "const_data", obj_mach_o_known_section, 10},
  { "static_data", obj_mach_o_known_section, 11},
  { "mod_init_func", obj_mach_o_known_section, 12},
  { "mod_term_func", obj_mach_o_known_section, 13},
  { "dyld", obj_mach_o_known_section, 14},
  { "cfstring", obj_mach_o_known_section, 15},

  { "objc_class", obj_mach_o_objc_section, 1},
  { "objc_meta_class", obj_mach_o_objc_section, 2},
  { "objc_cat_cls_meth", obj_mach_o_objc_section, 3},
  { "objc_cat_inst_meth", obj_mach_o_objc_section, 4},
  { "objc_protocol", obj_mach_o_objc_section, 5},
  { "objc_string_object", obj_mach_o_objc_section, 6},
  { "objc_cls_meth", obj_mach_o_objc_section, 7},
  { "objc_inst_meth", obj_mach_o_objc_section, 8},
  { "objc_cls_refs", obj_mach_o_objc_section, 9},
  { "objc_message_refs", obj_mach_o_objc_section, 10},
  { "objc_symbols", obj_mach_o_objc_section, 11},
  { "objc_category", obj_mach_o_objc_section, 12},
  { "objc_class_vars", obj_mach_o_objc_section, 13},
  { "objc_instance_vars", obj_mach_o_objc_section, 14},
  { "objc_module_info", obj_mach_o_objc_section, 15},
  { "objc_class_names", obj_mach_o_objc_section, 16}, /* Alias for .cstring */
  { "objc_meth_var_types", obj_mach_o_objc_section, 17}, /* Alias for .cstring */
  { "objc_meth_var_names", obj_mach_o_objc_section, 18}, /* Alias for .cstring */
  { "objc_selector_strs", obj_mach_o_objc_section, 19},
  { "objc_image_info", obj_mach_o_objc_section, 20}, /* extension.  */
  { "objc_selector_fixup", obj_mach_o_objc_section, 21}, /* extension.  */
  { "objc1_class_ext", obj_mach_o_objc_section, 22}, /* ObjC-1 extension.  */
  { "objc1_property_list", obj_mach_o_objc_section, 23}, /* ObjC-1 extension.  */
  { "objc1_protocol_ext", obj_mach_o_objc_section, 24}, /* ObjC-1 extension.  */

  { "debug_frame", obj_mach_o_debug_section, 1}, /* extension.  */
  { "debug_info", obj_mach_o_debug_section, 2}, /* extension.  */
  { "debug_abbrev", obj_mach_o_debug_section, 3}, /* extension.  */
  { "debug_aranges", obj_mach_o_debug_section, 4}, /* extension.  */
  { "debug_macinfo", obj_mach_o_debug_section, 5}, /* extension.  */
  { "debug_line", obj_mach_o_debug_section, 6}, /* extension.  */
  { "debug_loc", obj_mach_o_debug_section, 7}, /* extension.  */
  { "debug_pubnames", obj_mach_o_debug_section, 8}, /* extension.  */
  { "debug_pubtypes", obj_mach_o_debug_section, 9}, /* extension.  */
  { "debug_str", obj_mach_o_debug_section, 10}, /* extension.  */
  { "debug_ranges", obj_mach_o_debug_section, 11}, /* extension.  */
  { "debug_macro", obj_mach_o_debug_section, 12}, /* extension.  */
  
  { "lazy_symbol_pointer", obj_mach_o_opt_tgt_section, 1},
  { "lazy_symbol_pointer2", obj_mach_o_opt_tgt_section, 2}, /* extension.  */
  { "lazy_symbol_pointer3", obj_mach_o_opt_tgt_section, 3}, /* extension.  */
  { "non_lazy_symbol_pointer", obj_mach_o_opt_tgt_section, 4},
  { "non_lazy_symbol_pointer_x86", obj_mach_o_opt_tgt_section, 5}, /* extension.  */
  { "symbol_stub", obj_mach_o_opt_tgt_section, 6},
  { "symbol_stub1", obj_mach_o_opt_tgt_section, 7}, /* extension.  */
  { "picsymbol_stub", obj_mach_o_opt_tgt_section, 8}, /* extension.  */
  { "picsymbol_stub1", obj_mach_o_opt_tgt_section, 9}, /* extension.  */
  { "picsymbol_stub2", obj_mach_o_opt_tgt_section, 4}, /* extension.  */
  { "picsymbol_stub3", obj_mach_o_opt_tgt_section, 4}, /* extension.  */

  { "section", obj_mach_o_section, 0},

  /* Symbol-related.  */
  { "indirect_symbol", obj_mach_o_placeholder, 0},
  { "weak_definition", obj_mach_o_placeholder, 0},
  { "private_extern", obj_mach_o_placeholder, 0},
  { "weak", obj_mach_o_weak, 0},   /* extension */

  /* File flags.  */
  { "subsections_via_symbols", obj_mach_o_subsections_via_symbols, 0 },

  {NULL, NULL, 0}
};
