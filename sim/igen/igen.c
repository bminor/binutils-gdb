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



#include <getopt.h>

#include "misc.h"
#include "lf.h"
#include "table.h"
#include "config.h"
#include "filter.h"

#include "igen.h"

#include "ld-insn.h"
#include "ld-decode.h"
#include "ld-cache.h"

#include "gen.h"

#include "gen-model.h"
#include "gen-icache.h"
#include "gen-itable.h"
#include "gen-idecode.h"
#include "gen-semantics.h"
#include "gen-engine.h"
#include "gen-support.h"
#include "gen-engine.h"


/****************************************************************/


/* Semantic functions */

int
print_semantic_function_formal (lf *file,
				int nr_prefetched_words)
{
  int nr = 0;
  int word_nr;
  if (options.gen.icache || nr_prefetched_words < 0)
    {
      nr += lf_printf (file, "SIM_DESC sd,\n");
      nr += lf_printf (file, "%sidecode_cache *cache_entry,\n",
		       options.prefix.global.name);
      nr += lf_printf (file, "%sinstruction_address cia",
		       options.prefix.global.name);
    }
  else if (options.gen.smp)
    {
      nr += lf_printf (file, "sim_cpu *cpu,\n");
      for (word_nr = 0; word_nr < nr_prefetched_words; word_nr++)
	{
	  nr += lf_printf (file, "%sinstruction_word instruction_%d,\n",
			   options.prefix.global.name,
			   word_nr);
	}
      nr += lf_printf (file, "%sinstruction_address cia",
		       options.prefix.global.name);
    }
  else
    {
      nr += lf_printf (file, "SIM_DESC sd,\n");
      for (word_nr = 0; word_nr < nr_prefetched_words; word_nr++)
	{
	  nr += lf_printf (file, "%sinstruction_word instruction_%d,\n",
			   options.prefix.global.name,
			   word_nr);
	}
      nr += lf_printf (file, "%sinstruction_address cia",
		       options.prefix.global.name);
    }
  return nr;
}

int
print_semantic_function_actual (lf *file,
				int nr_prefetched_words)
{
  int nr = 0;
  int word_nr;
  if (options.gen.icache || nr_prefetched_words < 0)
    {
      nr += lf_printf (file, "sd, cache_entry, cia");
    }
  else
    {
      if (options.gen.smp)
	nr += lf_printf (file, "cpu");
      else
	nr += lf_printf (file, "sd");
      for (word_nr = 0; word_nr < nr_prefetched_words; word_nr++)
	nr += lf_printf (file, ", instruction_%d", word_nr);
      nr += lf_printf (file, ", cia");
    }
  return nr;
}

int
print_semantic_function_type (lf *file)
{
  int nr = 0;
  nr += lf_printf (file, "%sinstruction_address",
		   options.prefix.global.name);
  return nr;
}


/* Idecode functions */

int
print_icache_function_formal (lf *file,
			      int nr_prefetched_words)
{
  int nr = 0;
  int word_nr;
  if (options.gen.smp)
      nr += lf_printf (file, "sim_cpu *cpu,\n");
  else
      nr += lf_printf (file, "SIM_DESC sd,\n");
  for (word_nr = 0; word_nr < nr_prefetched_words; word_nr++)
    nr += lf_printf (file, " %sinstruction_word instruction_%d,\n",
		     options.prefix.global.name, word_nr);
  nr += lf_printf (file, " %sinstruction_address cia,\n",
		   options.prefix.global.name);
  nr += lf_printf (file, " %sidecode_cache *cache_entry",
		   options.prefix.global.name);
  return nr;
}

int
print_icache_function_actual (lf *file,
			      int nr_prefetched_words)
{
  int nr = 0;
  int word_nr;
  if (options.gen.smp)
    nr += lf_printf (file, "cpu");
  else
    nr += lf_printf (file, "sd");
  for (word_nr = 0; word_nr < nr_prefetched_words; word_nr++)
    nr += lf_printf (file, ", instruction_%d", word_nr);
  nr += lf_printf (file, ", cia, cache_entry");
  return nr;
}

int
print_icache_function_type (lf *file)
{
  int nr;
  if (options.gen.semantic_icache)
    {
      nr = print_semantic_function_type (file);
    }
  else
    {
      nr = lf_printf (file, "%sidecode_semantic *",
		      options.prefix.global.name);
    }
  return nr;
}


/* Function names */

static int
print_opcode_bits (lf *file,
		   opcode_bits *bits)
{
  int nr = 0;
  if (bits == NULL)
    return nr;
  nr += lf_putchr (file, '_');
  nr += lf_putstr (file, bits->field->val_string);
  if (bits->opcode->is_boolean && bits->value == 0)
    nr += lf_putint (file, bits->opcode->boolean_constant);
  else if (!bits->opcode->is_boolean) {
    if (bits->opcode->last < bits->field->last)
      nr += lf_putint (file, bits->value << (bits->field->last - bits->opcode->last));
    else
      nr += lf_putint (file, bits->value);
  }
  nr += print_opcode_bits (file, bits->next);
  return nr;
}

static int
print_c_name (lf *file,
	      const char *name)
{
  int nr = 0;
  const char *pos;
  for (pos = name; *pos != '\0'; pos++)
    {
      switch (*pos)
	{
	case '/':
	case '-':
	  break;
	case ' ':
	case '.':
	  nr += lf_putchr (file, '_');
	  break;
	default:
	  nr += lf_putchr (file, *pos);
	  break;
	}
    }
  return nr;
}

