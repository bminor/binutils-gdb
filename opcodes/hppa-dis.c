/* Disassembler for the PA-RISC. Somewhat derived from sparc-pinsn.c.
   Copyright 1989, 1990, 1992, 1993 Free Software Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* Define this name if you want to restrict the
   disassembler to host-native formats. */

/* #define LOCAL_ONLY 1 */ 

#include <ansidecl.h>
#include "sysdep.h"
#include "dis-asm.h"
#include "libhppa.h"
#include "opcode/hppa.h"

#ifdef LOCAL_ONLY
/* Needed for HP-specific architecture version numbers. */
#include <unistd.h>
#endif

/* Integer register names, indexed by the numbers which appear in the
   opcodes.  */
static const char *const reg_names[] = 
 {"flags",
          "r1",  "rp",  "r3",  "r4",  "r5",  "r6",  "r7",  "r8", "r9",
  "r10", "r11", "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19",
  "r20", "r21", "r22", "r23", "r24", "r25", "r26", "dp", "ret0", "ret1",
  "sp",  "r31"};

/* Floating point register names, indexed by the numbers which appear in the
   opcodes.  */
static const char *const fp_reg_names[] = 
 {"fpsr",  "fpe2", "fpe4", "fpe6", "fr4",  "fr5",  "fr6",  "fr7",  "fr8", "fr9",
  "fr10", "fr11", "fr12", "fr13", "fr14", "fr15", "fr16", "fr17", "fr18", "fr19",
  "fr20", "fr21", "fr22", "fr23", "fr24", "fr25", "fr26", "fr27", "fr28", "fr29",
  "fr30", "fr31"};

/* (No longer) Format '-': Sign-extension completers */
static const char *const sign_extension_names[] = { ",u", ",s" };

/* Format '/': Deposit completers */
static const char *const deposit_names[] = { ",z", "" };

/* Format '}': Floating conversion types */
static const char *const conversion_names[] =
               { "ff", "xf", "fx", "fxt", "", "uxf", "fxu", "fxut" };

/* Format <none yet>: Kinds of floating point test */
static const char *const float_test_names[] =
               { "", ",acc",  ",rej", "", "", ",acc8", ",rej8", "",
                 "", ",acc6", "",     "", "", ",acc4", "",      "",
                 "", ",acc2" };

typedef unsigned int CORE_ADDR;

/* Get at various relevent fields of an instruction word. */

#define MASK_5 0x1f
#define MASK_11 0x7ff
#define MASK_14 0x3fff
#define MASK_21 0x1fffff

/* These macros get bit fields using HP's numbering (MSB = 0) */

/* Now defined in "libhppa.h"
 #define GET_FIELD(X, FROM, TO) \
  ((X) >> (31 - (TO)) & ((1 << ((TO) - (FROM) + 1)) - 1))
  
 #define GET_BIT( X, WHICH ) \
  GET_FIELD( X, WHICH, WHICH )
*/

/* Some of these have been converted to 2-d arrays because they
   consume less storage this way.  If the maintenance becomes a
   problem, convert them back to const 1-d pointer arrays. */
static const char control_reg[][6] =
{
  "rctr",
  "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7",
  "pidr1", "pidr2", "ccr", "sar", "pidr3", "pidr4",
  "iva", "eiem", "itmr", "pcsq", "pcoq", "iir", "isr",
  "ior", "ipsw", "eirr",
  "tr0", "tr1", "tr2", "tr3", "tr4", "tr5", "tr6", "tr7"
};

static const char compare_cond_names[][5] = {
  "", ",=", ",<", ",<=", ",<<", ",<<=", ",sv",
  ",od", ",tr", ",<>", ",>=", ",>", ",>>=",
  ",>>", ",nsv", ",ev"
};

static const char compare_cond_names_double[][6] = {
  "*", ",*=", ",*<", ",*<=", ",*<<", ",*<<=", ",*sv",
  ",*od", ",*tr", ",*<>", ",*>=", ",*>", ",*>>=",
  ",*>>", ",*nsv", ",*ev"
};

static const char add_cond_names[][5] = {
  "", ",=", ",<", ",<=", ",nuv", ",znv", ",sv",
  ",od", ",tr", ",<>", ",>=", ",>", ",uv",
  ",vnz", ",nsv", ",ev"
};

