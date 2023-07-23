/* tc-bpf.c -- Assembler for the Linux eBPF.
   Copyright (C) 2019-2023 Free Software Foundation, Inc.
   Contributed by Oracle, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street - Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "as.h"
#include "subsegs.h"
#include "symcat.h"
#include "opcode/bpf.h"
#include "elf/common.h"
#include "elf/bpf.h"
#include "dwarf2dbg.h"
#include "libiberty.h"
#include <ctype.h>

/* Data structure representing a parsed BPF instruction.  */

struct bpf_insn
{
  enum bpf_insn_id id;
  int size; /* Instruction size in bytes.  */
  bpf_insn_word opcode;
  uint8_t dst;
  uint8_t src;
  expressionS offset16;
  expressionS imm32;
  expressionS imm64;
  expressionS disp16;
  expressionS disp32;

  unsigned int has_dst : 1;
  unsigned int has_src : 1;
  unsigned int has_offset16 : 1;
  unsigned int has_disp16 : 1;
  unsigned int has_disp32 : 1;
  unsigned int has_imm32 : 1;
  unsigned int has_imm64 : 1;
};

const char comment_chars[]        = ";";
const char line_comment_chars[]   = "#";
const char line_separator_chars[] = "`";
const char EXP_CHARS[]            = "eE";
const char FLT_CHARS[]            = "fFdD";

/* Like s_lcomm_internal in gas/read.c but the alignment string
   is allowed to be optional.  */

static symbolS *
pe_lcomm_internal (int needs_align, symbolS *symbolP, addressT size)
{
  addressT align = 0;

  SKIP_WHITESPACE ();

  if (needs_align
      && *input_line_pointer == ',')
    {
      align = parse_align (needs_align - 1);

      if (align == (addressT) -1)
	return NULL;
    }
  else
    {
      if (size >= 8)
	align = 3;
      else if (size >= 4)
	align = 2;
      else if (size >= 2)
	align = 1;
      else
	align = 0;
    }

  bss_alloc (symbolP, size, align);
  return symbolP;
}

static void
pe_lcomm (int needs_align)
{
  s_comm_internal (needs_align * 2, pe_lcomm_internal);
}

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
    { "half",      cons,              2 },
    { "word",      cons,              4 },
    { "dword",     cons,              8 },
    { "lcomm",	   pe_lcomm,	      1 },
    { NULL,        NULL,              0 }
};



/* Command-line options processing.  */

enum options
{
  OPTION_LITTLE_ENDIAN = OPTION_MD_BASE,
  OPTION_BIG_ENDIAN,
  OPTION_XBPF,
  OPTION_DIALECT,
  OPTION_ISA_SPEC,
};

struct option md_longopts[] =
{
  { "EL", no_argument, NULL, OPTION_LITTLE_ENDIAN },
  { "EB", no_argument, NULL, OPTION_BIG_ENDIAN },
  { "mxbpf", no_argument, NULL, OPTION_XBPF },
  { "mdialect", required_argument, NULL, OPTION_DIALECT},
  { "misa-spec", required_argument, NULL, OPTION_ISA_SPEC},
  { NULL,          no_argument, NULL, 0 },
};

size_t md_longopts_size = sizeof (md_longopts);

const char * md_shortopts = "";

/* BPF supports little-endian and big-endian variants.  The following
   global records what endianness to use.  It can be configured using
   command-line options.  It defaults to the host endianness
   initialized in md_begin.  */

static int set_target_endian = 0;
extern int target_big_endian;

/* The ISA specification can be one of BPF_V1, BPF_V2, BPF_V3, BPF_V4
   or BPF_XPBF.  The ISA spec to use can be configured using
   command-line options.  It defaults to the latest BPF spec.  */

static int isa_spec = BPF_V4;

/* The assembler supports two different dialects: "normal" syntax and
   "pseudoc" syntax.  The dialect to use can be configured using
   command-line options.  */

enum target_asm_dialect
{
  DIALECT_NORMAL,
  DIALECT_PSEUDOC
};

static int asm_dialect = DIALECT_NORMAL;

