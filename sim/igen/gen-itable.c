/*  This file is part of the program psim.

    Copyright (C) 1994-1995, Andrew Cagney <cagney@highland.com.au>

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */



#include "misc.h"
#include "lf.h"
#include "table.h"
#include "filter.h"
#include "igen.h"

#include "ld-insn.h"
#include "ld-decode.h"

#include "gen.h"

#include "gen-itable.h"

#ifndef NULL
#define NULL 0
#endif



static void
itable_h_insn (lf *file,
	       insn_table *entry,
	       insn_entry *instruction,
	       void *data)
{
  lf_print__line_ref (file, instruction->line);
  lf_printf (file, "  ");
  print_function_name (file,
		       instruction->name,
		       instruction->format_name,
		       NULL,
		       NULL,
		       function_name_prefix_itable);
  lf_printf (file, ",\n");
}


/* print the list of all the different options */

static void
itable_print_enum (lf *file,
		   filter *set,
		   char *name)
{
  char *elem;
  lf_printf (file, "typedef enum {\n");
  lf_indent (file, +2);
  for (elem = filter_next (set, "");
       elem != NULL;
       elem = filter_next (set, elem))
    {
      lf_printf (file, "%sitable_%s_%s,\n",
		 options.prefix.itable.name, name, elem);
      if (strlen (options.prefix.itable.name) > 0)
	{
	  lf_indent_suppress (file);
	  lf_printf (file, "#define itable_%s_%s %sitable_%s_%s\n",
		     name, elem, options.prefix.itable.name, name, elem);
	}
    }
  lf_printf (file, "nr_%sitable_%ss,", options.prefix.itable.name, name);
  
  lf_indent (file, -2);
  lf_printf (file, "\n} %sitable_%ss;\n", options.prefix.itable.name, name);
  if (strlen (options.prefix.itable.name) > 0)
    {
      lf_indent_suppress (file);
      lf_printf (file, "#define itable_%ss %sitable_%ss\n",
		 name, options.prefix.itable.name, name);
      lf_indent_suppress (file);
      lf_printf (file, "#define nr_itable_%ss nr_%sitable_%ss\n",
		 name, options.prefix.itable.name, name);
    }
  lf_printf (file, "\n");
}

extern void 
gen_itable_h (lf *file,
	      insn_table *isa)
{

  /* output an enumerated type for each instruction */
  lf_printf (file, "typedef enum {\n");
  insn_table_traverse_insn (file, isa, itable_h_insn, NULL);
  lf_printf (file, "  nr_%sitable_entries,\n", options.prefix.itable.name);
  lf_printf (file, "} %sitable_index;\n", options.prefix.itable.name);
  lf_printf (file, "\n");

  /* output an enumeration type for each flag */
  itable_print_enum (file, isa->flags, "flag");
    
  /* output an enumeration of all the possible options */
  itable_print_enum (file, isa->options, "option");

  /* output an enumeration of all the processor models */
  itable_print_enum (file, isa->model->processors, "processor");

  /* output the table that contains the actual instruction info */
  lf_printf (file, "typedef struct _%sitable_instruction_info {\n",
	     options.prefix.itable.name);
  lf_printf (file, "  %sitable_index nr;\n", options.prefix.itable.name);
  lf_printf (file, "  char *format;\n");
  lf_printf (file, "  char *form;\n");
  lf_printf (file, "  char *flags;\n");
  lf_printf (file, "  char flag[nr_%sitable_flags];\n",
	     options.prefix.itable.name);
  lf_printf (file, "  char *options;\n");
  lf_printf (file, "  char option[nr_%sitable_options];\n",
	     options.prefix.itable.name);
  lf_printf (file, "  char *processors;\n");
  lf_printf (file, "  char processor[nr_%sitable_processors];\n",
	     options.prefix.itable.name);
  lf_printf (file, "  char *name;\n");
  lf_printf (file, "  char *file;\n");
  lf_printf (file, "  int line_nr;\n");
  lf_printf (file, "} %sitable_info;\n", options.prefix.itable.name);
  lf_printf (file, "\n");
  lf_printf (file, "extern %sitable_info %sitable[nr_%sitable_entries];\n",
	     options.prefix.itable.name, options.prefix.itable.name,
	     options.prefix.itable.name);
  if (strlen (options.prefix.itable.name) > 0)
    {
      lf_indent_suppress (file);
      lf_printf (file, "#define itable %sitable\n",
		 options.prefix.itable.name);
    }
}


/****************************************************************/

static void
itable_print_set (lf *file,
		  filter *set,
		  filter *members)
{
  char *elem;
  lf_printf (file, "\"");
  elem = filter_next (members, "");
  if (elem != NULL)
    {
      while (1)
	{
	  lf_printf (file, "%s", elem);
	  elem = filter_next (members, elem);
	  if (elem == NULL)
	    break;
	  lf_printf (file, ",");
	}
    }
  lf_printf (file, "\",\n");

  lf_printf(file, "{");
  for (elem = filter_next (set, "");
       elem != NULL;
       elem = filter_next (set, elem))
    {
      if (filter_is_member (members, elem))
	{
	  lf_printf (file, " 1,");
	}
      else
	{
	  lf_printf (file, " 0,");
	}
      
    }
  lf_printf(file, " },\n");
}


static void
itable_c_insn (lf *file,
	       insn_table *isa,
	       insn_entry *instruction,
	       void *data)
{
  lf_printf (file, "{ ");
  lf_indent (file, +2);
  print_function_name (file,
		       instruction->name,
		       instruction->format_name,
		       NULL,
		       NULL,
		       function_name_prefix_itable);
  lf_printf (file, ",\n");
  lf_printf (file, "\"");
  print_insn_words (file, instruction);
  lf_printf (file, "\",\n");
  lf_printf (file, "\"%s\",\n", instruction->format_name);

  itable_print_set (file, isa->flags, instruction->flags);
  itable_print_set (file, isa->options, instruction->options);
  itable_print_set (file, isa->model->processors, instruction->processors);

  lf_printf(file, "\"%s\",\n", instruction->name);
  lf_printf(file, "\"%s\",\n",
	    filter_filename (instruction->line->file_name));
  lf_printf(file, "%d,\n", instruction->line->line_nr);
  lf_printf(file, "},\n");
  lf_indent (file, -2);
}


extern void 
gen_itable_c (lf *file,
	      insn_table *isa)
{
  /* leader */
  lf_printf(file, "#include \"%sitable.h\"\n", options.prefix.itable.name);
  lf_printf(file, "\n");

  /* FIXME - output model data??? */
  /* FIXME - output assembler data??? */

  /* output the table that contains the actual instruction info */
  lf_printf(file, "%sitable_info %sitable[nr_%sitable_entries] = {\n",
	    options.prefix.itable.name,
	    options.prefix.itable.name,
	    options.prefix.itable.name);
  insn_table_traverse_insn (file, isa, itable_c_insn, NULL);

  lf_printf(file, "};\n");
}