extern int
print_function_name (lf *file,
		     const char *basename,
		     const char *format_name,
		     const char *model_name,
		     opcode_bits *expanded_bits,
		     lf_function_name_prefixes prefix)
{
  int nr = 0;
  /* the prefix */
  switch (prefix)
    {
    case function_name_prefix_semantics:
      nr += lf_printf (file, "%s", options.prefix.semantics.name);
      nr += lf_printf (file, "semantic_");
      break;
    case function_name_prefix_idecode:
      nr += lf_printf (file, "%s", options.prefix.idecode.name);
      nr += lf_printf (file, "idecode_");
      break;
    case function_name_prefix_itable:
      nr += lf_printf (file, "%sitable_", options.prefix.itable.name);
      break;
    case function_name_prefix_icache:
      nr += lf_printf (file, "%s", options.prefix.icache.name);
      nr += lf_printf (file, "icache_");
      break;
    case function_name_prefix_engine:
      nr += lf_printf (file, "%s", options.prefix.engine.name);
      nr += lf_printf (file, "engine_");
    default:
      break;
    }
  
  if (model_name != NULL)
    {
      nr += print_c_name (file, model_name);
      nr += lf_printf (file, "_");
    }

  /* the function name */
  nr += print_c_name (file, basename);
  
  /* the format name if available */
  if (format_name != NULL)
    {
      nr += print_c_name (file, format_name);
      nr += lf_printf (file, "_");
    }

  /* the suffix */
  nr += print_opcode_bits (file, expanded_bits);

  return nr;
}


void
print_my_defines (lf *file,
		  const char *basename,
		  const char *format_name,
		  opcode_bits *expanded_bits)
{
  /* #define MY_INDEX xxxxx */
  lf_indent_suppress (file);
  lf_printf (file, "#undef MY_INDEX\n");
  lf_indent_suppress (file);
  lf_printf (file, "#define MY_INDEX ");
  print_function_name (file,
		       basename, format_name, NULL,
		       NULL,
		       function_name_prefix_itable);
  lf_printf (file, "\n");
  /* #define MY_PREFIX xxxxxx */
  lf_indent_suppress (file);
  lf_printf (file, "#undef ");
  print_function_name (file,
		       basename, format_name, NULL,
		       expanded_bits,
		       function_name_prefix_none);
  lf_printf (file, "\n");
  lf_indent_suppress (file);
  lf_printf (file, "#undef MY_PREFIX\n");
  lf_indent_suppress (file);
  lf_printf (file, "#define MY_PREFIX ");
  print_function_name (file,
		       basename, format_name, NULL,
		       expanded_bits,
		       function_name_prefix_none);
  lf_printf (file, "\n");
  /* #define MY_NAME xxxxxx */
  lf_indent_suppress (file);
  lf_indent_suppress (file);
  lf_printf (file, "#undef MY_NAME\n");
  lf_indent_suppress (file);
  lf_printf (file, "#define MY_NAME \"");
  print_function_name (file,
		       basename, format_name, NULL,
		       expanded_bits,
		       function_name_prefix_none);
  lf_printf (file, "\"\n");
}


static int
print_itrace_prefix (lf *file,
		     const char *phase_lc)
{
  const char *prefix = "trace_one_insn (";
  int indent = strlen (prefix);
  lf_printf (file, "%sSD, CPU, %s, TRACE_LINENUM_P (CPU),\n",
	     prefix, (options.gen.delayed_branch ? "cia.ip" : "cia"));
  lf_indent (file, +indent);
  lf_printf (file, "%sitable[MY_INDEX].file,\n", options.prefix.itable.name);
  lf_printf (file, "%sitable[MY_INDEX].line_nr,\n", options.prefix.itable.name);
  lf_printf (file, "\"%s\",\n", phase_lc);
  lf_printf (file, "\"%%-18s - ");
  return indent;
}


