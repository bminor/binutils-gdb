/* LoongArch assembler/disassembler support.

   Copyright (C) 2021-2025 Free Software Foundation, Inc.
   Contributed by Loongson Ltd.

   This file is part of GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the license, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING3.  If not,
   see <http://www.gnu.org/licenses/>.  */

#ifndef _LOONGARCH_H_
#define _LOONGARCH_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  #define LARCH_NOP 0x03400000
  #define LARCH_B 0x50000000
  /* BCEQZ/BCNEZ.  */
  #define LARCH_BRANCH_INVERT_BIT 0x04000000
  #define LARCH_FLOAT_BRANCH_INVERT_BIT 0x00000100

  #define LARCH_MK_ADDI_D 0xffc00000
  #define LARCH_OP_ADDI_D 0x02c00000
  #define LARCH_OP_ADDI_W 0x02800000
  #define LARCH_MK_PCADDI 0xfe000000
  #define LARCH_OP_PCADDI 0x18000000
  #define LARCH_MK_B 0xfc000000
  #define LARCH_OP_B 0x50000000
  #define LARCH_MK_BL 0xfc000000
  #define LARCH_OP_BL 0x54000000
  #define LARCH_MK_ORI 0xffc00000
  #define LARCH_OP_ORI 0x03800000
  #define LARCH_OP_OR 0x00150000
  #define LARCH_MK_LU12I_W 0xfe000000
  #define LARCH_OP_LU12I_W 0x14000000
  #define LARCH_MK_LD_D 0xffc00000
  #define LARCH_OP_LD_D 0x28c00000
  #define LARCH_MK_JIRL 0xfc000000
  #define LARCH_OP_JIRL 0x4c000000
  #define LARCH_MK_BCEQZ 0xfc000300
  #define LARCH_OP_BCEQZ 0x48000000
  #define LARCH_MK_BCNEZ 0xfc000300
  #define LARCH_OP_BCNEZ 0x48000100
  #define LARCH_MK_ATOMIC_MEM 0xffff8000
  #define LARCH_MK_BSTRINS_W 0xffe08000
  #define LARCH_OP_BSTRINS_W 0x00600000
  #define LARCH_MK_BSTRPICK_W 0xffe08000
  #define LARCH_OP_BSTRPICK_W 0x00608000
  #define LARCH_MK_BSTRINS_D 0xffc00000
  #define LARCH_OP_BSTRINS_D 0x00800000
  #define LARCH_MK_BSTRPICK_D 0xffc00000
  #define LARCH_OP_BSTRPICK_D 0x00c00000
  #define LARCH_MK_CSRRD 0xff0003e0
  #define LARCH_OP_CSRRD 0x04000000
  #define LARCH_MK_CSRWR 0xff0003e0
  #define LARCH_OP_CSRWR 0x04000020
  #define LARCH_MK_CSRXCHG 0xff000000
  #define LARCH_OP_CSRXCHG 0x04000000
  #define LARCH_MK_GCSRXCHG 0xff000000
  #define LARCH_OP_GCSRXCHG 0x05000000

  #define LARCH_INSN_OPS(insn, op) ((insn & LARCH_MK_##op) == LARCH_OP_##op)
  #define LARCH_INSN_ADDI_D(insn) LARCH_INSN_OPS((insn), ADDI_D)
  #define LARCH_INSN_PCADDI(insn) LARCH_INSN_OPS((insn), PCADDI)
  #define LARCH_INSN_B(insn) LARCH_INSN_OPS((insn), B)
  #define LARCH_INSN_BL(insn) LARCH_INSN_OPS((insn), BL)
  #define LARCH_INSN_ORI(insn) LARCH_INSN_OPS((insn), ORI)
  #define LARCH_INSN_LU12I_W(insn) LARCH_INSN_OPS((insn), LU12I_W)
  #define LARCH_INSN_LD_D(insn) LARCH_INSN_OPS((insn), LD_D)
  #define LARCH_INSN_JIRL(insn) LARCH_INSN_OPS((insn), JIRL)
  #define LARCH_INSN_BCEQZ(insn) LARCH_INSN_OPS((insn), BCEQZ)
  #define LARCH_INSN_BCNEZ(insn) LARCH_INSN_OPS((insn), BCNEZ)
  #define LARCH_INSN_FLOAT_BRANCH(insn) (LARCH_INSN_BCEQZ(insn) || LARCH_INSN_BCNEZ(insn))
  #define LARCH_INSN_BSTRINS_W(insn) LARCH_INSN_OPS((insn), BSTRINS_W)
  #define LARCH_INSN_BSTRPICK_W(insn) LARCH_INSN_OPS((insn), BSTRPICK_W)
  #define LARCH_INSN_BSTRINS_D(insn) LARCH_INSN_OPS((insn), BSTRINS_D)
  #define LARCH_INSN_BSTRPICK_D(insn) LARCH_INSN_OPS((insn), BSTRPICK_D)
  #define LARCH_INSN_CSRXCHG(insn) LARCH_INSN_OPS((insn), CSRXCHG)
  #define LARCH_INSN_GCSRXCHG(insn) LARCH_INSN_OPS((insn), GCSRXCHG)

  #define LARCH_INSN_ATOMIC_MEM(insn)			\
	((insn & 0xfff80000) == 0x38580000	\
	 || (insn & 0xfff00000) == 0x38600000	\
	 || (insn & 0xffff0000) == 0x38700000	\
	 || (insn & 0xffff0000) == 0x38710000)

  #define ENCODE_BRANCH16_IMM(x) (((x) >> 2) << 10)

  #define OUT_OF_RANGE(value, bits, align)	\
    ((value) < (-(1 << ((bits) - 1) << align)) 	\
      || (value) > ((((1 << ((bits) - 1)) - 1) << align)))

  #define LARCH_RD_A0 0x04
  #define LARCH_RD_RJ_A0 0x084
  #define LARCH_GET_RD(insn) (insn & 0x1f)
  #define LARCH_GET_RJ(insn) ((insn >> 5) & 0x1f)

  typedef uint32_t insn_t;

  struct loongarch_opcode
  {
    const insn_t match;
    const insn_t mask; /* High 1 byte is main opcode and it must be 0xf.  */
#define LARCH_INSN_OPC(insn) ((insn & 0xf0000000) >> 28)
    const char *const name;

    /* ACTUAL PARAMETER:

  // BNF with regular expression.
args : token* end

  // just few char separate 'iden'
token : ','
| '('
| ')'
| iden	     // maybe a label (include at least one alphabet),
		      maybe a number, maybe a expr
| regname

regname : '$' iden

iden : [a-zA-Z0-9\.\+\-]+

end : '\0'


FORMAT: A string to describe the format of actual parameter including
bit field infomation.  For example, "r5:5,r0:5,sr10:16<<2" matches
"$12,$13,12345" and "$4,$7,a_label".  That 'sr' means the instruction
may need relocate. '10:16' means bit field of instruction.
In a 'format', every 'escape's can be replaced to 'iden' or 'regname'
acrroding to its meaning.  We fill all information needed by
disassembing and assembing to 'format'.

  // BNF with regular expression.
format : escape (literal+ escape)* literal* end
| (literal+ escape)* literal* end

end : '\0'       // Get here means parse end.

  // The intersection between any two among FIRST (end), FIRST
  // (literal) and FIRST (escape) must be empty.
  // So we can build a simple parser.
literal : ','
| '('
| ')'

  // Double '<'s means the real number is the immediate after shifting left.
escape : esc_ch bit_field '<' '<' dec2
| esc_ch bit_field
| esc_ch    // for MACRO. non-macro format must indicate 'bit_field'

  // '|' means to concatenate nonadjacent bit fields
  // For example, "10:16|0:4" means
  // "16 bits starting from the 10th bit concatenating with 4 bits
  // starting from the 0th bit".
  // This is to say "[25..10]||[3..0]" (little endian).
b_field : dec2 ':' dec2
| dec2 ':' dec2 '|' bit_field

esc_ch : 's' 'r'   // signed immediate or label need relocate
| 's'       // signed immediate no need relocate
| 'u'       // unsigned immediate
| 'l'       // label needed relocate
| 'r'       // general purpose registers
| 'f'       // FPU registers
| 'v'       // 128 bit SIMD register
| 'x'       // 256 bit SIMD register

dec2 : [1-9][0-9]?
| 0

*/
    const char *const format;

    /* MACRO: Indicate how a macro instruction expand for assembling.
       The main is to replace the '%num'(means the 'num'th 'escape' in
       'format') in 'macro' string to get the real instruction.

       Maybe need
       */
    const char *const macro;
    const int *include;
    const int *exclude;

    const unsigned long pinfo;
#define USELESS 0x0l
/* Instruction is a simple alias only for disassembler use.  */
#define INSN_DIS_ALIAS		0x00000001l
  };

  struct hash_control;

  struct loongarch_ase
  {
    const int *enabled;
    struct loongarch_opcode *const opcodes;
    const int *include;
    const int *exclude;

    /* For disassemble to create main opcode hash table.  */
    const struct loongarch_opcode *opc_htab[16];
    unsigned char opc_htab_inited;

    /* For GAS to create hash table.  */
    struct htab *name_hash_entry;
  };

  extern int is_unsigned (const char *);
  extern int is_signed (const char *);
  extern int is_branch_label (const char *);

  extern int loongarch_get_bit_field_width (const char *bit_field, char **end);
  extern int32_t loongarch_decode_imm (const char *bit_field, insn_t insn,
				       int si);

