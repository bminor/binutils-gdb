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

#include "ld-decode.h"
#include "ld-cache.h"
#include "ld-insn.h"

#include "igen.h"

#include "gen-model.h"
#include "gen-icache.h"
#include "gen-itable.h"
#include "gen-idecode.h"
#include "gen-semantics.h"
#include "gen-support.h"

int hi_bit_nr = 0;
int insn_bit_size = default_insn_bit_size;
int insn_specifying_widths = 0;
const char *global_name_prefix = "";
const char *global_uname_prefix = "";

int code = generate_calls;

int generate_expanded_instructions;
int icache_size = 1024;
int generate_smp;

/****************************************************************/


/* Semantic functions */

int
print_semantic_function_formal(lf *file)
{
  int nr;
  if ((code & generate_with_icache))
    nr = lf_printf(file, "SIM_DESC sd,\n %sidecode_cache *cache_entry,\n instruction_address cia",
		   global_name_prefix);
  else if (generate_smp)
    nr = lf_printf(file, "sim_cpu *cpu,\n %sinstruction_word instruction,\n instruction_address cia",
		   global_name_prefix);
  else
    nr = lf_printf(file, "SIM_DESC sd,\n %sinstruction_word instruction,\n instruction_address cia",
		   global_name_prefix);
  return nr;
}

int
print_semantic_function_actual(lf *file)
{
  int nr;
  if ((code & generate_with_icache))
    nr = lf_printf(file, "sd, cache_entry, cia");
  else if (generate_smp)
    nr = lf_printf(file, "cpu, instruction, cia");
  else
    nr = lf_printf(file, "sd, instruction, cia");
  return nr;
}

int
print_semantic_function_type(lf *file)
{
  int nr;
  nr = lf_printf(file, "instruction_address");
  return nr;
}


/* Idecode functions */

int
print_icache_function_formal(lf *file)
{
  int nr = 0;
  if (generate_smp)
    nr += lf_printf(file, "sim_cpu *cpu,\n");
  else
    nr += lf_printf(file, "SIM_DESC sd,\n");
  nr += lf_printf(file, " %sinstruction_word instruction,\n", global_name_prefix);
  nr += lf_printf(file, " instruction_address cia,\n");
  nr += lf_printf(file, " %sidecode_cache *cache_entry", global_name_prefix);
  return nr;
}

int
print_icache_function_actual(lf *file)
{
  int nr;
  if (generate_smp)
    nr = lf_printf(file, "cpu, instruction, cia, cache_entry");
  else
    nr = lf_printf(file, "sd, instruction, cia, cache_entry");
  return nr;
}

int
print_icache_function_type(lf *file)
{
  int nr;
  if ((code & generate_with_semantic_icache))
    nr = print_semantic_function_type(file);
  else
    nr = lf_printf(file, "%sidecode_semantic *", global_name_prefix);
  return nr;
}


/* Function names */

static int
print_insn_bits(lf *file, insn_bits *bits)
{
  int nr = 0;
  if (bits == NULL)
    return nr;
  nr += print_insn_bits(file, bits->last);
  nr += lf_putchr(file, '_');
  nr += lf_putstr(file, bits->field->val_string);
  if (bits->opcode->is_boolean && bits->value == 0)
    nr += lf_putint(file, bits->opcode->boolean_constant);
  else if (!bits->opcode->is_boolean) {
    if (bits->opcode->last < bits->field->last)
      nr += lf_putint(file, bits->value << (bits->field->last - bits->opcode->last));
    else
      nr += lf_putint(file, bits->value);
  }
  return nr;
}

extern int
print_function_name(lf *file,
		    const char *basename,
		    insn_bits *expanded_bits,
		    lf_function_name_prefixes prefix)
{
  int nr = 0;
  /* the prefix */
  switch (prefix) {
  case function_name_prefix_semantics:
    nr += lf_putstr(file, global_name_prefix);
    nr += lf_putstr(file, "semantic_");
    break;
  case function_name_prefix_idecode:
    nr += lf_putstr(file, global_name_prefix);
    nr += lf_printf(file, "idecode_");
    break;
  case function_name_prefix_itable:
    nr += lf_putstr(file, "itable_");
    break;
  case function_name_prefix_icache:
    nr += lf_putstr(file, global_name_prefix);
    nr += lf_putstr(file, "icache_");
    break;
  default:
    break;
  }

  /* the function name */
  {
    const char *pos;
    for (pos = basename;
	 *pos != '\0';
	 pos++) {
      switch (*pos) {
      case '/':
      case '-':
	break;
      case ' ':
      case '.':
	nr += lf_putchr(file, '_');
	break;
      default:
	nr += lf_putchr(file, *pos);
	break;
      }
    }
  }

  /* the suffix */
  if (generate_expanded_instructions)
    nr += print_insn_bits(file, expanded_bits);

  return nr;
}