static void
print_itrace_format (lf *file,
		     insn_mnemonic_entry *assembler)
{
  /* pass=1 is fmt string; pass=2 is is arguments */
  int pass;
  /* print the format string */
  for (pass = 1; pass <= 2; pass++)
    {
      const char *chp = assembler->format;
      chp++; /* skip the leading quote */
      /* prefix the format with the insn `name' */
      if (pass == 2)
	{
	  lf_printf (file, ",\n");
	  lf_printf (file, "%sitable[MY_INDEX].name", options.prefix.itable.name);
	}
      /* write out the format/args */
      while (*chp != '\0')
	{
	  if (chp[0] == '\\' && (chp[1] == '<' || chp[1] == '>'))
	    {
	      if (pass == 1)
		lf_putchr (file, chp[1]);
	      chp += 2;
	    }
	  else if (chp[0] == '<' || chp[0] == '%')
	    {
	      /* parse [ "%" ... ] "<" [ func "#" ] param ">" */
	      const char *fmt;
	      const char *func;
	      int strlen_func;
	      const char *param;
	      int strlen_param;
	      /* the "%" ... "<" format */
	      fmt = chp;
	      while (chp[0] != '<' && chp[0] != '\0')
		chp++;
	      if (chp[0] != '<')
		error (assembler->line, "Missing `<' after `%%'\n");
	      chp++;
	      /* [ "func" # ] OR "param" */
	      func = chp;
	      param = chp;
	      while (chp[0] != '>' && chp[0] != '#' && chp[0] != '\0')
		chp++;
	      strlen_func = chp - func;
	      if (chp[0] == '#')
		{
		  chp++;
		  param = chp;
		  while (chp[0] != '>' && chp[0] != '\0')
		    chp++;
		}
	      strlen_param = chp - param;
	      if (chp[0] != '>')
		error (assembler->line, "Missing closing `>' in assembler string\n");
	      chp++;
	      /* now process it */
	      if (pass == 2)
		lf_printf (file, ",\n");
	      if (strncmp (fmt, "<", 1) == 0)
		/* implicit long int format */
		{
		  if (pass == 1)
		    lf_printf (file, "%%ld");
		  else
		    {
		      lf_printf (file, "(long) ");
		      lf_write (file, param, strlen_param);
		    }
		}
	      else if (strncmp (fmt, "%<", 2) == 0)
		/* explicit format */
		{
		  if (pass == 1)
		    lf_printf (file, "%%");
		  else
		    lf_write (file, param, strlen_param);
		}
	      else if (strncmp (fmt, "%s<", 3) == 0)
		/* string format */
		{
		  if (pass == 1)
		    lf_printf (file, "%%s");
		  else
		    {
		      lf_printf (file, "%sstr_", options.prefix.global.name);
		      lf_write (file, func, strlen_func);
		      lf_printf (file, " (SD_, ");
		      lf_write (file, param, strlen_param);
		      lf_printf (file, ")");
		    }
		}
	      else if (strncmp (fmt, "%lx<", 4) == 0)
		/* simple hex */
		{
		  if (pass == 1)
		    lf_printf (file, "%%lx");
		  else
		    {
		      lf_printf (file, "(unsigned long) ");
		      lf_write (file, param, strlen_param);
		    }
		}
	      else if (strncmp (fmt, "%08lx<", 6) == 0)
		/* simple hex */
		{
		  if (pass == 1)
		    lf_printf (file, "%%08lx");
		  else
		    {
		      lf_printf (file, "(unsigned long) ");
		      lf_write (file, param, strlen_param);
		    }
		}
	      else
		error (assembler->line, "Unknown assembler string format\n");
	    }
	  else
	    {
	      if (pass == 1)
		lf_putchr (file, chp[0]);
	      chp += 1;
	    }
	}
    }
  lf_printf (file, ");\n");
}


void
print_itrace (lf *file,
	      insn_entry *insn,
	      int idecode)
{
  const char *phase = (idecode) ? "DECODE" : "INSN";
  const char *phase_lc = (idecode) ? "decode" : "insn";
  lf_printf (file, "\n");
  lf_indent_suppress (file);
  lf_printf (file, "#if defined (WITH_TRACE)\n");
  lf_printf (file, "/* trace the instructions execution if enabled */\n");
  lf_printf (file, "if (TRACE_%s_P (CPU)) {\n", phase);
  lf_indent (file, +2);
  if (insn->mnemonics != NULL)
    {
      insn_mnemonic_entry *assembler = insn->mnemonics;
      int is_first = 1;
      do
	{
	  if (assembler->condition != NULL)
	    {
	      int indent;
	      lf_printf (file, "%sif (%s)\n",
			 is_first ? "" : "else ",
			 assembler->condition);
	      lf_indent (file, +2);
	      indent = print_itrace_prefix (file, phase_lc);
	      print_itrace_format (file, assembler);
	      lf_indent (file, -indent);
	      lf_indent (file, -2);
	      if (assembler->next == NULL)
		error (assembler->line, "Missing final unconditional assembler\n");
	    }
	  else
	    {
	      int indent;
	      if (!is_first)
		{
		  lf_printf (file, "else\n");
		  lf_indent (file, +2);
		}
	      indent = print_itrace_prefix (file, phase_lc);
	      print_itrace_format (file, assembler);
	      lf_indent (file, -indent);
	      if (!is_first)
		lf_indent (file, -2);
	      if (assembler->next != NULL)
		error (assembler->line, "Unconditional assembler is not last\n");
	    }
	  is_first = 0;
	  assembler = assembler->next;
	}
      while (assembler != NULL);
    }
  else
    {
      int indent = print_itrace_prefix (file, phase_lc);
      lf_printf (file, "?\",\n");
      lf_printf (file, "itable[MY_INDEX].name);\n");
      lf_indent (file, -indent);
    }
  lf_indent (file, -2);
  lf_printf (file, "}\n");
  lf_indent_suppress (file);
  lf_printf (file, "#endif\n");
}


void
print_sim_engine_abort (lf *file,
			const char *message)
{
  lf_printf (file, "sim_engine_abort (SD, CPU, cia, ");
  lf_printf (file, "\"%s\"", message);
  lf_printf (file, ");\n");
}


/****************************************************************/


static void
gen_semantics_h (lf *file,
		 insn_list *semantics,
		 int max_nr_words)
{
  int word_nr;
  insn_list *semantic;
  for (word_nr = -1; word_nr <= max_nr_words; word_nr++)
    {
      lf_printf (file, "typedef ");
      print_semantic_function_type (file);
      lf_printf (file, " %sidecode_semantic",
		 options.prefix.global.name);
      if (word_nr >= 0)
	lf_printf (file, "_%d", word_nr);
      lf_printf (file, "\n(");
      lf_indent (file, +1);
      print_semantic_function_formal (file, word_nr);
      lf_indent (file, -1);
      lf_printf (file, ");\n");
      lf_printf (file, "\n");
    }
  switch (options.gen.code)
    {
    case generate_calls:
      for (semantic = semantics; semantic != NULL; semantic = semantic->next)
	{
	  /* Ignore any special/internal instructions */
	  if (semantic->insn->nr_words == 0)
	    continue;
	  print_semantic_declaration (file,
				      semantic->insn,
				      semantic->expanded_bits,
				      semantic->opcodes,
				      semantic->nr_prefetched_words);
	}
      break;
    case generate_jumps:
      lf_print__this_file_is_empty (file, "generating jumps");
      break;
    }
}