static const char add_cond_names_double[][6] = {
  "*", ",*=", ",*<", ",*<=", ",*nuv", ",*znv", ",*sv",
  ",*od", ",*tr", ",*<>", ",*>=", ",*>", ",*uv",
  ",*vnz", ",*nsv", ",*ev"
};

static const char *const logical_cond_names[] = {
  "", ",=", ",<", ",<=", 0, 0, 0, ",od",
  ",tr", ",<>", ",>=", ",>", 0, 0, 0, ",ev"
};

static const char *const logical_cond_names_double[] = {
  "*", ",*=", ",*<", ",*<=", 0, 0, 0, ",*od",
  ",*tr", ",*<>", ",*>=", ",*>", 0, 0, 0, ",*ev"
};
static const char *const unit_cond_names[] = {
  "", 0, ",sbz", ",shz", ",sdc", 0, ",sbc", ",shc",
  ",tr", 0, ",nbz", ",nhz", ",ndc", 0, ",nbc", ",nhc"
};
static const char *const unit_cond_names_double[] = {
  "*", 0, ",*sbz", ",*shz", ",*sdc", 0, ",*sbc", ",*shc",
  ",*tr", 0, ",*nbz", ",*nhz", ",*ndc", 0, ",*nbc", ",*nhc"
};
static const char shift_cond_names[][4] = {
  "", ",=", ",<", ",od", ",tr", ",<>", ",>=", ",ev"
};

static const char shift_cond_names_double[][5] = {
  "*", ",*=", ",*<", ",*od", ",*tr", ",*<>", ",*>=", ",*ev"
};

/* Format 'c' */
static const char index_compl_names[][4] = {"", ",m", ",s", ",sm"};

/* Format 'C' */
static const char short_ldst_compl_names[][4] = {"", ",ma", ",o", ",mb"};

/* Format 'Y' */
static const char *const short_bytes_compl_names[] = { "", ",b,m", ",e", ",e,m" };

/* Format '$' */
static const char *const branch_push_pop_names[] = { "", ",pop", ",l", ",l,push" };

/* Format '=' */
static const char *const saturation_names[] = { ",us", ",ss", "", "" };

/* Format '3' */
static const char *const shift_names[] = { "", "", ",u", ",s" };

/* Format 'e' */
static const char *const mix_names[] = { ",l", "", ",r", "" };

static const char *const float_format_names[] = {",sgl", ",dbl", "", ",quad"};

static const char float_comp_names[][8] =
{
  ",false?", ",false", ",?", ",!<=>", ",=", ",=t", ",?=", ",!<>",
  ",!?>=", ",<", ",?<", ",!>=", ",!?>", ",<=", ",?<=", ",!>",
  ",!?<=", ",>", ",?>", ",!<=", ",!?<", ",>=", ",?>=", ",!<",
  ",!?=", ",<>", ",!=", ",!=t", ",!?", ",<=>", ",true?", ",true"
};

/* For a bunch of different instructions form an index into a 
   completer name table. */
#define GET_COMPL(insn) (GET_FIELD (insn, 26, 26) | \
			 GET_FIELD (insn, 18, 18) << 1)

/* Like GET_COMPL, but if the last five bits are 0 and the M bit is
 * set, return "2" for ",o" */
#define GET_COMPL_O(insn) ( (GET_COMPL(insn) == 1) \
                             ? ((GET_FIELD (insn, 27, 31 ) == 0) ? 2 : 1 )\
                             : GET_COMPL(insn))

#define GET_COND(insn) (GET_FIELD ((insn), 16, 18) + \
			(GET_FIELD ((insn), 19, 19) ? 8 : 0))

#define GET_PUSH_POP(insn) ((GET_BIT ((insn), 18) << 1) | GET_BIT((insn), 31))

/* Two-part register extract */
#define MERGED_REG(insn) ((GET_FIELD((insn), 16, 18)) << 2 | GET_FIELD((insn), 21, 22))

/* Utility function to print registers.  Put these first, so gcc's function
   inlining can do its stuff.  */

#define fputs_filtered(STR,F)	(*info->fprintf_func) (info->stream, "%s", STR)

static void
fput_reg (reg, info)
     unsigned reg;
     disassemble_info *info;
{
  (*info->fprintf_func) (info->stream, reg ? reg_names[reg] : "r0");
}

