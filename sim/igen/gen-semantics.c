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
#include "gen-icache.h"
#include "gen-idecode.h"


static void
print_semantic_function_header(lf *file,
			       const char *basename,
			       insn_bits *expanded_bits,
			       int is_function_definition)
{
  int indent;
  lf_printf(file, "\n");
  lf_print_function_type_function(file, print_semantic_function_type, "EXTERN_SEMANTICS",
				  (is_function_definition ? "\n" : " "));
  indent = print_function_name(file,
			       basename,
			       expanded_bits,
			       function_name_prefix_semantics);
  if (is_function_definition)
    lf_indent(file, +indent);
  else
    lf_printf(file, "\n");
  lf_printf(file, "(");
  print_semantic_function_formal(file);
  lf_printf(file, ")");
  if (is_function_definition)
    lf_indent(file, -indent);
  else
    lf_printf(file, ";");
  lf_printf(file, "\n");
}

void
print_semantic_declaration(insn_table *entry,
			   lf *file,
			   void *data,
			   insn *instruction,
			   int depth)
{
  if (generate_expanded_instructions) {
    ASSERT(entry->nr_insn == 1);
    print_semantic_function_header(file,
				   instruction->file_entry->fields[insn_name],
				   entry->expanded_bits,
				   0/* is not function definition*/);
  }
  else {
    print_semantic_function_header(file,
				   instruction->file_entry->fields[insn_name],
				   NULL,
				   0/* is not function definition*/);
  }
}



/* generate the semantics.c file */


void
print_idecode_invalid(lf *file,
		      const char *result,
		      invalid_type type)
{
  const char *name;
  switch (type) {
  default: name = "unknown"; break;
  case invalid_illegal: name = "illegal"; break;
  case invalid_fp_unavailable: name = "fp_unavailable"; break;
  case invalid_wrong_slot: name = "wrong_slot"; break;
  }
  if ((code & generate_jumps))
    lf_printf(file, "goto %s_%s;\n",
	      (code & generate_with_icache) ? "icache" : "semantic",
	      name);
  else if ((code & generate_with_icache)) {
    lf_printf(file, "%s %sicache_%s(", result, global_name_prefix, name);
    print_icache_function_actual(file);
    lf_printf(file, ");\n");
  }
  else {
    lf_printf(file, "%s %ssemantic_%s(", result, global_name_prefix, name);
    print_semantic_function_actual(file);
    lf_printf(file, ");\n");
  }
}


void
print_semantic_body(lf *file,
		    insn *instruction,
		    insn_bits *expanded_bits,
		    opcode_field *opcodes)
{
  print_itrace(file, instruction->file_entry, 0/*put_value_in_cache*/);

  /* validate the instruction, if a cache this has already been done */
  if (!(code & generate_with_icache))
    print_idecode_validate(file, instruction, opcodes);

  /* generate the profiling call - this is delayed until after the
     instruction has been verified */
  lf_printf(file, "\n");
  lf_indent_suppress(file);
  lf_printf(file, "#if defined(WITH_MON)\n");
  lf_printf(file, "/* monitoring: */\n");
  lf_printf(file, "if (WITH_MON & MONITOR_INSTRUCTION_ISSUE) {\n");
  lf_printf(file, "  mon_issue(");
  print_function_name(file,
		      instruction->file_entry->fields[insn_name],
		      NULL,
		      function_name_prefix_itable);
  lf_printf(file, ", cpu, cia);\n");
  lf_printf(file, "}\n");
  lf_indent_suppress(file);
  lf_printf(file, "#endif\n");
  lf_printf(file, "\n");

  /* determine the new instruction address */
  lf_printf(file, "/* keep the next instruction address handy */\n");
  if ((code & generate_with_semantic_returning_modified_nia_only))
    lf_printf(file, "nia = -1;\n");
  else if ((code & generate_with_semantic_delayed_branch)) {
    lf_printf(file, "nia.ip = cia.dp; /* instruction pointer */\n");
    lf_printf(file, "nia.dp = cia.dp + %d; /* delayed-slot pointer */\n",
	      insn_bit_size / 8);
  }
  else
    lf_printf(file, "nia = cia + %d;\n", insn_bit_size / 8);

  /* if conditional, generate code to verify that the instruction
     should be issued */
  if (it_is("c", instruction->file_entry->fields[insn_options])
      || (code & generate_with_semantic_conditional_issue)) {
    lf_printf(file, "\n");
    lf_printf(file, "/* execute only if conditional passes */\n");
    lf_printf(file, "if (IS_CONDITION_OK) {\n");
    lf_indent(file, +2);
    /* FIXME - need to log a conditional failure */
  }
  
  /* Architecture expects r0 to be zero.  Instead of having to check
     every read to see if it is refering to r0 just zap the r0
     register */
  if ((code & generate_with_semantic_delayed_branch))
    {
      lf_printf (file, "\n");
      lf_printf (file, "GPR(0) = 0;\n");
    }

  /* generate the code (or at least something */
  lf_printf(file, "\n");
  lf_printf(file, "/* semantics: */\n");
  if (instruction->file_entry->annex != NULL) {
    /* true code */
    table_entry_print_cpp_line_nr(file, instruction->file_entry);
    lf_printf(file, "{\n");
    lf_indent(file, +2);
    lf_print__c_code(file, instruction->file_entry->annex);
    lf_indent(file, -2);
    lf_printf(file, "}\n");
    lf_print__internal_reference(file);
  }
  else if (it_is("nop", instruction->file_entry->fields[insn_flags])) {
    lf_print__internal_reference(file);
  }
  else {
    /* abort so it is implemented now */
    table_entry_print_cpp_line_nr(file, instruction->file_entry);
    lf_printf(file, "engine_error (SD, CPU, cia, \"%s:%d:0x%%08lx:%%s unimplemented\\n\",\n",
	      filter_filename(instruction->file_entry->file_name),
	      instruction->file_entry->line_nr);
    if ((code & generate_with_semantic_delayed_branch))
      lf_printf(file, "              (long)cia.ip,\n");
    else
      lf_printf(file, "              (long)cia,\n");
    lf_printf(file, "              itable[MY_INDEX].name);\n");
    lf_print__internal_reference(file);
  }

  /* Close off the conditional execution */
  if (it_is("c", instruction->file_entry->fields[insn_options])
      || (code & generate_with_semantic_conditional_issue)) {
    lf_indent(file, -2);
    lf_printf(file, "}\n");
  }  
}