static void
gen_semantics_c (lf *file,
		 insn_list *semantics,
		 cache_entry *cache_rules)
{
  if (options.gen.code == generate_calls)
    {
      insn_list *semantic;
      lf_printf (file, "\n");
      lf_printf (file, "#include \"sim-main.h\"\n");
      lf_printf (file, "#include \"%sitable.h\"\n",
		 options.prefix.itable.name);
      lf_printf (file, "#include \"%sidecode.h\"\n",
		 options.prefix.idecode.name);
      lf_printf (file, "#include \"%ssemantics.h\"\n",
		 options.prefix.semantics.name);
      lf_printf (file, "#include \"%ssupport.h\"\n",
		 options.prefix.support.name);
      lf_printf (file, "\n");
      for (semantic = semantics; semantic != NULL; semantic = semantic->next)
	{
	  /* Ignore any special/internal instructions */
	  if (semantic->insn->nr_words == 0)
	    continue;
	  print_semantic_definition (file,
				     semantic->insn,
				     semantic->expanded_bits,
				     semantic->opcodes,
				     cache_rules,
				     semantic->nr_prefetched_words);
	}
    }
  else
    {
      lf_print__this_file_is_empty (file, "generating jump engine");
    }
}


/****************************************************************/


static void
gen_icache_h (lf *file,
	      insn_list *semantic,
	      function_entry *functions,
	      int max_nr_words)
{
  int word_nr;
  for (word_nr = 0; word_nr <= max_nr_words; word_nr++)
    {
      lf_printf (file, "typedef ");
      print_icache_function_type(file);
      lf_printf (file, " %sidecode_icache_%d\n(",
		 options.prefix.global.name,
		 word_nr);
      print_icache_function_formal(file, word_nr);
      lf_printf (file, ");\n");
      lf_printf (file, "\n");
    }
  if (options.gen.code == generate_calls
      && options.gen.icache)
    {
      function_entry_traverse (file, functions,
			       print_icache_internal_function_declaration,
			       NULL);
      while (semantic != NULL)
	{
	  print_icache_declaration (file,
				    semantic->insn,
				    semantic->expanded_bits,
				    semantic->opcodes,
				    semantic->nr_prefetched_words);
	  semantic = semantic->next;
	}
    }
  else
    {
      lf_print__this_file_is_empty (file, "generating jump engine");
    }
}

static void
gen_icache_c (lf *file,
	      insn_list *semantic,
	      function_entry *functions,
	      cache_entry *cache_rules)
{
  /* output `internal' invalid/floating-point unavailable functions
     where needed */
  if (options.gen.code == generate_calls
      && options.gen.icache)
    {
      lf_printf (file, "\n");
      lf_printf (file, "#include \"cpu.h\"\n");
      lf_printf (file, "#include \"idecode.h\"\n");
      lf_printf (file, "#include \"semantics.h\"\n");
      lf_printf (file, "#include \"icache.h\"\n");
      lf_printf (file, "#include \"support.h\"\n");
      lf_printf (file, "\n");
      function_entry_traverse (file, functions,
			       print_icache_internal_function_definition,
			       NULL);
      lf_printf (file, "\n");
      while (semantic != NULL)
	{
	  print_icache_definition (file,
				   semantic->insn,
				   semantic->expanded_bits,
				   semantic->opcodes,
				   cache_rules,
				   semantic->nr_prefetched_words);
	  semantic = semantic->next;
	}
    }
  else
    {
      lf_print__this_file_is_empty (file, "generating jump engine");
    }
}


/****************************************************************/


static void
gen_idecode_h (lf *file,
	       gen_table *gen,
	       insn_table *insns,
	       cache_entry *cache_rules)
{
  lf_printf (file, "typedef unsigned%d %sinstruction_word;\n",
	     options.insn_bit_size, options.prefix.global.name);
  if (options.gen.delayed_branch)
    {
      lf_printf (file, "typedef struct _%sinstruction_address {\n",
		 options.prefix.global.name);
      lf_printf (file, "  address_word ip; /* instruction pointer */\n");
      lf_printf (file, "  address_word dp; /* delayed-slot pointer */\n");
      lf_printf (file, "} %sinstruction_address;\n", options.prefix.global.name);
    }
  else
    {
      lf_printf (file, "typedef address_word %sinstruction_address;\n",
		 options.prefix.global.name);
      
    }
  if (options.gen.nia == nia_is_invalid
      && strlen (options.prefix.global.uname) > 0)
    {
      lf_indent_suppress (file);
      lf_printf (file, "#define %sINVALID_INSTRUCTION_ADDRESS ",
		 options.prefix.global.uname);
      lf_printf (file, "INVALID_INSTRUCTION_ADDRESS\n");
    }
  lf_printf (file, "\n");
  print_icache_struct (file, insns, cache_rules);
  lf_printf (file, "\n");
  if (options.gen.icache)
    {
      ERROR ("FIXME - idecode with icache suffering from bit-rot");
    }
  else
    {
      gen_list *entry;
      for (entry = gen->tables; entry != NULL; entry = entry->next)
	{
	  print_idecode_issue_function_header (file,
					       entry->processor,
					       0/*is definition*/,
					       1/*ALWAYS ONE WORD*/);
	}
    }
}