static void
fput_fp_reg (reg, info)
     unsigned reg;
     disassemble_info *info;
{
  (*info->fprintf_func) (info->stream, reg ? fp_reg_names[reg] : "fr0");
}

static void
fput_fp_reg_r (reg, info)
     unsigned reg;
     disassemble_info *info;
{
  /* Special case floating point exception registers.  */
  if (reg < 4)
    (*info->fprintf_func) (info->stream, "fpe%d", reg * 2 + 1);
  else
    (*info->fprintf_func) (info->stream, "%sR", reg ? fp_reg_names[reg] 
						    : "fr0");
}

static void
fput_creg (reg, info)
     unsigned reg;
     disassemble_info *info;
{
  (*info->fprintf_func) (info->stream, control_reg[reg]);
}

/* print constants in hex with sign */

static void
fput_hex_const (num, info)
     unsigned num;
     disassemble_info *info;
{
  /* Mark negative numbers as negative; only mark
     numbers as hex if necessary. */
  if ((int)num < 0)
    {
      if ((int)num > -10)
	(*info->fprintf_func) (info->stream, "-%d", -(int)num );
      else
	(*info->fprintf_func) (info->stream, "-0x%x", -(int)num);
    }
  else if ((int)num < 10)
    (*info->fprintf_func) (info->stream, "%d", num );
  else
    (*info->fprintf_func) (info->stream, "0x%x", num);
}

/* print constants in decimal with sign */

static void
fput_decimal_const (num, info)
     unsigned num;
     disassemble_info *info;
{
  if ((int)num < 0)
    (*info->fprintf_func) (info->stream, "-%d", -(int)num);
  else
    (*info->fprintf_func) (info->stream, "%d", num);
}

/* Routines to extract various sized constants out of hppa
   instructions. */

/* extract a 3-bit space register number from a be, ble,
   mtsp, pitlb or mfsp */
static int
extract_3 (word)
     unsigned word;
{
  return GET_FIELD (word, 18, 18) << 2 | GET_FIELD (word, 16, 17);
}

static int
extract_5_load (word)
     unsigned word;
{
  return low_sign_extend (word >> 16 & MASK_5, 5);
}

/* extract the immediate field from a st{bhw}s instruction */
static int
extract_5_store (word)
     unsigned word;
{
  return low_sign_extend (word & MASK_5, 5);
}

/* extract the immediate field from a break instruction */
static unsigned
extract_5r_store (word)
     unsigned word;
{
  return (word & MASK_5);
}

/* extract the immediate field from a {sr}sm instruction */
static unsigned
extract_5R_store (word)
     unsigned word;
{
  return (word >> 16 & MASK_5);
}

/* extract the immediate field from a bb instruction */
static unsigned
extract_5Q_store (word)
     unsigned word;
{
  return (word >> 21 & MASK_5);
}

/* extract an 11 bit immediate field */
static int
extract_11 (word)
     unsigned word;
{
  return low_sign_extend (word & MASK_11, 11);
}

/* extract a 14 bit immediate field */
static int
extract_14 (word)
     unsigned word;
{
  return low_sign_extend (word & MASK_14, 14);
}

/* extract a 21 bit constant */

static int
extract_21 (word)
     unsigned word;
{
  int val;

  word &= MASK_21;
  word <<= 11;
  val = GET_FIELD (word, 20, 20);
  val <<= 11;
  val |= GET_FIELD (word, 9, 19);
  val <<= 2;
  val |= GET_FIELD (word, 5, 6);
  val <<= 5;
  val |= GET_FIELD (word, 0, 4);
  val <<= 2;
  val |= GET_FIELD (word, 7, 8);
  return sign_extend (val, 21) << 11;
}

/* extract a 12 bit constant from branch instructions */

static int
extract_12 (word)
     unsigned word;
{
  return sign_extend (GET_FIELD (word, 19, 28) |
                      GET_FIELD (word, 29, 29) << 10 |
                      (word & 0x1) << 11, 12) << 2;
}

/* extract a 17 bit constant from branch instructions, returning the
   19 bit signed value. */

static int
extract_17 (word)
     unsigned word;
{
  return sign_extend (GET_FIELD (word, 19, 28) |
                      GET_FIELD (word, 29, 29) << 10 |
                      GET_FIELD (word, 11, 15) << 11 |
                      (word & 0x1) << 16, 17) << 2;
}