int
md_parse_option (int c, const char * arg)
{
  switch (c)
    {
    case OPTION_BIG_ENDIAN:
      set_target_endian = 1;
      target_big_endian = 1;
      break;
    case OPTION_LITTLE_ENDIAN:
      set_target_endian = 0;
      target_big_endian = 0;
      break;
    case OPTION_DIALECT:
      if (strcmp (arg, "normal") == 0)
        asm_dialect = DIALECT_NORMAL;
      else if (strcmp (arg, "pseudoc") == 0)
        asm_dialect = DIALECT_PSEUDOC;
      else
        as_fatal (_("-mdialect=%s is not valid.  Expected normal or pseudoc"),
                  arg);
      break;
    case OPTION_ISA_SPEC:
      if (strcmp (arg, "v1") == 0)
        isa_spec = BPF_V1;
      else if (strcmp (arg, "v2") == 0)
        isa_spec = BPF_V2;
      else if (strcmp (arg, "v3") == 0)
        isa_spec = BPF_V3;
      else if (strcmp (arg, "v4") == 0)
        isa_spec = BPF_V4;
      else if (strcmp (arg, "xbpf") == 0)
        isa_spec = BPF_XBPF;
      else
        as_fatal (_("-misa-spec=%s is not valid.  Expected v1, v2, v3, v4 o xbpf"),
                  arg);
      break;
    case OPTION_XBPF:
      /* This is an alias for -misa-spec=xbpf.  */
      isa_spec = BPF_XBPF;
      break;
    default:
      return 0;
    }

  return 1;
}

void
md_show_usage (FILE * stream)
{
  fprintf (stream, _("\nBPF options:\n"));
  fprintf (stream, _("\
BPF options:\n\
  -EL                         generate code for a little endian machine\n\
  -EB                         generate code for a big endian machine\n\
  -mdialect=DIALECT           set the assembly dialect (normal, pseudoc)\n\
  -misa-spec                  set the BPF ISA spec (v1, v2, v3, v4, xbpf)\n\
  -mxbpf                      alias for -misa-spec=xbpf\n"));
}


/* This function is called once, at assembler startup time.  This
   should set up all the tables, etc that the MD part of the assembler
   needs.  */

void
md_begin (void)
{
  /* If not specified in the command line, use the host
     endianness.  */
  if (!set_target_endian)
    {
#ifdef WORDS_BIGENDIAN
      target_big_endian = 1;
#else
      target_big_endian = 0;
#endif
    }

  /* Ensure that lines can begin with '*' in BPF store pseudoc instruction.  */
  lex_type['*'] |= LEX_BEGIN_NAME;

  /* Set the machine type. */
  bfd_default_set_arch_mach (stdoutput, bfd_arch_bpf, bfd_mach_bpf);
}

/* Round up a section size to the appropriate boundary.  */

valueT
md_section_align (segT segment, valueT size)
{
  int align = bfd_section_alignment (segment);

  return ((size + (1 << align) - 1) & -(1 << align));
}


/* Functions concerning relocs.  */

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */

long
md_pcrel_from_section (fixS *fixP, segT sec)
{
  if (fixP->fx_addsy != (symbolS *) NULL
      && (! S_IS_DEFINED (fixP->fx_addsy)
          || (S_GET_SEGMENT (fixP->fx_addsy) != sec)
          || S_IS_EXTERNAL (fixP->fx_addsy)
          || S_IS_WEAK (fixP->fx_addsy)))
    {
        /* The symbol is undefined (or is defined but not in this section).
         Let the linker figure it out.  */
      return 0;
    }

  return fixP->fx_where + fixP->fx_frag->fr_address;
}

/* Write a value out to the object file, using the appropriate endianness.  */

void
md_number_to_chars (char * buf, valueT val, int n)
{
  if (target_big_endian)
    number_to_chars_bigendian (buf, val, n);
  else
    number_to_chars_littleendian (buf, val, n);
}

arelent *
tc_gen_reloc (asection *sec ATTRIBUTE_UNUSED, fixS *fixP)
{
  bfd_reloc_code_real_type r_type = fixP->fx_r_type;
  arelent *reloc;

  reloc = XNEW (arelent);

  if (fixP->fx_pcrel)
   {
      r_type = (r_type == BFD_RELOC_8 ? BFD_RELOC_8_PCREL
                : r_type == BFD_RELOC_16 ? BFD_RELOC_16_PCREL
                : r_type == BFD_RELOC_24 ? BFD_RELOC_24_PCREL
                : r_type == BFD_RELOC_32 ? BFD_RELOC_32_PCREL
                : r_type == BFD_RELOC_64 ? BFD_RELOC_64_PCREL
                : r_type);
   }

  reloc->howto = bfd_reloc_type_lookup (stdoutput, r_type);

  if (reloc->howto == (reloc_howto_type *) NULL)
    {
      as_bad_where (fixP->fx_file, fixP->fx_line,
		    _("relocation is not supported"));
      return NULL;
    }

  //XXX  gas_assert (!fixP->fx_pcrel == !reloc->howto->pc_relative);

  reloc->sym_ptr_ptr = XNEW (asymbol *);
  *reloc->sym_ptr_ptr = symbol_get_bfdsym (fixP->fx_addsy);

  /* Use fx_offset for these cases.  */
  if (fixP->fx_r_type == BFD_RELOC_VTABLE_ENTRY
      || fixP->fx_r_type == BFD_RELOC_VTABLE_INHERIT)
    reloc->addend = fixP->fx_offset;
  else
    reloc->addend = fixP->fx_addnumber;

  reloc->address = fixP->fx_frag->fr_address + fixP->fx_where;
  return reloc;
}


