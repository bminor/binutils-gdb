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


/* What does the instruction look like - bit ordering, size, widths or offesets */
extern int hi_bit_nr;
extern int insn_bit_size;
extern int insn_specifying_widths;


/* what should global names be prefixed with? */
extern const char *global_name_prefix;
extern const char *global_uname_prefix;


/* generation options: */


enum {
  generate_with_direct_access = 0x1,
  generate_with_icache = 0x2,
  generate_with_semantic_icache = 0x4,
  generate_with_insn_in_icache = 0x8,

  generate_with_semantic_returning_modified_nia_only = 0x010,
  generate_with_semantic_conditional_issue           = 0x020,
  generate_with_idecode_slot_verification            = 0x040,
  generate_with_semantic_delayed_branch              = 0x080,
  generate_with_semantic_zero_r0                     = 0x100
};


typedef enum {

  /* Transfer control to an instructions semantic code using the the
     standard call/return mechanism */

  generate_calls = 0x1000,


  /* Transfer control to an instructions semantic code using
     (computed) goto's instead of the more conventional call/return
     mechanism */

  generate_jumps = 0x2000,


} igen_code;

extern int code;




extern int icache_size;


/* Instruction expansion?

   Should the semantic code for each instruction, when the oportunity
   arrises, be expanded according to the variable opcode files that
   the instruction decode process renders constant */

extern int generate_expanded_instructions;


/* SMP?

   Should the generated code include SMP support (>0) and if so, for
   how many processors? */

extern int generate_smp;




/* Misc junk */



/* Function header definitions */


/* Cache functions: */

extern int print_icache_function_formal
(lf *file);

extern int print_icache_function_actual
(lf *file);

extern int print_icache_function_type
(lf *file);

extern int print_semantic_function_formal
(lf *file);

extern int print_semantic_function_actual
(lf *file);

extern int print_semantic_function_type
(lf *file);

extern void print_my_defines
(lf *file,
 insn_bits *expanded_bits,
 table_entry *file_entry);

extern void print_itrace
(lf *file,
 table_entry *file_entry,
 int idecode);


typedef enum {
  function_name_prefix_semantics,
  function_name_prefix_idecode,
  function_name_prefix_itable,
  function_name_prefix_icache,
  function_name_prefix_none
} lf_function_name_prefixes;

extern int print_function_name
(lf *file,
 const char *basename,
 insn_bits *expanded_bits,
 lf_function_name_prefixes prefix);