static void
gen_idecode_c (lf *file,
	       gen_table *gen,
	       insn_table *isa,
	       cache_entry *cache_rules)
{
  /* the intro */
  lf_printf (file, "#include \"sim-main.h\"\n");
  lf_printf (file, "#include \"%sidecode.h\"\n", options.prefix.global.name);
  lf_printf (file, "#include \"%ssemantics.h\"\n", options.prefix.global.name);
  lf_printf (file, "#include \"%sicache.h\"\n", options.prefix.global.name);
  lf_printf (file, "#include \"%ssupport.h\"\n", options.prefix.global.name);
  lf_printf (file, "\n");
  lf_printf (file, "\n");

  print_idecode_globals (file);
  lf_printf (file, "\n");
  
  switch (options.gen.code)
    {
    case generate_calls:
      {
	gen_list *entry;
	for (entry = gen->tables; entry != NULL; entry = entry->next)
	  {
	    print_idecode_lookups (file, entry->table, cache_rules);
	
	    /* output the main idecode routine */
	    if (!options.gen.icache)
	      {
		print_idecode_issue_function_header (file,
						     entry->processor,
						     1/*is definition*/,
						     1/*ALWAYS ONE WORD*/);
		lf_printf (file, "{\n");
		lf_indent (file, +2);
		lf_printf (file, "%sinstruction_address nia;\n",
			   options.prefix.global.name);
		print_idecode_body (file, entry->table, "nia =");
		lf_printf (file, "return nia;");
		lf_indent (file, -2);
		lf_printf (file, "}\n");
	      }
	  }
	break;
      }
    case generate_jumps:
      {
	lf_print__this_file_is_empty (file, "generating a jump engine");
	break;
      }
    }
}


/****************************************************************/


static void
gen_run_c (lf *file,
	   gen_table *gen)
{
  gen_list *entry;
  lf_printf (file, "#include \"sim-main.h\"\n");
  lf_printf (file, "#include \"engine.h\"\n");
  lf_printf (file, "#include \"bfd.h\"\n");
  lf_printf (file, "\n");
  lf_printf (file, "void\n");
  lf_printf (file, "sim_engine_run (SIM_DESC sd,\n");
  lf_printf (file, "                int next_cpu_nr,\n");
  lf_printf (file, "                int siggnal)\n");
  lf_printf (file, "{\n");
  lf_indent (file, +2);
  if (options.gen.multi_sim)
    {
      lf_printf (file, "int mach;\n");
      lf_printf (file, "if (STATE_ARCHITECTURE (sd) == NULL)\n");
      lf_printf (file, "  mach = 0;\n");
      lf_printf (file, "else\n");
      lf_printf (file, "  mach = STATE_ARCHITECTURE (sd)->mach;\n");
      lf_printf (file, "switch (mach)\n");
      lf_printf (file, "  {\n");
      lf_indent (file, +2);
      for (entry = gen->tables; entry != NULL; entry = entry->next)
	{
	  lf_printf (file, "case bfd_mach_%s:\n", entry->processor);
	  lf_indent (file, +2);
	  print_function_name (file,
			       "run",
			       NULL, /* format name */
			       entry->processor,
			       NULL, /* expanded bits */
			       function_name_prefix_engine);
	  lf_printf (file, " (sd, next_cpu_nr, siggnal);\n");
	  lf_printf (file, "break;\n");
	  lf_indent (file, -2);
	}
      lf_printf (file, "default:\n");
      lf_indent (file, +2);
      lf_printf (file, "sim_engine_abort (sd, NULL, NULL_CIA,\n");
      lf_printf (file, "                  \"sim_engine_run - unknown machine\");\n");
      lf_printf (file, "break;\n");
      lf_indent (file, -2);
      lf_indent (file, -2);
      lf_printf (file, "  }\n");
    }
  else
    {
      print_function_name (file,
			   "run",
			   NULL, /* format name */
			   NULL, /* NO processor */
			   NULL, /* expanded bits */
			   function_name_prefix_engine);
      lf_printf (file, " (sd, next_cpu_nr, siggnal);\n");
    }
  lf_indent (file, -2);
  lf_printf (file, "}\n");
}

/****************************************************************/

static gen_table *
do_gen (insn_table *isa,
	decode_table *decode_rules)
{
  gen_table *gen;
  if (decode_rules == NULL)
    error (NULL, "Must specify a decode table\n");
  if (isa == NULL)
    error (NULL, "Must specify an instruction table\n");
  if (decode_table_max_word_nr (decode_rules) > 0)
    options.gen.multi_word = decode_table_max_word_nr (decode_rules);
  gen = make_gen_tables (isa, decode_rules);
  gen_tables_expand_insns (gen);
  gen_tables_expand_semantics (gen);
  return gen;
}

/****************************************************************/

igen_options options;

