/*  This file is part of the program psim.

    Copyright (C) 1994-1997, Andrew Cagney <cagney@highland.com.au>

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

#include "ld-decode.h"
#include "ld-cache.h"
#include "ld-insn.h"

#include "igen.h"

#include "gen-semantics.h"
#include "gen-support.h"

static void
print_support_function_name(lf *file,
			    table_entry *function,
			    int is_function_definition)
{
  if (it_is("internal", function->fields[insn_flags])) {
    lf_print_function_type_function(file, print_semantic_function_type, "INLINE_SUPPORT",
				    (is_function_definition ? "\n" : " "));
    print_function_name(file,
			function->fields[function_name],
			NULL,
			function_name_prefix_semantics);
    lf_printf(file, "\n(");
    print_semantic_function_formal(file);
    lf_printf(file, ")");
    if (!is_function_definition)
      lf_printf(file, ";");
    lf_printf(file, "\n");
  }
  else {
    /* map the name onto a globally valid name */
    if (!is_function_definition && strcmp(global_name_prefix, "") != 0) {
      lf_indent_suppress(file);
      lf_printf(file, "#define %s %s%s\n",
		function->fields[function_name],
		global_name_prefix,
		function->fields[function_name]);
    }
    lf_print_function_type(file,
			   function->fields[function_type],
			   "INLINE_SUPPORT",
			   (is_function_definition ? "\n" : " "));
    lf_printf(file, "%s%s\n(",
	      global_name_prefix,
	      function->fields[function_name]);
    if (generate_smp)
      lf_printf(file, "sim_cpu *cpu");
    else
      lf_printf(file, "SIM_DESC sd");
    if (strcmp(function->fields[function_param], "") != 0)
      lf_printf(file, ", %s", function->fields[function_param]);
    lf_printf(file, ")%s", (is_function_definition ? "\n" : ";\n"));
  }
}


static void
support_h_function(insn_table *entry,
		   lf *file,
		   void *data,
		   table_entry *function)
{
  ASSERT(function->fields[function_type] != NULL);
  ASSERT(function->fields[function_param] != NULL);
  print_support_function_name(file,
			      function,
			      0/*!is_definition*/);
  lf_printf(file, "\n");
}


extern void
gen_support_h(insn_table *table,
	      lf *file)
{
  /* output the definition of `_SD'*/
  if (generate_smp) {
    lf_printf(file, "#define _SD cpu\n");
    lf_printf(file, "#define SD cpu->sd\n");
    lf_printf(file, "#define CPU cpu\n");
  }
  else {
    lf_printf(file, "#define _SD sd\n");
    lf_printf(file, "#define SD sd\n");
    lf_printf(file, "#define CPU (&sd->cpu)\n");
  }
  lf_printf(file, "\n");
  /* output a declaration for all functions */
  insn_table_traverse_function(table,
			       file, NULL,
			       support_h_function);
  lf_printf(file, "\n");
  lf_printf(file, "#if defined(SUPPORT_INLINE)\n");
  lf_printf(file, "# if ((SUPPORT_INLINE & INCLUDE_MODULE)\\\n");
  lf_printf(file, "      && (SUPPORT_INLINE & INCLUDED_BY_MODULE))\n");
  lf_printf(file, "#  include \"%ssupport.c\"\n", global_name_prefix);
  lf_printf(file, "# endif\n");
  lf_printf(file, "#endif\n");
}

static void
support_c_function(insn_table *table,
		   lf *file,
		   void *data,
		   table_entry *function)
{
  ASSERT (function->fields[function_type] != NULL);
  print_support_function_name (file,
			       function,
			       1/*!is_definition*/);
  table_entry_print_cpp_line_nr (file, function);
  lf_printf (file, "{\n");
  lf_indent (file, +2);
  if (function->annex == NULL)
    error ("%s:%d: Function without body (or null statement)",
	   function->file_name,
	   function->line_nr);
    lf_print__c_code (file, function->annex);
  if (it_is ("internal", function->fields[insn_flags]))
    {
      lf_printf (file, "sim_io_error (sd, \"Internal function must longjump\\n\");\n");
      lf_printf (file, "return cia;\n");
    }
  lf_indent (file, -2);
  lf_printf (file, "}\n");
  lf_print__internal_reference (file);
  lf_printf (file, "\n");
}


void
gen_support_c(insn_table *table,
	      lf *file)
{
  lf_printf(file, "#include \"sim-main.h\"\n");
  lf_printf(file, "#include \"%sidecode.h\"\n", global_name_prefix);
  lf_printf(file, "#include \"%ssupport.h\"\n", global_name_prefix);
  lf_printf(file, "\n");

  /* output a definition (c-code) for all functions */
  insn_table_traverse_function(table,
			       file, NULL,
			       support_c_function);
}