/* Print one instruction.  */
int
print_insn_hppa (memaddr, info)
     bfd_vma memaddr;
     disassemble_info *info;
{
  bfd_byte buffer[4];
  unsigned int insn, i;

#ifdef LOCAL_ONLY
  static   int got_version_id = 0;
  static   enum pa_arch pa_version;
#endif

  /* Get the instruction to disassemble.
   */
  {
    int status =
      (*info->read_memory_func) (memaddr, buffer, sizeof (buffer), info);
    if (status != 0)
      {
	(*info->memory_error_func) (status, memaddr, info);
	return -1;
      }
  }
  insn = bfd_getb32 (buffer);

#ifdef LOCAL_ONLY
  /* Get the architecture version of this machine, and assume
     it's the same as our target (this won't work for remote
     or core debugging, nor for looking at PA2.0 binaries from
     a 1.x machine, which is not only legal but part of our
     test system!).  Values are:

       CPU_PA_RISC1_0  0x20B
       CPU_PA_RISC1_1  0x210
       CPU_PA_RISC1_2  0x211
       CPU_PA_RISC2_0  0x214
    
     What we really want is a way to query the bfd for the
     architecture the binary was compiled/assembled for. */
  if(!got_version_id)
    {
      int version_id;
      
      got_version_id = 1;
      version_id = sysconf (_SC_CPU_VERSION);
      switch (version_id)
	{
	case CPU_PA_RISC1_0 :
	case CPU_PA_RISC1_1 :
	case CPU_PA_RISC1_2 :
	  pa_version = pa10;
	  break;

	case CPU_PA_RISC2_0 :
	  pa_version = pa20;
	  break;

	default:
	  /* Now what? */
	  break;
	}
    }
#endif

  /* This linear search through the opcode table is potentially
     a bottleneck.  If it becomes one, we can use the six-bit actual
     opcode as an index into a table of pointers to smaller tables.

     A better organization might use the fact that there are only
     about 40 distinct formats for instructions, rather than looking
     at the hundred-plus kinds of operands. */
  for (i = 0; i < NUMOPCODES; ++i)
    {
      const struct pa_opcode *opcode = &pa_opcodes[i];
      if ((insn & opcode->mask) == opcode->match)
	{
	  register const char *s;
          int added_space = 0;

#ifdef LOCAL_ONLY
          if (opcode->arch == pa20
	      && pa_version == pa10)
              /* Target file has new architecture, host is old.
                 This can't be a correct match, can it?
                 NOTE: the other way is ok. */
	    continue;
#endif
              
	  fputs_filtered (opcode->name, info);

	  for (s = opcode->args; *s != '\0'; ++s)
	    {
  	      if (!added_space
		  && 0 == strchr(completer_chars, *s))
		{
		  /* This is the first non-completer.
		     Print a space here, after all completers,
		     before any regular operands. */
		  fputs_filtered (" ", info);
		  added_space = 1;
		}
 
              /* '*s' describes either an extraction and a format,
                 or is a literal string to dump to the disassembly. */
	      switch (*s)
		{
		case 'x':
		  fput_reg (GET_FIELD (insn, 11, 15), info);
		  break;

		case 'X':
                  if (GET_FIELD (insn, 25, 25))
		    fput_fp_reg_r (GET_FIELD (insn, 11, 15), info);
		  else
		    fput_fp_reg (GET_FIELD (insn, 11, 15), info);
		  break;

		case 'g':
                  if (GET_FIELD (insn, 30, 30))
		    fput_fp_reg_r (GET_FIELD (insn, 11, 15), info);
		  else
		    fput_fp_reg (GET_FIELD (insn, 11, 15), info);
		  break;

		case 'b':
		  fput_reg (GET_FIELD (insn, 6, 10), info);
		  break;

		case '^':
		  fput_creg (GET_FIELD (insn, 6, 10), info);
		  break;

		case 'E':
                  if (GET_FIELD (insn, 25, 25))
		    fput_fp_reg_r (GET_FIELD (insn, 6, 10), info);
		  else
		    fput_fp_reg (GET_FIELD (insn, 6, 10), info);
		  break;

		case 't':
		  fput_reg (GET_FIELD (insn, 27, 31), info);
		  break;

		case 'v':
                  if (GET_FIELD (insn, 25, 25))
		    fput_fp_reg_r (GET_FIELD (insn, 27, 31), info);
		  else
		    fput_fp_reg (GET_FIELD (insn, 27, 31), info);
		  break;

		case 'y':
		  fput_fp_reg (GET_FIELD (insn, 27, 31), info);
		  break;

		case 'B':
		  fput_fp_reg (GET_FIELD (insn, 11, 15), info);
		  break;

		case '4':
		  {
		    int reg = GET_FIELD (insn, 6, 10);

		    reg |= (GET_FIELD (insn, 26, 26) << 4);
		    fput_fp_reg (reg, info);
		    break;
		  }

		case '6':
		  {
		    int reg = GET_FIELD (insn, 11, 15);

		    reg |= (GET_FIELD (insn, 26, 26) << 4);
		    fput_fp_reg (reg, info);
		    break;
		  }

		case '7':
		  {
		    int reg = GET_FIELD (insn, 27, 31);

		    reg |= (GET_FIELD (insn, 26, 26) << 4);
		    fput_fp_reg (reg, info);
		    break;
		  }

		case '8':
		  {
		    int reg = GET_FIELD (insn, 16, 20);

		    reg |= (GET_FIELD (insn, 26, 26) << 4);
		    fput_fp_reg (reg, info);
		    break;
		  }

		case '9':
		  {
		    int reg = GET_FIELD (insn, 21, 25);

		    reg |= (GET_FIELD (insn, 26, 26) << 4);
		    fput_fp_reg (reg, info);
		    break;
		  }

		case '5':
		  fput_hex_const (extract_5_load (insn), info);
		  break;

		case 's':
		  (*info->fprintf_func) (info->stream,
					 "sr%d", GET_FIELD (insn, 16, 17));
		  break;

		case 'S':
                  /* Used when 'assemble_3' is specified.
                   */
		  (*info->fprintf_func) (info->stream, "sr%d",
					 extract_3 (insn));
		  break;

		case 'c':
		  fputs_filtered (index_compl_names[GET_COMPL (insn)], info);
		  break;

		case 'C':
		  fputs_filtered (short_ldst_compl_names[GET_COMPL_O (insn)], info);
		  break;

		case 'm':
		  fputs_filtered (short_ldst_compl_names[
                                         (GET_BIT(insn,29) << 1 | GET_BIT(insn,28)) ],
				  info);
		  break;

		case 'Y':
		  fputs_filtered (short_bytes_compl_names[GET_COMPL (insn)], info);
		  break;

		/* these four conditions are for the set of instructions
		   which distinguish true/false conditions by opcode rather
		   than by the 'f' bit (sigh): comb, comib, addb, addib */
		case '<':
		  fputs_filtered (compare_cond_names[GET_FIELD (insn, 16, 18)],
				  info);
		  break;

		case '?':
		  fputs_filtered (compare_cond_names[GET_FIELD (insn, 16, 18)
				  + GET_FIELD (insn, 4, 4) * 8], info);
		  break;

		case '@':
		  fputs_filtered (add_cond_names[GET_FIELD (insn, 16, 18)
				  + GET_FIELD (insn, 4, 4) * 8], info);
		  break;

		case 'a':
		  fputs_filtered (compare_cond_names[GET_COND (insn)], info);
		  break;

		case 'd':
		  fputs_filtered (add_cond_names[GET_COND (insn)], info);
		  break;
                  
		case '!':
		  fputs_filtered (add_cond_names[GET_FIELD (insn, 16, 18)], info);
		  break;

		case '&':
		  fputs_filtered (logical_cond_names[GET_COND (insn)], info);
		  break;
                  
		case 'U':
		  fputs_filtered (unit_cond_names[GET_COND (insn)], info);
		  break;
                  
		case '|':
		case '>':
		case '~':
		  fputs_filtered (shift_cond_names[GET_FIELD (insn, 16, 18)], info);
		  break;

		case 'V':
		  fput_hex_const (extract_5_store (insn), info);
		  break;

		case 'r':
		  fput_hex_const (extract_5r_store (insn), info);
		  break;

		case 'R':
		  fput_hex_const (extract_5R_store (insn), info);
		  break;

		case 'Q':
		  fput_hex_const (extract_5Q_store (insn), info);
		  break;

		case 'i':
		  fput_hex_const (extract_11 (insn), info);
		  break;

		case 'j':
		  fput_hex_const (extract_14 (insn), info);
		  break;

		case 'k':
		  fput_hex_const (extract_21 (insn), info);
		  break;

		case 'n':
		  if (insn & 0x2)
		    fputs_filtered (",n", info);
		  break;

		case 'N':
		  if ((insn & 0x20) && s[1])
		    fputs_filtered (",n", info);
		  else if (insn & 0x20)
		    fputs_filtered (",n", info);
		  break;

		case 'w':
		  (*info->print_address_func) (memaddr + 8 + extract_12 (insn),
					       info);
		  break;

		case 'W':
		  /* 17 bit PC-relative branch. */
		  (*info->print_address_func) ((memaddr + 8 
						+ extract_17 (insn)),
					       info);
		  break;

		case 'z':
		  /* 17 bit displacement.  This is an offset from a register
		     so it gets disasssembled as just a number, not any sort
		     of address. */
		  fput_hex_const (extract_17 (insn), info);
		  break;

		case 'p':
                  if( pa20 != opcode->arch ) {
		    fput_decimal_const (31 - GET_FIELD (insn, 22, 26), info);
                  }
                  else {
		    fput_decimal_const (63 - CATENATE (GET_BIT (insn, 20), 1,
						       GET_FIELD (insn, 22, 26), 5),
					info);
		  }
                  break;

		case 'P':
		  fput_decimal_const (GET_FIELD (insn, 22, 26), info);
		  break;

		case 'T':
		  fput_decimal_const (32 - GET_FIELD (insn, 27, 31), info);
		  break;

		case 'A':
		  fput_hex_const (GET_FIELD (insn, 6, 18), info);
		  break;

		case 'Z':
		  if (GET_FIELD (insn, 26, 26))
		    fputs_filtered (",m", info);
		  break;

		case 'D':
		  fput_hex_const (GET_FIELD (insn, 6, 31), info);
		  break;

		case 'f':
		  fput_decimal_const (GET_FIELD (insn, 23, 25), info);
		  break;

		case 'O':
		  fput_hex_const ((GET_FIELD (insn, 6,20) << 5 |
			       GET_FIELD (insn, 27, 31)), info);
		  break;

		case 'o':
		  fput_hex_const (GET_FIELD (insn, 6, 20), info);
		  break;

		case '2':
		  fput_hex_const ((GET_FIELD (insn, 6, 22) << 5 |
			       GET_FIELD (insn, 27, 31)), info);
		  break;

		case '1':
		  fput_hex_const ((GET_FIELD (insn, 11, 20) << 5 |
			       GET_FIELD (insn, 27, 31)), info);
		  break;

		case '0':
		  fput_hex_const ((GET_FIELD (insn, 16, 20) << 5 |
			       GET_FIELD (insn, 27, 31)), info);
		  break;

		case 'u':
		  fput_decimal_const (GET_FIELD (insn, 23, 25), info);
		  break;

		case 'F':
		  fputs_filtered (float_format_names[GET_FIELD (insn, 19, 20)],
				    info);
		  break;

		case 'G':
		  fputs_filtered (float_format_names[GET_FIELD (insn, 17, 18)], 
				  info);
		  break;

		case 'H':
		  if (GET_FIELD (insn, 26, 26) == 1)
		    fputs_filtered (float_format_names[0], info);
		  else
		    fputs_filtered (float_format_names[1], info);
		  break;

		case 'I':
		  /* if no destination completer and not before a completer
		     for fcmp, need a space here */
		    fputs_filtered (float_format_names[GET_FIELD (insn, 20, 20)],
				    info);
		  break;

		case 'J':
                  if (GET_FIELD (insn, 24, 24))
		    fput_fp_reg_r (GET_FIELD (insn, 6, 10), info);
		  else
		    fput_fp_reg (GET_FIELD (insn, 6, 10), info);
		  break;

		case 'K':
                  if (GET_FIELD (insn, 19, 19))
		    fput_fp_reg_r (GET_FIELD (insn, 11, 15), info);
		  else
		    fput_fp_reg (GET_FIELD (insn, 11, 15), info);
		  break;

		case 'M':
		  fputs_filtered (float_comp_names[GET_FIELD (insn, 27, 31)], 
				  info);
		  break;

                case 'L': {
                  long temp;

                  temp = GET_FIELD (insn, 18, 27) << 1;
                  temp = assemble_16a (GET_FIELD (insn, 16, 17),
                                       temp,
                                       GET_BIT (insn, 31));
                  fput_hex_const (temp, info);
                  break;
                  }

                case 'l': {
                  long temp;

                  temp = assemble_16a (s,
                                       GET_FIELD (insn, 18, 28),
                                       GET_BIT (insn, 31));
                  fput_hex_const (temp, info);
                  break;
                  }

                case 'q':

/* TEMP HACK - FIXME - edie */
                  fput_hex_const (sign_extend (GET_FIELD (insn, 20, 30), 0), info);
                  break;

                case '#':
		  fput_decimal_const (GET_FIELD (insn, 20, 28), info);
                  break;

                case '$':
		  fputs_filtered (branch_push_pop_names[GET_PUSH_POP(insn)], info);
		  break;

                case '.':
                  fput_creg( 11, info );      /* %cr11, printed by gdb as "sar" */
                  break;

                case '-':
		  /* 22 bit PC-relative branch.  */
                  (*info->print_address_func) (memaddr + 8 +
					       (assemble_22 (GET_FIELD (insn,  6, 10),
							     GET_FIELD (insn, 11, 15),
							     GET_FIELD (insn, 19, 29),
							     GET_FIELD (insn, 31, 31)) << 2), 
					       info);
                  break;

                case '/':
		  fputs_filtered (deposit_names[GET_BIT(insn,21)], info);
                  break;

                case '*':
/* TEMP HACK - FIXME - edie */
                  fput_decimal_const (sign_extend (assemble_6 (GET_BIT (insn, 23), 
							       GET_FIELD (insn, 27, 31)), 0), 
				      info);
                  break;

                case '[':
/* TEMP HACK - FIXME - edie */
                  fput_decimal_const (sign_extend (CATENATE (GET_BIT (insn, 20), 1,
							     GET_FIELD (insn, 22, 26), 5), 
						   0), 
				      info);
                  break;
                  
                case ']':
/* TEMP HACK - FIXME - edie */
                  fput_decimal_const (sign_extend (assemble_6 (GET_BIT (insn, 19), GET_FIELD (insn, 27, 31)), 0), info);
                  break;

                case '=':
		  fputs_filtered (saturation_names[GET_FIELD(insn,24,25)], info);
		  break;

                case ';':
                  /* Always positive */
                  fput_decimal_const (GET_FIELD (insn, 24, 25), info);
                  break;

                case ':':
                  /* Always positive */
                  fput_decimal_const (GET_FIELD (insn, 22, 25), info);
                  break;
                  
                case '3':
		  fputs_filtered (shift_names[GET_FIELD(insn,20,21)], info);
		  break;

                case '%':
		  fputs_filtered (",", info);
                  fput_decimal_const (GET_FIELD (insn, 17, 18), info );
                  fput_decimal_const (GET_FIELD (insn, 20, 21), info );
                  fput_decimal_const (GET_FIELD (insn, 22, 23), info );
                  fput_decimal_const (GET_FIELD (insn, 24, 25), info );
                  break;

                case 'e':
		  fputs_filtered (mix_names[GET_FIELD(insn,17,18)], info);
		  break;

                case '}':
		  fputs_filtered (conversion_names[GET_FIELD(insn,14,16)], info);
		  break;

		case 'h':
		  fput_hex_const (GET_FIELD (insn, 6, 15), info);
		  break;

		case '_':
		  fput_decimal_const ((GET_FIELD (insn, 16, 18) - 1), info);
		  break;

		case '+': {
                  int temp = GET_FIELD (insn, 16, 18) ^ 1;
                  
                  if (temp == 0)
		    /* shouldn't happen, as spec says that if
		       this field is "1", then it's a different
		       format. */
		    fput_decimal_const (7, info);
                  else
		    fput_decimal_const (temp - 1, info);
		  break;
                }

		case '{':
                  /* Funky two-part six-bit register specifier */
                  if (GET_BIT (insn, 23))
		    fput_fp_reg_r (MERGED_REG (insn), info);
		  else
		    fput_fp_reg (MERGED_REG (insn), info);
		  break;

		default:
		  (*info->fprintf_func) (info->stream, "%c", *s);
		  break;
		}
	    }   /* For each operand */
            
	  return sizeof (insn);
	}   /* If matched  */
    }   /* For each opcode */

  (*info->fprintf_func) (info->stream, "#%8x", insn);
  return sizeof(insn);
}