/* *FRAGP has been relaxed to its final size, and now needs to have
   the bytes inside it modified to conform to the new size.

   Called after relaxation is finished.
   fragP->fr_type == rs_machine_dependent.
   fragP->fr_subtype is the subtype of what the address relaxed to.  */

void
md_convert_frag (bfd *abfd ATTRIBUTE_UNUSED,
		 segT sec ATTRIBUTE_UNUSED,
		 fragS *fragP ATTRIBUTE_UNUSED)
{
  as_fatal (_("convert_frag called"));
}

int
md_estimate_size_before_relax (fragS *fragP ATTRIBUTE_UNUSED,
                               segT segment ATTRIBUTE_UNUSED)
{
  as_fatal (_("estimate_size_before_relax called"));
  return 0;
}


/* Apply a fixS (fixup of an instruction or data that we didn't have
   enough info to complete immediately) to the data in a frag.  */

void
md_apply_fix (fixS *fixP, valueT *valP, segT seg ATTRIBUTE_UNUSED)
{
  char *where = fixP->fx_frag->fr_literal + fixP->fx_where;

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_BPF_DISP16:
      /* Convert from bytes to number of 64-bit words to the target,
         minus one.  */
      *valP = (((long) (*valP)) - 8) / 8;
      break;
    case BFD_RELOC_BPF_DISPCALL32:
    case BFD_RELOC_BPF_DISP32:
      /* Convert from bytes to number of 64-bit words to the target,
         minus one.  */
      *valP = (((long) (*valP)) - 8) / 8;

      if (fixP->fx_r_type == BFD_RELOC_BPF_DISPCALL32)
        {
          /* eBPF supports two kind of CALL instructions: the so
             called pseudo calls ("bpf to bpf") and external calls
             ("bpf to kernel").

             Both kind of calls use the same instruction (CALL).
             However, external calls are constructed by passing a
             constant argument to the instruction, whereas pseudo
             calls result from expressions involving symbols.  In
             practice, instructions requiring a fixup are interpreted
             as pseudo-calls.  If we are executing this code, this is
             a pseudo call.

             The kernel expects for pseudo-calls to be annotated by
             having BPF_PSEUDO_CALL in the SRC field of the
             instruction.  But beware the infamous nibble-swapping of
             eBPF and take endianness into account here.

             Note that the CALL instruction has only one operand, so
             this code is executed only once per instruction.  */
          md_number_to_chars (where + 1, target_big_endian ? 0x01 : 0x10, 1);
        }
      break;
    case BFD_RELOC_16_PCREL:
      /* Convert from bytes to number of 64-bit words to the target,
         minus one.  */
      *valP = (((long) (*valP)) - 8) / 8;
      break;
    default:
      break;
    }

  if (fixP->fx_addsy == (symbolS *) NULL)
    fixP->fx_done = 1;

  if (fixP->fx_done)
    {
      /* We're finished with this fixup.  Install it because
	 bfd_install_relocation won't be called to do it.  */
      switch (fixP->fx_r_type)
	{
	case BFD_RELOC_8:
	  md_number_to_chars (where, *valP, 1);
	  break;
	case BFD_RELOC_16:
	  md_number_to_chars (where, *valP, 2);
	  break;
	case BFD_RELOC_32:
	  md_number_to_chars (where, *valP, 4);
	  break;
	case BFD_RELOC_64:
	  md_number_to_chars (where, *valP, 8);
	  break;
        case BFD_RELOC_BPF_DISP16:
          md_number_to_chars (where + 2, (uint16_t) *valP, 2);
          break;
        case BFD_RELOC_BPF_DISP32:
        case BFD_RELOC_BPF_DISPCALL32:
          md_number_to_chars (where + 4, (uint32_t) *valP, 4);
          break;
        case BFD_RELOC_16_PCREL:
          md_number_to_chars (where + 2, (uint32_t) *valP, 2);
          break;
	default:
	  as_bad_where (fixP->fx_file, fixP->fx_line,
			_("internal error: can't install fix for reloc type %d (`%s')"),
			fixP->fx_r_type, bfd_get_reloc_code_name (fixP->fx_r_type));
	  break;
	}
    }

  /* Tuck `value' away for use by tc_gen_reloc.
     See the comment describing fx_addnumber in write.h.
     This field is misnamed (or misused :-).  */
  fixP->fx_addnumber = *valP;
}

