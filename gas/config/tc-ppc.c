/* tc-ppc.c -- Assemble for the PowerPC or POWER (RS/6000)
   Copyright (C) 1994 Free Software Foundation, Inc.
   Written by Ian Lance Taylor, Cygnus Support.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

#include <stdio.h>
#include <ctype.h>
#include "as.h"
#include "subsegs.h"

#include "opcode/ppc.h"

/* This is the assembler for the PowerPC or POWER (RS/6000) chips.  */

/* FIXME: This should be handled in a different way.  */
extern int target_big_endian;

static void ppc_set_cpu PARAMS ((void));
static unsigned long ppc_insert_operand
  PARAMS ((unsigned long insn, const struct powerpc_operand *operand,
	   offsetT val, char *file, unsigned int line));
static void ppc_macro PARAMS ((char *str, const struct powerpc_macro *macro));
static void ppc_byte PARAMS ((int));
static int ppc_is_toc_sym PARAMS ((symbolS *sym));
static void ppc_tc PARAMS ((int));
#ifdef OBJ_COFF
static void ppc_comm PARAMS ((int));
static void ppc_bb PARAMS ((int));
static void ppc_bf PARAMS ((int));
static void ppc_biei PARAMS ((int));
static void ppc_bs PARAMS ((int));
static void ppc_eb PARAMS ((int));
static void ppc_ef PARAMS ((int));
static void ppc_es PARAMS ((int));
static void ppc_csect PARAMS ((int));
static void ppc_function PARAMS ((int));
static void ppc_extern PARAMS ((int));
static void ppc_lglobl PARAMS ((int));
static void ppc_stabx PARAMS ((int));
static void ppc_rename PARAMS ((int));
static void ppc_toc PARAMS ((int));
#endif

/* Generic assembler global variables which must be defined by all
   targets.  */

/* Characters which always start a comment.  */
const char comment_chars[] = "#";

/* Characters which start a comment at the beginning of a line.  */
const char line_comment_chars[] = "#";

/* Characters which may be used to separate multiple commands on a
   single line.  */
const char line_separator_chars[] = ";";

/* Characters which are used to indicate an exponent in a floating
   point number.  */
const char EXP_CHARS[] = "eE";

/* Characters which mean that a number is a floating point constant,
   as in 0d1.0.  */
const char FLT_CHARS[] = "dD";

/* The target specific pseudo-ops which we support.  */

const pseudo_typeS md_pseudo_table[] =
{
  /* Pseudo-ops which must be overridden.  */
  { "byte",	ppc_byte,	0 },

#ifdef OBJ_COFF
  /* Pseudo-ops specific to the RS/6000 XCOFF format.  Some of these
     legitimately belong in the obj-*.c file.  However, XCOFF is based
     on COFF, and is only implemented for the RS/6000.  We just use
     obj-coff.c, and add what we need here.  */
  { "comm",	ppc_comm,	0 },
  { "lcomm",	ppc_comm,	1 },
  { "bb",	ppc_bb,		0 },
  { "bf",	ppc_bf,		0 },
  { "bi",	ppc_biei,	0 },
  { "bs",	ppc_bs,		0 },
  { "csect",	ppc_csect,	0 },
  { "eb",	ppc_eb,		0 },
  { "ef",	ppc_ef,		0 },
  { "ei",	ppc_biei,	1 },
  { "es",	ppc_es,		0 },
  { "extern",	ppc_extern,	0 },
  { "function",	ppc_function,	0 },
  { "lglobl",	ppc_lglobl,	0 },
  { "rename",	ppc_rename,	0 },
  { "stabx",	ppc_stabx,	0 },
  { "toc",	ppc_toc,	0 },
#endif

  /* This pseudo-op is used even when not generating XCOFF output.  */
  { "tc",	ppc_tc,		0 },

  { NULL,	NULL,		0 }
};

/* Local variables.  */

/* The type of processor we are assembling for.  This is one or more
   of the PPC_OPCODE flags defined in opcode/ppc.h.  */
static int ppc_cpu = 0;

/* The size of the processor we are assembling for.  This is either
   PPC_OPCODE_32 or PPC_OPCODE_64.  */
static int ppc_size = PPC_OPCODE_32;

/* The endianness we are using.  */
static int ppc_big_endian = PPC_BIG_ENDIAN;

/* Opcode hash table.  */
static struct hash_control *ppc_hash;

/* Macro hash table.  */
static struct hash_control *ppc_macro_hash;

#ifdef OBJ_COFF

/* The RS/6000 assembler uses the .csect pseudo-op to generate code
   using a bunch of different sections.  These assembler sections,
   however, are all encompassed within the .text or .data sections of
   the final output file.  We handle this by using different
   subsegments within these main segments.  */

/* Next subsegment to allocate within the .text segment.  */
static subsegT ppc_text_subsegment = 2;

/* Linked list of csects in the text section.  */
static symbolS *ppc_text_csects;

/* Next subsegment to allocate within the .data segment.  */
static subsegT ppc_data_subsegment = 2;

/* Linked list of csects in the data section.  */
static symbolS *ppc_data_csects;

/* The current csect.  */
static symbolS *ppc_current_csect;

/* The RS/6000 assembler uses a TOC which holds addresses of functions
   and variables.  Symbols are put in the TOC with the .tc pseudo-op.
   A special relocation is used when accessing TOC entries.  We handle
   the TOC as a subsegment within the .data segment.  We set it up if
   we see a .toc pseudo-op, and save the csect symbol here.  */
static symbolS *ppc_toc_csect;

/* The first frag in the TOC subsegment.  */
static fragS *ppc_toc_frag;

/* The first frag in the first subsegment after the TOC in the .data
   segment.  NULL if there are no subsegments after the TOC.  */
static fragS *ppc_after_toc_frag;

/* The current static block.  */
static symbolS *ppc_current_block;

/* The COFF debugging section; set by md_begin.  This is not the
   .debug section, but is instead the secret BFD section which will
   cause BFD to set the section number of a symbol to N_DEBUG.  */
static asection *ppc_coff_debug_section;

/* The size of the .debug section.  */
static bfd_size_type ppc_debug_name_section_size;

#endif /* OBJ_COFF */