static void
print_c_semantic(lf *file,
		 insn *instruction,
		 insn_bits *expanded_bits,
		 opcode_field *opcodes,
		 cache_table *cache_rules)
{

  lf_printf(file, "{\n");
  lf_indent(file, +2);

  print_my_defines(file, expanded_bits, instruction->file_entry);
  lf_printf(file, "\n");
  print_icache_body(file,
		    instruction,
		    expanded_bits,
		    cache_rules,
		    ((code & generate_with_direct_access)
		     ? define_variables
		     : declare_variables),
		    ((code & generate_with_icache)
		     ? get_values_from_icache
		     : do_not_use_icache));

  lf_printf(file, "%sinstruction_address nia;\n", global_name_prefix);
  print_semantic_body(file,
		      instruction,
		      expanded_bits,
		      opcodes);
  lf_printf(file, "return nia;\n");

  /* generate something to clean up any #defines created for the cache */
  if (code & generate_with_direct_access)
    print_icache_body(file,
		      instruction,
		      expanded_bits,
		      cache_rules,
		      undef_variables,
		      ((code & generate_with_icache)
		       ? get_values_from_icache
		       : do_not_use_icache));

  lf_indent(file, -2);
  lf_printf(file, "}\n");
}

static void
print_c_semantic_function(lf *file,
			  insn *instruction,
			  insn_bits *expanded_bits,
			  opcode_field *opcodes,
			  cache_table *cache_rules)
{
  /* build the semantic routine to execute the instruction */
  print_semantic_function_header(file,
				 instruction->file_entry->fields[insn_name],
				 expanded_bits,
				 1/*is-function-definition*/);
  print_c_semantic(file,
		   instruction,
		   expanded_bits,
		   opcodes,
		   cache_rules);
}

void
print_semantic_definition(insn_table *entry,
			  lf *file,
			  void *data,
			  insn *instruction,
			  int depth)
{
  cache_table *cache_rules = (cache_table*)data;
  if (generate_expanded_instructions) {
    ASSERT(entry->nr_insn == 1
	   && entry->opcode == NULL
	   && entry->parent != NULL
	   && entry->parent->opcode != NULL);
    ASSERT(entry->nr_insn == 1
	   && entry->opcode == NULL
	   && entry->parent != NULL
	   && entry->parent->opcode != NULL
	   && entry->parent->opcode_rule != NULL);
    print_c_semantic_function(file,
			      entry->insns,
			      entry->expanded_bits,
			      entry->parent->opcode,
			      cache_rules);
  }
  else {
    print_c_semantic_function(file, instruction,
			      NULL, NULL,
			      cache_rules);
  }
}