/* Parse an operand expression.  Returns the first character that is
   not part of the expression, or NULL in case of parse error.

   See md_operand below to see how exp_parse_failed is used.  */

static int exp_parse_failed = 0;

static char *
parse_expression (char *s, expressionS *exp)
{
  char *saved_input_line_pointer = input_line_pointer;
  char *saved_s = s;

  exp_parse_failed = 0;
  input_line_pointer = s;
  expression (exp);
  s = input_line_pointer;
  input_line_pointer = saved_input_line_pointer;

  switch (exp->X_op == O_absent || exp_parse_failed)
    return NULL;

  /* The expression parser may consume trailing whitespaces.  We have
     to undo that since the instruction templates may be expecting
     these whitespaces.  */
  {
    char *p;
    for (p = s - 1; p >= saved_s && *p == ' '; --p)
      --s;
  }

  return s;
}

/* Parse a BPF register name and return the corresponding register
   number.  Return NULL in case of parse error, or a pointer to the
   first character in S that is not part of the register name.  */

static char *
parse_bpf_register (char *s, char rw, uint8_t *regno)
{
  if (asm_dialect == DIALECT_NORMAL)
    {
      rw = 'r';
      if (*s != '%')
	return NULL;
      s += 1;

      if (*s == 'f' && *(s + 1) == 'p')
	{
	  *regno = 10;
	  s += 2;
	  return s;
	}
    }

  if (*s != rw)
    return NULL;
  s += 1;

  if (*s == '1')
    {
      if (*(s + 1) == '0')
        {
          *regno = 10;
          s += 2;
        }
      else
        {
          *regno = 1;
          s += 1;
        }
    }
  else if (*s >= '0' && *s <= '9')
    {
      *regno = *s - '0';
      s += 1;
    }

  return s;
}

/* Collect a parse error message.  */

static int partial_match_length = 0;
static char *errmsg = NULL;

static void
parse_error (int length, const char *fmt, ...)
{
  if (length > partial_match_length)
    {
      va_list args;

      free (errmsg);
      va_start (args, fmt);
      errmsg = xvasprintf (fmt, args);
      va_end (args);
      partial_match_length = length;
    }
}

/* Assemble a machine instruction in STR and emit the frags/bytes it
   assembles to.  */