int
main (int argc,
      char **argv,
      char **envp)
{
  cache_entry *cache_rules = NULL;
  lf_file_references file_references = lf_include_references;
  decode_table *decode_rules = NULL;
  insn_table *isa = NULL;
  gen_table *gen = NULL;
  char *real_file_name = NULL;
  int is_header = 0;
  int ch;
  lf *standard_out = lf_open ("-", "stdout", lf_omit_references, lf_is_text, "igen");

  INIT_OPTIONS (options);

  if (argc == 1)
    {
      printf ("Usage:\n");
      printf ("\n");
      printf ("  igen <config-opts> ... <input-opts>... <output-opts>...\n");
      printf ("\n");
      printf ("Config options:\n");
      printf ("\n");
      printf ("  -B <bit-size>\n");
      printf ("\t Set the number of bits in an instruction (depreciated).\n");
      printf ("\t This option can now be set directly in the instruction table.\n");
      printf ("\n");
      printf ("  -D <data-structure>\n");
      printf ("\t Dump the specified data structure to stdout. Valid structures include:\n");
      printf ("\t processor-names - list the names of all the processors (models)\n");
      printf ("\n");
      printf ("  -F <filter-list>\n");
      printf ("\t Filter out any instructions with a non-empty flags field that contains\n");
      printf ("\t a flag not listed in the <filter-list>.\n");
      printf ("\n");
      printf ("  -H <high-bit>\n");
      printf ("\t Set the number of the high (most significant) instruction bit (depreciated).\n");
      printf ("\t This option can now be set directly in the instruction table.\n");
      printf ("\n");
      printf ("  -I <icache-size>\n");
      printf ("\t Specify size of the cracking instruction cache (default %d instructions).\n",
	      options.gen.icache_size);
      printf ("\t Implies -G icache.\n");
      printf ("\n");
      printf ("  -M <model-list>\n");
      printf ("\t Filter out any instructions that do not support at least one of the listed\n");
      printf ("\t models (An instructions with no model information is considered to support\n");
      printf ("\n all models.).\n");
      printf ("\n");
      printf ("  -N <nr-cpus>\n");
      printf ("\t Generate a simulator supporting <nr-cpus>\n");
      printf ("\t Specify `-N 0' to disable generation of the SMP. Specifying `-N 1' will\n");
      printf ("\t still generate an SMP enabled simulator but will only support one CPU.\n");
      printf ("\n");
      printf ("  -T <mechanism>\n");
      printf ("\t Override the decode mechanism specified by the decode rules\n");
      printf ("\n");
      printf ("  -P <prefix>\n");
      printf ("\t Prepend global names (except itable) with the string <prefix>.\n");
      printf ("\t Specify -P <module>=<prefix> to set the <modules> prefix.\n");
      printf ("\n");
      printf ("  -Werror\n");
      printf ("\t Make warnings errors\n");
      printf ("\n");
      printf ("  -G [!]<gen-option>\n");
      printf ("\t Any of the following options:\n");
      printf ("\n");
      printf ("\t decode-duplicate       - Override the decode rules, forcing the duplication of\n");
      printf ("\t                          semantic functions\n");
      printf ("\t decode-combine         - Combine any duplicated entries within a table\n");
      printf ("\t decode-zero-reserved   - Override the decode rules, forcing reserved bits to be\n");
      printf ("\t                          treated as zero.\n");
      printf ("\t decode-switch-is-goto  - Overfide the padded-switch code type as a goto-switch\n");
      printf ("\n");
      printf ("\t gen-conditional-issue  - conditionally issue each instruction\n");
      printf ("\t gen-delayed-branch     - need both cia and nia passed around\n");
      printf ("\t gen-direct-access      - use #defines to directly access values\n");
      printf ("\t gen-zero-r<N>          - arch assumes GPR(<N>) == 0, keep it that way\n");
      printf ("\t gen-icache             - generate an instruction cracking cache\n");
      printf ("\t gen-insn-in-icache     - save original instruction when cracking\n");
      printf ("\t gen-multi-sim          - generate multiple simulators - one per model\n");
      printf ("\t                          By default, a single simulator that will\n");
      printf ("\t                          execute any instruction is generated\n");
      printf ("\t gen-multi-word         - generate code allowing for multi-word insns\n");
      printf ("\t gen-semantic-icache    - include semantic code in cracking functions\n");
      printf ("\t gen-slot-verification  - perform slot verification as part of decode\n");
      printf ("\t gen-nia-invalid        - NIA defaults to nia_invalid\n");
      printf ("\t gen-nia-void           - do not compute/return NIA\n");
      printf ("\n");
      printf ("\t trace-combine          - report combined entries a rule application\n");
      printf ("\t trace-entries          - report entries after a rules application\n");
      printf ("\t trace-rule-rejection   - report each rule as rejected\n");
      printf ("\t trace-rule-selection   - report each rule as selected\n");
      printf ("\n");
      printf ("\t field-widths          - instruction formats specify widths (depreciated)\n");
      printf ("\t                         By default, an instruction format specifies bit\n");
      printf ("\t                         positions\n");
      printf ("\t                         This option can now be set directly in the\n");
      printf ("\t                         instruction table\n");
      printf ("\t jumps                 - use jumps instead of function calls\n");
      printf ("\t omit-line-numbers     - do not include line number information in the output\n");
      printf ("\n");
      printf ("Input options:\n");
      printf ("\n");
      printf ("  -k <cache-rules> (depreciated)\n");
      printf ("  -o <decode-rules>\n");
      printf ("  -i <instruction-table>\n");
      printf ("\n");
      printf ("Output options:\n");
      printf ("\n");
      printf ("  -x                    Perform expansion (required)\n");
      printf ("  -n <real-name>        Specify the real name of the next output file\n"); 
      printf ("  -h 		       Generate the header (.h) file rather than the body (.c)\n");
      printf ("  -c <output-file>      output icache\n");
      printf ("  -d <output-file>      output idecode\n");
      printf ("  -e <output-file>      output engine\n");
      printf ("  -f <output-file>      output support functions\n");
      printf ("  -m <output-file>      output model\n");
      printf ("  -r <output-file>      output multi-sim run\n");
      printf ("  -s <output-file>      output schematic\n");
      printf ("  -t <output-file>      output itable\n");
    }
  
  while ((ch = getopt(argc, argv,
		      "B:D:F:G:H:I:M:N:P:T:W:o:k:i:n:hc:d:e:m:r:s:t:f:x"))
	 != -1)
    {
      fprintf (stderr, "  -%c ", ch);
      if (optarg)
	fprintf (stderr, "%s ", optarg);
      fprintf (stderr, "\\\n");
      
      switch(ch)
	{
	  
	case 'M':
	  filter_parse (&options.model_filter, optarg);
	  break;

	case 'D':
	  if (strcmp (optarg, "processor-names"))
	    {
	      char *processor;
	      for (processor = filter_next (options.model_filter, "");
		   processor != NULL;
		   processor = filter_next (options.model_filter, processor))
		lf_printf (standard_out, "%s\n", processor);
	    }
	  else
	    error (NULL, "Unknown data structure %s, not dumped\n", optarg);
	  break;

	case 'F':
	  filter_parse (&options.flags_filter, optarg);
	  break;
	  
	case 'I':
	  options.gen.icache_size = a2i (optarg);
	  options.gen.icache = 1;
	  break;
	  
	case 'B':
	  options.insn_bit_size = a2i (optarg);
	  if (options.insn_bit_size <= 0
	      || options.insn_bit_size > max_insn_bit_size)
	    {
	      error (NULL, "Instruction bitsize must be in range 1..%d\n",
		     max_insn_bit_size);
	    }
	  if (options.hi_bit_nr != options.insn_bit_size - 1
	      && options.hi_bit_nr != 0)
	    {
	      error (NULL, "Conflict betweem hi-bit-nr and insn-bit-size\n");
	    }
	  break;
	  
	case 'H':
	  options.hi_bit_nr = a2i (optarg);
	  if (options.hi_bit_nr != options.insn_bit_size - 1
	      && options.hi_bit_nr != 0)
	    {
	      error (NULL, "Conflict between hi-bit-nr and insn-bit-size\n");
	    }
	  break;
	  
	case 'N':
	  options.gen.smp = a2i (optarg);
	  break;
	  
	case 'P':
	  {
	    igen_prefix_name *names;
	    char *chp;
	    chp = strchr (optarg, '=');
	    if (chp == NULL)
	      {
		names = &options.prefix.global;
		chp = optarg;
	      }
	    else
	      {
		chp = chp + 1; /* skip `=' */
		if (strncmp (optarg, "global=", chp - optarg) == 0)
		  {
		    names = &options.prefix.global;
		  }
		if (strncmp (optarg, "engine=", chp - optarg) == 0)
		  {
		    names = &options.prefix.engine;
		  }
		if (strncmp (optarg, "icache=", chp - optarg) == 0)
		  {
		    names = &options.prefix.icache;
		  }
		if (strncmp (optarg, "idecode=", chp - optarg) == 0)
		  {
		    names = &options.prefix.idecode;
		  }
		if (strncmp (optarg, "itable=", chp - optarg) == 0)
		  {
		    names = &options.prefix.itable;
		  }
		if (strncmp (optarg, "semantics=", chp - optarg) == 0)
		  {
		    names = &options.prefix.semantics;
		  }
		if (strncmp (optarg, "support=", chp - optarg) == 0)
		  {
		    names = &options.prefix.support;
		  }
		else
		  {
		    names = NULL;
		    error (NULL, "Prefix `%s' unreconized\n", optarg);
		  }
	      }
	    names->name = strdup (chp);
	    names->uname = strdup (chp);
	    chp = names->uname;
	    while (*chp) {
	      if (islower(*chp))
		*chp = toupper(*chp);
	      chp++;
	    }
	    if (names == &options.prefix.global)
	      {
		options.prefix.engine = options.prefix.global;
		options.prefix.icache = options.prefix.global;
		options.prefix.idecode = options.prefix.global;
		/* options.prefix.itable = options.prefix.global; */
		options.prefix.semantics = options.prefix.global;
		options.prefix.support = options.prefix.global;
	      }
	    break;
	  }
	
	case 'W':
	  {
	    if (strcmp (optarg, "error") == 0)
	      options.warning = error;
	    else
	      error (NULL, "Unknown -W argument `%s'\n", optarg);
	    break;
	  }


	case 'G':
	  if (strcmp (optarg, "decode-duplicate") == 0)
	    {
	      options.decode.duplicate = 1;
	    }
	  else if (strcmp (optarg, "decode-combine") == 0)
	    {
	      options.decode.combine = 1;
	    }
	  else if (strcmp (optarg, "decode-zero-reserved") == 0)
	    {
	      options.decode.zero_reserved = 1;
	    }

	  else if (strcmp (optarg, "gen-conditional-issue") == 0)
	    {
	      options.gen.conditional_issue = 1;
	    }
	  else if (strcmp (optarg, "conditional-issue") == 0)
	    {
	      options.gen.conditional_issue = 1;
	      options.warning (NULL, "Option conditional-issue replaced by gen-conditional-issue\n");
	    }
	  else if (strcmp (optarg, "gen-delayed-branch") == 0)
	    {
	      options.gen.delayed_branch = 1;
	    }
	  else if (strcmp (optarg, "delayed-branch") == 0)
	    {
	      options.gen.delayed_branch = 1;
	      options.warning (NULL, "Option delayed-branch replaced by gen-delayed-branch\n");
	    }
	  else if (strcmp (optarg, "gen-direct-access") == 0)
	    {
	      options.gen.direct_access = 1;
	    }
	  else if (strcmp (optarg, "direct-access") == 0)
	    {
	      options.gen.direct_access = 1;
	      options.warning (NULL, "Option direct-access replaced by gen-direct-access\n");
	    }
	  else if (strncmp (optarg, "gen-zero-r", strlen ("gen-zero-r")) == 0)
	    {
	      options.gen.zero_reg = 1;
	      options.gen.zero_reg_nr = atoi (optarg + strlen ("gen-zero-r"));
	    }
	  else if (strncmp (optarg, "zero-r", strlen ("zero-r")) == 0)
	    {
	      options.gen.zero_reg = 1;
	      options.gen.zero_reg_nr = atoi (optarg + strlen ("zero-r"));
	      options.warning (NULL, "Option zero-r<N> replaced by gen-zero-r<N>\n");
	    }
	  else if (strcmp (optarg, "gen-icache") == 0)
	    {
	      options.gen.icache = 1;
	    }
	  else if (strcmp (optarg, "gen-insn-in-icache") == 0)
	    {
	      options.gen.insn_in_icache = 1;
	    }
	  else if (strcmp (optarg, "gen-multi-sim") == 0)
	    {
	      options.gen.multi_sim = 1;
	    }
	  else if (strcmp (optarg, "gen-multi-word") == 0)
	    {
	      options.gen.multi_word = 1;
	    }
	  else if (strcmp (optarg, "gen-semantic-icache") == 0)
	    {
	      options.gen.semantic_icache = 1;
	    }
	  else if (strcmp (optarg, "gen-slot-verification") == 0)
	    {
	      options.gen.slot_verification = 1;
	    }
	  else if (strcmp (optarg, "verify-slot") == 0)
	    {
	      options.gen.slot_verification = 1;
	      options.warning (NULL, "Option verify-slot replaced by gen-slot-verification\n");
	    }
	  else if (strcmp (optarg, "gen-nia-invalid") == 0)
	    {
	      options.gen.nia = nia_is_invalid;
	    }
	  else if (strcmp (optarg, "default-nia-minus-one") == 0)
	    {
	      options.gen.nia = nia_is_invalid;
	      options.warning (NULL, "Option default-nia-minus-one replaced by gen-nia-invalid\n");
	    }
	  else if (strcmp (optarg, "gen-nia-void") == 0)
	    {
	      options.gen.nia = nia_is_void;
	    }
	  else if (strcmp (optarg, "trace-combine") == 0)
	    {
	      options.trace.combine = 1;
	    }
	  else if (strcmp (optarg, "trace-entries") == 0)
	    {
	      options.trace.entries = 1;
	    }
	  else if (strcmp (optarg, "trace-rule-rejection") == 0)
	    {
	      options.trace.rule_rejection = 1;
	    }
	  else if (strcmp (optarg, "trace-rule-selection") == 0)
	    {
	      options.trace.rule_selection = 1;
	    }
	  else if (strcmp (optarg, "jumps") == 0)
	    {
	      options.gen.code = generate_jumps;
	    }
	  else if (strcmp (optarg, "field-widths") == 0)
	    {
	      options.insn_specifying_widths = 1;
	    }
	  else if (strcmp (optarg, "omit-line-numbers") == 0)
	    {
	      file_references = lf_omit_references;
	    }
	  else
	    error (NULL, "Unknown option %s\n", optarg);
	  break;
	  
	case 'i':
	  isa = load_insn_table (optarg, cache_rules);
	  if (isa->illegal_insn == NULL)
	    error (NULL, "illegal-instruction missing from insn table\n");
	  break;

	case 'x':
	  gen = do_gen (isa, decode_rules);
	  break;

	case 'o':
	  decode_rules = load_decode_table (optarg);
	  break;

	case 'k':
	  if (isa != NULL)
	    error (NULL, "Cache file must appear before the insn file\n");
	  cache_rules = load_cache_table (optarg);
	  break;

	case 'n':
	  real_file_name = strdup(optarg);
	  break;

	case 'h':
	  is_header = 1;
	  break;
	  
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'm':
	case 'r':
	case 's':
	case 't':
	  {
	    lf *file = lf_open(optarg, real_file_name, file_references,
			       (is_header ? lf_is_h : lf_is_c),
			       argv[0]);
	    if (gen == NULL && ch != 't' && ch != 'm' && ch != 'f')
	      {
		options.warning (NULL, "Explicitly generate tables with -x option\n");
		gen = do_gen (isa, decode_rules);
	      }
	    lf_print__file_start(file);
	    switch (ch)
	      {
	      case 'm':
		if (is_header)
		  gen_model_h (file, isa);
		else
		  gen_model_c (file, isa);
		break;
	      case 't':
		if (is_header)
		  gen_itable_h (file, isa);
		else
		  gen_itable_c (file, isa);
		break;
	      case 'f':
		if (is_header)
		  gen_support_h (file, isa);
		else
		  gen_support_c (file, isa);
		break;
	      case 'r':
		if (is_header)
		  options.warning (NULL, "-hr option ignored\n");
		else
		  gen_run_c (file, gen);
		break;
	      case 's':
		if(is_header)
		  gen_semantics_h (file, gen->semantics, isa->max_nr_words);
		else
		  gen_semantics_c (file, gen->semantics, isa->caches);
		break;
	      case 'd':
		if (is_header)
		  gen_idecode_h (file, gen, isa, cache_rules);
		else
		  gen_idecode_c (file, gen, isa, cache_rules);
		break;
	      case 'e':
		if (is_header)
		  gen_engine_h (file, gen, isa, cache_rules);
		else
		  gen_engine_c (file, gen, isa, cache_rules);
		break;
	      case 'c':
		if (is_header)
		  gen_icache_h (file,
				gen->semantics,
				isa->functions,
				isa->max_nr_words);
		else
		  gen_icache_c (file,
				gen->semantics,
				isa->functions,
				cache_rules);
		break;
	      }
	    lf_print__file_finish(file);
	    lf_close(file);
	    is_header = 0;
	  }
	real_file_name = NULL;
	break;
	default:
	  ERROR ("Bad switch");
	}
    }
  return (0);
}