void
print_my_defines(lf *file,
		 insn_bits *expanded_bits,
		 table_entry *file_entry)
{
  /* #define MY_INDEX xxxxx */
  lf_indent_suppress(file);
  lf_printf(file, "#undef MY_INDEX\n");
  lf_indent_suppress(file);
  lf_printf(file, "#define MY_INDEX ");
  print_function_name(file,
                      file_entry->fields[insn_name],
                      NULL,
                      function_name_prefix_itable);
  lf_printf(file, "\n");
  /* #define MY_PREFIX xxxxxx */
  lf_indent_suppress(file);
  lf_printf(file, "#undef MY_PREFIX\n");
  lf_indent_suppress(file);
  lf_printf(file, "#define MY_PREFIX ");
  print_function_name(file,
		      file_entry->fields[insn_name],
		      expanded_bits,
		      function_name_prefix_none);
  lf_printf(file, "\n");
}


void
print_itrace(lf *file,
	     table_entry *file_entry,
	     int idecode)
{
  const char *object = idecode ? "DECODE" : "INSN";
  lf_printf(file, "\n");
  lf_indent_suppress(file);
  lf_printf(file, "#if defined(WITH_TRACE)\n");
  lf_printf(file, "/* trace the instructions execution if enabled */\n");
  lf_printf(file, "if (TRACE_%s_P (CPU)) {\n", object);
  lf_printf(file, "  trace_printf (CPU,\n");
  lf_printf(file, "    \"%s:%d:0x%%08lx:%%s\\n\", %s, %s);\n",
	    filter_filename(file_entry->file_name),
	    file_entry->line_nr,
	    ((code & generate_with_semantic_delayed_branch) ? "(long)cia.ip" : "(long)cia"),
	    "itable[MY_INDEX].name");
  lf_printf(file, "}\n");
  lf_indent_suppress(file);
  lf_printf(file, "#endif\n");
}


/****************************************************************/


static void
gen_semantics_h(insn_table *table,
		lf *file,
		igen_code generate)
{
  lf_printf(file, "typedef ");
  print_semantic_function_type(file);
  lf_printf(file, " %sidecode_semantic\n(", global_name_prefix);
  print_semantic_function_formal(file);
  lf_printf(file, ");\n");
  lf_printf(file, "\n");
  if ((code & generate_calls)) {
    if (generate_expanded_instructions)
      insn_table_traverse_tree(table,
			       file, NULL,
			       1,
			       NULL, /* start */
			       print_semantic_declaration, /* leaf */
			       NULL, /* end */
			       NULL); /* padding */
    else
      insn_table_traverse_insn(table,
			       file, NULL,
			       print_semantic_declaration);
    
  }
  else {
    lf_print__this_file_is_empty(file);
  }
}


static void
gen_semantics_c(insn_table *table,
		cache_table *cache_rules,
		lf *file,
		igen_code generate)
{
  if ((code & generate_calls)) {
    lf_printf(file, "\n");
    lf_printf(file, "#include \"sim-main.h\"\n");
    lf_printf(file, "#include \"%sidecode.h\"\n", global_name_prefix);
    lf_printf(file, "#include \"%ssemantics.h\"\n", global_name_prefix);
    lf_printf(file, "#include \"%ssupport.h\"\n", global_name_prefix);
    lf_printf(file, "\n");
    if (generate_expanded_instructions)
      insn_table_traverse_tree(table,
			       file, cache_rules,
			       1,
			       NULL, /* start */
			       print_semantic_definition, /* leaf */
			       NULL, /* end */
			       NULL); /* padding */
    else
      insn_table_traverse_insn(table,
			       file, cache_rules,
			       print_semantic_definition);
    
  }
  else {
    lf_print__this_file_is_empty(file);
  }
}


/****************************************************************/


static void
gen_icache_h(insn_table *table,
	     lf *file,
	     igen_code generate)
{
  lf_printf(file, "typedef ");
  print_icache_function_type(file);
  lf_printf(file, " %sidecode_icache\n(", global_name_prefix);
  print_icache_function_formal(file);
  lf_printf(file, ");\n");
  lf_printf(file, "\n");
  if ((code & generate_calls)
      && (code & generate_with_icache)) {
    insn_table_traverse_function(table,
				 file, NULL,
				 print_icache_internal_function_declaration);
    if (generate_expanded_instructions)
      insn_table_traverse_tree(table,
			       file, NULL,
			       1,
			       NULL, /* start */
			       print_icache_declaration, /* leaf */
			       NULL, /* end */
			       NULL); /* padding */
    else
      insn_table_traverse_insn(table,
			       file, NULL,
			       print_icache_declaration);
    
  }
  else {
    lf_print__this_file_is_empty(file);
  }
}