void
md_assemble (char *str ATTRIBUTE_UNUSED)
{
  /* There are two different syntaxes that can be used to write BPF
     instructions.  One is very conventional and like any other
     assembly language where each instruction is conformed by an
     instruction mnemonic followed by its operands.  This is what we
     call the "normal" syntax.  The other syntax tries to look like C
     statements. We have to support both syntaxes in this assembler.

     One of the many nuisances introduced by this eccentricity is that
     in the pseudo-c syntax it is not possible to hash the opcodes
     table by instruction mnemonic, because there is none.  So we have
     no other choice than to try to parse all instruction opcodes
     until one matches.  This is slow.

     Another problem is that emitting detailed diagnostics becomes
     tricky, since the lack of mnemonic means it is not clear what
     instruction was intended by the user, and we cannot emit
     diagnostics for every attempted template.  So if an instruction
     is not parsed, we report the diagnostic corresponding to the
     partially parsed instruction that was matched further.  */

  unsigned int idx = 0;
  struct bpf_insn insn;
  const struct bpf_opcode *opcode;

  /* Initialize the global diagnostic variables.  See the parse_error
     function above.  */
  partial_match_length = 0;
  errmsg = NULL;

#define PARSE_ERROR(...) parse_error (s - str, __VA_ARGS__)

  while ((opcode = bpf_get_opcode (idx++)) != NULL)
    {
      const char *p;
      char *s;
      const char *template
        = (asm_dialect == DIALECT_PSEUDOC ? opcode->pseudoc : opcode->normal);

      /* Do not try to match opcodes with a higher version than the
         selected ISA spec.  */
      if (opcode->version > isa_spec)
        continue;

      memset (&insn, 0, sizeof (struct bpf_insn));
      insn.size = 8;
      for (s = str, p = template; *p != '\0';)
        {
          if (*p == ' ')
            {
              /* Expect zero or more spaces.  */
              while (*s != '\0' && (*s == ' ' || *s == '\t'))
                s += 1;
              p += 1;
            }
          else if (*p == '%')
            {
              if (*(p + 1) == '%')
                {
                  if (*s != '%')
                    {
                      PARSE_ERROR ("expected '%%'");
                      break;
                    }
                  p += 2;
                  s += 1;
                }
              else if (*(p + 1) == 'w')
                {
                  /* Expect zero or more spaces.  */
                  while (*s != '\0' && (*s == ' ' || *s == '\t'))
                    s += 1;
                  p += 2;
                }
              else if (*(p + 1) == 'W')
                {
                  /* Expect one or more spaces.  */
                  if (*s != ' ' && *s != '\t')
                    {
                      PARSE_ERROR ("expected white space, got '%s'",
                                   s);
                      break;
                    }
                  while (*s != '\0' && (*s == ' ' || *s == '\t'))
                    s += 1;
                  p += 2;
                }
              else if (strncmp (p, "%dr", 3) == 0)
                {
                  uint8_t regno;
                  char *news = parse_bpf_register (s, 'r', &regno);

                  if (news == NULL || (insn.has_dst && regno != insn.dst))
                    {
                      if (news != NULL)
                        PARSE_ERROR ("expected register r%d, got r%d",
                                     insn.dst, regno);
                      else
                        PARSE_ERROR ("expected register name, got '%s'", s);
                      break;
                    }
                  s = news;
                  insn.dst = regno;
                  insn.has_dst = 1;
                  p += 3;
                }
              else if (strncmp (p, "%sr", 3) == 0)
                {
                  uint8_t regno;
                  char *news = parse_bpf_register (s, 'r', &regno);

                  if (news == NULL || (insn.has_src && regno != insn.src))
                    {
                      if (news != NULL)
                        PARSE_ERROR ("expected register r%d, got r%d",
                                     insn.dst, regno);
                      else
                        PARSE_ERROR ("expected register name, got '%s'", s);
                      break;
                    }
                  s = news;
                  insn.src = regno;
                  insn.has_src = 1;
                  p += 3;
                }
              else if (strncmp (p, "%dw", 3) == 0)
                {
                  uint8_t regno;
                  char *news = parse_bpf_register (s, 'w', &regno);

                  if (news == NULL || (insn.has_dst && regno != insn.dst))
                    {
                      if (news != NULL)
                        PARSE_ERROR ("expected register r%d, got r%d",
                                     insn.dst, regno);
                      else
                        PARSE_ERROR ("expected register name, got '%s'", s);
                      break;
                    }
                  s = news;
                  insn.dst = regno;
                  insn.has_dst = 1;
                  p += 3;
                }
              else if (strncmp (p, "%sw", 3) == 0)
                {
                  uint8_t regno;
                  char *news = parse_bpf_register (s, 'w', &regno);

                  if (news == NULL || (insn.has_src && regno != insn.src))
                    {
                      if (news != NULL)
                        PARSE_ERROR ("expected register r%d, got r%d",
                                     insn.dst, regno);
                      else
                        PARSE_ERROR ("expected register name, got '%s'", s);
                      break;
                    }
                  s = news;
                  insn.src = regno;
                  insn.has_src = 1;
                  p += 3;
                }
              else if (strncmp (p, "%i32", 4) == 0
                       || strncmp (p, "%I32", 4) == 0)
                {
                  if (p[1] == 'I')
                    {
                      while (*s == ' ' || *s == '\t')
                        s += 1;
                      if (*s != '+' && *s != '-')
                        {
                          PARSE_ERROR ("expected `+' or `-', got `%c'", *s);
                          break;
                        }
                    }

                  s = parse_expression (s, &insn.imm32);
                  if (s == NULL)
                    {
                      PARSE_ERROR ("expected signed 32-bit immediate");
                      break;
                    }
                  insn.has_imm32 = 1;
                  p += 4;
                }
              else if (strncmp (p, "%o16", 4) == 0)
                {
                  while (*s == ' ' || *s == '\t')
                    s += 1;
                  if (*s != '+' && *s != '-')
                    {
                      PARSE_ERROR ("expected `+' or `-', got `%c'", *s);
                      break;
                    }

                  s = parse_expression (s, &insn.offset16);
                  if (s == NULL)
                    {
                      PARSE_ERROR ("expected signed 16-bit offset");
                      break;
                    }
                  insn.has_offset16 = 1;
                  p += 4;
                }
              else if (strncmp (p, "%d16", 4) == 0)
                {
                  s = parse_expression (s, &insn.disp16);
                  if (s == NULL)
                    {
                      PARSE_ERROR ("expected signed 16-bit displacement");
                      break;
                    }
                  insn.has_disp16 = 1;
                  p += 4;
                }
              else if (strncmp (p, "%d32", 4) == 0)
                {
                  s = parse_expression (s, &insn.disp32);
                  if (s == NULL)
                    {
                      PARSE_ERROR ("expected signed 32-bit displacement");
                      break;
                    }
                  insn.has_disp32 = 1;
                  p += 4;
                }
              else if (strncmp (p, "%i64", 4) == 0)
                {
                  s = parse_expression (s, &insn.imm64);
                  if (s == NULL)
                    {
                      PARSE_ERROR ("expected signed 64-bit immediate");
                      break;
                    }
                  insn.has_imm64 = 1;
                  insn.size = 16;
                  p += 4;
                }
              else
                as_fatal (_("invalid %%-tag in BPF opcode '%s'\n"), template);
            }
          else
            {
              /* Match a literal character.  */
              if (*s != *p)
                {
                  if (*s == '\0')
                    PARSE_ERROR ("expected '%c'", *p);
                  else if (*s == '%')
                    {
                      /* This is to workaround a bug in as_bad. */
                      char tmp[3];

                      tmp[0] = '%';
                      tmp[1] = '%';
                      tmp[2] = '\0';

                      PARSE_ERROR ("expected '%c', got '%s'", *p, tmp);
                    }
                  else
                    PARSE_ERROR ("expected '%c', got '%c'", *p, *s);
                  break;
                }
              p += 1;
              s += 1;
            }
        }

      if (*p == '\0')
        {
          /* Allow white spaces at the end of the line.  */
          while (*s != '\0' && (*s == ' ' || *s == '\t'))
            s += 1;
          if (*s == '\0')
            /* We parsed an instruction successfully.  */
            break;
          PARSE_ERROR ("extra junk at end of line");
        }
    }

  if (opcode == NULL)
    {
      as_bad (_("unrecognized instruction `%s'"), str);
      if (errmsg != NULL)
        {
          as_bad (errmsg);
          free (errmsg);
        }

      return;
    }
  insn.id = opcode->id;
  insn.opcode = opcode->opcode;

#undef PARSE_ERROR

  /* Generate the frags and fixups for the parsed instruction.  */
  {
    char *this_frag = frag_more (insn.size);
    char bytes[16];
    uint8_t src, dst;
    int i;

    /* Zero all the bytes.  */
    memset (bytes, 0, 16);

    /* First encode the opcodes.  Note that we have to handle the
       endianness groups of the BPF instructions: 8 | 4 | 4 | 16 |
       32. */
    if (target_big_endian)
      {
        /* code */
        bytes[0] = (insn.opcode >> 56) & 0xff;
        /* regs */
        bytes[1] = (insn.opcode >> 48) & 0xff;
        /* offset16 */
        bytes[2] = (insn.opcode >> 40) & 0xff;
        bytes[3] = (insn.opcode >> 32) & 0xff;
        /* imm32 */
        bytes[4] = (insn.opcode >> 24) & 0xff;
        bytes[5] = (insn.opcode >> 16) & 0xff;
        bytes[6] = (insn.opcode >> 8) & 0xff;
        bytes[7] = insn.opcode & 0xff;
      }
    else
      {
        /* code */
        bytes[0] = (insn.opcode >> 56) & 0xff;
        /* regs */
        bytes[1] = (((((insn.opcode >> 48) & 0xff) & 0xf) << 4)
                    | (((insn.opcode >> 48) & 0xff) & 0xf));
        /* offset16 */
        bytes[3] = (insn.opcode >> 40) & 0xff;
        bytes[2] = (insn.opcode >> 32) & 0xff;
        /* imm32 */
        bytes[7] = (insn.opcode >> 24) & 0xff;
        bytes[6] = (insn.opcode >> 16) & 0xff;
        bytes[5] = (insn.opcode >> 8) & 0xff;
        bytes[4] = insn.opcode & 0xff;
      }

    /* Now the registers.  */
    src = insn.has_src ? insn.src : 0;
    dst = insn.has_dst ? insn.dst : 0;

    if (target_big_endian)
      bytes[1] = ((dst & 0xf) << 4) | (src & 0xf);
    else
      bytes[1] = ((src & 0xf) << 4) | (dst & 0xf);

    /* Now the immediates.  */
    if (insn.has_imm64)
      {
        switch (insn.imm64.X_op)
          {
          case O_constant:
            {
              uint64_t imm64 = insn.imm64.X_add_number;

              if (target_big_endian)
                {
                  bytes[12] = (imm64 >> 56) & 0xff;
                  bytes[13] = (imm64 >> 48) & 0xff;
                  bytes[14] = (imm64 >> 40) & 0xff;
                  bytes[15] = (imm64 >> 32) & 0xff;
                  bytes[4] = (imm64 >> 24) & 0xff;
                  bytes[5] = (imm64 >> 16) & 0xff;
                  bytes[6] = (imm64 >> 8) & 0xff;
                  bytes[7] = imm64 & 0xff;
                }
              else
                {
                  bytes[15] = (imm64 >> 56) & 0xff;
                  bytes[14] = (imm64 >> 48) & 0xff;
                  bytes[13] = (imm64 >> 40) & 0xff;
                  bytes[12] = (imm64 >> 32) & 0xff;
                  bytes[7] = (imm64 >> 24) & 0xff;
                  bytes[6] = (imm64 >> 16) & 0xff;
                  bytes[5] = (imm64 >> 8) & 0xff;
                  bytes[4] = imm64 & 0xff;
                }
              break;
            }
          case O_symbol:
          case O_subtract:
          case O_add:
            {
              reloc_howto_type *reloc_howto;
              int size;

              reloc_howto = bfd_reloc_type_lookup (stdoutput, BFD_RELOC_BPF_64);
              if (!reloc_howto)
                abort ();

              size = bfd_get_reloc_size (reloc_howto);

              fix_new_exp (frag_now, this_frag - frag_now->fr_literal,
                           size, &insn.imm64, reloc_howto->pc_relative,
                           BFD_RELOC_BPF_64);
              break;
            }
          default:
            abort ();
          }
      }

    if (insn.has_imm32)
      {
        switch (insn.imm32.X_op)
          {
          case O_constant:
            {
              uint32_t imm32 = insn.imm32.X_add_number;

              if (target_big_endian)
                {
                  bytes[4] = (imm32 >> 24) & 0xff;
                  bytes[5] = (imm32 >> 16) & 0xff;
                  bytes[6] = (imm32 >> 8) & 0xff;
                  bytes[7] = imm32 & 0xff;
                }
              else
                {
                  bytes[7] = (imm32 >> 24) & 0xff;
                  bytes[6] = (imm32 >> 16) & 0xff;
                  bytes[5] = (imm32 >> 8) & 0xff;
                  bytes[4] = imm32 & 0xff;
                }
              break;
            }
          case O_symbol:
          case O_subtract:
          case O_add:
          case O_uminus:
            {
              reloc_howto_type *reloc_howto;
              int size;

              reloc_howto = bfd_reloc_type_lookup (stdoutput, BFD_RELOC_32);
              if (!reloc_howto)
                abort ();

              size = bfd_get_reloc_size (reloc_howto);

              fix_new_exp (frag_now, this_frag - frag_now->fr_literal + 4,
                           size, &insn.imm32, reloc_howto->pc_relative,
                           BFD_RELOC_32);
              break;
            }
          default:
            abort ();
          }
      }

    if (insn.has_disp32)
      {
        switch (insn.disp32.X_op)
          {
          case O_constant:
            {
              uint32_t disp32 = insn.disp32.X_add_number;

              if (target_big_endian)
                {
                  bytes[4] = (disp32 >> 24) & 0xff;
                  bytes[5] = (disp32 >> 16) & 0xff;
                  bytes[6] = (disp32 >> 8) & 0xff;
                  bytes[7] = disp32 & 0xff;
                }
              else
                {
                  bytes[7] = (disp32 >> 24) & 0xff;
                  bytes[6] = (disp32 >> 16) & 0xff;
                  bytes[5] = (disp32 >> 8) & 0xff;
                  bytes[4] = disp32 & 0xff;
                }
              break;
            }
          case O_symbol:
          case O_subtract:
          case O_add:
            {
              reloc_howto_type *reloc_howto;
              int size;
              unsigned int bfd_reloc
                = (insn.id == BPF_INSN_CALL
                   ? BFD_RELOC_BPF_DISPCALL32
                   : BFD_RELOC_BPF_DISP32);

              reloc_howto = bfd_reloc_type_lookup (stdoutput, bfd_reloc);
              if (!reloc_howto)
                abort ();

              size = bfd_get_reloc_size (reloc_howto);

              fix_new_exp (frag_now, this_frag - frag_now->fr_literal,
                           size, &insn.disp32, reloc_howto->pc_relative,
                           bfd_reloc);
              break;
            }
          default:
            abort ();
          }
      }

    if (insn.has_offset16)
      {
        switch (insn.offset16.X_op)
          {
          case O_constant:
            {
              uint32_t offset16 = insn.offset16.X_add_number;

              if (target_big_endian)
                {
                  bytes[2] = (offset16 >> 8) & 0xff;
                  bytes[3] = offset16 & 0xff;
                }
              else
                {
                  bytes[3] = (offset16 >> 8) & 0xff;
                  bytes[2] = offset16 & 0xff;
                }
              break;
            }
          case O_symbol:
          case O_subtract:
          case O_add:
            {
              reloc_howto_type *reloc_howto;
              int size;

              reloc_howto = bfd_reloc_type_lookup (stdoutput, BFD_RELOC_BPF_DISP16);
              if (!reloc_howto)
                abort ();

              size = bfd_get_reloc_size (reloc_howto);

              fix_new_exp (frag_now, this_frag - frag_now->fr_literal,
                           size, &insn.offset16, reloc_howto->pc_relative,
                           BFD_RELOC_BPF_DISP16);
              break;
            }
          default:
            abort ();
          }
      }

    if (insn.has_disp16)
      {
        switch (insn.disp16.X_op)
          {
          case O_constant:
            {
              uint32_t disp16 = insn.disp16.X_add_number;

              if (target_big_endian)
                {
                  bytes[2] = (disp16 >> 8) & 0xff;
                  bytes[3] = disp16 & 0xff;
                }
              else
                {
                  bytes[3] = (disp16 >> 8) & 0xff;
                  bytes[2] = disp16 & 0xff;
                }
              break;
            }
          case O_symbol:
          case O_subtract:
          case O_add:
            {
              reloc_howto_type *reloc_howto;
              int size;

              reloc_howto = bfd_reloc_type_lookup (stdoutput, BFD_RELOC_BPF_DISP16);
              if (!reloc_howto)
                abort ();

              size = bfd_get_reloc_size (reloc_howto);

              fix_new_exp (frag_now, this_frag - frag_now->fr_literal,
                           size, &insn.disp16, reloc_howto->pc_relative,
                           BFD_RELOC_BPF_DISP16);
              break;
            }
          default:
            abort ();
          }
      }

    /* Emit bytes.  */
    for (i = 0; i < insn.size; ++i)
      {
        md_number_to_chars (this_frag, (valueT) bytes[i], 1);
        this_frag += 1;
      }
  }

  /* Emit DWARF2 debugging information.  */
  dwarf2_emit_insn (insn.size);
}