#ifdef OBJ_ELF
CONST char *md_shortopts = "um:VQ:";
#else
CONST char *md_shortopts = "um:";
#endif
struct option md_longopts[] = {
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof(md_longopts);

int
md_parse_option (c, arg)
     int c;
     char *arg;
{
  switch (c)
    {
    case 'u':
      /* -u means that any undefined symbols should be treated as
	 external, which is the default for gas anyhow.  */
      break;

    case 'm':
      /* -mpwrx and -mpwr2 mean to assemble for the IBM POWER/2
         (RIOS2).  */
      if (strcmp (arg, "pwrx") == 0 || strcmp (arg, "pwr2") == 0)
	ppc_cpu = PPC_OPCODE_POWER | PPC_OPCODE_POWER2;
      /* -mpwr means to assemble for the IBM POWER (RIOS1).  */
      else if (strcmp (arg, "pwr") == 0)
	ppc_cpu = PPC_OPCODE_POWER;
      /* -m601 means to assemble for the Motorola PowerPC 601.  FIXME: We
	 ignore the option for now, but we should really use it to permit
	 instructions defined on the 601 that are not part of the standard
	 PowerPC architecture (mostly holdovers from the POWER).  */
      else if (strcmp (arg, "601") == 0)
	ppc_cpu = PPC_OPCODE_PPC | PPC_OPCODE_601;
      /* -mppc, -mppc32, -m603, and -m604 mean to assemble for the
         Motorola PowerPC 603/604.  */
      else if (strcmp (arg, "ppc") == 0
	       || strcmp (arg, "ppc32") == 0
	       || strcmp (arg, "603") == 0
	       || strcmp (arg, "604") == 0)
	ppc_cpu = PPC_OPCODE_PPC;
      /* -mppc64 and -m620 mean to assemble for the 64-bit PowerPC
         620.  */
      else if (strcmp (arg, "ppc64") == 0 || strcmp (arg, "620") == 0)
	{
	  ppc_cpu = PPC_OPCODE_PPC;
	  ppc_size = PPC_OPCODE_64;
	}
      /* -many means to assemble for any architecture (PWR/PWRX/PPC).  */
      else if (strcmp (arg, "any") == 0)
	ppc_cpu = PPC_OPCODE_POWER | PPC_OPCODE_POWER2 | PPC_OPCODE_PPC;
      else
	{
	  as_bad ("invalid architecture -m%s", arg);
	  return 0;
	}
      break;

#ifdef OBJ_ELF
      /* -V: SVR4 argument to print version ID.  */
    case 'V':
      print_version_id ();
      break;

      /* -Qy, -Qn: SVR4 arguments controlling whether a .comment section
	 should be emitted or not.  FIXME: Not implemented.  */
    case 'Q':
      break;
#endif

    default:
      return 0;
    }

  return 1;
}

void
md_show_usage (stream)
     FILE *stream;
{
  fprintf(stream, "\
PowerPC options:\n\
-u			ignored\n\
-mpwrx			generate code for IBM POWER/2 (RIOS2)\n\
-mpwr			generate code for IBM POWER (RIOS1)\n\
-m601			generate code for Motorola PowerPC 601\n\
-mppc			generate code for Motorola PowerPC 603/604\n\
-many			generate code for any architecture (PWR/PWRX/PPC)\n");
#ifdef OBJ_ELF
  fprintf(stream, "\
-V			print assembler version number\n\
-Qy, -Qn		ignored\n");
#endif
}

/* Set ppc_cpu if it is not already set.  */

static void
ppc_set_cpu ()
{
  const char *default_cpu = TARGET_CPU;

  if (ppc_cpu == 0)
    {
      if (strcmp (default_cpu, "rs6000") == 0)
	ppc_cpu = PPC_OPCODE_POWER;
      else if (strcmp (default_cpu, "powerpc") == 0)
	ppc_cpu = PPC_OPCODE_PPC;
      else
	abort ();
    }
}

/* Figure out the BFD architecture to use.  */

enum bfd_architecture
ppc_arch ()
{
  ppc_set_cpu ();

  if ((ppc_cpu & PPC_OPCODE_PPC) != 0)
    return bfd_arch_powerpc;
  else if ((ppc_cpu & PPC_OPCODE_POWER) != 0)
    return bfd_arch_rs6000;
  else
    abort ();
}

/* This function is called when the assembler starts up.  It is called
   after the options have been parsed and the output file has been
   opened.  */

void
md_begin ()
{
  register const struct powerpc_opcode *op;
  const struct powerpc_opcode *op_end;
  const struct powerpc_macro *macro;
  const struct powerpc_macro *macro_end;

  ppc_set_cpu ();

  /* Insert the opcodes into a hash table.  */
  ppc_hash = hash_new ();

  op_end = powerpc_opcodes + powerpc_num_opcodes;
  for (op = powerpc_opcodes; op < op_end; op++)
    {
      know ((op->opcode & op->mask) == op->opcode);

      if ((op->flags & ppc_cpu) != 0
	  && ((op->flags & (PPC_OPCODE_32 | PPC_OPCODE_64)) == 0
	      || (op->flags & (PPC_OPCODE_32 | PPC_OPCODE_64)) == ppc_size))
	{
	  const char *retval;

	  retval = hash_insert (ppc_hash, op->name, (PTR) op);
	  if (retval != (const char *) NULL)
	    {
	      /* When using -many, the comparison instructions are a
		 harmless special case.  */
	      if (ppc_cpu != (PPC_OPCODE_POWER
			      | PPC_OPCODE_POWER2
			      | PPC_OPCODE_PPC)
		  || (strcmp (op->name, "cmpli") != 0
		      && strcmp (op->name, "cmpi") != 0
		      && strcmp (op->name, "cmp") != 0
		      && strcmp (op->name, "cmpl") != 0))
		abort ();
	    }
	}
    }

  /* Insert the macros into a hash table.  */
  ppc_macro_hash = hash_new ();

  macro_end = powerpc_macros + powerpc_num_macros;
  for (macro = powerpc_macros; macro < macro_end; macro++)
    {
      if ((macro->flags & ppc_cpu) != 0)
	{
	  const char *retval;

	  retval = hash_insert (ppc_macro_hash, macro->name, (PTR) macro);
	  if (retval != (const char *) NULL)
	    abort ();
	}
    }

  /* Tell the main code what the endianness is.  */
  target_big_endian = ppc_big_endian;

#ifdef OBJ_COFF
  ppc_coff_debug_section = coff_section_from_bfd_index (stdoutput, N_DEBUG);

  /* Create dummy symbols to serve as initial csects.  This forces the
     text csects to precede the data csects.  These symbols will not
     be output.  */
  ppc_text_csects = symbol_make ("dummy\001");
  ppc_text_csects->sy_tc.within = ppc_text_csects;
  ppc_data_csects = symbol_make ("dummy\001");
  ppc_data_csects->sy_tc.within = ppc_data_csects;
#endif
}

/* Insert an operand value into an instruction.  */

static unsigned long
ppc_insert_operand (insn, operand, val, file, line)
     unsigned long insn;
     const struct powerpc_operand *operand;
     offsetT val;
     char *file;
     unsigned int line;
{
  if (operand->bits != 32)
    {
      long min, max;
      offsetT test;

      if ((operand->flags & PPC_OPERAND_SIGNED) != 0)
	{
	  if ((operand->flags & PPC_OPERAND_SIGNOPT) != 0
	      && ppc_size == PPC_OPCODE_32)
	    max = (1 << operand->bits) - 1;
	  else
	    max = (1 << (operand->bits - 1)) - 1;
	  min = - (1 << (operand->bits - 1));
	}
      else
	{
	  max = (1 << operand->bits) - 1;
	  min = 0;
	}

      if ((operand->flags & PPC_OPERAND_NEGATIVE) != 0)
	test = - val;
      else
	test = val;

      if (test < (offsetT) min || test > (offsetT) max)
	{
	  const char *err =
	    "operand out of range (%s not between %ld and %ld)";
	  char buf[100];

	  sprint_value (buf, test);
	  if (file == (char *) NULL)
	    as_warn (err, buf, min, max);
	  else
	    as_warn_where (file, line, err, buf, min, max);
	}
    }

  if (operand->insert)
    {
      const char *errmsg;

      errmsg = NULL;
      insn = (*operand->insert) (insn, (long) val, &errmsg);
      if (errmsg != (const char *) NULL)
	as_warn (errmsg);
    }
  else
    insn |= (((long) val & ((1 << operand->bits) - 1))
	     << operand->shift);

  return insn;
}

/* We need to keep a list of fixups.  We can't simply generate them as
   we go, because that would require us to first create the frag, and
   that would screw up references to ``.''.  */

struct ppc_fixup
{
  expressionS exp;
  int opindex;
};

#define MAX_INSN_FIXUPS (5)

/* This routine is called for each instruction to be assembled.  */

void
md_assemble (str)
     char *str;
{
  char *s;
  const struct powerpc_opcode *opcode;
  unsigned long insn;
  const unsigned char *opindex_ptr;
  int skip_optional;
  int need_paren;
  int next_opindex;
  struct ppc_fixup fixups[MAX_INSN_FIXUPS];
  int fc;
  char *f;
  int i;

  /* Get the opcode.  */
  for (s = str; *s != '\0' && ! isspace (*s); s++)
    ;
  if (*s != '\0')
    *s++ = '\0';

  /* Look up the opcode in the hash table.  */
  opcode = (const struct powerpc_opcode *) hash_find (ppc_hash, str);
  if (opcode == (const struct powerpc_opcode *) NULL)
    {
      const struct powerpc_macro *macro;

      macro = (const struct powerpc_macro *) hash_find (ppc_macro_hash, str);
      if (macro == (const struct powerpc_macro *) NULL)
	as_bad ("Unrecognized opcode: `%s'", str);
      else
	ppc_macro (s, macro);

      return;
    }

  insn = opcode->opcode;

  str = s;
  while (isspace (*str))
    ++str;

  /* PowerPC operands are just expressions.  The only real issue is
     that a few operand types are optional.  All cases which might use
     an optional operand separate the operands only with commas (in
     some cases parentheses are used, as in ``lwz 1,0(1)'' but such
     cases never have optional operands).  There is never more than
     one optional operand for an instruction.  So, before we start
     seriously parsing the operands, we check to see if we have an
     optional operand, and, if we do, we count the number of commas to
     see whether the operand should be omitted.  */
  skip_optional = 0;
  for (opindex_ptr = opcode->operands; *opindex_ptr != 0; opindex_ptr++)
    {
      const struct powerpc_operand *operand;

      operand = &powerpc_operands[*opindex_ptr];
      if ((operand->flags & PPC_OPERAND_OPTIONAL) != 0)
	{
	  unsigned int opcount;

	  /* There is an optional operand.  Count the number of
	     commas in the input line.  */
	  if (*str == '\0')
	    opcount = 0;
	  else
	    {
	      opcount = 1;
	      s = str;
	      while ((s = strchr (s, ',')) != (char *) NULL)
		{
		  ++opcount;
		  ++s;
		}
	    }

	  /* If there are fewer operands in the line then are called
	     for by the instruction, we want to skip the optional
	     operand.  */
	  if (opcount < strlen (opcode->operands))
	    skip_optional = 1;

	  break;
	}
    }

  /* Gather the operands.  */
  need_paren = 0;
  next_opindex = 0;
  fc = 0;
  for (opindex_ptr = opcode->operands; *opindex_ptr != 0; opindex_ptr++)
    {
      const struct powerpc_operand *operand;
      const char *errmsg;
      char *hold;
      expressionS ex;
      char endc;

      if (next_opindex == 0)
	operand = &powerpc_operands[*opindex_ptr];
      else
	{
	  operand = &powerpc_operands[next_opindex];
	  next_opindex = 0;
	}

      errmsg = NULL;

      /* If this is a fake operand, then we do not expect anything
	 from the input.  */
      if ((operand->flags & PPC_OPERAND_FAKE) != 0)
	{
	  insn = (*operand->insert) (insn, 0L, &errmsg);
	  if (errmsg != (const char *) NULL)
	    as_warn (errmsg);
	  continue;
	}

      /* If this is an optional operand, and we are skipping it, just
	 insert a zero.  */
      if ((operand->flags & PPC_OPERAND_OPTIONAL) != 0
	  && skip_optional)
	{
	  if (operand->insert)
	    {
	      insn = (*operand->insert) (insn, 0L, &errmsg);
	      if (errmsg != (const char *) NULL)
		as_warn (errmsg);
	    }
	  if ((operand->flags & PPC_OPERAND_NEXT) != 0)
	    next_opindex = *opindex_ptr + 1;
	  continue;
	}

      /* Gather the operand.  */
      hold = input_line_pointer;
      input_line_pointer = str;
      expression (&ex);
      str = input_line_pointer;
      input_line_pointer = hold;

      if (ex.X_op == O_illegal)
	as_bad ("illegal operand");
      else if (ex.X_op == O_absent)
	as_bad ("missing operand");
      else if (ex.X_op == O_constant)
	insn = ppc_insert_operand (insn, operand, ex.X_add_number,
				   (char *) NULL, 0);
      else
	{
	  /* We need to generate a fixup for this expression.  */
	  if (fc >= MAX_INSN_FIXUPS)
	    as_fatal ("too many fixups");
	  fixups[fc].exp = ex;
	  fixups[fc].opindex = *opindex_ptr;
	  ++fc;
	}

      if (need_paren)
	{
	  endc = ')';
	  need_paren = 0;
	}
      else if ((operand->flags & PPC_OPERAND_PARENS) != 0)
	{
	  endc = '(';
	  need_paren = 1;
	}
      else
	endc = ',';

      /* The call to expression should have advanced str past any
	 whitespace.  */
      if (*str != endc
	  && (endc != ',' || *str != '\0'))
	{
	  as_bad ("syntax error; found `%c' but expected `%c'", *str, endc);
	  break;
	}

      if (*str != '\0')
	++str;
    }

  while (isspace (*str))
    ++str;

  if (*str != '\0')
    as_bad ("junk at end of line: `%s'", str);

  /* Write out the instruction.  */
  f = frag_more (4);
  md_number_to_chars (f, insn, 4);

  /* Create any fixups.  At this point we do not use a
     bfd_reloc_code_real_type, but instead just use the operand index.
     This lets us easily handle fixups for any operand type, although
     that is admittedly not a very exciting feature.  We pick a BFD
     reloc type in md_apply_fix.  */
  for (i = 0; i < fc; i++)
    {
      const struct powerpc_operand *operand;

      operand = &powerpc_operands[fixups[i].opindex];
      fix_new_exp (frag_now, f - frag_now->fr_literal, 4,
		   &fixups[i].exp,
		   (operand->flags & PPC_OPERAND_RELATIVE) != 0,
		   ((bfd_reloc_code_real_type)
		    (fixups[i].opindex + (int) BFD_RELOC_UNUSED)));
    }
}

/* Handle a macro.  Gather all the operands, transform them as
   described by the macro, and call md_assemble recursively.  All the
   operands are separated by commas; we don't accept parentheses
   around operands here.  */

static void
ppc_macro (str, macro)
     char *str;
     const struct powerpc_macro *macro;
{
  char *operands[10];
  int count;
  char *s;
  unsigned int len;
  const char *format;
  int arg;
  char *send;
  char *complete;

  /* Gather the users operands into the operands array.  */
  count = 0;
  s = str;
  while (1)
    {
      if (count >= sizeof operands / sizeof operands[0])
	break;
      operands[count++] = s;
      s = strchr (s, ',');
      if (s == (char *) NULL)
	break;
      *s++ = '\0';
    }  

  if (count != macro->operands)
    {
      as_bad ("wrong number of operands");
      return;
    }

  /* Work out how large the string must be (the size is unbounded
     because it includes user input).  */
  len = 0;
  format = macro->format;
  while (*format != '\0')
    {
      if (*format != '%')
	{
	  ++len;
	  ++format;
	}
      else
	{
	  arg = strtol (format + 1, &send, 10);
	  know (send != format && arg >= 0 && arg < count);
	  len += strlen (operands[arg]);
	  format = send;
	}
    }

  /* Put the string together.  */
  complete = s = (char *) alloca (len + 1);
  format = macro->format;
  while (*format != '\0')
    {
      if (*format != '%')
	*s++ = *format++;
      else
	{
	  arg = strtol (format + 1, &send, 10);
	  strcpy (s, operands[arg]);
	  s += strlen (s);
	  format = send;
	}
    }
  *s = '\0';

  /* Assemble the constructed instruction.  */
  md_assemble (complete);
}  

/* Pseudo-op handling.  */

/* The .byte pseudo-op.  This is similar to the normal .byte
   pseudo-op, but it can also take a single ASCII string.  */

static void
ppc_byte (ignore)
     int ignore;
{
  if (*input_line_pointer != '\"')
    {
      cons (1);
      return;
    }

  /* Gather characters.  A real double quote is doubled.  Unusual
     characters are not permitted.  */
  ++input_line_pointer;
  while (1)
    {
      char c;

      c = *input_line_pointer++;

      if (c == '\"')
	{
	  if (*input_line_pointer != '\"')
	    break;
	  ++input_line_pointer;
	}

      FRAG_APPEND_1_CHAR (c);
    }

  demand_empty_rest_of_line ();
}

#ifdef OBJ_COFF

/* XCOFF specific pseudo-op handling.  */

/* The .comm and .lcomm pseudo-ops for XCOFF.  XCOFF puts common
   symbols in the .bss segment as though they were local common
   symbols, and uses a different smclas.  */

static void
ppc_comm (lcomm)
     int lcomm;
{
  asection *current_seg = now_seg;
  subsegT current_subseg = now_subseg;
  char *name;
  char endc;
  char *end_name;
  offsetT size;
  offsetT align;
  symbolS *lcomm_sym = NULL;
  symbolS *sym;
  char *pfrag;

  name = input_line_pointer;
  endc = get_symbol_end ();
  end_name = input_line_pointer;
  *end_name = endc;

  if (*input_line_pointer != ',')
    {
      as_bad ("missing size");
      ignore_rest_of_line ();
      return;
    }
  ++input_line_pointer;

  size = get_absolute_expression ();
  if (size < 0)
    {
      as_bad ("negative size");
      ignore_rest_of_line ();
      return;
    }

  if (! lcomm)
    {
      /* The third argument to .comm is the alignment.  */
      if (*input_line_pointer != ',')
	align = 3;
      else
	{
	  ++input_line_pointer;
	  align = get_absolute_expression ();
	  if (align <= 0)
	    {
	      as_warn ("ignoring bad alignment");
	      align = 3;
	    }
	}
    }
  else
    {
      char *lcomm_name;
      char lcomm_endc;

      if (size <= 1)
	align = 0;
      else if (size <= 2)
	align = 1;
      else if (size <= 4)
	align = 2;
      else
	align = 3;

      /* The third argument to .lcomm appears to be the real local
	 common symbol to create.  References to the symbol named in
	 the first argument are turned into references to the third
	 argument.  */
      if (*input_line_pointer != ',')
	{
	  as_bad ("missing real symbol name");
	  ignore_rest_of_line ();
	  return;
	}
      ++input_line_pointer;

      lcomm_name = input_line_pointer;
      lcomm_endc = get_symbol_end ();
      
      lcomm_sym = symbol_find_or_make (lcomm_name);

      *input_line_pointer = lcomm_endc;
    }

  *end_name = '\0';
  sym = symbol_find_or_make (name);
  *end_name = endc;

  if (S_IS_DEFINED (sym)
      || S_GET_VALUE (sym) != 0)
    {
      as_bad ("attempt to redefine symbol");
      ignore_rest_of_line ();
      return;
    }
    
  record_alignment (bss_section, align);
	  
  if (! lcomm
      || ! S_IS_DEFINED (lcomm_sym))
    {
      symbolS *def_sym;
      offsetT def_size;

      if (! lcomm)
	{
	  def_sym = sym;
	  def_size = size;
	  S_SET_EXTERNAL (sym);
	}
      else
	{
	  lcomm_sym->sy_tc.output = 1;
	  def_sym = lcomm_sym;
	  def_size = 0;
	}

      subseg_set (bss_section, 1);
      frag_align (align, 0);
  
      def_sym->sy_frag = frag_now;
      pfrag = frag_var (rs_org, 1, 1, (relax_substateT) 0, def_sym,
			def_size, (char *) NULL);
      *pfrag = 0;
      S_SET_SEGMENT (def_sym, bss_section);
      def_sym->sy_tc.align = align;
    }
  else if (lcomm)
    {
      /* Align the size of lcomm_sym.  */
      lcomm_sym->sy_frag->fr_offset =
	((lcomm_sym->sy_frag->fr_offset + (1 << align) - 1)
	 &~ ((1 << align) - 1));
      if (align > lcomm_sym->sy_tc.align)
	lcomm_sym->sy_tc.align = align;
    }

  if (lcomm)
    {
      /* Make sym an offset from lcomm_sym.  */
      S_SET_SEGMENT (sym, bss_section);
      sym->sy_frag = lcomm_sym->sy_frag;
      S_SET_VALUE (sym, lcomm_sym->sy_frag->fr_offset);
      lcomm_sym->sy_frag->fr_offset += size;
    }

  subseg_set (current_seg, current_subseg);

  demand_empty_rest_of_line ();
}

/* The .csect pseudo-op.  This switches us into a different
   subsegment.  The first argument is a symbol whose value is the
   start of the .csect.  In COFF, csect symbols get special aux
   entries defined by the x_csect field of union internal_auxent.  The
   optional second argument is the alignment (the default is 2).  */

static void
ppc_csect (ignore)
     int ignore;
{
  char *name;
  char endc;
  symbolS *sym;

  name = input_line_pointer;
  endc = get_symbol_end ();
  
  sym = symbol_find_or_make (name);

  *input_line_pointer = endc;

  if (S_IS_DEFINED (sym))
    subseg_set (S_GET_SEGMENT (sym), sym->sy_tc.subseg);
  else
    {
      symbolS **list_ptr;
      int after_toc;
      symbolS *list;

      /* This is a new csect.  We need to look at the symbol class to
	 figure out whether it should go in the text section or the
	 data section.  */
      after_toc = 0;
      switch (sym->sy_tc.class)
	{
	case XMC_PR:
	case XMC_RO:
	case XMC_DB:
	case XMC_GL:
	case XMC_XO:
	case XMC_SV:
	case XMC_TI:
	case XMC_TB:
	  S_SET_SEGMENT (sym, text_section);
	  sym->sy_tc.subseg = ppc_text_subsegment;
	  ++ppc_text_subsegment;
	  list_ptr = &ppc_text_csects;
	  break;
	case XMC_RW:
	case XMC_TC0:
	case XMC_TC:
	case XMC_DS:
	case XMC_UA:
	case XMC_BS:
	case XMC_UC:
	  if (ppc_toc_csect->sy_tc.subseg + 1 == ppc_data_subsegment)
	    after_toc = 1;
	  S_SET_SEGMENT (sym, data_section);
	  sym->sy_tc.subseg = ppc_data_subsegment;
	  ++ppc_data_subsegment;
	  list_ptr = &ppc_data_csects;
	  break;
	default:
	  abort ();
	}

      subseg_new (segment_name (S_GET_SEGMENT (sym)), sym->sy_tc.subseg);
      if (after_toc)
	ppc_after_toc_frag = frag_now;

      sym->sy_frag = frag_now;
      S_SET_VALUE (sym, (valueT) frag_now_fix ());

      sym->sy_tc.align = 2;
      sym->sy_tc.output = 1;
      sym->sy_tc.within = sym;
	  
      for (list = *list_ptr;
	   list->sy_tc.next != (symbolS *) NULL;
	   list = list->sy_tc.next)
	;
      list->sy_tc.next = sym;
	  
      symbol_remove (sym, &symbol_rootP, &symbol_lastP);
      symbol_append (sym, list->sy_tc.within, &symbol_rootP, &symbol_lastP);
    }

  if (*input_line_pointer == ',')
    {
      ++input_line_pointer;
      sym->sy_tc.align = get_absolute_expression ();
    }

  ppc_current_csect = sym;

  demand_empty_rest_of_line ();
}

/* The .extern pseudo-op.  We create an undefined symbol.  */

static void
ppc_extern (ignore)
     int ignore;
{
  char *name;
  char endc;

  name = input_line_pointer;
  endc = get_symbol_end ();

  (void) symbol_find_or_make (name);

  *input_line_pointer = endc;

  demand_empty_rest_of_line ();
}

/* The .lglobl pseudo-op.  I think the RS/6000 assembler only needs
   this because it can't handle undefined symbols.  I think we can
   just ignore it.  */

static void
ppc_lglobl (ignore)
     int ignore;
{
  s_ignore (0);
}

/* The .rename pseudo-op.  The RS/6000 assembler can rename symbols,
   although I don't know why it bothers.  */

static void
ppc_rename (ignore)
     int ignore;
{
  char *name;
  char endc;
  symbolS *sym;
  int len;

  name = input_line_pointer;
  endc = get_symbol_end ();

  sym = symbol_find_or_make (name);

  *input_line_pointer = endc;

  if (*input_line_pointer != ',')
    {
      as_bad ("missing rename string");
      ignore_rest_of_line ();
      return;
    }
  ++input_line_pointer;

  sym->sy_tc.real_name = demand_copy_C_string (&len);

  demand_empty_rest_of_line ();
}

/* The .stabx pseudo-op.  This is similar to a normal .stabs
   pseudo-op, but slightly different.  A sample is
       .stabx "main:F-1",.main,142,0
   The first argument is the symbol name to create.  The second is the
   value, and the third is the storage class.  The fourth seems to be
   always zero, and I am assuming it is the type.  */

static void
ppc_stabx (ignore)
     int ignore;
{
  char *name;
  int len;
  symbolS *sym;
  expressionS exp;

  name = demand_copy_C_string (&len);

  if (*input_line_pointer != ',')
    {
      as_bad ("missing value");
      return;
    }
  ++input_line_pointer;

  sym = symbol_make (name);

  (void) expression (&exp);

  switch (exp.X_op)
    {
    case O_illegal:
    case O_absent:
    case O_big:
      as_bad ("illegal .stabx expression; zero assumed");
      exp.X_add_number = 0;
      /* Fall through.  */
    case O_constant:
      S_SET_VALUE (sym, (valueT) exp.X_add_number);
      sym->sy_frag = &zero_address_frag;
      break;

    case O_symbol:
      if (S_GET_SEGMENT (exp.X_add_symbol) == undefined_section)
	sym->sy_value = exp;
      else
	{
	  S_SET_VALUE (sym,
		       exp.X_add_number + S_GET_VALUE (exp.X_add_symbol));
	  sym->sy_frag = exp.X_add_symbol->sy_frag;
	}
      break;

    default:
      /* The value is some complex expression.  This will probably
         fail at some later point, but this is probably the right
         thing to do here.  */
      sym->sy_value = exp;
      break;
    }

  S_SET_SEGMENT (sym, ppc_coff_debug_section);
  sym->bsym->flags |= BSF_DEBUGGING;

  if (*input_line_pointer != ',')
    {
      as_bad ("missing class");
      return;
    }
  ++input_line_pointer;

  S_SET_STORAGE_CLASS (sym, get_absolute_expression ());

  if (*input_line_pointer != ',')
    {
      as_bad ("missing type");
      return;
    }
  ++input_line_pointer;

  S_SET_DATA_TYPE (sym, get_absolute_expression ());

  sym->sy_tc.output = 1;

  if (S_GET_STORAGE_CLASS (sym) == C_STSYM)
    sym->sy_tc.within = ppc_current_block;

  if (exp.X_op != O_symbol
      || ! S_IS_EXTERNAL (exp.X_add_symbol)
      || S_GET_SEGMENT (exp.X_add_symbol) != bss_section)
    ppc_frob_label (sym);
  else
    {
      symbol_remove (sym, &symbol_rootP, &symbol_lastP);
      symbol_append (sym, exp.X_add_symbol, &symbol_rootP, &symbol_lastP);
      if (ppc_current_csect->sy_tc.within == exp.X_add_symbol)
	ppc_current_csect->sy_tc.within = sym;
    }

  if (strlen (name) > SYMNMLEN)
    {
      /* For some reason, each name is preceded by a two byte length
	 and followed by a null byte.  */
      ppc_debug_name_section_size += strlen (name) + 3;
    }

  demand_empty_rest_of_line ();
}

/* The .function pseudo-op.  This takes several arguments.  The first
   argument seems to be the external name of the symbol.  The second
   argment seems to be the label for the start of the function.  gcc
   uses the same name for both.  I have no idea what the third and
   fourth arguments are meant to be.  The optional fifth argument is
   an expression for the size of the function.  In COFF this symbol
   gets an aux entry like that used for a csect.  */

static void
ppc_function (ignore)
     int ignore;
{
  char *name;
  char endc;
  char *s;
  symbolS *ext_sym;
  symbolS *lab_sym;

  name = input_line_pointer;
  endc = get_symbol_end ();

  /* Ignore any [PR] suffix.  */
  name = ppc_canonicalize_symbol_name (name);
  s = strchr (name, '[');
  if (s != (char *) NULL
      && strcmp (s + 1, "PR]") == 0)
    *s = '\0';

  ext_sym = symbol_find_or_make (name);

  *input_line_pointer = endc;

  if (*input_line_pointer != ',')
    {
      as_bad ("missing symbol name");
      ignore_rest_of_line ();
      return;
    }
  ++input_line_pointer;

  name = input_line_pointer;
  endc = get_symbol_end ();

  lab_sym = symbol_find_or_make (name);

  *input_line_pointer = endc;

  if (ext_sym != lab_sym)
    {
      ext_sym->sy_value.X_op = O_symbol;
      ext_sym->sy_value.X_add_symbol = lab_sym;
      ext_sym->sy_value.X_op_symbol = NULL;
      ext_sym->sy_value.X_add_number = 0;
    }

  if (ext_sym->sy_tc.class == -1)
    ext_sym->sy_tc.class = XMC_PR;
  ext_sym->sy_tc.output = 1;

  if (*input_line_pointer == ',')
    {
      expressionS ignore;

      /* Ignore the third argument.  */
      ++input_line_pointer;
      expression (&ignore);
      if (*input_line_pointer == ',')
	{
	  /* Ignore the fourth argument.  */
	  ++input_line_pointer;
	  expression (&ignore);
	  if (*input_line_pointer == ',')
	    {
	      /* The fifth argument is the function size.  */
	      ++input_line_pointer;
	      ext_sym->sy_tc.size = symbol_new ("L0\001",
						absolute_section,
						(valueT) 0,
						&zero_address_frag);
	      pseudo_set (ext_sym->sy_tc.size);
	    }
	}
    }

  S_SET_DATA_TYPE (ext_sym, DT_FCN << N_BTSHFT);
  SF_SET_FUNCTION (ext_sym);
  SF_SET_PROCESS (ext_sym);
  coff_add_linesym (ext_sym);

  demand_empty_rest_of_line ();
}

/* The .bf pseudo-op.  This is just like a COFF C_FCN symbol named
   ".bf".  */

static void
ppc_bf (ignore)
     int ignore;
{
  symbolS *sym;

  sym = symbol_make (".bf");
  S_SET_SEGMENT (sym, text_section);
  sym->sy_frag = frag_now;
  S_SET_VALUE (sym, frag_now_fix ());
  S_SET_STORAGE_CLASS (sym, C_FCN);

  coff_line_base = get_absolute_expression ();

  S_SET_NUMBER_AUXILIARY (sym, 1);
  SA_SET_SYM_LNNO (sym, coff_line_base);

  sym->sy_tc.output = 1;

  ppc_frob_label (sym);

  demand_empty_rest_of_line ();
}

/* The .ef pseudo-op.  This is just like a COFF C_FCN symbol named
   ".ef", except that the line number is absolute, not relative to the
   most recent ".bf" symbol.  */

static void
ppc_ef (ignore)
     int ignore;
{
  symbolS *sym;

  sym = symbol_make (".ef");
  S_SET_SEGMENT (sym, text_section);
  sym->sy_frag = frag_now;
  S_SET_VALUE (sym, frag_now_fix ());
  S_SET_STORAGE_CLASS (sym, C_FCN);
  S_SET_NUMBER_AUXILIARY (sym, 1);
  SA_SET_SYM_LNNO (sym, get_absolute_expression ());
  sym->sy_tc.output = 1;

  ppc_frob_label (sym);

  demand_empty_rest_of_line ();
}

/* The .bi and .ei pseudo-ops.  These take a string argument and
   generates a C_BINCL or C_EINCL symbol, which goes at the start of
   the symbol list.  */

static void
ppc_biei (ei)
     int ei;
{
  char *name;
  int len;
  symbolS *sym;
  symbolS *look;

  name = demand_copy_C_string (&len);

  sym = symbol_make (name);
  S_SET_SEGMENT (sym, ppc_coff_debug_section);
  sym->bsym->flags |= BSF_DEBUGGING;

  /* FIXME: The value of the .bi or .ei symbol is supposed to be the
     offset in the file to the line number entry to use.  That is
     quite difficult to implement using BFD, so I'm just not doing it.
     Sorry.  Please add it if you can figure out how.  Note that this
     approach is the only way to support multiple files in COFF, since
     line numbers are associated with function symbols.  Note further
     that it still doesn't work, since the line numbers are stored as
     offsets from a base line number.  */

  S_SET_STORAGE_CLASS (sym, ei ? C_EINCL : C_BINCL);
  sym->sy_tc.output = 1;
  
  for (look = symbol_rootP;
       (look != (symbolS *) NULL
	&& (S_GET_STORAGE_CLASS (look) == C_FILE
	    || S_GET_STORAGE_CLASS (look) == C_BINCL
	    || S_GET_STORAGE_CLASS (look) == C_EINCL));
       look = symbol_next (look))
    ;
  if (look != (symbolS *) NULL)
    {
      symbol_remove (sym, &symbol_rootP, &symbol_lastP);
      symbol_insert (sym, look, &symbol_rootP, &symbol_lastP);
    }

  demand_empty_rest_of_line ();
}

/* The .bs pseudo-op.  This generates a C_BSTAT symbol named ".bs".
   There is one argument, which is a csect symbol.  The value of the
   .bs symbol is the index of this csect symbol.  */

static void
ppc_bs (ignore)
     int ignore;
{
  char *name;
  char endc;
  symbolS *csect;
  symbolS *sym;

  if (ppc_current_block != NULL)
    as_bad ("nested .bs blocks");

  name = input_line_pointer;
  endc = get_symbol_end ();

  csect = symbol_find_or_make (name);

  *input_line_pointer = endc;

  sym = symbol_make (".bs");
  S_SET_SEGMENT (sym, now_seg);
  S_SET_STORAGE_CLASS (sym, C_BSTAT);
  sym->bsym->flags |= BSF_DEBUGGING;
  sym->sy_tc.output = 1;

  sym->sy_tc.within = csect;

  ppc_frob_label (sym);

  ppc_current_block = sym;

  demand_empty_rest_of_line ();
}

/* The .es pseudo-op.  Generate a C_ESTART symbol named .es.  */

static void
ppc_es (ignore)
     int ignore;
{
  symbolS *sym;

  if (ppc_current_block == NULL)
    as_bad (".es without preceding .bs");

  sym = symbol_make (".es");
  S_SET_SEGMENT (sym, now_seg);
  S_SET_STORAGE_CLASS (sym, C_ESTAT);
  sym->bsym->flags |= BSF_DEBUGGING;
  sym->sy_tc.output = 1;

  ppc_frob_label (sym);

  ppc_current_block = NULL;

  demand_empty_rest_of_line ();
}

/* The .bb pseudo-op.  Generate a C_BLOCK symbol named .bb, with a
   line number.  */

static void
ppc_bb (ignore)
     int ignore;
{
  symbolS *sym;

  sym = symbol_make (".bb");
  S_SET_SEGMENT (sym, text_section);
  sym->sy_frag = frag_now;
  S_SET_VALUE (sym, frag_now_fix ());
  S_SET_STORAGE_CLASS (sym, C_BLOCK);

  S_SET_NUMBER_AUXILIARY (sym, 1);
  SA_SET_SYM_LNNO (sym, get_absolute_expression ());

  sym->sy_tc.output = 1;

  ppc_frob_label (sym);

  demand_empty_rest_of_line ();
}

/* The .eb pseudo-op.  Generate a C_BLOCK symbol named .eb, with a
   line number.  */

static void
ppc_eb (ignore)
     int ignore;
{
  symbolS *sym;

  sym = symbol_make (".eb");
  S_SET_SEGMENT (sym, text_section);
  sym->sy_frag = frag_now;
  S_SET_VALUE (sym, frag_now_fix ());
  S_SET_STORAGE_CLASS (sym, C_FCN);
  S_SET_NUMBER_AUXILIARY (sym, 1);
  SA_SET_SYM_LNNO (sym, get_absolute_expression ());
  sym->sy_tc.output = 1;

  ppc_frob_label (sym);

  demand_empty_rest_of_line ();
}

/* The .toc pseudo-op.  Switch to the .toc subsegment.  */

static void
ppc_toc (ignore)
     int ignore;
{
  if (ppc_toc_csect != (symbolS *) NULL)
    subseg_set (data_section, ppc_toc_csect->sy_tc.subseg);
  else
    {
      subsegT subseg;
      symbolS *sym;
      symbolS *list;
    
      subseg = ppc_data_subsegment;
      ++ppc_data_subsegment;

      subseg_new (segment_name (data_section), subseg);
      ppc_toc_frag = frag_now;

      sym = symbol_find_or_make ("TOC[TC0]");
      sym->sy_frag = frag_now;
      S_SET_SEGMENT (sym, data_section);
      S_SET_VALUE (sym, (valueT) frag_now_fix ());
      sym->sy_tc.subseg = subseg;
      sym->sy_tc.output = 1;
      sym->sy_tc.within = sym;

      ppc_toc_csect = sym;
	  
      for (list = ppc_data_csects;
	   list->sy_tc.next != (symbolS *) NULL;
	   list = list->sy_tc.next)
	;
      list->sy_tc.next = sym;

      symbol_remove (sym, &symbol_rootP, &symbol_lastP);
      symbol_append (sym, list->sy_tc.within, &symbol_rootP, &symbol_lastP);
    }

  ppc_current_csect = ppc_toc_csect;

  demand_empty_rest_of_line ();
}

#endif /* OBJ_COFF */

/* The .tc pseudo-op.  This is used when generating either XCOFF or
   ELF.  This takes two or more arguments.

   When generating XCOFF output, the first argument is the name to
   give to this location in the toc; this will be a symbol with class
   TC.  The rest of the arguments are 4 byte values to actually put at
   this location in the TOC; often there is just one more argument, a
   relocateable symbol reference.

   When not generating XCOFF output, the arguments are the same, but
   the first argument is simply ignored.  */

static void
ppc_tc (ignore)
     int ignore;
{
#ifdef OBJ_COFF

  /* Define the TOC symbol name.  */
  {
    char *name;
    char endc;
    symbolS *sym;

    if (ppc_toc_csect == (symbolS *) NULL
	|| ppc_toc_csect != ppc_current_csect)
      {
	as_bad (".tc not in .toc section");
	ignore_rest_of_line ();
	return;
      }

    name = input_line_pointer;
    endc = get_symbol_end ();

    sym = symbol_find_or_make (name);

    *input_line_pointer = endc;

    if (S_IS_DEFINED (sym))
      {
	symbolS *label;

	label = ppc_current_csect->sy_tc.within;
	if (label->sy_tc.class != XMC_TC0)
	  {
	    as_warn (".tc with no label");
	    ignore_rest_of_line ();
	    return;
	  }

	S_SET_SEGMENT (label, S_GET_SEGMENT (sym));
	label->sy_frag = sym->sy_frag;
	S_SET_VALUE (label, S_GET_VALUE (sym));

	while (! is_end_of_line[(unsigned char) *input_line_pointer])
	  ++input_line_pointer;

	return;
      }

    S_SET_SEGMENT (sym, now_seg);
    sym->sy_frag = frag_now;
    S_SET_VALUE (sym, (valueT) frag_now_fix ());
    sym->sy_tc.class = XMC_TC;
    sym->sy_tc.output = 1;

    ppc_frob_label (sym);
  }

#else /* ! defined (OBJ_COFF) */

  /* Skip the TOC symbol name.  */
  while (is_part_of_name (*input_line_pointer)
	 || *input_line_pointer == '['
	 || *input_line_pointer == ']'
	 || *input_line_pointer == '{'
	 || *input_line_pointer == '}')
    ++input_line_pointer;

  /* Align to a four byte boundary.  */
  frag_align (2, 0);
  record_alignment (now_seg, 2);

#endif /* ! defined (OBJ_COFF) */

  if (*input_line_pointer != ',')
    demand_empty_rest_of_line ();
  else
    {
      ++input_line_pointer;
      cons (4);
    }
}

#ifdef OBJ_COFF

/* XCOFF specific symbol and file handling.  */

/* Canonicalize the symbol name.  We use the to force the suffix, if
   any, to use square brackets, and to be in upper case.  */

char *
ppc_canonicalize_symbol_name (name)
     char *name;
{
  char *s;

  for (s = name; *s != '\0' && *s != '{' && *s != '['; s++)
    ;
  if (*s != '\0')
    {
      char brac;

      if (*s == '[')
	brac = ']';
      else
	{
	  *s = '[';
	  brac = '}';
	}

      for (s++; *s != '\0' && *s != brac; s++)
	if (islower (*s))
	  *s = toupper (*s);

      if (*s == '\0' || s[1] != '\0')
	as_bad ("bad symbol suffix");

      *s = ']';
    }

  return name;
}

/* Set the class of a symbol based on the suffix, if any.  This is
   called whenever a new symbol is created.  */

void
ppc_symbol_new_hook (sym)
     symbolS *sym;
{
  const char *s;

  sym->sy_tc.next = NULL;
  sym->sy_tc.output = 0;
  sym->sy_tc.class = -1;
  sym->sy_tc.real_name = NULL;
  sym->sy_tc.subseg = 0;
  sym->sy_tc.align = 0;
  sym->sy_tc.size = NULL;
  sym->sy_tc.within = NULL;

  s = strchr (S_GET_NAME (sym), '[');
  if (s == (const char *) NULL)
    {
      /* There is no suffix.  */
      return;
    }

  ++s;

  switch (s[0])
    {
    case 'B':
      if (strcmp (s, "BS]") == 0)
	sym->sy_tc.class = XMC_BS;
      break;
    case 'D':
      if (strcmp (s, "DB]") == 0)
	sym->sy_tc.class = XMC_DB;
      else if (strcmp (s, "DS]") == 0)
	sym->sy_tc.class = XMC_DS;
      break;
    case 'G':
      if (strcmp (s, "GL]") == 0)
	sym->sy_tc.class = XMC_GL;
      break;
    case 'P':
      if (strcmp (s, "PR]") == 0)
	sym->sy_tc.class = XMC_PR;
      break;
    case 'R':
      if (strcmp (s, "RO]") == 0)
	sym->sy_tc.class = XMC_RO;
      else if (strcmp (s, "RW]") == 0)
	sym->sy_tc.class = XMC_RW;
      break;
    case 'S':
      if (strcmp (s, "SV]") == 0)
	sym->sy_tc.class = XMC_SV;
      break;
    case 'T':
      if (strcmp (s, "TC]") == 0)
	sym->sy_tc.class = XMC_TC;
      else if (strcmp (s, "TI]") == 0)
	sym->sy_tc.class = XMC_TI;
      else if (strcmp (s, "TB]") == 0)
	sym->sy_tc.class = XMC_TB;
      else if (strcmp (s, "TC0]") == 0 || strcm (s, "T0]") == 0)
	sym->sy_tc.class = XMC_TC0;
      break;
    case 'U':
      if (strcmp (s, "UA]") == 0)
	sym->sy_tc.class = XMC_UA;
      else if (strcmp (s, "UC]") == 0)
	sym->sy_tc.class = XMC_UC;
      break;
    case 'X':
      if (strcmp (s, "XO]") == 0)
	sym->sy_tc.class = XMC_XO;
      break;
    }

  if (sym->sy_tc.class == -1)
    as_bad ("Unrecognized symbol suffix");
}

/* Set the class of a label based on where it is defined.  This
   handles symbols without suffixes.  Also, move the symbol so that it
   follows the csect symbol.  */

void
ppc_frob_label (sym)
     symbolS *sym;
{
  if (ppc_current_csect != (symbolS *) NULL)
    {
      if (sym->sy_tc.class == -1)
	sym->sy_tc.class = ppc_current_csect->sy_tc.class;

      symbol_remove (sym, &symbol_rootP, &symbol_lastP);
      symbol_append (sym, ppc_current_csect->sy_tc.within, &symbol_rootP,
		     &symbol_lastP);
      ppc_current_csect->sy_tc.within = sym;
    }
}

/* Change the name of a symbol just before writing it out.  Set the
   real name if the .rename pseudo-op was used.  Otherwise, remove any
   class suffix.  Return 1 if the symbol should not be included in the
   symbol table.  */

int
ppc_frob_symbol (sym)
     symbolS *sym;
{
  static symbolS *ppc_last_function;
  static symbolS *set_end;

  /* Discard symbols that should not be included in the output symbol
     table.  */
  if (! sym->sy_used_in_reloc
      && ((sym->bsym->flags & BSF_SECTION_SYM) != 0
	  || (! S_IS_EXTERNAL (sym)
	      && ! sym->sy_tc.output
	      && S_GET_STORAGE_CLASS (sym) != C_FILE)))
    return 1;

  if (sym->sy_tc.real_name != (char *) NULL)
    S_SET_NAME (sym, sym->sy_tc.real_name);
  else
    {
      const char *name;
      const char *s;

      name = S_GET_NAME (sym);
      s = strchr (name, '[');
      if (s != (char *) NULL)
	{
	  unsigned int len;
	  char *snew;

	  len = s - name;
	  snew = xmalloc (len + 1);
	  memcpy (snew, name, len);
	  snew[len] = '\0';

	  S_SET_NAME (sym, snew);
	}
    }

  if (set_end != (symbolS *) NULL)
    {
      SA_SET_SYM_ENDNDX (set_end, sym);
      set_end = NULL;
    }

  if (SF_GET_FUNCTION (sym))
    {
      if (ppc_last_function != (symbolS *) NULL)
	as_warn ("two .function pseudo-ops with no intervening .ef");
      ppc_last_function = sym;
      if (sym->sy_tc.size != (symbolS *) NULL)
	{
	  resolve_symbol_value (sym->sy_tc.size);
	  SA_SET_SYM_FSIZE (sym, (long) S_GET_VALUE (sym->sy_tc.size));
	}
    }
  else if (S_GET_STORAGE_CLASS (sym) == C_FCN
	   && strcmp (S_GET_NAME (sym), ".ef") == 0)
    {
      if (ppc_last_function == (symbolS *) NULL)
	as_warn (".ef with no preceding .function");
      else
	{
	  set_end = ppc_last_function;
	  ppc_last_function = NULL;

	  /* We don't have a C_EFCN symbol, but we need to force the
	     COFF backend to believe that it has seen one.  */
	  coff_last_function = NULL;
	}
    }

  if (! S_IS_EXTERNAL (sym)
      && (sym->bsym->flags & BSF_SECTION_SYM) == 0
      && S_GET_STORAGE_CLASS (sym) != C_FILE
      && S_GET_STORAGE_CLASS (sym) != C_FCN
      && S_GET_STORAGE_CLASS (sym) != C_BSTAT
      && S_GET_STORAGE_CLASS (sym) != C_ESTAT
      && S_GET_SEGMENT (sym) != ppc_coff_debug_section)
    S_SET_STORAGE_CLASS (sym, C_HIDEXT);

  if (S_GET_STORAGE_CLASS (sym) == C_EXT
      || S_GET_STORAGE_CLASS (sym) == C_HIDEXT)
    {
      int i;
      union internal_auxent *a;

      /* Create a csect aux.  */
      i = S_GET_NUMBER_AUXILIARY (sym);
      S_SET_NUMBER_AUXILIARY (sym, i + 1);
      a = &coffsymbol (sym->bsym)->native[i + 1].u.auxent;
      if (sym->sy_tc.class == XMC_TC0)
	{
	  /* This is the TOC table.  */
	  know (strcmp (S_GET_NAME (sym), "TOC") == 0);
	  a->x_csect.x_scnlen.l = 0;
	  a->x_csect.x_smtyp = (2 << 3) | XTY_SD;
	}
      else if (sym->sy_tc.subseg != 0)
	{
	  /* This is a csect symbol.  x_scnlen is the size of the
	     csect.  */
	  if (sym->sy_tc.next == (symbolS *) NULL)
	    a->x_csect.x_scnlen.l = (bfd_section_size (stdoutput,
						       S_GET_SEGMENT (sym))
				     - S_GET_VALUE (sym));
	  else
	    {
	      resolve_symbol_value (sym->sy_tc.next);
	      a->x_csect.x_scnlen.l = (S_GET_VALUE (sym->sy_tc.next)
				       - S_GET_VALUE (sym));
	    }
	  a->x_csect.x_smtyp = (sym->sy_tc.align << 3) | XTY_SD;
	}
      else if (S_GET_SEGMENT (sym) == bss_section)
	{
	  /* This is a common symbol.  */
	  a->x_csect.x_scnlen.l = sym->sy_frag->fr_offset;
	  a->x_csect.x_smtyp = (sym->sy_tc.align << 3) | XTY_CM;
	  if (S_IS_EXTERNAL (sym))
	    sym->sy_tc.class = XMC_RW;
	  else
	    sym->sy_tc.class = XMC_BS;
	}
      else if (! S_IS_DEFINED (sym))
	{
	  /* This is an external symbol.  */
	  a->x_csect.x_scnlen.l = 0;
	  a->x_csect.x_smtyp = XTY_ER;
	}
      else if (sym->sy_tc.class == XMC_TC)
	{
	  symbolS *next;

	  /* This is a TOC definition.  x_scnlen is the size of the
	     TOC entry.  */
	  next = symbol_next (sym);
	  while (next->sy_tc.class == XMC_TC0)
	    next = symbol_next (next);
	  if (next == (symbolS *) NULL
	      || next->sy_tc.class != XMC_TC)
	    {
	      if (ppc_after_toc_frag == (fragS *) NULL)
		a->x_csect.x_scnlen.l = (bfd_section_size (stdoutput,
							   data_section)
					 - S_GET_VALUE (sym));
	      else
		a->x_csect.x_scnlen.l = (ppc_after_toc_frag->fr_address
					 - S_GET_VALUE (sym));
	    }
	  else
	    {
	      resolve_symbol_value (next);
	      a->x_csect.x_scnlen.l = (S_GET_VALUE (next)
				       - S_GET_VALUE (sym));
	    }
	  a->x_csect.x_smtyp = (2 << 3) | XTY_SD;
	}
      else
	{
	  symbolS *csect;

	  /* This is a normal symbol definition.  x_scnlen is the
	     symbol index of the containing csect.  */
	  if (S_GET_SEGMENT (sym) == text_section)
	    csect = ppc_text_csects;
	  else if (S_GET_SEGMENT (sym) == data_section)
	    csect = ppc_data_csects;
	  else
	    abort ();

	  /* Skip the initial dummy symbol.  */
	  csect = csect->sy_tc.next;

	  if (csect == (symbolS *) NULL)
	    a->x_csect.x_scnlen.l = 0;
	  else
	    {
	      while (csect->sy_tc.next != (symbolS *) NULL)
		{
		  resolve_symbol_value (csect->sy_tc.next);
		  if (S_GET_VALUE (csect->sy_tc.next) > S_GET_VALUE (sym))
		    break;
		  csect = csect->sy_tc.next;
		}

	      a->x_csect.x_scnlen.p = coffsymbol (csect->bsym)->native;
	      coffsymbol (sym->bsym)->native[i + 1].fix_scnlen = 1;
	    }
	  a->x_csect.x_smtyp = XTY_LD;
	}
	
      a->x_csect.x_parmhash = 0;
      a->x_csect.x_snhash = 0;
      if (sym->sy_tc.class == -1)
	a->x_csect.x_smclas = XMC_PR;
      else
	a->x_csect.x_smclas = sym->sy_tc.class;
      a->x_csect.x_stab = 0;
      a->x_csect.x_snstab = 0;
    }
  else if (S_GET_STORAGE_CLASS (sym) == C_BSTAT)
    {
      /* We want the value to be the symbol index of the referenced
	 csect symbol.  BFD will do that for us if we set the right
	 flags.  */
      S_SET_VALUE (sym,
		   (valueT) coffsymbol (sym->sy_tc.within->bsym)->native);
      coffsymbol (sym->bsym)->native->fix_value = 1;
    }
  else if (S_GET_STORAGE_CLASS (sym) == C_STSYM)
    {
      symbolS *block;
      symbolS *csect;

      /* The value is the offset from the enclosing csect.  */
      block = sym->sy_tc.within;
      csect = block->sy_tc.within;
      resolve_symbol_value (csect);
      S_SET_VALUE (sym, S_GET_VALUE (sym) - S_GET_VALUE (csect));
    }

  return 0;
}

/* Set the VMA for a section.  This is called on all the sections in
   turn.  */

void
ppc_frob_section (sec)
     asection *sec;
{
  static bfd_size_type vma = 0;

  bfd_set_section_vma (stdoutput, sec, vma);
  vma += bfd_section_size (stdoutput, sec);
}

/* Adjust the file by adding a .debug section if needed.  */

void
ppc_frob_file ()
{
  if (ppc_debug_name_section_size > 0)
    {
      asection *sec;

      sec = bfd_make_section (stdoutput, ".debug");
      if (sec == (asection *) NULL
	  || ! bfd_set_section_size (stdoutput, sec,
				     ppc_debug_name_section_size)
	  || ! bfd_set_section_flags (stdoutput, sec,
				      SEC_HAS_CONTENTS | SEC_LOAD))
	as_fatal ("can't make .debug section");
    }
}

#endif /* OBJ_COFF */

/* Turn a string in input_line_pointer into a floating point constant
   of type type, and store the appropriate bytes in *litp.  The number
   of LITTLENUMS emitted is stored in *sizep .  An error message is
   returned, or NULL on OK.  */

char *
md_atof (type, litp, sizep)
     int type;
     char *litp;
     int *sizep;
{
  int prec;
  LITTLENUM_TYPE words[4];
  char *t;
  int i;

  switch (type)
    {
    case 'f':
      prec = 2;
      break;

    case 'd':
      prec = 4;
      break;

    default:
      *sizep = 0;
      return "bad call to md_atof";
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizep = prec * 2;

  if (ppc_big_endian)
    {
      for (i = 0; i < prec; i++)
	{
	  md_number_to_chars (litp, (valueT) words[i], 2);
	  litp += 2;
	}
    }
  else
    {
      for (i = prec - 1; i >= 0; i--)
	{
	  md_number_to_chars (litp, (valueT) words[i], 2);
	  litp += 2;
	}
    }
     
  return NULL;
}

/* Write a value out to the object file, using the appropriate
   endianness.  */

void
md_number_to_chars (buf, val, n)
     char *buf;
     valueT val;
     int n;
{
  if (ppc_big_endian)
    number_to_chars_bigendian (buf, val, n);
  else
    number_to_chars_littleendian (buf, val, n);
}

/* Align a section (I don't know why this is machine dependent).  */

valueT
md_section_align (seg, addr)
     asection *seg;
     valueT addr;
{
  int align = bfd_get_section_alignment (stdoutput, seg);

  return ((addr + (1 << align) - 1) & (-1 << align));
}

/* We don't have any form of relaxing.  */

int
md_estimate_size_before_relax (fragp, seg)
     fragS *fragp;
     asection *seg;
{
  abort ();
}

const relax_typeS md_relax_table[] =
{
  { 0 }
};

/* Convert a machine dependent frag.  We never generate these.  */

void
md_convert_frag (abfd, sec, fragp)
     bfd *abfd;
     asection *sec;
     fragS *fragp;
{
  abort ();
}

/* Parse an operand that is machine-specific.  We just return without
   modifying the expression if we have nothing to do.  */

/*ARGSUSED*/
void
md_operand (expressionP)
     expressionS *expressionP;
{
}

/* We have no need to default values of symbols.  */

/*ARGSUSED*/
symbolS *
md_undefined_symbol (name)
     char *name;
{
  return 0;
}

/* Functions concerning relocs.  */

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */

long
md_pcrel_from (fixp)
     fixS *fixp;
{
#ifdef OBJ_ELF
  if (fixp->fx_addsy != (symbolS *) NULL
      && ! S_IS_DEFINED (fixp->fx_addsy))
    return 0;
#endif

  return fixp->fx_frag->fr_address + fixp->fx_where;
}

#ifdef OBJ_COFF

/* This is called to see whether a fixup should be adjusted to use a
   section symbol.  We take the opportunity to change a fixup against
   a symbol in the TOC subsegment into a reloc against the
   corresponding .tc symbol.  */

int
ppc_fix_adjustable (fix)
     fixS *fix;
{
  valueT val;

  resolve_symbol_value (fix->fx_addsy);
  val = S_GET_VALUE (fix->fx_addsy);
  if (ppc_toc_csect != (symbolS *) NULL
      && fix->fx_addsy != (symbolS *) NULL
      && fix->fx_addsy != ppc_toc_csect
      && S_GET_SEGMENT (fix->fx_addsy) == data_section
      && val >= ppc_toc_frag->fr_address
      && (ppc_after_toc_frag == (fragS *) NULL
	  || val < ppc_after_toc_frag->fr_address))
    {
      symbolS *sy;

      for (sy = symbol_next (ppc_toc_csect);
	   sy != (symbolS *) NULL;
	   sy = symbol_next (sy))
	{
	  if (sy->sy_tc.class == XMC_TC0)
	    continue;
	  if (sy->sy_tc.class != XMC_TC)
	    break;
	  resolve_symbol_value (sy);
	  if (val == S_GET_VALUE (sy))
	    {
	      fix->fx_addsy = sy;
	      fix->fx_addnumber = val - ppc_toc_frag->fr_address;
	      return 0;
	    }
	}

      as_bad_where (fix->fx_file, fix->fx_line,
		    "symbol in .toc does not match any .tc");
    }

  /* Possibly adjust the reloc to be against the csect.  */
  if (fix->fx_addsy != (symbolS *) NULL
      && fix->fx_addsy->sy_tc.subseg == 0
      && fix->fx_addsy->sy_tc.class != XMC_TC0
      && fix->fx_addsy->sy_tc.class != XMC_TC
      && S_GET_SEGMENT (fix->fx_addsy) != bss_section)
    {
      symbolS *csect;

      if (S_GET_SEGMENT (fix->fx_addsy) == text_section)
	csect = ppc_text_csects;
      else if (S_GET_SEGMENT (fix->fx_addsy) == data_section)
	csect = ppc_data_csects;
      else
	abort ();

      /* Skip the initial dummy symbol.  */
      csect = csect->sy_tc.next;

      if (csect != (symbolS *) NULL)
	{
	  while (csect->sy_tc.next != (symbolS *) NULL
		 && (csect->sy_tc.next->sy_frag->fr_address
		     <= fix->fx_addsy->sy_frag->fr_address))
	    csect = csect->sy_tc.next;

	  fix->fx_offset += (S_GET_VALUE (fix->fx_addsy)
			     - csect->sy_frag->fr_address);
	  fix->fx_addsy = csect;
	}
    }

  /* Adjust a reloc against a .lcomm symbol to be against the base
     .lcomm.  */
  if (fix->fx_addsy != (symbolS *) NULL
      && S_GET_SEGMENT (fix->fx_addsy) == bss_section
      && ! S_IS_EXTERNAL (fix->fx_addsy))
    {
      resolve_symbol_value (fix->fx_addsy->sy_frag->fr_symbol);
      fix->fx_offset += (S_GET_VALUE (fix->fx_addsy)
			 - S_GET_VALUE (fix->fx_addsy->sy_frag->fr_symbol));
      fix->fx_addsy = fix->fx_addsy->sy_frag->fr_symbol;
    }

  return 0;
}

#endif

/* See whether a symbol is in the TOC section.  */

static int
ppc_is_toc_sym (sym)
     symbolS *sym;
{
#ifdef OBJ_COFF
  return sym->sy_tc.class == XMC_TC;
#else
  return strcmp (segment_name (S_GET_SEGMENT (sym)), ".got") == 0;
#endif
}

/* Apply a fixup to the object code.  This is called for all the
   fixups we generated by the call to fix_new_exp, above.  In the call
   above we used a reloc code which was the largest legal reloc code
   plus the operand index.  Here we undo that to recover the operand
   index.  At this point all symbol values should be fully resolved,
   and we attempt to completely resolve the reloc.  If we can not do
   that, we determine the correct reloc code and put it back in the
   fixup.  */

int
md_apply_fix (fixp, valuep)
     fixS *fixp;
     valueT *valuep;
{
  valueT value;

  /* FIXME FIXME FIXME: The value we are passed in *valuep includes
     the symbol values.  Since we are using BFD_ASSEMBLER, if we are
     doing this relocation the code in write.c is going to call
     bfd_perform_relocation, which is also going to use the symbol
     value.  That means that if the reloc is fully resolved we want to
     use *valuep since bfd_perform_relocation is not being used.
     However, if the reloc is not fully resolved we do not want to use
     *valuep, and must use fx_offset instead.  However, if the reloc
     is PC relative, we do want to use *valuep since it includes the
     result of md_pcrel_from.  This is confusing.  */

  if (fixp->fx_addsy == (symbolS *) NULL)
    {
      value = *valuep;
      fixp->fx_done = 1;
    }
  else if (fixp->fx_pcrel)
    value = *valuep;
  else
    {
      value = fixp->fx_offset;
      if (fixp->fx_subsy != (symbolS *) NULL)
	{
	  if (S_GET_SEGMENT (fixp->fx_subsy) == absolute_section)
	    value -= S_GET_VALUE (fixp->fx_subsy);
	  else
	    {
	      /* We can't actually support subtracting a symbol.  */
	      as_bad_where (fixp->fx_file, fixp->fx_line,
			    "expression too complex");
	    }
	}
    }

  if ((int) fixp->fx_r_type >= (int) BFD_RELOC_UNUSED)
    {
      int opindex;
      const struct powerpc_operand *operand;
      char *where;
      unsigned long insn;

      opindex = (int) fixp->fx_r_type - (int) BFD_RELOC_UNUSED;

      operand = &powerpc_operands[opindex];

#ifdef OBJ_COFF
      /* It appears that an instruction like
	     l 9,LC..1(30)
	 when LC..1 is not a TOC symbol does not generate a reloc.  It
	 uses the offset of LC..1 within its csect.  However, .long
	 LC..1 will generate a reloc.  I can't find any documentation
	 on how these cases are to be distinguished, so this is a wild
	 guess.  These cases are generated by gcc -mminimal-toc.  */
      if ((operand->flags & PPC_OPERAND_PARENS) != 0
	  && operand->bits == 16
	  && operand->shift == 0
	  && operand->insert == NULL
	  && fixp->fx_addsy != NULL
	  && fixp->fx_addsy->sy_tc.subseg != 0
	  && fixp->fx_addsy->sy_tc.class != XMC_TC
	  && fixp->fx_addsy->sy_tc.class != XMC_TC0
	  && S_GET_SEGMENT (fixp->fx_addsy) != bss_section)
	{
	  value = fixp->fx_offset;
	  fixp->fx_done = 1;
	}
#endif

      /* Fetch the instruction, insert the fully resolved operand
	 value, and stuff the instruction back again.  */
      where = fixp->fx_frag->fr_literal + fixp->fx_where;
      if (ppc_big_endian)
	insn = bfd_getb32 ((unsigned char *) where);
      else
	insn = bfd_getl32 ((unsigned char *) where);
      insn = ppc_insert_operand (insn, operand, (offsetT) value,
				 fixp->fx_file, fixp->fx_line);
      if (ppc_big_endian)
	bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);
      else
	bfd_putl32 ((bfd_vma) insn, (unsigned char *) where);

      if (fixp->fx_done)
	{
	  /* Nothing else to do here.  */
	  return 1;
	}

      /* Determine a BFD reloc value based on the operand information.
	 We are only prepared to turn a few of the operands into
	 relocs.
	 FIXME: We need to handle the DS field at the very least.
	 FIXME: Handling 16 bit branches would also be reasonable.
	 FIXME: Selecting the reloc type is a bit haphazard; perhaps
	 there should be a new field in the operand table.  */
      if ((operand->flags & PPC_OPERAND_RELATIVE) != 0
	  && operand->bits == 26
	  && operand->shift == 0)
	fixp->fx_r_type = BFD_RELOC_PPC_B26;
      else if ((operand->flags & PPC_OPERAND_ABSOLUTE) != 0
	       && operand->bits == 26
	       && operand->shift == 0)
	fixp->fx_r_type = BFD_RELOC_PPC_BA26;
      else if ((operand->flags & PPC_OPERAND_PARENS) != 0
	       && operand->bits == 16
	       && operand->shift == 0
	       && operand->insert == NULL
	       && fixp->fx_addsy != NULL
	       && ppc_is_toc_sym (fixp->fx_addsy))
	{
	  fixp->fx_size = 2;
	  if (ppc_big_endian)
	    fixp->fx_where += 2;
	  fixp->fx_r_type = BFD_RELOC_PPC_TOC16;
	}
      else
	{
	  as_bad_where (fixp->fx_file, fixp->fx_line,
			"unresolved expression that must be resolved");
	  fixp->fx_done = 1;
	  return 1;
	}
    }
  else
    {
      switch (fixp->fx_r_type)
	{
	case BFD_RELOC_32:
	  md_number_to_chars (fixp->fx_frag->fr_literal + fixp->fx_where,
			      value, 4);
	  break;
	case BFD_RELOC_16:
	  md_number_to_chars (fixp->fx_frag->fr_literal + fixp->fx_where,
			      value, 2);
	  break;
	case BFD_RELOC_8:
	  md_number_to_chars (fixp->fx_frag->fr_literal + fixp->fx_where,
			      value, 1);
	  break;
	default:
	  abort ();
	}
    }

#ifdef OBJ_ELF
  fixp->fx_addnumber = value;
#else
  if (fixp->fx_r_type != BFD_RELOC_PPC_TOC16)
    fixp->fx_addnumber = 0;
  else
    {
      /* We want to use the offset within the data segment of the
	 symbol, not the actual VMA of the symbol.  */
      fixp->fx_addnumber =
	- bfd_get_section_vma (stdoutput, S_GET_SEGMENT (fixp->fx_addsy));
    }
#endif

  return 1;
}

/* Generate a reloc for a fixup.  */

arelent *
tc_gen_reloc (seg, fixp)
     asection *seg;
     fixS *fixp;
{
  arelent *reloc;

  reloc = (arelent *) bfd_alloc_by_size_t (stdoutput, sizeof (arelent));

  reloc->sym_ptr_ptr = &fixp->fx_addsy->bsym;
  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->howto = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);
  if (reloc->howto == (reloc_howto_type *) NULL)
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
		    "reloc not supported by object file format");
      return NULL;
    }
  reloc->addend = fixp->fx_addnumber;

  return reloc;
}
