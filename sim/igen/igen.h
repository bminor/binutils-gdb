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

/* code-generation options: */

typedef enum {

  /* Transfer control to an instructions semantic code using the the
     standard call/return mechanism */

  generate_calls,

  /* Transfer control to an instructions semantic code using
     (computed) goto's instead of the more conventional call/return
     mechanism */

  generate_jumps,

} igen_code;

typedef enum {
  nia_is_cia_plus_one,
  nia_is_void,
  nia_is_invalid,
} igen_nia;



typedef struct _igen_gen_options igen_gen_options;
struct _igen_gen_options {
  int direct_access;
  int semantic_icache;
  int insn_in_icache;
  int conditional_issue;
  int slot_verification;
  int delayed_branch;

  /* If zeroing a register, which one? */
  int zero_reg;
  int zero_reg_nr;

  /* should multiple simulators be generated? */
  int multi_sim;

  /* should the simulator support multi word instructions and if so,
     what is the max nr of words. */
  int multi_word;

  /* SMP?  Should the generated code include SMP support (>0) and if
     so, for how many processors? */
  int smp;

  /* how should the next instruction address be computed? */
  igen_nia nia;

  /* nr of instructions in the decoded instruction cache */
  int icache;
  int icache_size;

  /* see above */
  igen_code code;
};


typedef struct _igen_trace_options igen_trace_options;
struct _igen_trace_options {
  int rule_selection;
  int rule_rejection;
  int entries;
  int combine;
};

typedef struct _igen_prefix_name {
  char *name;
  char *uname;
} igen_prefix_name;

typedef struct _igen_prefix_options {
  igen_prefix_name global;
  igen_prefix_name engine;
  igen_prefix_name icache;
  igen_prefix_name idecode;
  igen_prefix_name itable;
  igen_prefix_name semantics;
  igen_prefix_name support;
} igen_prefix_options;

typedef struct _igen_decode_options igen_decode_options ;
struct _igen_decode_options {

  /* Combine tables?  Should the generator make a second pass through
     each generated table looking for any sub-entries that contain the
     same instructions.  Those entries being merged into a single
     table */
  int combine;

  /* Instruction expansion? Should the semantic code for each
     instruction, when the oportunity arrises, be expanded according
     to the variable opcode files that the instruction decode process
     renders constant */
  int duplicate;

  /* Treat reserved fields as constant (zero) instead of ignoring
     their value when determining decode tables */
  int zero_reserved;

  /* Convert any padded switch rules into goto_switch */
  int switch_as_goto;

  /* Force all tables to be generated with this lookup mechanism */
  char *overriding_gen;
};


typedef struct _igen_warn_options igen_warn_options;
struct _igen_warn_options {
  int discard;
};


typedef struct _igen_options igen_options;
struct _igen_options {

  /* What does the instruction look like - bit ordering, size, widths or
     offesets */
  int hi_bit_nr;
  int insn_bit_size;
  int insn_specifying_widths;

  /* what should global names be prefixed with? */
  igen_prefix_options prefix;

  /* See above for options and flags */
  igen_gen_options gen;

  /* See above for trace options */
  igen_trace_options trace;

  /* See above for decode options */
  igen_decode_options decode;

  /* Filter set to be used on the flag field of the instruction table */
  filter *flags_filter;

  /* See above for warn options */
  igen_warn_options warn;

  /* Be more picky about the input */
  error_func (*warning);

  /* Model (processor) set - like flags_filter. Used to select the
     specific ISA within a processor family. */
  filter *model_filter;

  /* Format name set */
  filter *format_name_filter;
};

extern igen_options options;

/* default options - hopefully backward compatible */ \
#define INIT_OPTIONS(OPTIONS) \
do { \
  memset (&(OPTIONS), 0, sizeof (OPTIONS)); \
  memset (&(OPTIONS).warn, -1, sizeof ((OPTIONS).warn)); \
  (OPTIONS).hi_bit_nr = 0; \
  (OPTIONS).insn_bit_size = default_insn_bit_size; \
  (OPTIONS).insn_specifying_widths = 0; \
  (OPTIONS).prefix.global.name = ""; \
  (OPTIONS).prefix.global.uname = ""; \
  (OPTIONS).prefix.engine = (OPTIONS).prefix.global; \
  (OPTIONS).prefix.icache = (OPTIONS).prefix.global; \
  (OPTIONS).prefix.idecode = (OPTIONS).prefix.global; \
  (OPTIONS).prefix.itable = (OPTIONS).prefix.global; \
  (OPTIONS).prefix.semantics = (OPTIONS).prefix.global; \
  (OPTIONS).prefix.support = (OPTIONS).prefix.global; \
  (OPTIONS).gen.code = generate_calls; \
  (OPTIONS).gen.icache_size = 1024; \
  (OPTIONS).warning = warning; \
} while (0)