#define MAX_ARG_NUM_PLUS_2 9

  extern size_t loongarch_split_args_by_comma (char *args,
					       const char *arg_strs[]);
  extern char *loongarch_cat_splited_strs (const char *arg_strs[]);
  extern insn_t loongarch_foreach_args (
    const char *format, const char *arg_strs[],
    int32_t (*helper) (char esc1, char esc2, const char *bit_field,
		       const char *arg, void *context),
    void *context);

  extern int loongarch_check_format (const char *format);
  extern int loongarch_check_macro (const char *format, const char *macro);

  extern char *loongarch_expand_macro_with_format_map (
    const char *format, const char *macro, const char *const arg_strs[],
    const char *(*map) (char esc1, char esc2, const char *arg),
    char *(*helper) (const char *const arg_strs[], void *context),
    void *context, size_t len_str);
  extern char *loongarch_expand_macro (
    const char *macro, const char *const arg_strs[],
    char *(*helper) (const char *const arg_strs[], void *context),
    void *context, size_t len_str);
  extern size_t loongarch_bits_imm_needed (int64_t imm, int si);

  extern void loongarch_eliminate_adjacent_repeat_char (char *dest, char c);

  extern const char *const loongarch_r_normal_name[32];
  extern const char *const loongarch_r_alias[32];
  extern const char *const loongarch_r_alias_1[32];
  extern const char *const loongarch_r_alias_deprecated[32];
  extern const char *const loongarch_f_normal_name[32];
  extern const char *const loongarch_f_alias[32];
  extern const char *const loongarch_f_alias_deprecated[32];
  extern const char *const loongarch_fc_normal_name[4];
  extern const char *const loongarch_fc_numeric_name[4];
  extern const char *const loongarch_c_normal_name[8];
  extern const char *const loongarch_cr_normal_name[4];
  extern const char *const loongarch_v_normal_name[32];
  extern const char *const loongarch_x_normal_name[32];
  extern const char *const loongarch_r_cfi_name[32];
  extern const char *const loongarch_r_cfi_name_alias[32];
  extern const char *const loongarch_f_cfi_name[32];
  extern const char *const loongarch_f_cfi_name_alias[32];

  extern struct loongarch_ase loongarch_ASEs[];

  extern struct loongarch_ASEs_option
  {
    struct opt_abi
    {
	    int elf_abi;
    } abi;
#define ase_abi abi.elf_abi

    struct opt_isa
    {
	    int use_ilp32;
	    int use_lp64;

	    int use_soft_float;
	    int use_single_float;
	    int use_double_float;

	    int use_lsx;
	    int use_lasx;

	    int use_lvz;
	    int use_lbt;

	    int use_la_local_with_abs;
	    int use_la_global_with_pcrel;
	    int use_la_global_with_abs;
    } isa;
#define ase_ilp32	isa.use_ilp32
#define ase_lp64	isa.use_lp64

#define ase_nf		isa.use_soft_float
#define ase_sf		isa.use_single_float
#define ase_df		isa.use_double_float

#define ase_lsx		isa.use_lsx
#define ase_lasx	isa.use_lasx

#define ase_lvz		isa.use_lvz
#define ase_lbt		isa.use_lbt

#define ase_labs	isa.use_la_local_with_abs
#define ase_gpcr	isa.use_la_global_with_pcrel
#define ase_gabs	isa.use_la_global_with_abs

    int relax;
    int thin_add_sub;
    int ignore_start_align;
  } LARCH_opts;

  extern size_t loongarch_insn_length (insn_t insn);

#ifdef __cplusplus
}
#endif

#endif /* _LOONGARCH_H_ */