static void
gen_icache_c(insn_table *table,
	     cache_table *cache_rules,
	     lf *file,
	     igen_code generate)
{
  /* output `internal' invalid/floating-point unavailable functions
     where needed */
  if ((code & generate_calls)
      && (code & generate_with_icache)) {
    lf_printf(file, "\n");
    lf_printf(file, "#include \"cpu.h\"\n");
    lf_printf(file, "#include \"idecode.h\"\n");
    lf_printf(file, "#include \"semantics.h\"\n");
    lf_printf(file, "#include \"icache.h\"\n");
    lf_printf(file, "#include \"support.h\"\n");
    lf_printf(file, "\n");
    insn_table_traverse_function(table,
				 file, NULL,
				 print_icache_internal_function_definition);
    lf_printf(file, "\n");
    if (generate_expanded_instructions)
      insn_table_traverse_tree(table,
			       file, cache_rules,
			       1,
			       NULL, /* start */
			       print_icache_definition, /* leaf */
			       NULL, /* end */
			       NULL); /* padding */
    else
      insn_table_traverse_insn(table,
			       file, cache_rules,
			       print_icache_definition);
    
  }
  else {
    lf_print__this_file_is_empty(file);
  }
}


/****************************************************************/


int
main(int argc,
     char **argv,
     char **envp)
{
  cache_table *cache_rules = NULL;
  lf_file_references file_references = lf_include_references;
  decode_table *decode_rules = NULL;
  filter *filters = NULL;
  insn_table *instructions = NULL;
  char *real_file_name = NULL;
  int is_header = 0;
  int ch;

  if (argc == 1) {
    printf("Usage:\n");
    printf("  igen <config-opts> ... <input-opts>... <output-opts>...\n");
    printf("Config options:\n");
    printf("  -F <filter-flag>      eg -F 32 to include 32bit instructions\n");
    printf("  -I <icache-size>  Specify size of cracking cache\n");
    printf("  -B <bit-size>         Set the number of bits in an instruction\n");
    printf("  -H <high-bit>         Set the number of the high (msb) instruction bit\n");
    printf("  -N <nr-cpus>          Specify the max number of cpus the simulation will support\n");
    printf("  -T <mechanism>        Override the decode mechanism specified by the decode rules\n");
    printf("  -P <prefix>           Prepend all functions with the specified prefix\n");
    printf("  -G <gen-option>   Any of the following options:\n");
    printf("                      field-widths - instruction table specifies widths (not ofsesets)\n");
    printf("                      jumps - use jumps instead of function calls\n");
    printf("                      duplicate - expand (duplicate) semantic functions\n");
    printf("                      omit-line-numbers - do not include line nr info in output\n");
    printf("                      direct-access - use #defines to directly access values\n");
    printf("                      icache - generate an instruction cracking cache\n");
    printf("                      semantic-icache - include semantic code in cracking functions\n");
    printf("                      insn-in-icache - save original instruction when cracking\n");
    printf("                      default-nia-minus-one - instead of cia + insn-size\n");
    printf("                      delayed-branch - instead of cia + insn-size\n");
    printf("                      conditional-issue - conditionally issue each instruction\n");
    printf("                      validate-slot - perform slot verification as part of decode\n");
    printf("\n");
    printf("Input options:\n");
    printf("  -o <decode-rules>\n");
    printf("  -k <cache-rules>\n");
    printf("  -i <instruction-table>\n");
    printf("\n");
    printf("Output options:\n");
    printf("  -n <real-name>        Specify the real name of the next output file\n"); 
    printf("  -h 		    Generate header file\n");
    printf("  -c <output-file>      output icache\n");
    printf("  -d <output-file>      output idecode\n");
    printf("  -e <output-file>      output engine\n");
    printf("  -f <output-file>      output support functions\n");
    printf("  -m <output-file>      output model\n");
    printf("  -s <output-file>      output schematic\n");
    printf("  -t <output-file>      output itable\n");
  }

  while ((ch = getopt(argc, argv,
		      "F:I:B:H:N:T:P:G:o:k:i:n:hc:d:e:m:s:t:f:"))
	 != -1) {
    fprintf(stderr, "\t-%c %s\n", ch, (optarg ? optarg : ""));

    switch(ch) {

    case 'F':
      filters = new_filter(optarg, filters);
      break;

    case 'I':
      icache_size = a2i(optarg);
      code |= generate_with_icache;
      break;

    case 'B':
      insn_bit_size = a2i(optarg);
      ASSERT(insn_bit_size > 0 && insn_bit_size <= max_insn_bit_size
	     && (hi_bit_nr == insn_bit_size-1 || hi_bit_nr == 0));
      break;

    case 'H':
      hi_bit_nr = a2i(optarg);
      ASSERT(hi_bit_nr == insn_bit_size-1 || hi_bit_nr == 0);
      break;

    case 'N':
      generate_smp = a2i(optarg);
      break;

    case 'T':
      force_decode_gen_type(optarg);
      break;

    case 'P':
      {
	char *chp;
	global_name_prefix = strdup(optarg);
	chp = strdup(optarg);
	global_uname_prefix = chp;
	while (*chp) {
	  if (islower(*chp))
	    *chp = toupper(*chp);
	  chp++;
	}
      }
      break;

    case 'G':
      if (strcmp(optarg, "jumps") == 0) {
	code &= ~generate_calls;
	code |= generate_jumps;
      }
      else if (strcmp(optarg, "field-widths") == 0) {
	insn_specifying_widths = 1;
      }
      else if (strcmp(optarg, "duplicate") == 0) {
	generate_expanded_instructions = 1;
      }
      else if (strcmp(optarg, "omit-line-numbers") == 0) {
	file_references = lf_omit_references;
      }
      else if (strcmp(optarg, "direct-access") == 0) {
	code |= generate_with_direct_access;
      }
      else if (strcmp(optarg, "icache") == 0) {
      }
      else if (strcmp(optarg, "semantic-icache") == 0) {
	code |= generate_with_icache;
	code |= generate_with_semantic_icache;
      }
      else if (strcmp(optarg, "insn-in-icache") == 0) {
	code |= generate_with_icache;
	code |= generate_with_insn_in_icache;
      }
      else if (strcmp(optarg, "default-nia-minus-one") == 0) {
	code |= generate_with_semantic_returning_modified_nia_only;
	code &= ~generate_with_semantic_delayed_branch;
      }
      else if (strcmp(optarg, "delayed-branch") == 0) {
	code |= generate_with_semantic_delayed_branch;
	code &= ~generate_with_semantic_returning_modified_nia_only;
      }
      else if (strcmp(optarg, "conditional-issue") == 0) {
	code |= generate_with_semantic_conditional_issue;
      }
      else if (strcmp(optarg, "verify-slot") == 0) {
	code |= generate_with_idecode_slot_verification;
      }
      else
	error("Unknown option %s", optarg);
      break;

    case 'i':
      if (decode_rules == NULL || cache_rules == NULL) {
	fprintf(stderr, "Must specify decode and cache tables\n");
	exit (1);
      }
      instructions = load_insn_table(optarg, decode_rules, filters);
      fprintf(stderr, "\texpanding ...\n");
      insn_table_expand_insns(instructions);
      break;
    case 'o':
      decode_rules = load_decode_table(optarg, hi_bit_nr);
      break;
    case 'k':
      cache_rules = load_cache_table(optarg, hi_bit_nr);
      break;
    case 'n':
      real_file_name = strdup(optarg);
      break;
    case 'h':
      is_header = 1;
      break;
    case 's':
    case 'd':
    case 'e':
    case 'm':
    case 't':
    case 'f':
    case 'c':
      {
	lf *file = lf_open(optarg, real_file_name, file_references,
			   (is_header ? lf_is_h : lf_is_c),
			   argv[0]);
	lf_print__file_start(file);
	ASSERT(instructions != NULL);
	switch (ch) {
	case 's':
	  if(is_header)
	    gen_semantics_h(instructions, file, code);
	  else
	    gen_semantics_c(instructions, cache_rules, file, code);
	  break;
	case 'd':
	  if (is_header)
	    gen_idecode_h(file, instructions, cache_rules);
	  else
	    gen_idecode_c(file, instructions, cache_rules);
	  break;
	case 'e':
	  if (is_header)
	    gen_engine_h(file, instructions, cache_rules);
	  else
	    gen_engine_c(file, instructions, cache_rules);
	  break;
	case 'm':
	  if (is_header)
	    gen_model_h(instructions, file);
	  else
	    gen_model_c(instructions, file);
	  break;
	case 't':
	  if (is_header)
	    gen_itable_h(instructions, file);
	  else
	    gen_itable_c(instructions, file);
	  break;
	case 'f':
	  if (is_header)
	    gen_support_h(instructions, file);
	  else
	    gen_support_c(instructions, file);
	  break;
	case 'c':
	  if (is_header)
	    gen_icache_h(instructions, file, code);
	  else
	    gen_icache_c(instructions, cache_rules, file, code);
	  break;
	}
	lf_print__file_finish(file);
	lf_close(file);
	is_header = 0;
      }
      real_file_name = NULL;
      break;
    default:
      error("unknown option\n");
    }
  }
  return 0;
}