/* Parse an operand that is machine-specific.  */

void
md_operand (expressionS *expressionP)
{
  /* If this hook is invoked it means GAS failed to parse a generic
  expression.  We should inhibit the as_bad in expr.c, so we can fail
  while parsing instruction alternatives.  To do that, we change the
  expression to not have an O_absent.  But then we also need to set
  exp_parse_failed to parse_expression above does the right thing.  */
  ++input_line_pointer;
  expressionP->X_op = O_constant;
  expressionP->X_add_number = 0;
  exp_parse_failed = 1;
}

symbolS *
md_undefined_symbol (char *name ATTRIBUTE_UNUSED)
{
  return NULL;
}


/* Turn a string in input_line_pointer into a floating point constant
   of type TYPE, and store the appropriate bytes in *LITP.  The number
   of LITTLENUMS emitted is stored in *SIZEP.  An error message is
   returned, or NULL on OK.  */

const char *
md_atof (int type, char *litP, int *sizeP)
{
  return ieee_md_atof (type, litP, sizeP, false);
}


/* Determine whether the equal sign in the given string corresponds to
   a BPF instruction, i.e. when it is not to be considered a symbol
   assignment.  */

bool
bpf_tc_equal_in_insn (int c ATTRIBUTE_UNUSED, char *str ATTRIBUTE_UNUSED)
{
  uint8_t regno;

  /* Only pseudo-c instructions can have equal signs, and of these,
     all that could be confused with a symbol assignment all start
     with a register name.  */
  if (asm_dialect == DIALECT_PSEUDOC)
    {
      char *w = parse_bpf_register (str, 'w', &regno);
      char *r = parse_bpf_register (str, 'r', &regno);

      if ((w != NULL && *w == '\0')
          || (r != NULL && *r == '\0'))
        return 1;
    }

  return 0;
}
