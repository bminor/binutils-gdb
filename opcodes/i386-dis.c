/* Print i386 instructions for GDB, the GNU debugger.
   Copyright 1988, 1989, 1991, 1993, 1994, 1995, 1996, 1997, 1998, 1999,
   2001, 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.

   This file is part of GDB.

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
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

/* 80386 instruction printer by Pace Willisson (pace@prep.ai.mit.edu)
   July 1988
    modified by John Hassey (hassey@dg-rtp.dg.com)
    x86-64 support added by Jan Hubicka (jh@suse.cz)
    VIA PadLock support by Michal Ludvig (mludvig@suse.cz).  */

/* The main tables describing the instructions is essentially a copy
   of the "Opcode Map" chapter (Appendix A) of the Intel 80386
   Programmers Manual.  Usually, there is a capital letter, followed
   by a small letter.  The capital letter tell the addressing mode,
   and the small letter tells about the operand size.  Refer to
   the Intel manual for details.  */

#include "dis-asm.h"
#include "sysdep.h"
#include "opintl.h"

#define MAXLEN 15

#include <setjmp.h>

#ifndef UNIXWARE_COMPAT
/* Set non-zero for broken, compatible instructions.  Set to zero for
   non-broken opcodes.  */
#define UNIXWARE_COMPAT 1
#endif

static int fetch_data (struct disassemble_info *, bfd_byte *);
static void ckprefix (void);
static const char *prefix_name (int, int);
static int print_insn (bfd_vma, disassemble_info *);
static void dofloat (int);
static void OP_ST (int, int);
static void OP_STi (int, int);
static int putop (const char *, int);
static void oappend (const char *);
static void append_seg (void);
static void OP_indirE (int, int);
static void print_operand_value (char *, int, bfd_vma);
static void OP_E (int, int);
static void OP_G (int, int);
static bfd_vma get64 (void);
static bfd_signed_vma get32 (void);
static bfd_signed_vma get32s (void);
static int get16 (void);
static void set_op (bfd_vma, int);
static void OP_REG (int, int);
static void OP_IMREG (int, int);
static void OP_I (int, int);
static void OP_I64 (int, int);
static void OP_sI (int, int);
static void OP_J (int, int);
static void OP_SEG (int, int);
static void OP_DIR (int, int);
static void OP_OFF (int, int);
static void OP_OFF64 (int, int);
static void ptr_reg (int, int);
static void OP_ESreg (int, int);
static void OP_DSreg (int, int);
static void OP_C (int, int);
static void OP_D (int, int);
static void OP_T (int, int);
static void OP_Rd (int, int);
static void OP_MMX (int, int);
static void OP_XMM (int, int);
static void OP_EM (int, int);
static void OP_EX (int, int);
static void OP_EMC (int,int);
static void OP_MXC (int,int);
static void OP_MS (int, int);
static void OP_XS (int, int);
static void OP_M (int, int);
static void OP_VMX (int, int);
static void OP_0fae (int, int);
static void OP_0f07 (int, int);
static void NOP_Fixup1 (int, int);
static void NOP_Fixup2 (int, int);
static void OP_3DNowSuffix (int, int);
static void OP_SIMD_Suffix (int, int);
static void SIMD_Fixup (int, int);
static void PNI_Fixup (int, int);
static void SVME_Fixup (int, int);
static void INVLPG_Fixup (int, int);
static void BadOp (void);
static void VMX_Fixup (int, int);
static void REP_Fixup (int, int);
static void CMPXCHG8B_Fixup (int, int);

struct dis_private {
  /* Points to first byte not fetched.  */
  bfd_byte *max_fetched;
  bfd_byte the_buffer[MAXLEN];
  bfd_vma insn_start;
  int orig_sizeflag;
  jmp_buf bailout;
};

/* The opcode for the fwait instruction, which we treat as a prefix
   when we can.  */
#define FWAIT_OPCODE (0x9b)

enum address_mode
{
  mode_16bit,
  mode_32bit,
  mode_64bit
};

enum address_mode address_mode;

/* Flags for the prefixes for the current instruction.  See below.  */
static int prefixes;

/* REX prefix the current instruction.  See below.  */
static int rex;
/* Bits of REX we've already used.  */
static int rex_used;
#define REX_MODE64	8
#define REX_EXTX	4
#define REX_EXTY	2
#define REX_EXTZ	1
/* Mark parts used in the REX prefix.  When we are testing for
   empty prefix (for 8bit register REX extension), just mask it
   out.  Otherwise test for REX bit is excuse for existence of REX
   only in case value is nonzero.  */
#define USED_REX(value)					\
  {							\
    if (value)						\
      rex_used |= (rex & value) ? (value) | 0x40 : 0;	\
    else						\
      rex_used |= 0x40;					\
  }

/* Flags for prefixes which we somehow handled when printing the
   current instruction.  */
static int used_prefixes;

/* Flags stored in PREFIXES.  */
#define PREFIX_REPZ 1
#define PREFIX_REPNZ 2
#define PREFIX_LOCK 4
#define PREFIX_CS 8
#define PREFIX_SS 0x10
#define PREFIX_DS 0x20
#define PREFIX_ES 0x40
#define PREFIX_FS 0x80
#define PREFIX_GS 0x100
#define PREFIX_DATA 0x200
#define PREFIX_ADDR 0x400
#define PREFIX_FWAIT 0x800

/* Make sure that bytes from INFO->PRIVATE_DATA->BUFFER (inclusive)
   to ADDR (exclusive) are valid.  Returns 1 for success, longjmps
   on error.  */
#define FETCH_DATA(info, addr) \
  ((addr) <= ((struct dis_private *) (info->private_data))->max_fetched \
   ? 1 : fetch_data ((info), (addr)))

static int
fetch_data (struct disassemble_info *info, bfd_byte *addr)
{
  int status;
  struct dis_private *priv = (struct dis_private *) info->private_data;
  bfd_vma start = priv->insn_start + (priv->max_fetched - priv->the_buffer);

  if (addr <= priv->the_buffer + MAXLEN)
    status = (*info->read_memory_func) (start,
					priv->max_fetched,
					addr - priv->max_fetched,
					info);
  else
    status = -1;
  if (status != 0)
    {
      /* If we did manage to read at least one byte, then
	 print_insn_i386 will do something sensible.  Otherwise, print
	 an error.  We do that here because this is where we know
	 STATUS.  */
      if (priv->max_fetched == priv->the_buffer)
	(*info->memory_error_func) (status, start, info);
      longjmp (priv->bailout, 1);
    }
  else
    priv->max_fetched = addr;
  return 1;
}

#define XX NULL, 0

#define Eb OP_E, b_mode
#define Ev OP_E, v_mode
#define Ed OP_E, d_mode
#define Edq OP_E, dq_mode
#define Edqw OP_E, dqw_mode
#define indirEv OP_indirE, stack_v_mode
#define indirEp OP_indirE, f_mode
#define stackEv OP_E, stack_v_mode
#define Em OP_E, m_mode
#define Ew OP_E, w_mode
#define M OP_M, 0		/* lea, lgdt, etc. */
#define Ma OP_M, v_mode
#define Mp OP_M, f_mode		/* 32 or 48 bit memory operand for LDS, LES etc */
#define Mq OP_M, q_mode
#define Gb OP_G, b_mode
#define Gv OP_G, v_mode
#define Gd OP_G, d_mode
#define Gdq OP_G, dq_mode
#define Gm OP_G, m_mode
#define Gw OP_G, w_mode
#define Rd OP_Rd, d_mode
#define Rm OP_Rd, m_mode
#define Ib OP_I, b_mode
#define sIb OP_sI, b_mode	/* sign extened byte */
#define Iv OP_I, v_mode
#define Iq OP_I, q_mode
#define Iv64 OP_I64, v_mode
#define Iw OP_I, w_mode
#define I1 OP_I, const_1_mode
#define Jb OP_J, b_mode
#define Jv OP_J, v_mode
#define Cm OP_C, m_mode
#define Dm OP_D, m_mode
#define Td OP_T, d_mode

#define RMeAX OP_REG, eAX_reg
#define RMeBX OP_REG, eBX_reg
#define RMeCX OP_REG, eCX_reg
#define RMeDX OP_REG, eDX_reg
#define RMeSP OP_REG, eSP_reg
#define RMeBP OP_REG, eBP_reg
#define RMeSI OP_REG, eSI_reg
#define RMeDI OP_REG, eDI_reg
#define RMrAX OP_REG, rAX_reg
#define RMrBX OP_REG, rBX_reg
#define RMrCX OP_REG, rCX_reg
#define RMrDX OP_REG, rDX_reg
#define RMrSP OP_REG, rSP_reg
#define RMrBP OP_REG, rBP_reg
#define RMrSI OP_REG, rSI_reg
#define RMrDI OP_REG, rDI_reg
#define RMAL OP_REG, al_reg
#define RMAL OP_REG, al_reg
#define RMCL OP_REG, cl_reg
#define RMDL OP_REG, dl_reg
#define RMBL OP_REG, bl_reg
#define RMAH OP_REG, ah_reg
#define RMCH OP_REG, ch_reg
#define RMDH OP_REG, dh_reg
#define RMBH OP_REG, bh_reg
#define RMAX OP_REG, ax_reg
#define RMDX OP_REG, dx_reg

#define eAX OP_IMREG, eAX_reg
#define eBX OP_IMREG, eBX_reg
#define eCX OP_IMREG, eCX_reg
#define eDX OP_IMREG, eDX_reg
#define eSP OP_IMREG, eSP_reg
#define eBP OP_IMREG, eBP_reg
#define eSI OP_IMREG, eSI_reg
#define eDI OP_IMREG, eDI_reg
#define AL OP_IMREG, al_reg
#define CL OP_IMREG, cl_reg
#define DL OP_IMREG, dl_reg
#define BL OP_IMREG, bl_reg
#define AH OP_IMREG, ah_reg
#define CH OP_IMREG, ch_reg
#define DH OP_IMREG, dh_reg
#define BH OP_IMREG, bh_reg
#define AX OP_IMREG, ax_reg
#define DX OP_IMREG, dx_reg
#define zAX OP_IMREG, z_mode_ax_reg
#define indirDX OP_IMREG, indir_dx_reg

#define Sw OP_SEG, w_mode
#define Sv OP_SEG, v_mode
#define Ap OP_DIR, 0
#define Ob OP_OFF64, b_mode
#define Ov OP_OFF64, v_mode
#define Xb OP_DSreg, eSI_reg
#define Xv OP_DSreg, eSI_reg
#define Xz OP_DSreg, eSI_reg
#define Yb OP_ESreg, eDI_reg
#define Yv OP_ESreg, eDI_reg
#define DSBX OP_DSreg, eBX_reg

#define es OP_REG, es_reg
#define ss OP_REG, ss_reg
#define cs OP_REG, cs_reg
#define ds OP_REG, ds_reg
#define fs OP_REG, fs_reg
#define gs OP_REG, gs_reg

#define MX OP_MMX, 0
#define XM OP_XMM, 0
#define EM OP_EM, v_mode
#define EX OP_EX, v_mode
#define MS OP_MS, v_mode
#define XS OP_XS, v_mode
#define EMC OP_EMC, v_mode
#define MXC OP_MXC, 0
#define VM OP_VMX, q_mode
#define OPSUF OP_3DNowSuffix, 0
#define OPSIMD OP_SIMD_Suffix, 0

/* Used handle "rep" prefix for string instructions.  */
#define Xbr REP_Fixup, eSI_reg
#define Xvr REP_Fixup, eSI_reg
#define Ybr REP_Fixup, eDI_reg
#define Yvr REP_Fixup, eDI_reg
#define Yzr REP_Fixup, eDI_reg
#define indirDXr REP_Fixup, indir_dx_reg
#define ALr REP_Fixup, al_reg
#define eAXr REP_Fixup, eAX_reg

#define cond_jump_flag NULL, cond_jump_mode
#define loop_jcxz_flag NULL, loop_jcxz_mode

/* bits in sizeflag */
#define SUFFIX_ALWAYS 4
#define AFLAG 2
#define DFLAG 1

#define b_mode 1  /* byte operand */
#define v_mode 2  /* operand size depends on prefixes */
#define w_mode 3  /* word operand */
#define d_mode 4  /* double word operand  */
#define q_mode 5  /* quad word operand */
#define t_mode 6  /* ten-byte operand */
#define x_mode 7  /* 16-byte XMM operand */
#define m_mode 8  /* d_mode in 32bit, q_mode in 64bit mode.  */
#define cond_jump_mode 9
#define loop_jcxz_mode 10
#define dq_mode 11 /* operand size depends on REX prefixes.  */
#define dqw_mode 12 /* registers like dq_mode, memory like w_mode.  */
#define f_mode 13 /* 4- or 6-byte pointer operand */
#define const_1_mode 14
#define stack_v_mode 15 /* v_mode for stack-related opcodes.  */
#define z_mode 16 /* non-quad operand size depends on prefixes */
#define o_mode 17  /* 16-byte operand */

#define es_reg 100
#define cs_reg 101
#define ss_reg 102
#define ds_reg 103
#define fs_reg 104
#define gs_reg 105

#define eAX_reg 108
#define eCX_reg 109
#define eDX_reg 110
#define eBX_reg 111
#define eSP_reg 112
#define eBP_reg 113
#define eSI_reg 114
#define eDI_reg 115

#define al_reg 116
#define cl_reg 117
#define dl_reg 118
#define bl_reg 119
#define ah_reg 120
#define ch_reg 121
#define dh_reg 122
#define bh_reg 123

#define ax_reg 124
#define cx_reg 125
#define dx_reg 126
#define bx_reg 127
#define sp_reg 128
#define bp_reg 129
#define si_reg 130
#define di_reg 131

#define rAX_reg 132
#define rCX_reg 133
#define rDX_reg 134
#define rBX_reg 135
#define rSP_reg 136
#define rBP_reg 137
#define rSI_reg 138
#define rDI_reg 139

#define z_mode_ax_reg 149
#define indir_dx_reg 150

#define FLOATCODE 1
#define USE_GROUPS 2
#define USE_PREFIX_USER_TABLE 3
#define X86_64_SPECIAL 4
#define IS_3BYTE_OPCODE 5

#define FLOAT	  NULL, NULL, FLOATCODE, NULL, 0, NULL, 0, NULL, 0

#define GRP1b	  NULL, NULL, USE_GROUPS, NULL,  0, NULL, 0, NULL, 0
#define GRP1S	  NULL, NULL, USE_GROUPS, NULL,  1, NULL, 0, NULL, 0
#define GRP1Ss	  NULL, NULL, USE_GROUPS, NULL,  2, NULL, 0, NULL, 0
#define GRP2b	  NULL, NULL, USE_GROUPS, NULL,  3, NULL, 0, NULL, 0
#define GRP2S	  NULL, NULL, USE_GROUPS, NULL,  4, NULL, 0, NULL, 0
#define GRP2b_one NULL, NULL, USE_GROUPS, NULL,  5, NULL, 0, NULL, 0
#define GRP2S_one NULL, NULL, USE_GROUPS, NULL,  6, NULL, 0, NULL, 0
#define GRP2b_cl  NULL, NULL, USE_GROUPS, NULL,  7, NULL, 0, NULL, 0
#define GRP2S_cl  NULL, NULL, USE_GROUPS, NULL,  8, NULL, 0, NULL, 0
#define GRP3b	  NULL, NULL, USE_GROUPS, NULL,  9, NULL, 0, NULL, 0
#define GRP3S	  NULL, NULL, USE_GROUPS, NULL, 10, NULL, 0, NULL, 0
#define GRP4	  NULL, NULL, USE_GROUPS, NULL, 11, NULL, 0, NULL, 0
#define GRP5	  NULL, NULL, USE_GROUPS, NULL, 12, NULL, 0, NULL, 0
#define GRP6	  NULL, NULL, USE_GROUPS, NULL, 13, NULL, 0, NULL, 0
#define GRP7	  NULL, NULL, USE_GROUPS, NULL, 14, NULL, 0, NULL, 0
#define GRP8	  NULL, NULL, USE_GROUPS, NULL, 15, NULL, 0, NULL, 0
#define GRP9	  NULL, NULL, USE_GROUPS, NULL, 16, NULL, 0, NULL, 0
#define GRP11_C6  NULL, NULL, USE_GROUPS, NULL, 17, NULL, 0, NULL, 0
#define GRP11_C7  NULL, NULL, USE_GROUPS, NULL, 18, NULL, 0, NULL, 0
#define GRP12	  NULL, NULL, USE_GROUPS, NULL, 19, NULL, 0, NULL, 0
#define GRP13	  NULL, NULL, USE_GROUPS, NULL, 20, NULL, 0, NULL, 0
#define GRP14	  NULL, NULL, USE_GROUPS, NULL, 21, NULL, 0, NULL, 0
#define GRP15	  NULL, NULL, USE_GROUPS, NULL, 22, NULL, 0, NULL, 0
#define GRP16	  NULL, NULL, USE_GROUPS, NULL, 23, NULL, 0, NULL, 0
#define GRPAMD	  NULL, NULL, USE_GROUPS, NULL, 24, NULL, 0, NULL, 0
#define GRPPADLCK1 NULL, NULL, USE_GROUPS, NULL, 25, NULL, 0, NULL, 0
#define GRPPADLCK2 NULL, NULL, USE_GROUPS, NULL, 26, NULL, 0, NULL, 0

#define PREGRP0   NULL, NULL, USE_PREFIX_USER_TABLE, NULL,  0, NULL, 0, NULL, 0
#define PREGRP1   NULL, NULL, USE_PREFIX_USER_TABLE, NULL,  1, NULL, 0, NULL, 0
#define PREGRP2   NULL, NULL, USE_PREFIX_USER_TABLE, NULL,  2, NULL, 0, NULL, 0
#define PREGRP3   NULL, NULL, USE_PREFIX_USER_TABLE, NULL,  3, NULL, 0, NULL, 0
#define PREGRP4   NULL, NULL, USE_PREFIX_USER_TABLE, NULL,  4, NULL, 0, NULL, 0
#define PREGRP5   NULL, NULL, USE_PREFIX_USER_TABLE, NULL,  5, NULL, 0, NULL, 0
#define PREGRP6   NULL, NULL, USE_PREFIX_USER_TABLE, NULL,  6, NULL, 0, NULL, 0
#define PREGRP7   NULL, NULL, USE_PREFIX_USER_TABLE, NULL,  7, NULL, 0, NULL, 0
#define PREGRP8   NULL, NULL, USE_PREFIX_USER_TABLE, NULL,  8, NULL, 0, NULL, 0
#define PREGRP9   NULL, NULL, USE_PREFIX_USER_TABLE, NULL,  9, NULL, 0, NULL, 0
#define PREGRP10  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 10, NULL, 0, NULL, 0
#define PREGRP11  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 11, NULL, 0, NULL, 0
#define PREGRP12  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 12, NULL, 0, NULL, 0
#define PREGRP13  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 13, NULL, 0, NULL, 0
#define PREGRP14  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 14, NULL, 0, NULL, 0
#define PREGRP15  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 15, NULL, 0, NULL, 0
#define PREGRP16  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 16, NULL, 0, NULL, 0
#define PREGRP17  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 17, NULL, 0, NULL, 0
#define PREGRP18  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 18, NULL, 0, NULL, 0
#define PREGRP19  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 19, NULL, 0, NULL, 0
#define PREGRP20  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 20, NULL, 0, NULL, 0
#define PREGRP21  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 21, NULL, 0, NULL, 0
#define PREGRP22  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 22, NULL, 0, NULL, 0
#define PREGRP23  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 23, NULL, 0, NULL, 0
#define PREGRP24  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 24, NULL, 0, NULL, 0
#define PREGRP25  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 25, NULL, 0, NULL, 0
#define PREGRP26  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 26, NULL, 0, NULL, 0
#define PREGRP27  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 27, NULL, 0, NULL, 0
#define PREGRP28  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 28, NULL, 0, NULL, 0
#define PREGRP29  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 29, NULL, 0, NULL, 0
#define PREGRP30  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 30, NULL, 0, NULL, 0
#define PREGRP31  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 31, NULL, 0, NULL, 0
#define PREGRP32  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 32, NULL, 0, NULL, 0
#define PREGRP33  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 33, NULL, 0, NULL, 0
#define PREGRP34  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 34, NULL, 0, NULL, 0
#define PREGRP35  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 35, NULL, 0, NULL, 0
#define PREGRP36  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 36, NULL, 0, NULL, 0
#define PREGRP37  NULL, NULL, USE_PREFIX_USER_TABLE, NULL, 37, NULL, 0, NULL, 0


#define X86_64_0  NULL, NULL, X86_64_SPECIAL, NULL, 0, NULL, 0, NULL, 0
#define X86_64_1  NULL, NULL, X86_64_SPECIAL, NULL, 1, NULL, 0, NULL, 0
#define X86_64_2  NULL, NULL, X86_64_SPECIAL, NULL, 2, NULL, 0, NULL, 0
#define X86_64_3  NULL, NULL, X86_64_SPECIAL, NULL, 3, NULL, 0, NULL, 0

#define THREE_BYTE_0 NULL, NULL, IS_3BYTE_OPCODE, NULL, 0, NULL, 0, NULL, 0
#define THREE_BYTE_1 NULL, NULL, IS_3BYTE_OPCODE, NULL, 1, NULL, 0, NULL, 0

typedef void (*op_rtn) (int bytemode, int sizeflag);

struct dis386 {
  const char *name;
  op_rtn op1;
  int bytemode1;
  op_rtn op2;
  int bytemode2;
  op_rtn op3;
  int bytemode3;
  op_rtn op4;
  int bytemode4;
};

/* Upper case letters in the instruction names here are macros.
   'A' => print 'b' if no register operands or suffix_always is true
   'B' => print 'b' if suffix_always is true
   'C' => print 's' or 'l' ('w' or 'd' in Intel mode) depending on operand
   .      size prefix
   'D' => print 'w' if no register operands or 'w', 'l' or 'q', if
   .      suffix_always is true
   'E' => print 'e' if 32-bit form of jcxz
   'F' => print 'w' or 'l' depending on address size prefix (loop insns)
   'G' => print 'w' or 'l' depending on operand size prefix (i/o insns)
   'H' => print ",pt" or ",pn" branch hint
   'I' => honor following macro letter even in Intel mode (implemented only
   .      for some of the macro letters)
   'J' => print 'l'
   'L' => print 'l' if suffix_always is true
   'N' => print 'n' if instruction has no wait "prefix"
   'O' => print 'd' or 'o' (or 'q' in Intel mode)
   'P' => print 'w', 'l' or 'q' if instruction has an operand size prefix,
   .      or suffix_always is true.  print 'q' if rex prefix is present.
   'Q' => print 'w', 'l' or 'q' if no register operands or suffix_always
   .      is true
   'R' => print 'w', 'l' or 'q' ('d' for 'l' and 'e' in Intel mode)
   'S' => print 'w', 'l' or 'q' if suffix_always is true
   'T' => print 'q' in 64bit mode and behave as 'P' otherwise
   'U' => print 'q' in 64bit mode and behave as 'Q' otherwise
   'V' => print 'q' in 64bit mode and behave as 'S' otherwise
   'W' => print 'b', 'w' or 'l' ('d' in Intel mode)
   'X' => print 's', 'd' depending on data16 prefix (for XMM)
   'Y' => 'q' if instruction has an REX 64bit overwrite prefix
   'Z' => print 'q' in 64bit mode and behave as 'L' otherwise

   Many of the above letters print nothing in Intel mode.  See "putop"
   for the details.

   Braces '{' and '}', and vertical bars '|', indicate alternative
   mnemonic strings for AT&T, Intel, X86_64 AT&T, and X86_64 Intel
   modes.  In cases where there are only two alternatives, the X86_64
   instruction is reserved, and "(bad)" is printed.
*/

static const struct dis386 dis386[] = {
  /* 00 */
  { "addB",		Eb, Gb, XX, XX },
  { "addS",		Ev, Gv, XX, XX },
  { "addB",		Gb, Eb, XX, XX },
  { "addS",		Gv, Ev, XX, XX },
  { "addB",		AL, Ib, XX, XX },
  { "addS",		eAX, Iv, XX, XX },
  { "push{T|}",		es, XX, XX, XX },
  { "pop{T|}",		es, XX, XX, XX },
  /* 08 */
  { "orB",		Eb, Gb, XX, XX },
  { "orS",		Ev, Gv, XX, XX },
  { "orB",		Gb, Eb, XX, XX },
  { "orS",		Gv, Ev, XX, XX },
  { "orB",		AL, Ib, XX, XX },
  { "orS",		eAX, Iv, XX, XX },
  { "push{T|}",		cs, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },	/* 0x0f extended opcode escape */
  /* 10 */
  { "adcB",		Eb, Gb, XX, XX },
  { "adcS",		Ev, Gv, XX, XX },
  { "adcB",		Gb, Eb, XX, XX },
  { "adcS",		Gv, Ev, XX, XX },
  { "adcB",		AL, Ib, XX, XX },
  { "adcS",		eAX, Iv, XX, XX },
  { "push{T|}",		ss, XX, XX, XX },
  { "pop{T|}",		ss, XX, XX, XX },
  /* 18 */
  { "sbbB",		Eb, Gb, XX, XX },
  { "sbbS",		Ev, Gv, XX, XX },
  { "sbbB",		Gb, Eb, XX, XX },
  { "sbbS",		Gv, Ev, XX, XX },
  { "sbbB",		AL, Ib, XX, XX },
  { "sbbS",		eAX, Iv, XX, XX },
  { "push{T|}",		ds, XX, XX, XX },
  { "pop{T|}",		ds, XX, XX, XX },
  /* 20 */
  { "andB",		Eb, Gb, XX, XX },
  { "andS",		Ev, Gv, XX, XX },
  { "andB",		Gb, Eb, XX, XX },
  { "andS",		Gv, Ev, XX, XX },
  { "andB",		AL, Ib, XX, XX },
  { "andS",		eAX, Iv, XX, XX },
  { "(bad)",		XX, XX, XX, XX },	/* SEG ES prefix */
  { "daa{|}",		XX, XX, XX, XX },
  /* 28 */
  { "subB",		Eb, Gb, XX, XX },
  { "subS",		Ev, Gv, XX, XX },
  { "subB",		Gb, Eb, XX, XX },
  { "subS",		Gv, Ev, XX, XX },
  { "subB",		AL, Ib, XX, XX },
  { "subS",		eAX, Iv, XX, XX },
  { "(bad)",		XX, XX, XX, XX },	/* SEG CS prefix */
  { "das{|}",		XX, XX, XX, XX },
  /* 30 */
  { "xorB",		Eb, Gb, XX, XX },
  { "xorS",		Ev, Gv, XX, XX },
  { "xorB",		Gb, Eb, XX, XX },
  { "xorS",		Gv, Ev, XX, XX },
  { "xorB",		AL, Ib, XX, XX },
  { "xorS",		eAX, Iv, XX, XX },
  { "(bad)",		XX, XX, XX, XX },	/* SEG SS prefix */
  { "aaa{|}",		XX, XX, XX, XX },
  /* 38 */
  { "cmpB",		Eb, Gb, XX, XX },
  { "cmpS",		Ev, Gv, XX, XX },
  { "cmpB",		Gb, Eb, XX, XX },
  { "cmpS",		Gv, Ev, XX, XX },
  { "cmpB",		AL, Ib, XX, XX },
  { "cmpS",		eAX, Iv, XX, XX },
  { "(bad)",		XX, XX, XX, XX },	/* SEG DS prefix */
  { "aas{|}",		XX, XX, XX, XX },
  /* 40 */
  { "inc{S|}",		RMeAX, XX, XX, XX },
  { "inc{S|}",		RMeCX, XX, XX, XX },
  { "inc{S|}",		RMeDX, XX, XX, XX },
  { "inc{S|}",		RMeBX, XX, XX, XX },
  { "inc{S|}",		RMeSP, XX, XX, XX },
  { "inc{S|}",		RMeBP, XX, XX, XX },
  { "inc{S|}",		RMeSI, XX, XX, XX },
  { "inc{S|}",		RMeDI, XX, XX, XX },
  /* 48 */
  { "dec{S|}",		RMeAX, XX, XX, XX },
  { "dec{S|}",		RMeCX, XX, XX, XX },
  { "dec{S|}",		RMeDX, XX, XX, XX },
  { "dec{S|}",		RMeBX, XX, XX, XX },
  { "dec{S|}",		RMeSP, XX, XX, XX },
  { "dec{S|}",		RMeBP, XX, XX, XX },
  { "dec{S|}",		RMeSI, XX, XX, XX },
  { "dec{S|}",		RMeDI, XX, XX, XX },
  /* 50 */
  { "pushV",		RMrAX, XX, XX, XX },
  { "pushV",		RMrCX, XX, XX, XX },
  { "pushV",		RMrDX, XX, XX, XX },
  { "pushV",		RMrBX, XX, XX, XX },
  { "pushV",		RMrSP, XX, XX, XX },
  { "pushV",		RMrBP, XX, XX, XX },
  { "pushV",		RMrSI, XX, XX, XX },
  { "pushV",		RMrDI, XX, XX, XX },
  /* 58 */
  { "popV",		RMrAX, XX, XX, XX },
  { "popV",		RMrCX, XX, XX, XX },
  { "popV",		RMrDX, XX, XX, XX },
  { "popV",		RMrBX, XX, XX, XX },
  { "popV",		RMrSP, XX, XX, XX },
  { "popV",		RMrBP, XX, XX, XX },
  { "popV",		RMrSI, XX, XX, XX },
  { "popV",		RMrDI, XX, XX, XX },
  /* 60 */
  { X86_64_0 },
  { X86_64_1 },
  { X86_64_2 },
  { X86_64_3 },
  { "(bad)",		XX, XX, XX, XX },	/* seg fs */
  { "(bad)",		XX, XX, XX, XX },	/* seg gs */
  { "(bad)",		XX, XX, XX, XX },	/* op size prefix */
  { "(bad)",		XX, XX, XX, XX },	/* adr size prefix */
  /* 68 */
  { "pushT",		Iq, XX, XX, XX },
  { "imulS",		Gv, Ev, Iv, XX },
  { "pushT",		sIb, XX, XX, XX },
  { "imulS",		Gv, Ev, sIb, XX },
  { "ins{b||b|}",	Ybr, indirDX, XX, XX },
  { "ins{R||G|}",	Yzr, indirDX, XX, XX },
  { "outs{b||b|}",	indirDXr, Xb, XX, XX },
  { "outs{R||G|}",	indirDXr, Xz, XX, XX },
  /* 70 */
  { "joH",		Jb, XX, cond_jump_flag, XX },
  { "jnoH",		Jb, XX, cond_jump_flag, XX },
  { "jbH",		Jb, XX, cond_jump_flag, XX },
  { "jaeH",		Jb, XX, cond_jump_flag, XX },
  { "jeH",		Jb, XX, cond_jump_flag, XX },
  { "jneH",		Jb, XX, cond_jump_flag, XX },
  { "jbeH",		Jb, XX, cond_jump_flag, XX },
  { "jaH",		Jb, XX, cond_jump_flag, XX },
  /* 78 */
  { "jsH",		Jb, XX, cond_jump_flag, XX },
  { "jnsH",		Jb, XX, cond_jump_flag, XX },
  { "jpH",		Jb, XX, cond_jump_flag, XX },
  { "jnpH",		Jb, XX, cond_jump_flag, XX },
  { "jlH",		Jb, XX, cond_jump_flag, XX },
  { "jgeH",		Jb, XX, cond_jump_flag, XX },
  { "jleH",		Jb, XX, cond_jump_flag, XX },
  { "jgH",		Jb, XX, cond_jump_flag, XX },
  /* 80 */
  { GRP1b },
  { GRP1S },
  { "(bad)",		XX, XX, XX, XX },
  { GRP1Ss },
  { "testB",		Eb, Gb, XX, XX },
  { "testS",		Ev, Gv, XX, XX },
  { "xchgB",		Eb, Gb, XX, XX },
  { "xchgS",		Ev, Gv, XX, XX },
  /* 88 */
  { "movB",		Eb, Gb, XX, XX },
  { "movS",		Ev, Gv, XX, XX },
  { "movB",		Gb, Eb, XX, XX },
  { "movS",		Gv, Ev, XX, XX },
  { "movD",		Sv, Sw, XX, XX },
  { "leaS",		Gv, M, XX, XX },
  { "movD",		Sw, Sv, XX, XX },
  { "popU",		stackEv, XX, XX, XX },
  /* 90 */
  { "xchgS",		NOP_Fixup1, eAX_reg, NOP_Fixup2, eAX_reg, XX, XX },
  { "xchgS",		RMeCX, eAX, XX, XX },
  { "xchgS",		RMeDX, eAX, XX, XX },
  { "xchgS",		RMeBX, eAX, XX, XX },
  { "xchgS",		RMeSP, eAX, XX, XX },
  { "xchgS",		RMeBP, eAX, XX, XX },
  { "xchgS",		RMeSI, eAX, XX, XX },
  { "xchgS",		RMeDI, eAX, XX, XX },
  /* 98 */
  { "cW{t||t|}R",	XX, XX, XX, XX },
  { "cR{t||t|}O",	XX, XX, XX, XX },
  { "Jcall{T|}",	Ap, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },	/* fwait */
  { "pushfT",		XX, XX, XX, XX },
  { "popfT",		XX, XX, XX, XX },
  { "sahf{|}",		XX, XX, XX, XX },
  { "lahf{|}",		XX, XX, XX, XX },
  /* a0 */
  { "movB",		AL, Ob, XX, XX },
  { "movS",		eAX, Ov, XX, XX },
  { "movB",		Ob, AL, XX, XX },
  { "movS",		Ov, eAX, XX, XX },
  { "movs{b||b|}",	Ybr, Xb, XX, XX },
  { "movs{R||R|}",	Yvr, Xv, XX, XX },
  { "cmps{b||b|}",	Xb, Yb, XX, XX },
  { "cmps{R||R|}",	Xv, Yv, XX, XX },
  /* a8 */
  { "testB",		AL, Ib, XX, XX },
  { "testS",		eAX, Iv, XX, XX },
  { "stosB",		Ybr, AL, XX, XX },
  { "stosS",		Yvr, eAX, XX, XX },
  { "lodsB",		ALr, Xb, XX, XX },
  { "lodsS",		eAXr, Xv, XX, XX },
  { "scasB",		AL, Yb, XX, XX },
  { "scasS",		eAX, Yv, XX, XX },
  /* b0 */
  { "movB",		RMAL, Ib, XX, XX },
  { "movB",		RMCL, Ib, XX, XX },
  { "movB",		RMDL, Ib, XX, XX },
  { "movB",		RMBL, Ib, XX, XX },
  { "movB",		RMAH, Ib, XX, XX },
  { "movB",		RMCH, Ib, XX, XX },
  { "movB",		RMDH, Ib, XX, XX },
  { "movB",		RMBH, Ib, XX, XX },
  /* b8 */
  { "movS",		RMeAX, Iv64, XX, XX },
  { "movS",		RMeCX, Iv64, XX, XX },
  { "movS",		RMeDX, Iv64, XX, XX },
  { "movS",		RMeBX, Iv64, XX, XX },
  { "movS",		RMeSP, Iv64, XX, XX },
  { "movS",		RMeBP, Iv64, XX, XX },
  { "movS",		RMeSI, Iv64, XX, XX },
  { "movS",		RMeDI, Iv64, XX, XX },
  /* c0 */
  { GRP2b },
  { GRP2S },
  { "retT",		Iw, XX, XX, XX },
  { "retT",		XX, XX, XX, XX },
  { "les{S|}",		Gv, Mp, XX, XX },
  { "ldsS",		Gv, Mp, XX, XX },
  { GRP11_C6 },
  { GRP11_C7 },
  /* c8 */
  { "enterT",		Iw, Ib, XX, XX },
  { "leaveT",		XX, XX, XX, XX },
  { "lretP",		Iw, XX, XX, XX },
  { "lretP",		XX, XX, XX, XX },
  { "int3",		XX, XX, XX, XX },
  { "int",		Ib, XX, XX, XX },
  { "into{|}",		XX, XX, XX, XX },
  { "iretP",		XX, XX, XX, XX },
  /* d0 */
  { GRP2b_one },
  { GRP2S_one },
  { GRP2b_cl },
  { GRP2S_cl },
  { "aam{|}",		sIb, XX, XX, XX },
  { "aad{|}",		sIb, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { "xlat",		DSBX, XX, XX, XX },
  /* d8 */
  { FLOAT },
  { FLOAT },
  { FLOAT },
  { FLOAT },
  { FLOAT },
  { FLOAT },
  { FLOAT },
  { FLOAT },
  /* e0 */
  { "loopneFH",		Jb, XX, loop_jcxz_flag, XX },
  { "loopeFH",		Jb, XX, loop_jcxz_flag, XX },
  { "loopFH",		Jb, XX, loop_jcxz_flag, XX },
  { "jEcxzH",		Jb, XX, loop_jcxz_flag, XX },
  { "inB",		AL, Ib, XX, XX },
  { "inG",		zAX, Ib, XX, XX },
  { "outB",		Ib, AL, XX, XX },
  { "outG",		Ib, zAX, XX, XX },
  /* e8 */
  { "callT",		Jv, XX, XX, XX },
  { "jmpT",		Jv, XX, XX, XX },
  { "Jjmp{T|}",		Ap, XX, XX, XX },
  { "jmp",		Jb, XX, XX, XX },
  { "inB",		AL, indirDX, XX, XX },
  { "inG",		zAX, indirDX, XX, XX },
  { "outB",		indirDX, AL, XX, XX },
  { "outG",		indirDX, zAX, XX, XX },
  /* f0 */
  { "(bad)",		XX, XX, XX, XX },	/* lock prefix */
  { "icebp",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },	/* repne */
  { "(bad)",		XX, XX, XX, XX },	/* repz */
  { "hlt",		XX, XX, XX, XX },
  { "cmc",		XX, XX, XX, XX },
  { GRP3b },
  { GRP3S },
  /* f8 */
  { "clc",		XX, XX, XX, XX },
  { "stc",		XX, XX, XX, XX },
  { "cli",		XX, XX, XX, XX },
  { "sti",		XX, XX, XX, XX },
  { "cld",		XX, XX, XX, XX },
  { "std",		XX, XX, XX, XX },
  { GRP4 },
  { GRP5 },
};

static const struct dis386 dis386_twobyte[] = {
  /* 00 */
  { GRP6 },
  { GRP7 },
  { "larS",		Gv, Ew, XX, XX },
  { "lslS",		Gv, Ew, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { "syscall",		XX, XX, XX, XX },
  { "clts",		XX, XX, XX, XX },
  { "sysretP",		XX, XX, XX, XX },
  /* 08 */
  { "invd",		XX, XX, XX, XX },
  { "wbinvd",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { "ud2a",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { GRPAMD },
  { "femms",		XX, XX, XX, XX },
  { "",			MX, EM, OPSUF, XX }, /* See OP_3DNowSuffix.  */
  /* 10 */
  { PREGRP8 },
  { PREGRP9 },
  { PREGRP30 },
  { "movlpX",		EX, XM, SIMD_Fixup, 'h', XX },
  { "unpcklpX",		XM, EX, XX, XX },
  { "unpckhpX",		XM, EX, XX, XX },
  { PREGRP31 },
  { "movhpX",		EX, XM, SIMD_Fixup, 'l', XX },
  /* 18 */
  { GRP16 },
  { "(bad)",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { "nopQ",		Ev, XX, XX, XX },
  /* 20 */
  { "movZ",		Rm, Cm, XX, XX },
  { "movZ",		Rm, Dm, XX, XX },
  { "movZ",		Cm, Rm, XX, XX },
  { "movZ",		Dm, Rm, XX, XX },
  { "movL",		Rd, Td, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { "movL",		Td, Rd, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  /* 28 */
  { "movapX",		XM, EX, XX, XX },
  { "movapX",		EX, XM, XX, XX },
  { PREGRP2 },
  { PREGRP33 },
  { PREGRP4 },
  { PREGRP3 },
  { "ucomisX",		XM,EX, XX, XX },
  { "comisX",		XM,EX, XX, XX },
  /* 30 */
  { "wrmsr",		XX, XX, XX, XX },
  { "rdtsc",		XX, XX, XX, XX },
  { "rdmsr",		XX, XX, XX, XX },
  { "rdpmc",		XX, XX, XX, XX },
  { "sysenter",		XX, XX, XX, XX },
  { "sysexit",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  /* 38 */
  { THREE_BYTE_0 },
  { "(bad)",		XX, XX, XX, XX },
  { THREE_BYTE_1 },
  { "(bad)",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  /* 40 */
  { "cmovo",		Gv, Ev, XX, XX },
  { "cmovno",		Gv, Ev, XX, XX },
  { "cmovb",		Gv, Ev, XX, XX },
  { "cmovae",		Gv, Ev, XX, XX },
  { "cmove",		Gv, Ev, XX, XX },
  { "cmovne",		Gv, Ev, XX, XX },
  { "cmovbe",		Gv, Ev, XX, XX },
  { "cmova",		Gv, Ev, XX, XX },
  /* 48 */
  { "cmovs",		Gv, Ev, XX, XX },
  { "cmovns",		Gv, Ev, XX, XX },
  { "cmovp",		Gv, Ev, XX, XX },
  { "cmovnp",		Gv, Ev, XX, XX },
  { "cmovl",		Gv, Ev, XX, XX },
  { "cmovge",		Gv, Ev, XX, XX },
  { "cmovle",		Gv, Ev, XX, XX },
  { "cmovg",		Gv, Ev, XX, XX },
  /* 50 */
  { "movmskpX",		Gdq, XS, XX, XX },
  { PREGRP13 },
  { PREGRP12 },
  { PREGRP11 },
  { "andpX",		XM, EX, XX, XX },
  { "andnpX",		XM, EX, XX, XX },
  { "orpX",		XM, EX, XX, XX },
  { "xorpX",		XM, EX, XX, XX },
  /* 58 */
  { PREGRP0 },
  { PREGRP10 },
  { PREGRP17 },
  { PREGRP16 },
  { PREGRP14 },
  { PREGRP7 },
  { PREGRP5 },
  { PREGRP6 },
  /* 60 */
  { "punpcklbw",	MX, EM, XX, XX },
  { "punpcklwd",	MX, EM, XX, XX },
  { "punpckldq",	MX, EM, XX, XX },
  { "packsswb",		MX, EM, XX, XX },
  { "pcmpgtb",		MX, EM, XX, XX },
  { "pcmpgtw",		MX, EM, XX, XX },
  { "pcmpgtd",		MX, EM, XX, XX },
  { "packuswb",		MX, EM, XX, XX },
  /* 68 */
  { "punpckhbw",	MX, EM, XX, XX },
  { "punpckhwd",	MX, EM, XX, XX },
  { "punpckhdq",	MX, EM, XX, XX },
  { "packssdw",		MX, EM, XX, XX },
  { PREGRP26 },
  { PREGRP24 },
  { "movd",		MX, Edq, XX, XX },
  { PREGRP19 },
  /* 70 */
  { PREGRP22 },
  { GRP12 },
  { GRP13 },
  { GRP14 },
  { "pcmpeqb",		MX, EM, XX, XX },
  { "pcmpeqw",		MX, EM, XX, XX },
  { "pcmpeqd",		MX, EM, XX, XX },
  { "emms",		XX, XX, XX, XX },
  /* 78 */
  { PREGRP34 },
  { PREGRP35 },
  { "(bad)",		XX, XX, XX, XX },
  { "(bad)",		XX, XX, XX, XX },
  { PREGRP28 },
  { PREGRP29 },
  { PREGRP23 },
  { PREGRP20 },
  /* 80 */
  { "joH",		Jv, XX, cond_jump_flag, XX },
  { "jnoH",		Jv, XX, cond_jump_flag, XX },
  { "jbH",		Jv, XX, cond_jump_flag, XX },
  { "jaeH",		Jv, XX, cond_jump_flag, XX },
  { "jeH",		Jv, XX, cond_jump_flag, XX },
  { "jneH",		Jv, XX, cond_jump_flag, XX },
  { "jbeH",		Jv, XX, cond_jump_flag, XX },
  { "jaH",		Jv, XX, cond_jump_flag, XX },
  /* 88 */
  { "jsH",		Jv, XX, cond_jump_flag, XX },
  { "jnsH",		Jv, XX, cond_jump_flag, XX },
  { "jpH",		Jv, XX, cond_jump_flag, XX },
  { "jnpH",		Jv, XX, cond_jump_flag, XX },
  { "jlH",		Jv, XX, cond_jump_flag, XX },
  { "jgeH",		Jv, XX, cond_jump_flag, XX },
  { "jleH",		Jv, XX, cond_jump_flag, XX },
  { "jgH",		Jv, XX, cond_jump_flag, XX },
  /* 90 */
  { "seto",		Eb, XX, XX, XX },
  { "setno",		Eb, XX, XX, XX },
  { "setb",		Eb, XX, XX, XX },
  { "setae",		Eb, XX, XX, XX },
  { "sete",		Eb, XX, XX, XX },
  { "setne",		Eb, XX, XX, XX },
  { "setbe",		Eb, XX, XX, XX },
  { "seta",		Eb, XX, XX, XX },
  /* 98 */
  { "sets",		Eb, XX, XX, XX },
  { "setns",		Eb, XX, XX, XX },
  { "setp",		Eb, XX, XX, XX },
  { "setnp",		Eb, XX, XX, XX },
  { "setl",		Eb, XX, XX, XX },
  { "setge",		Eb, XX, XX, XX },
  { "setle",		Eb, XX, XX, XX },
  { "setg",		Eb, XX, XX, XX },
  /* a0 */
  { "pushT",		fs, XX, XX, XX },
  { "popT",		fs, XX, XX, XX },
  { "cpuid",		XX, XX, XX, XX },
  { "btS",		Ev, Gv, XX, XX },
  { "shldS",		Ev, Gv, Ib, XX },
  { "shldS",		Ev, Gv, CL, XX },
  { GRPPADLCK2 },
  { GRPPADLCK1 },
  /* a8 */
  { "pushT",		gs, XX, XX, XX },
  { "popT",		gs, XX, XX, XX },
  { "rsm",		XX, XX, XX, XX },
  { "btsS",		Ev, Gv, XX, XX },
  { "shrdS",		Ev, Gv, Ib, XX },
  { "shrdS",		Ev, Gv, CL, XX },
  { GRP15 },
  { "imulS",		Gv, Ev, XX, XX },
  /* b0 */
  { "cmpxchgB",		Eb, Gb, XX, XX },
  { "cmpxchgS",		Ev, Gv, XX, XX },
  { "lssS",		Gv, Mp, XX, XX },
  { "btrS",		Ev, Gv, XX, XX },
  { "lfsS",		Gv, Mp, XX, XX },
  { "lgsS",		Gv, Mp, XX, XX },
  { "movz{bR|x|bR|x}",	Gv, Eb, XX, XX },
  { "movz{wR|x|wR|x}",	Gv, Ew, XX, XX }, /* yes, there really is movzww ! */
  /* b8 */
  { PREGRP37 },
  { "ud2b",		XX, XX, XX, XX },
  { GRP8 },
  { "btcS",		Ev, Gv, XX, XX },
  { "bsfS",		Gv, Ev, XX, XX },
  { PREGRP36 },
  { "movs{bR|x|bR|x}",	Gv, Eb, XX, XX },
  { "movs{wR|x|wR|x}",	Gv, Ew, XX, XX }, /* yes, there really is movsww ! */
  /* c0 */
  { "xaddB",		Eb, Gb, XX, XX },
  { "xaddS",		Ev, Gv, XX, XX },
  { PREGRP1 },
  { "movntiS",		Ev, Gv, XX, XX },
  { "pinsrw",		MX, Edqw, Ib, XX },
  { "pextrw",		Gdq, MS, Ib, XX },
  { "shufpX",		XM, EX, Ib, XX },
  { GRP9 },
  /* c8 */
  { "bswap",		RMeAX, XX, XX, XX },
  { "bswap",		RMeCX, XX, XX, XX },
  { "bswap",		RMeDX, XX, XX, XX },
  { "bswap",		RMeBX, XX, XX, XX },
  { "bswap",		RMeSP, XX, XX, XX },
  { "bswap",		RMeBP, XX, XX, XX },
  { "bswap",		RMeSI, XX, XX, XX },
  { "bswap",		RMeDI, XX, XX, XX },
  /* d0 */
  { PREGRP27 },
  { "psrlw",		MX, EM, XX, XX },
  { "psrld",		MX, EM, XX, XX },
  { "psrlq",		MX, EM, XX, XX },
  { "paddq",		MX, EM, XX, XX },
  { "pmullw",		MX, EM, XX, XX },
  { PREGRP21 },
  { "pmovmskb",		Gdq, MS, XX, XX },
  /* d8 */
  { "psubusb",		MX, EM, XX, XX },
  { "psubusw",		MX, EM, XX, XX },
  { "pminub",		MX, EM, XX, XX },
  { "pand",		MX, EM, XX, XX },
  { "paddusb",		MX, EM, XX, XX },
  { "paddusw",		MX, EM, XX, XX },
  { "pmaxub",		MX, EM, XX, XX },
  { "pandn",		MX, EM, XX, XX },
  /* e0 */
  { "pavgb",		MX, EM, XX, XX },
  { "psraw",		MX, EM, XX, XX },
  { "psrad",		MX, EM, XX, XX },
  { "pavgw",		MX, EM, XX, XX },
  { "pmulhuw",		MX, EM, XX, XX },
  { "pmulhw",		MX, EM, XX, XX },
  { PREGRP15 },
  { PREGRP25 },
  /* e8 */
  { "psubsb",		MX, EM, XX, XX },
  { "psubsw",		MX, EM, XX, XX },
  { "pminsw",		MX, EM, XX, XX },
  { "por",		MX, EM, XX, XX },
  { "paddsb",		MX, EM, XX, XX },
  { "paddsw",		MX, EM, XX, XX },
  { "pmaxsw",		MX, EM, XX, XX },
  { "pxor",		MX, EM, XX, XX },
  /* f0 */
  { PREGRP32 },
  { "psllw",		MX, EM, XX, XX },
  { "pslld",		MX, EM, XX, XX },
  { "psllq",		MX, EM, XX, XX },
  { "pmuludq",		MX, EM, XX, XX },
  { "pmaddwd",		MX, EM, XX, XX },
  { "psadbw",		MX, EM, XX, XX },
  { PREGRP18 },
  /* f8 */
  { "psubb",		MX, EM, XX, XX },
  { "psubw",		MX, EM, XX, XX },
  { "psubd",		MX, EM, XX, XX },
  { "psubq",		MX, EM, XX, XX },
  { "paddb",		MX, EM, XX, XX },
  { "paddw",		MX, EM, XX, XX },
  { "paddd",		MX, EM, XX, XX },
  { "(bad)",		XX, XX, XX, XX }
};

static const unsigned char onebyte_has_modrm[256] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
  /*       -------------------------------        */
  /* 00 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0, /* 00 */
  /* 10 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0, /* 10 */
  /* 20 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0, /* 20 */
  /* 30 */ 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0, /* 30 */
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 40 */
  /* 50 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 50 */
  /* 60 */ 0,0,1,1,0,0,0,0,0,1,0,1,0,0,0,0, /* 60 */
  /* 70 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 70 */
  /* 80 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 80 */
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 90 */
  /* a0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* a0 */
  /* b0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* b0 */
  /* c0 */ 1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0, /* c0 */
  /* d0 */ 1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1, /* d0 */
  /* e0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* e0 */
  /* f0 */ 0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1  /* f0 */
  /*       -------------------------------        */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
};

static const unsigned char twobyte_has_modrm[256] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
  /*       -------------------------------        */
  /* 00 */ 1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,1, /* 0f */
  /* 10 */ 1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1, /* 1f */
  /* 20 */ 1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1, /* 2f */
  /* 30 */ 0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0, /* 3f */
  /* 40 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 4f */
  /* 50 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 5f */
  /* 60 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 6f */
  /* 70 */ 1,1,1,1,1,1,1,0,1,1,0,0,1,1,1,1, /* 7f */
  /* 80 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8f */
  /* 90 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 9f */
  /* a0 */ 0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1, /* af */
  /* b0 */ 1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1, /* bf */
  /* c0 */ 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0, /* cf */
  /* d0 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* df */
  /* e0 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* ef */
  /* f0 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0  /* ff */
  /*       -------------------------------        */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
};

static const unsigned char twobyte_uses_DATA_prefix[256] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
  /*       -------------------------------        */
  /* 00 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0f */
  /* 10 */ 1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0, /* 1f */
  /* 20 */ 0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0, /* 2f */
  /* 30 */ 0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0, /* 3f */
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 4f */
  /* 50 */ 0,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1, /* 5f */
  /* 60 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1, /* 6f */
  /* 70 */ 1,0,0,0,0,0,0,0,1,1,0,0,1,1,1,1, /* 7f */
  /* 80 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8f */
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 9f */
  /* a0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* af */
  /* b0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* bf */
  /* c0 */ 0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0, /* cf */
  /* d0 */ 1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0, /* df */
  /* e0 */ 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0, /* ef */
  /* f0 */ 1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0  /* ff */
  /*       -------------------------------        */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
};

static const unsigned char twobyte_uses_REPNZ_prefix[256] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
  /*       -------------------------------        */
  /* 00 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0f */
  /* 10 */ 1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 1f */
  /* 20 */ 0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0, /* 2f */
  /* 30 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 3f */
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 4f */
  /* 50 */ 0,1,0,0,0,0,0,0,1,1,1,0,1,1,1,1, /* 5f */
  /* 60 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 6f */
  /* 70 */ 1,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0, /* 7f */
  /* 80 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8f */
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 9f */
  /* a0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* af */
  /* b0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* bf */
  /* c0 */ 0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0, /* cf */
  /* d0 */ 1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0, /* df */
  /* e0 */ 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0, /* ef */
  /* f0 */ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ff */
  /*       -------------------------------        */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
};

static const unsigned char twobyte_uses_REPZ_prefix[256] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
  /*       -------------------------------        */
  /* 00 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0f */
  /* 10 */ 1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0, /* 1f */
  /* 20 */ 0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0, /* 2f */
  /* 30 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 3f */
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 4f */
  /* 50 */ 0,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1, /* 5f */
  /* 60 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1, /* 6f */
  /* 70 */ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1, /* 7f */
  /* 80 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8f */
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 9f */
  /* a0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* af */
  /* b0 */ 0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0, /* bf */
  /* c0 */ 0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0, /* cf */
  /* d0 */ 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0, /* df */
  /* e0 */ 0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0, /* ef */
  /* f0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ff */
  /*       -------------------------------        */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
};

/* This is used to determine if opcode 0f 38 XX uses DATA prefix.  */ 
static const unsigned char threebyte_0x38_uses_DATA_prefix[256] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
  /*       -------------------------------        */
  /* 00 */ 1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0, /* 0f */
  /* 10 */ 0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0, /* 1f */
  /* 20 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 2f */
  /* 30 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 3f */
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 4f */
  /* 50 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 5f */
  /* 60 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 6f */
  /* 70 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 7f */
  /* 80 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8f */
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 9f */
  /* a0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* af */
  /* b0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* bf */
  /* c0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* cf */
  /* d0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* df */
  /* e0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ef */
  /* f0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ff */
  /*       -------------------------------        */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
};

/* This is used to determine if opcode 0f 38 XX uses REPNZ prefix.  */ 
static const unsigned char threebyte_0x38_uses_REPNZ_prefix[256] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
  /*       -------------------------------        */
  /* 00 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0f */
  /* 10 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 1f */
  /* 20 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 2f */
  /* 30 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 3f */
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 4f */
  /* 50 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 5f */
  /* 60 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 6f */
  /* 70 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 7f */
  /* 80 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8f */
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 9f */
  /* a0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* af */
  /* b0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* bf */
  /* c0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* cf */
  /* d0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* df */
  /* e0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ef */
  /* f0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ff */
  /*       -------------------------------        */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
};

/* This is used to determine if opcode 0f 38 XX uses REPZ prefix.  */ 
static const unsigned char threebyte_0x38_uses_REPZ_prefix[256] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
  /*       -------------------------------        */
  /* 00 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0f */
  /* 10 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 1f */
  /* 20 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 2f */
  /* 30 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 3f */
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 4f */
  /* 50 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 5f */
  /* 60 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 6f */
  /* 70 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 7f */
  /* 80 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8f */
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 9f */
  /* a0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* af */
  /* b0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* bf */
  /* c0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* cf */
  /* d0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* df */
  /* e0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ef */
  /* f0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ff */
  /*       -------------------------------        */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
};

/* This is used to determine if opcode 0f 3a XX uses DATA prefix.  */ 
static const unsigned char threebyte_0x3a_uses_DATA_prefix[256] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
  /*       -------------------------------        */
  /* 00 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1, /* 0f */
  /* 10 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 1f */
  /* 20 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 2f */
  /* 30 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 3f */
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 4f */
  /* 50 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 5f */
  /* 60 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 6f */
  /* 70 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 7f */
  /* 80 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8f */
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 9f */
  /* a0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* af */
  /* b0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* bf */
  /* c0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* cf */
  /* d0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* df */
  /* e0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ef */
  /* f0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ff */
  /*       -------------------------------        */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
};

/* This is used to determine if opcode 0f 3a XX uses REPNZ prefix.  */ 
static const unsigned char threebyte_0x3a_uses_REPNZ_prefix[256] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
  /*       -------------------------------        */
  /* 00 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0f */
  /* 10 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 1f */
  /* 20 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 2f */
  /* 30 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 3f */
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 4f */
  /* 50 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 5f */
  /* 60 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 6f */
  /* 70 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 7f */
  /* 80 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8f */
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 9f */
  /* a0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* af */
  /* b0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* bf */
  /* c0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* cf */
  /* d0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* df */
  /* e0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ef */
  /* f0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ff */
  /*       -------------------------------        */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
};

/* This is used to determine if opcode 0f 3a XX uses REPZ prefix.  */ 
static const unsigned char threebyte_0x3a_uses_REPZ_prefix[256] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
  /*       -------------------------------        */
  /* 00 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0f */
  /* 10 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 1f */
  /* 20 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 2f */
  /* 30 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 3f */
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 4f */
  /* 50 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 5f */
  /* 60 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 6f */
  /* 70 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 7f */
  /* 80 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8f */
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 9f */
  /* a0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* af */
  /* b0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* bf */
  /* c0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* cf */
  /* d0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* df */
  /* e0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ef */
  /* f0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ff */
  /*       -------------------------------        */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
};

static char obuf[100];
static char *obufp;
static char scratchbuf[100];
static unsigned char *start_codep;
static unsigned char *insn_codep;
static unsigned char *codep;
static disassemble_info *the_info;
static int mod;
static int rm;
static int reg;
static unsigned char need_modrm;

/* If we are accessing mod/rm/reg without need_modrm set, then the
   values are stale.  Hitting this abort likely indicates that you
   need to update onebyte_has_modrm or twobyte_has_modrm.  */
#define MODRM_CHECK  if (!need_modrm) abort ()

static const char **names64;
static const char **names32;
static const char **names16;
static const char **names8;
static const char **names8rex;
static const char **names_seg;
static const char **index16;

static const char *intel_names64[] = {
  "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
};
static const char *intel_names32[] = {
  "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi",
  "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"
};
static const char *intel_names16[] = {
  "ax", "cx", "dx", "bx", "sp", "bp", "si", "di",
  "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"
};
static const char *intel_names8[] = {
  "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh",
};
static const char *intel_names8rex[] = {
  "al", "cl", "dl", "bl", "spl", "bpl", "sil", "dil",
  "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"
};
static const char *intel_names_seg[] = {
  "es", "cs", "ss", "ds", "fs", "gs", "?", "?",
};
static const char *intel_index16[] = {
  "bx+si", "bx+di", "bp+si", "bp+di", "si", "di", "bp", "bx"
};

static const char *att_names64[] = {
  "%rax", "%rcx", "%rdx", "%rbx", "%rsp", "%rbp", "%rsi", "%rdi",
  "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"
};
static const char *att_names32[] = {
  "%eax", "%ecx", "%edx", "%ebx", "%esp", "%ebp", "%esi", "%edi",
  "%r8d", "%r9d", "%r10d", "%r11d", "%r12d", "%r13d", "%r14d", "%r15d"
};
static const char *att_names16[] = {
  "%ax", "%cx", "%dx", "%bx", "%sp", "%bp", "%si", "%di",
  "%r8w", "%r9w", "%r10w", "%r11w", "%r12w", "%r13w", "%r14w", "%r15w"
};
static const char *att_names8[] = {
  "%al", "%cl", "%dl", "%bl", "%ah", "%ch", "%dh", "%bh",
};
static const char *att_names8rex[] = {
  "%al", "%cl", "%dl", "%bl", "%spl", "%bpl", "%sil", "%dil",
  "%r8b", "%r9b", "%r10b", "%r11b", "%r12b", "%r13b", "%r14b", "%r15b"
};
static const char *att_names_seg[] = {
  "%es", "%cs", "%ss", "%ds", "%fs", "%gs", "%?", "%?",
};
static const char *att_index16[] = {
  "%bx,%si", "%bx,%di", "%bp,%si", "%bp,%di", "%si", "%di", "%bp", "%bx"
};

static const struct dis386 grps[][8] = {
  /* GRP1b */
  {
    { "addA",	Eb, Ib, XX, XX },
    { "orA",	Eb, Ib, XX, XX },
    { "adcA",	Eb, Ib, XX, XX },
    { "sbbA",	Eb, Ib, XX, XX },
    { "andA",	Eb, Ib, XX, XX },
    { "subA",	Eb, Ib, XX, XX },
    { "xorA",	Eb, Ib, XX, XX },
    { "cmpA",	Eb, Ib, XX, XX }
  },
  /* GRP1S */
  {
    { "addQ",	Ev, Iv, XX, XX },
    { "orQ",	Ev, Iv, XX, XX },
    { "adcQ",	Ev, Iv, XX, XX },
    { "sbbQ",	Ev, Iv, XX, XX },
    { "andQ",	Ev, Iv, XX, XX },
    { "subQ",	Ev, Iv, XX, XX },
    { "xorQ",	Ev, Iv, XX, XX },
    { "cmpQ",	Ev, Iv, XX, XX }
  },
  /* GRP1Ss */
  {
    { "addQ",	Ev, sIb, XX, XX },
    { "orQ",	Ev, sIb, XX, XX },
    { "adcQ",	Ev, sIb, XX, XX },
    { "sbbQ",	Ev, sIb, XX, XX },
    { "andQ",	Ev, sIb, XX, XX },
    { "subQ",	Ev, sIb, XX, XX },
    { "xorQ",	Ev, sIb, XX, XX },
    { "cmpQ",	Ev, sIb, XX, XX }
  },
  /* GRP2b */
  {
    { "rolA",	Eb, Ib, XX, XX },
    { "rorA",	Eb, Ib, XX, XX },
    { "rclA",	Eb, Ib, XX, XX },
    { "rcrA",	Eb, Ib, XX, XX },
    { "shlA",	Eb, Ib, XX, XX },
    { "shrA",	Eb, Ib, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "sarA",	Eb, Ib, XX, XX },
  },
  /* GRP2S */
  {
    { "rolQ",	Ev, Ib, XX, XX },
    { "rorQ",	Ev, Ib, XX, XX },
    { "rclQ",	Ev, Ib, XX, XX },
    { "rcrQ",	Ev, Ib, XX, XX },
    { "shlQ",	Ev, Ib, XX, XX },
    { "shrQ",	Ev, Ib, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "sarQ",	Ev, Ib, XX, XX },
  },
  /* GRP2b_one */
  {
    { "rolA",	Eb, I1, XX, XX },
    { "rorA",	Eb, I1, XX, XX },
    { "rclA",	Eb, I1, XX, XX },
    { "rcrA",	Eb, I1, XX, XX },
    { "shlA",	Eb, I1, XX, XX },
    { "shrA",	Eb, I1, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "sarA",	Eb, I1, XX, XX },
  },
  /* GRP2S_one */
  {
    { "rolQ",	Ev, I1, XX, XX },
    { "rorQ",	Ev, I1, XX, XX },
    { "rclQ",	Ev, I1, XX, XX },
    { "rcrQ",	Ev, I1, XX, XX },
    { "shlQ",	Ev, I1, XX, XX },
    { "shrQ",	Ev, I1, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "sarQ",	Ev, I1, XX, XX },
  },
  /* GRP2b_cl */
  {
    { "rolA",	Eb, CL, XX, XX },
    { "rorA",	Eb, CL, XX, XX },
    { "rclA",	Eb, CL, XX, XX },
    { "rcrA",	Eb, CL, XX, XX },
    { "shlA",	Eb, CL, XX, XX },
    { "shrA",	Eb, CL, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "sarA",	Eb, CL, XX, XX },
  },
  /* GRP2S_cl */
  {
    { "rolQ",	Ev, CL, XX, XX },
    { "rorQ",	Ev, CL, XX, XX },
    { "rclQ",	Ev, CL, XX, XX },
    { "rcrQ",	Ev, CL, XX, XX },
    { "shlQ",	Ev, CL, XX, XX },
    { "shrQ",	Ev, CL, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "sarQ",	Ev, CL, XX, XX }
  },
  /* GRP3b */
  {
    { "testA",	Eb, Ib, XX, XX },
    { "(bad)",	Eb, XX, XX, XX },
    { "notA",	Eb, XX, XX, XX },
    { "negA",	Eb, XX, XX, XX },
    { "mulA",	Eb, XX, XX, XX },	/* Don't print the implicit %al register,  */
    { "imulA",	Eb, XX, XX, XX },	/* to distinguish these opcodes from other */
    { "divA",	Eb, XX, XX, XX },	/* mul/imul opcodes.  Do the same for div  */
    { "idivA",	Eb, XX, XX, XX }	/* and idiv for consistency.		   */
  },
  /* GRP3S */
  {
    { "testQ",	Ev, Iv, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "notQ",	Ev, XX, XX, XX },
    { "negQ",	Ev, XX, XX, XX },
    { "mulQ",	Ev, XX, XX, XX },	/* Don't print the implicit register.  */
    { "imulQ",	Ev, XX, XX, XX },
    { "divQ",	Ev, XX, XX, XX },
    { "idivQ",	Ev, XX, XX, XX },
  },
  /* GRP4 */
  {
    { "incA",	Eb, XX, XX, XX },
    { "decA",	Eb, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
  },
  /* GRP5 */
  {
    { "incQ",	Ev, XX, XX, XX },
    { "decQ",	Ev, XX, XX, XX },
    { "callT",	indirEv, XX, XX, XX },
    { "JcallT",	indirEp, XX, XX, XX },
    { "jmpT",	indirEv, XX, XX, XX },
    { "JjmpT",	indirEp, XX, XX, XX },
    { "pushU",	stackEv, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
  },
  /* GRP6 */
  {
    { "sldtD",	Sv, XX, XX, XX },
    { "strD",	Sv, XX, XX, XX },
    { "lldt",	Ew, XX, XX, XX },
    { "ltr",	Ew, XX, XX, XX },
    { "verr",	Ew, XX, XX, XX },
    { "verw",	Ew, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX }
  },
  /* GRP7 */
  {
    { "sgdt{Q|IQ||}", VMX_Fixup, 0, XX, XX, XX },
    { "sidt{Q|IQ||}", PNI_Fixup, 0, XX, XX, XX },
    { "lgdt{Q|Q||}",	 M, XX, XX, XX },
    { "lidt{Q|Q||}",	 SVME_Fixup, 0, XX, XX, XX },
    { "smswD",	Sv, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "lmsw",	Ew, XX, XX, XX },
    { "invlpg",	INVLPG_Fixup, w_mode, XX, XX, XX },
  },
  /* GRP8 */
  {
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "btQ",	Ev, Ib, XX, XX },
    { "btsQ",	Ev, Ib, XX, XX },
    { "btrQ",	Ev, Ib, XX, XX },
    { "btcQ",	Ev, Ib, XX, XX },
  },
  /* GRP9 */
  {
    { "(bad)",	XX, XX, XX, XX },
    { "cmpxchg8b", CMPXCHG8B_Fixup, q_mode, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "",	VM, XX, XX, XX },		/* See OP_VMX.  */
    { "vmptrst", Mq, XX, XX, XX },
  },
  /* GRP11_C6 */
  {
    { "movA",	Eb, Ib, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
  },
  /* GRP11_C7 */
  {
    { "movQ",	Ev, Iv, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
  },
  /* GRP12 */
  {
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "psrlw",	MS, Ib, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "psraw",	MS, Ib, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "psllw",	MS, Ib, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
  },
  /* GRP13 */
  {
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "psrld",	MS, Ib, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "psrad",	MS, Ib, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "pslld",	MS, Ib, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
  },
  /* GRP14 */
  {
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "psrlq",	MS, Ib, XX, XX },
    { "psrldq",	MS, Ib, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "psllq",	MS, Ib, XX, XX },
    { "pslldq",	MS, Ib, XX, XX },
  },
  /* GRP15 */
  {
    { "fxsave", Ev, XX, XX, XX },
    { "fxrstor", Ev, XX, XX, XX },
    { "ldmxcsr", Ev, XX, XX, XX },
    { "stmxcsr", Ev, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "lfence", OP_0fae, 0, XX, XX, XX },
    { "mfence", OP_0fae, 0, XX, XX, XX },
    { "clflush", OP_0fae, 0, XX, XX, XX },
  },
  /* GRP16 */
  {
    { "prefetchnta", Ev, XX, XX, XX },
    { "prefetcht0", Ev, XX, XX, XX },
    { "prefetcht1", Ev, XX, XX, XX },
    { "prefetcht2", Ev, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
  },
  /* GRPAMD */
  {
    { "prefetch", Eb, XX, XX, XX },
    { "prefetchw", Eb, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
  },
  /* GRPPADLCK1 */
  {
    { "xstore-rng", OP_0f07, 0, XX, XX, XX },
    { "xcrypt-ecb", OP_0f07, 0, XX, XX, XX },
    { "xcrypt-cbc", OP_0f07, 0, XX, XX, XX },
    { "xcrypt-ctr", OP_0f07, 0, XX, XX, XX },
    { "xcrypt-cfb", OP_0f07, 0, XX, XX, XX },
    { "xcrypt-ofb", OP_0f07, 0, XX, XX, XX },
    { "(bad)",	OP_0f07, 0, XX, XX, XX },
    { "(bad)",	OP_0f07, 0, XX, XX, XX },
  },
  /* GRPPADLCK2 */
  {
    { "montmul", OP_0f07, 0, XX, XX, XX },
    { "xsha1",   OP_0f07, 0, XX, XX, XX },
    { "xsha256", OP_0f07, 0, XX, XX, XX },
    { "(bad)",	 OP_0f07, 0, XX, XX, XX },
    { "(bad)",   OP_0f07, 0, XX, XX, XX },
    { "(bad)",   OP_0f07, 0, XX, XX, XX },
    { "(bad)",	 OP_0f07, 0, XX, XX, XX },
    { "(bad)",	 OP_0f07, 0, XX, XX, XX },
  }
};

static const struct dis386 prefix_user_table[][4] = {
  /* PREGRP0 */
  {
    { "addps", XM, EX, XX, XX },
    { "addss", XM, EX, XX, XX },
    { "addpd", XM, EX, XX, XX },
    { "addsd", XM, EX, XX, XX },
  },
  /* PREGRP1 */
  {
    { "", XM, EX, OPSIMD, XX },	/* See OP_SIMD_SUFFIX.  */
    { "", XM, EX, OPSIMD, XX },
    { "", XM, EX, OPSIMD, XX },
    { "", XM, EX, OPSIMD, XX },
  },
  /* PREGRP2 */
  {
    { "cvtpi2ps", XM, EMC, XX, XX },
    { "cvtsi2ssY", XM, Ev, XX, XX },
    { "cvtpi2pd", XM, EMC, XX, XX },
    { "cvtsi2sdY", XM, Ev, XX, XX },
  },
  /* PREGRP3 */
  {
    { "cvtps2pi", MXC, EX, XX, XX },
    { "cvtss2siY", Gv, EX, XX, XX },
    { "cvtpd2pi", MXC, EX, XX, XX },
    { "cvtsd2siY", Gv, EX, XX, XX },
  },
  /* PREGRP4 */
  {
    { "cvttps2pi", MXC, EX, XX, XX },
    { "cvttss2siY", Gv, EX, XX, XX },
    { "cvttpd2pi", MXC, EX, XX, XX },
    { "cvttsd2siY", Gv, EX, XX, XX },
  },
  /* PREGRP5 */
  {
    { "divps", XM, EX, XX, XX },
    { "divss", XM, EX, XX, XX },
    { "divpd", XM, EX, XX, XX },
    { "divsd", XM, EX, XX, XX },
  },
  /* PREGRP6 */
  {
    { "maxps", XM, EX, XX, XX },
    { "maxss", XM, EX, XX, XX },
    { "maxpd", XM, EX, XX, XX },
    { "maxsd", XM, EX, XX, XX },
  },
  /* PREGRP7 */
  {
    { "minps", XM, EX, XX, XX },
    { "minss", XM, EX, XX, XX },
    { "minpd", XM, EX, XX, XX },
    { "minsd", XM, EX, XX, XX },
  },
  /* PREGRP8 */
  {
    { "movups", XM, EX, XX, XX },
    { "movss", XM, EX, XX, XX },
    { "movupd", XM, EX, XX, XX },
    { "movsd", XM, EX, XX, XX },
  },
  /* PREGRP9 */
  {
    { "movups", EX, XM, XX, XX },
    { "movss", EX, XM, XX, XX },
    { "movupd", EX, XM, XX, XX },
    { "movsd", EX, XM, XX, XX },
  },
  /* PREGRP10 */
  {
    { "mulps", XM, EX, XX, XX },
    { "mulss", XM, EX, XX, XX },
    { "mulpd", XM, EX, XX, XX },
    { "mulsd", XM, EX, XX, XX },
  },
  /* PREGRP11 */
  {
    { "rcpps", XM, EX, XX, XX },
    { "rcpss", XM, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
  },
  /* PREGRP12 */
  {
    { "rsqrtps", XM, EX, XX, XX },
    { "rsqrtss", XM, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
  },
  /* PREGRP13 */
  {
    { "sqrtps", XM, EX, XX, XX },
    { "sqrtss", XM, EX, XX, XX },
    { "sqrtpd", XM, EX, XX, XX },
    { "sqrtsd", XM, EX, XX, XX },
  },
  /* PREGRP14 */
  {
    { "subps", XM, EX, XX, XX },
    { "subss", XM, EX, XX, XX },
    { "subpd", XM, EX, XX, XX },
    { "subsd", XM, EX, XX, XX },
  },
  /* PREGRP15 */
  {
    { "(bad)", XM, EX, XX, XX },
    { "cvtdq2pd", XM, EX, XX, XX },
    { "cvttpd2dq", XM, EX, XX, XX },
    { "cvtpd2dq", XM, EX, XX, XX },
  },
  /* PREGRP16 */
  {
    { "cvtdq2ps", XM, EX, XX, XX },
    { "cvttps2dq",XM, EX, XX, XX },
    { "cvtps2dq",XM, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
  },
  /* PREGRP17 */
  {
    { "cvtps2pd", XM, EX, XX, XX },
    { "cvtss2sd", XM, EX, XX, XX },
    { "cvtpd2ps", XM, EX, XX, XX },
    { "cvtsd2ss", XM, EX, XX, XX },
  },
  /* PREGRP18 */
  {
    { "maskmovq", MX, MS, XX, XX },
    { "(bad)", XM, EX, XX, XX },
    { "maskmovdqu", XM, XS, XX, XX },
    { "(bad)", XM, EX, XX, XX },
  },
  /* PREGRP19 */
  {
    { "movq", MX, EM, XX, XX },
    { "movdqu", XM, EX, XX, XX },
    { "movdqa", XM, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
  },
  /* PREGRP20 */
  {
    { "movq", EM, MX, XX, XX },
    { "movdqu", EX, XM, XX, XX },
    { "movdqa", EX, XM, XX, XX },
    { "(bad)", EX, XM, XX, XX },
  },
  /* PREGRP21 */
  {
    { "(bad)", EX, XM, XX, XX },
    { "movq2dq", XM, MS, XX, XX },
    { "movq", EX, XM, XX, XX },
    { "movdq2q", MX, XS, XX, XX },
  },
  /* PREGRP22 */
  {
    { "pshufw", MX, EM, Ib, XX },
    { "pshufhw", XM, EX, Ib, XX },
    { "pshufd", XM, EX, Ib, XX },
    { "pshuflw", XM, EX, Ib, XX },
  },
  /* PREGRP23 */
  {
    { "movd", Edq, MX, XX, XX },
    { "movq", XM, EX, XX, XX },
    { "movd", Edq, XM, XX, XX },
    { "(bad)", Ed, XM, XX, XX },
  },
  /* PREGRP24 */
  {
    { "(bad)", MX, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
    { "punpckhqdq", XM, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
  },
  /* PREGRP25 */
  {
    { "movntq", EM, MX, XX, XX },
    { "(bad)", EM, XM, XX, XX },
    { "movntdq", EM, XM, XX, XX },
    { "(bad)", EM, XM, XX, XX },
  },
  /* PREGRP26 */
  {
    { "(bad)", MX, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
    { "punpcklqdq", XM, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
  },
  /* PREGRP27 */
  {
    { "(bad)", MX, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
    { "addsubpd", XM, EX, XX, XX },
    { "addsubps", XM, EX, XX, XX },
  },
  /* PREGRP28 */
  {
    { "(bad)", MX, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
    { "haddpd", XM, EX, XX, XX },
    { "haddps", XM, EX, XX, XX },
  },
  /* PREGRP29 */
  {
    { "(bad)", MX, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
    { "hsubpd", XM, EX, XX, XX },
    { "hsubps", XM, EX, XX, XX },
  },
  /* PREGRP30 */
  {
    { "movlpX", XM, EX, SIMD_Fixup, 'h', XX }, /* really only 2 operands */
    { "movsldup", XM, EX, XX, XX },
    { "movlpd", XM, EX, XX, XX },
    { "movddup", XM, EX, XX, XX },
  },
  /* PREGRP31 */
  {
    { "movhpX", XM, EX, SIMD_Fixup, 'l', XX },
    { "movshdup", XM, EX, XX, XX },
    { "movhpd", XM, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
  },
  /* PREGRP32 */
  {
    { "(bad)", XM, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
    { "(bad)", XM, EX, XX, XX },
    { "lddqu", XM, M, XX, XX },
  },
  /* PREGRP33 */
  {
    {"movntps",Ev, XM, XX, XX },
    {"movntss",Ev, XM, XX, XX },
    {"movntpd",Ev, XM, XX, XX },
    {"movntsd",Ev, XM, XX, XX },
  },

  /* PREGRP34 */
  {
    {"vmread", Em, Gm, XX, XX },
    {"(bad)",  XX, XX, XX, XX },
    {"extrq",  XS, Ib, Ib, XX },
    {"insertq",XM, XS, Ib, Ib },
  },
  
 /* PREGRP35 */  
  {
    {"vmwrite", Gm, Em, XX, XX },
    {"(bad)",   XX, XX, XX, XX },
    {"extrq",   XM, XS, XX, XX },
    {"insertq", XM, XS, XX, XX },
  }, 

  /* PREGRP36 */
  {
    { "bsrS",   Gv, Ev, XX, XX },
    { "lzcntS", Gv, Ev, XX, XX },
    { "bsrS",  Gv, Ev, XX, XX },
    { "(bad)",  XX, XX, XX, XX },
  },

  /* PREGRP37 */
  {
    { "(bad)",  XX, XX, XX, XX },
    { "popcntS",Gv, Ev, XX, XX },
    { "(bad)",  XX, XX, XX, XX },
    { "(bad)",  XX, XX, XX, XX },    
  },
};

static const struct dis386 x86_64_table[][2] = {
  {
    { "pusha{P|}",	XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
  },
  {
    { "popa{P|}",	XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
  },
  {
    { "bound{S|}",	Gv, Ma, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
  },
  {
    { "arpl",		Ew, Gw, XX, XX },
    { "movs{||lq|xd}",	Gv, Ed, XX, XX },
  },
};

static const struct dis386 three_byte_table[][256] = {
  /* THREE_BYTE_0 */
  {
    /* 00 */
    { "pshufb",		MX, EM, XX, XX },
    { "phaddw",		MX, EM, XX, XX },
    { "phaddd",		MX, EM, XX, XX },
    { "phaddsw",	MX, EM, XX, XX },
    { "pmaddubsw",	MX, EM, XX, XX },
    { "phsubw",		MX, EM, XX, XX },
    { "phsubd",		MX, EM, XX, XX },
    { "phsubsw",	MX, EM, XX, XX },
    /* 08 */
    { "psignb",		MX, EM, XX, XX },
    { "psignw",		MX, EM, XX, XX },
    { "psignd",		MX, EM, XX, XX },
    { "pmulhrsw",	MX, EM, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 10 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 18 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "pabsb",		MX, EM, XX, XX },
    { "pabsw",		MX, EM, XX, XX },
    { "pabsd",		MX, EM, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 20 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 28 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 30 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 38 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 40 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 48 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 50 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 58 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 60 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 68 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 70 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 78 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 80 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 88 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 90 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 98 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* a0 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* a8 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* b0 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* b8 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* c0 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* c8 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* d0 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* d8 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* e0 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* e8 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* f0 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* f8 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX }
  },
  /* THREE_BYTE_1 */
  {
    /* 00 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 08 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "palignr",	MX, EM, Ib, XX },
    /* 10 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 18 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 20 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 28 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 30 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 38 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 40 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 48 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 50 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 58 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 60 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 68 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 70 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 78 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 80 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 88 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 90 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* 98 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* a0 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* a8 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* b0 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* b8 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* c0 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* c8 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* d0 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* d8 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* e0 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* e8 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* f0 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    /* f8 */
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX },
    { "(bad)",		XX, XX, XX, XX }
  },
};

#define INTERNAL_DISASSEMBLER_ERROR _("<internal disassembler error>")

static void
ckprefix (void)
{
  int newrex;
  rex = 0;
  prefixes = 0;
  used_prefixes = 0;
  rex_used = 0;
  while (1)
    {
      FETCH_DATA (the_info, codep + 1);
      newrex = 0;
      switch (*codep)
	{
	/* REX prefixes family.  */
	case 0x40:
	case 0x41:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x45:
	case 0x46:
	case 0x47:
	case 0x48:
	case 0x49:
	case 0x4a:
	case 0x4b:
	case 0x4c:
	case 0x4d:
	case 0x4e:
	case 0x4f:
	    if (address_mode == mode_64bit)
	      newrex = *codep;
	    else
	      return;
	  break;
	case 0xf3:
	  prefixes |= PREFIX_REPZ;
	  break;
	case 0xf2:
	  prefixes |= PREFIX_REPNZ;
	  break;
	case 0xf0:
	  prefixes |= PREFIX_LOCK;
	  break;
	case 0x2e:
	  prefixes |= PREFIX_CS;
	  break;
	case 0x36:
	  prefixes |= PREFIX_SS;
	  break;
	case 0x3e:
	  prefixes |= PREFIX_DS;
	  break;
	case 0x26:
	  prefixes |= PREFIX_ES;
	  break;
	case 0x64:
	  prefixes |= PREFIX_FS;
	  break;
	case 0x65:
	  prefixes |= PREFIX_GS;
	  break;
	case 0x66:
	  prefixes |= PREFIX_DATA;
	  break;
	case 0x67:
	  prefixes |= PREFIX_ADDR;
	  break;
	case FWAIT_OPCODE:
	  /* fwait is really an instruction.  If there are prefixes
	     before the fwait, they belong to the fwait, *not* to the
	     following instruction.  */
	  if (prefixes || rex)
	    {
	      prefixes |= PREFIX_FWAIT;
	      codep++;
	      return;
	    }
	  prefixes = PREFIX_FWAIT;
	  break;
	default:
	  return;
	}
      /* Rex is ignored when followed by another prefix.  */
      if (rex)
	{
	  rex_used = rex;
	  return;
	}
      rex = newrex;
      codep++;
    }
}

/* Return the name of the prefix byte PREF, or NULL if PREF is not a
   prefix byte.  */

static const char *
prefix_name (int pref, int sizeflag)
{
  switch (pref)
    {
    /* REX prefixes family.  */
    case 0x40:
      return "rex";
    case 0x41:
      return "rexZ";
    case 0x42:
      return "rexY";
    case 0x43:
      return "rexYZ";
    case 0x44:
      return "rexX";
    case 0x45:
      return "rexXZ";
    case 0x46:
      return "rexXY";
    case 0x47:
      return "rexXYZ";
    case 0x48:
      return "rex64";
    case 0x49:
      return "rex64Z";
    case 0x4a:
      return "rex64Y";
    case 0x4b:
      return "rex64YZ";
    case 0x4c:
      return "rex64X";
    case 0x4d:
      return "rex64XZ";
    case 0x4e:
      return "rex64XY";
    case 0x4f:
      return "rex64XYZ";
    case 0xf3:
      return "repz";
    case 0xf2:
      return "repnz";
    case 0xf0:
      return "lock";
    case 0x2e:
      return "cs";
    case 0x36:
      return "ss";
    case 0x3e:
      return "ds";
    case 0x26:
      return "es";
    case 0x64:
      return "fs";
    case 0x65:
      return "gs";
    case 0x66:
      return (sizeflag & DFLAG) ? "data16" : "data32";
    case 0x67:
      if (address_mode == mode_64bit)
	return (sizeflag & AFLAG) ? "addr32" : "addr64";
      else
	return (sizeflag & AFLAG) ? "addr16" : "addr32";
    case FWAIT_OPCODE:
      return "fwait";
    default:
      return NULL;
    }
}

static char op1out[100], op2out[100], op3out[100], op4out[100];
static int op_ad, op_index[4];
static int two_source_ops;
static bfd_vma op_address[4];
static bfd_vma op_riprel[4];
static bfd_vma start_pc;

/*
 *   On the 386's of 1988, the maximum length of an instruction is 15 bytes.
 *   (see topic "Redundant prefixes" in the "Differences from 8086"
 *   section of the "Virtual 8086 Mode" chapter.)
 * 'pc' should be the address of this instruction, it will
 *   be used to print the target address if this is a relative jump or call
 * The function returns the length of this instruction in bytes.
 */

static char intel_syntax;
static char open_char;
static char close_char;
static char separator_char;
static char scale_char;

/* Here for backwards compatibility.  When gdb stops using
   print_insn_i386_att and print_insn_i386_intel these functions can
   disappear, and print_insn_i386 be merged into print_insn.  */
int
print_insn_i386_att (bfd_vma pc, disassemble_info *info)
{
  intel_syntax = 0;

  return print_insn (pc, info);
}

int
print_insn_i386_intel (bfd_vma pc, disassemble_info *info)
{
  intel_syntax = 1;

  return print_insn (pc, info);
}

int
print_insn_i386 (bfd_vma pc, disassemble_info *info)
{
  intel_syntax = -1;

  return print_insn (pc, info);
}

static int
print_insn (bfd_vma pc, disassemble_info *info)
{
  const struct dis386 *dp;
  int i;
  char *first, *second, *third, *fourth;
  int needcomma;
  unsigned char uses_DATA_prefix, uses_LOCK_prefix;
  unsigned char uses_REPNZ_prefix, uses_REPZ_prefix;
  int sizeflag;
  const char *p;
  struct dis_private priv;
  unsigned char op;

  if (info->mach == bfd_mach_x86_64_intel_syntax
      || info->mach == bfd_mach_x86_64)
    address_mode = mode_64bit;
  else
    address_mode = mode_32bit;

  if (intel_syntax == (char) -1)
    intel_syntax = (info->mach == bfd_mach_i386_i386_intel_syntax
		    || info->mach == bfd_mach_x86_64_intel_syntax);

  if (info->mach == bfd_mach_i386_i386
      || info->mach == bfd_mach_x86_64
      || info->mach == bfd_mach_i386_i386_intel_syntax
      || info->mach == bfd_mach_x86_64_intel_syntax)
    priv.orig_sizeflag = AFLAG | DFLAG;
  else if (info->mach == bfd_mach_i386_i8086)
    priv.orig_sizeflag = 0;
  else
    abort ();

  for (p = info->disassembler_options; p != NULL; )
    {
      if (CONST_STRNEQ (p, "x86-64"))
	{
	  address_mode = mode_64bit;
	  priv.orig_sizeflag = AFLAG | DFLAG;
	}
      else if (CONST_STRNEQ (p, "i386"))
	{
	  address_mode = mode_32bit;
	  priv.orig_sizeflag = AFLAG | DFLAG;
	}
      else if (CONST_STRNEQ (p, "i8086"))
	{
	  address_mode = mode_16bit;
	  priv.orig_sizeflag = 0;
	}
      else if (CONST_STRNEQ (p, "intel"))
	{
	  intel_syntax = 1;
	}
      else if (CONST_STRNEQ (p, "att"))
	{
	  intel_syntax = 0;
	}
      else if (CONST_STRNEQ (p, "addr"))
	{
	  if (p[4] == '1' && p[5] == '6')
	    priv.orig_sizeflag &= ~AFLAG;
	  else if (p[4] == '3' && p[5] == '2')
	    priv.orig_sizeflag |= AFLAG;
	}
      else if (CONST_STRNEQ (p, "data"))
	{
	  if (p[4] == '1' && p[5] == '6')
	    priv.orig_sizeflag &= ~DFLAG;
	  else if (p[4] == '3' && p[5] == '2')
	    priv.orig_sizeflag |= DFLAG;
	}
      else if (CONST_STRNEQ (p, "suffix"))
	priv.orig_sizeflag |= SUFFIX_ALWAYS;

      p = strchr (p, ',');
      if (p != NULL)
	p++;
    }

  if (intel_syntax)
    {
      names64 = intel_names64;
      names32 = intel_names32;
      names16 = intel_names16;
      names8 = intel_names8;
      names8rex = intel_names8rex;
      names_seg = intel_names_seg;
      index16 = intel_index16;
      open_char = '[';
      close_char = ']';
      separator_char = '+';
      scale_char = '*';
    }
  else
    {
      names64 = att_names64;
      names32 = att_names32;
      names16 = att_names16;
      names8 = att_names8;
      names8rex = att_names8rex;
      names_seg = att_names_seg;
      index16 = att_index16;
      open_char = '(';
      close_char =  ')';
      separator_char = ',';
      scale_char = ',';
    }

  /* The output looks better if we put 7 bytes on a line, since that
     puts most long word instructions on a single line.  */
  info->bytes_per_line = 7;

  info->private_data = &priv;
  priv.max_fetched = priv.the_buffer;
  priv.insn_start = pc;

  obuf[0] = 0;
  op1out[0] = 0;
  op2out[0] = 0;
  op3out[0] = 0;
  op4out[0] = 0;

  op_index[0] = op_index[1] = op_index[2] = op_index[3] = -1;

  the_info = info;
  start_pc = pc;
  start_codep = priv.the_buffer;
  codep = priv.the_buffer;

  if (setjmp (priv.bailout) != 0)
    {
      const char *name;

      /* Getting here means we tried for data but didn't get it.  That
	 means we have an incomplete instruction of some sort.  Just
	 print the first byte as a prefix or a .byte pseudo-op.  */
      if (codep > priv.the_buffer)
	{
	  name = prefix_name (priv.the_buffer[0], priv.orig_sizeflag);
	  if (name != NULL)
	    (*info->fprintf_func) (info->stream, "%s", name);
	  else
	    {
	      /* Just print the first byte as a .byte instruction.  */
	      (*info->fprintf_func) (info->stream, ".byte 0x%x",
				     (unsigned int) priv.the_buffer[0]);
	    }

	  return 1;
	}

      return -1;
    }

  obufp = obuf;
  ckprefix ();

  insn_codep = codep;
  sizeflag = priv.orig_sizeflag;

  FETCH_DATA (info, codep + 1);
  two_source_ops = (*codep == 0x62) || (*codep == 0xc8);

  if (((prefixes & PREFIX_FWAIT)
       && ((*codep < 0xd8) || (*codep > 0xdf)))
      || (rex && rex_used))
    {
      const char *name;

      /* fwait not followed by floating point instruction, or rex followed
	 by other prefixes.  Print the first prefix.  */
      name = prefix_name (priv.the_buffer[0], priv.orig_sizeflag);
      if (name == NULL)
	name = INTERNAL_DISASSEMBLER_ERROR;
      (*info->fprintf_func) (info->stream, "%s", name);
      return 1;
    }

  op = 0;
  if (*codep == 0x0f)
    {
      unsigned char threebyte;
      FETCH_DATA (info, codep + 2);
      threebyte = *++codep;
      dp = &dis386_twobyte[threebyte];
      need_modrm = twobyte_has_modrm[*codep];
      uses_DATA_prefix = twobyte_uses_DATA_prefix[*codep];
      uses_REPNZ_prefix = twobyte_uses_REPNZ_prefix[*codep];
      uses_REPZ_prefix = twobyte_uses_REPZ_prefix[*codep];
      uses_LOCK_prefix = (*codep & ~0x02) == 0x20;
      codep++;
      if (dp->name == NULL && dp->bytemode1 == IS_3BYTE_OPCODE)
	{
	  FETCH_DATA (info, codep + 2);
	  op = *codep++;
	  switch (threebyte)
	    {
	    case 0x38:
	      uses_DATA_prefix = threebyte_0x38_uses_DATA_prefix[op];
	      uses_REPNZ_prefix = threebyte_0x38_uses_REPNZ_prefix[op];
	      uses_REPZ_prefix = threebyte_0x38_uses_REPZ_prefix[op];
	      break;
	    case 0x3a:
	      uses_DATA_prefix = threebyte_0x3a_uses_DATA_prefix[op];
	      uses_REPNZ_prefix = threebyte_0x3a_uses_REPNZ_prefix[op];
	      uses_REPZ_prefix = threebyte_0x3a_uses_REPZ_prefix[op];
	      break;
	    default:
	      break;
	    }
	}
    }
  else
    {
      dp = &dis386[*codep];
      need_modrm = onebyte_has_modrm[*codep];
      uses_DATA_prefix = 0;
      uses_REPNZ_prefix = 0;
      uses_REPZ_prefix = 0;
      uses_LOCK_prefix = 0;
      codep++;
    }
  
  if (!uses_REPZ_prefix && (prefixes & PREFIX_REPZ))
    {
      oappend ("repz ");
      used_prefixes |= PREFIX_REPZ;
    }
  if (!uses_REPNZ_prefix && (prefixes & PREFIX_REPNZ))
    {
      oappend ("repnz ");
      used_prefixes |= PREFIX_REPNZ;
    }

  if (!uses_LOCK_prefix && (prefixes & PREFIX_LOCK))
    {
      oappend ("lock ");
      used_prefixes |= PREFIX_LOCK;
    }

  if (prefixes & PREFIX_ADDR)
    {
      sizeflag ^= AFLAG;
      if (dp->bytemode3 != loop_jcxz_mode || intel_syntax)
	{
	  if ((sizeflag & AFLAG) || address_mode == mode_64bit)
	    oappend ("addr32 ");
	  else
	    oappend ("addr16 ");
	  used_prefixes |= PREFIX_ADDR;
	}
    }

  if (!uses_DATA_prefix && (prefixes & PREFIX_DATA))
    {
      sizeflag ^= DFLAG;
      if (dp->bytemode3 == cond_jump_mode
	  && dp->bytemode1 == v_mode
	  && !intel_syntax)
	{
	  if (sizeflag & DFLAG)
	    oappend ("data32 ");
	  else
	    oappend ("data16 ");
	  used_prefixes |= PREFIX_DATA;
	}
    }

  if (dp->name == NULL && dp->bytemode1 == IS_3BYTE_OPCODE)
    {
      dp = &three_byte_table[dp->bytemode2][op];
      mod = (*codep >> 6) & 3;
      reg = (*codep >> 3) & 7;
      rm = *codep & 7;
    }
  else if (need_modrm)
    {
      FETCH_DATA (info, codep + 1);
      mod = (*codep >> 6) & 3;
      reg = (*codep >> 3) & 7;
      rm = *codep & 7;
    }

  if (dp->name == NULL && dp->bytemode1 == FLOATCODE)
    {
      dofloat (sizeflag);
    }
  else
    {
      int index;
      if (dp->name == NULL)
	{
	  switch (dp->bytemode1)
	    {
	    case USE_GROUPS:
	      dp = &grps[dp->bytemode2][reg];
	      break;

	    case USE_PREFIX_USER_TABLE:
	      index = 0;
	      used_prefixes |= (prefixes & PREFIX_REPZ);
	      if (prefixes & PREFIX_REPZ)
		index = 1;
	      else
		{
		  /* We should check PREFIX_REPNZ and PREFIX_REPZ
		     before PREFIX_DATA.  */
		  used_prefixes |= (prefixes & PREFIX_REPNZ);
		  if (prefixes & PREFIX_REPNZ)
		    index = 3;
		  else
		    {
		      used_prefixes |= (prefixes & PREFIX_DATA);
		      if (prefixes & PREFIX_DATA)
			index = 2;
		    }
		}
	      dp = &prefix_user_table[dp->bytemode2][index];
	      break;

	    case X86_64_SPECIAL:
	      index = address_mode == mode_64bit ? 1 : 0;
	      dp = &x86_64_table[dp->bytemode2][index];
	      break;

	    default:
	      oappend (INTERNAL_DISASSEMBLER_ERROR);
	      break;
	    }
	}

      if (putop (dp->name, sizeflag) == 0)
	{
	  obufp = op1out;
	  op_ad = 3;
	  if (dp->op1)
	    (*dp->op1) (dp->bytemode1, sizeflag);

	  obufp = op2out;
	  op_ad = 2;
	  if (dp->op2)
	    (*dp->op2) (dp->bytemode2, sizeflag);

	  obufp = op3out;
	  op_ad = 1;
	  if (dp->op3)
	    (*dp->op3) (dp->bytemode3, sizeflag);

	  obufp = op4out;
	  op_ad = 0;
	  if (dp->op4)
	    (*dp->op4) (dp->bytemode4, sizeflag);
	}
    }

  /* See if any prefixes were not used.  If so, print the first one
     separately.  If we don't do this, we'll wind up printing an
     instruction stream which does not precisely correspond to the
     bytes we are disassembling.  */
  if ((prefixes & ~used_prefixes) != 0)
    {
      const char *name;

      name = prefix_name (priv.the_buffer[0], priv.orig_sizeflag);
      if (name == NULL)
	name = INTERNAL_DISASSEMBLER_ERROR;
      (*info->fprintf_func) (info->stream, "%s", name);
      return 1;
    }
  if (rex & ~rex_used)
    {
      const char *name;
      name = prefix_name (rex | 0x40, priv.orig_sizeflag);
      if (name == NULL)
	name = INTERNAL_DISASSEMBLER_ERROR;
      (*info->fprintf_func) (info->stream, "%s ", name);
    }

  obufp = obuf + strlen (obuf);
  for (i = strlen (obuf); i < 6; i++)
    oappend (" ");
  oappend (" ");
  (*info->fprintf_func) (info->stream, "%s", obuf);

  /* The enter and bound instructions are printed with operands in the same
     order as the intel book; everything else is printed in reverse order.  */
  if (intel_syntax || two_source_ops)
    {
      first = op1out;
      second = op2out;
      third = op3out;
      fourth = op4out;
      op_ad = op_index[0];
      op_index[0] = op_index[3];
      op_index[3] = op_ad;
      op_ad = op_index[1];
      op_index[1] = op_index[2];
      op_index[2] = op_ad;

    }
  else
    {
      first = op4out;
      second = op3out;
      third = op2out;
      fourth = op1out;
    }
  needcomma = 0;
  if (*first)
    {
      if (op_index[0] != -1 && !op_riprel[0])
	(*info->print_address_func) ((bfd_vma) op_address[op_index[0]], info);
      else
	(*info->fprintf_func) (info->stream, "%s", first);
      needcomma = 1;
    }
  
  if (*second)
    {
      if (needcomma)
	(*info->fprintf_func) (info->stream, ",");
      if (op_index[1] != -1 && !op_riprel[1])
	(*info->print_address_func) ((bfd_vma) op_address[op_index[1]], info);
      else
	(*info->fprintf_func) (info->stream, "%s", second);
      needcomma = 1;
    }

  if (*third)
    {
      if (needcomma)
	(*info->fprintf_func) (info->stream, ",");
      if (op_index[2] != -1 && !op_riprel[2])
	(*info->print_address_func) ((bfd_vma) op_address[op_index[2]], info);
      else
	(*info->fprintf_func) (info->stream, "%s", third);
      needcomma = 1;
    }

  if (*fourth)
    {
      if (needcomma)
	(*info->fprintf_func) (info->stream, ",");
      if (op_index[3] != -1 && !op_riprel[3])
	(*info->print_address_func) ((bfd_vma) op_address[op_index[3]], info);
      else
	(*info->fprintf_func) (info->stream, "%s", fourth);
    }

  for (i = 0; i < 4; i++)
    if (op_index[i] != -1 && op_riprel[i])
      {
	(*info->fprintf_func) (info->stream, "        # ");
	(*info->print_address_func) ((bfd_vma) (start_pc + codep - start_codep
						+ op_address[op_index[i]]), info);
      }
  return codep - priv.the_buffer;
}

static const char *float_mem[] = {
  /* d8 */
  "fadd{s||s|}",
  "fmul{s||s|}",
  "fcom{s||s|}",
  "fcomp{s||s|}",
  "fsub{s||s|}",
  "fsubr{s||s|}",
  "fdiv{s||s|}",
  "fdivr{s||s|}",
  /* d9 */
  "fld{s||s|}",
  "(bad)",
  "fst{s||s|}",
  "fstp{s||s|}",
  "fldenvIC",
  "fldcw",
  "fNstenvIC",
  "fNstcw",
  /* da */
  "fiadd{l||l|}",
  "fimul{l||l|}",
  "ficom{l||l|}",
  "ficomp{l||l|}",
  "fisub{l||l|}",
  "fisubr{l||l|}",
  "fidiv{l||l|}",
  "fidivr{l||l|}",
  /* db */
  "fild{l||l|}",
  "fisttp{l||l|}",
  "fist{l||l|}",
  "fistp{l||l|}",
  "(bad)",
  "fld{t||t|}",
  "(bad)",
  "fstp{t||t|}",
  /* dc */
  "fadd{l||l|}",
  "fmul{l||l|}",
  "fcom{l||l|}",
  "fcomp{l||l|}",
  "fsub{l||l|}",
  "fsubr{l||l|}",
  "fdiv{l||l|}",
  "fdivr{l||l|}",
  /* dd */
  "fld{l||l|}",
  "fisttp{ll||ll|}",
  "fst{l||l|}",
  "fstp{l||l|}",
  "frstorIC",
  "(bad)",
  "fNsaveIC",
  "fNstsw",
  /* de */
  "fiadd",
  "fimul",
  "ficom",
  "ficomp",
  "fisub",
  "fisubr",
  "fidiv",
  "fidivr",
  /* df */
  "fild",
  "fisttp",
  "fist",
  "fistp",
  "fbld",
  "fild{ll||ll|}",
  "fbstp",
  "fistp{ll||ll|}",
};

static const unsigned char float_mem_mode[] = {
  /* d8 */
  d_mode,
  d_mode,
  d_mode,
  d_mode,
  d_mode,
  d_mode,
  d_mode,
  d_mode,
  /* d9 */
  d_mode,
  0,
  d_mode,
  d_mode,
  0,
  w_mode,
  0,
  w_mode,
  /* da */
  d_mode,
  d_mode,
  d_mode,
  d_mode,
  d_mode,
  d_mode,
  d_mode,
  d_mode,
  /* db */
  d_mode,
  d_mode,
  d_mode,
  d_mode,
  0,
  t_mode,
  0,
  t_mode,
  /* dc */
  q_mode,
  q_mode,
  q_mode,
  q_mode,
  q_mode,
  q_mode,
  q_mode,
  q_mode,
  /* dd */
  q_mode,
  q_mode,
  q_mode,
  q_mode,
  0,
  0,
  0,
  w_mode,
  /* de */
  w_mode,
  w_mode,
  w_mode,
  w_mode,
  w_mode,
  w_mode,
  w_mode,
  w_mode,
  /* df */
  w_mode,
  w_mode,
  w_mode,
  w_mode,
  t_mode,
  q_mode,
  t_mode,
  q_mode
};

#define ST OP_ST, 0
#define STi OP_STi, 0

#define FGRPd9_2 NULL, NULL, 0, NULL, 0, NULL, 0, NULL, 0
#define FGRPd9_4 NULL, NULL, 1, NULL, 0, NULL, 0, NULL, 0
#define FGRPd9_5 NULL, NULL, 2, NULL, 0, NULL, 0, NULL, 0
#define FGRPd9_6 NULL, NULL, 3, NULL, 0, NULL, 0, NULL, 0
#define FGRPd9_7 NULL, NULL, 4, NULL, 0, NULL, 0, NULL, 0
#define FGRPda_5 NULL, NULL, 5, NULL, 0, NULL, 0, NULL, 0
#define FGRPdb_4 NULL, NULL, 6, NULL, 0, NULL, 0, NULL, 0
#define FGRPde_3 NULL, NULL, 7, NULL, 0, NULL, 0, NULL, 0
#define FGRPdf_4 NULL, NULL, 8, NULL, 0, NULL, 0, NULL, 0

static const struct dis386 float_reg[][8] = {
  /* d8 */
  {
    { "fadd",	ST, STi, XX, XX },
    { "fmul",	ST, STi, XX, XX },
    { "fcom",	STi, XX, XX, XX },
    { "fcomp",	STi, XX, XX, XX },
    { "fsub",	ST, STi, XX, XX },
    { "fsubr",	ST, STi, XX, XX },
    { "fdiv",	ST, STi, XX, XX },
    { "fdivr",	ST, STi, XX, XX },
  },
  /* d9 */
  {
    { "fld",	STi, XX, XX, XX },
    { "fxch",	STi, XX, XX, XX },
    { FGRPd9_2 },
    { "(bad)",	XX, XX, XX, XX },
    { FGRPd9_4 },
    { FGRPd9_5 },
    { FGRPd9_6 },
    { FGRPd9_7 },
  },
  /* da */
  {
    { "fcmovb",	ST, STi, XX, XX },
    { "fcmove",	ST, STi, XX, XX },
    { "fcmovbe",ST, STi, XX, XX },
    { "fcmovu",	ST, STi, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { FGRPda_5 },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
  },
  /* db */
  {
    { "fcmovnb",ST, STi, XX, XX },
    { "fcmovne",ST, STi, XX, XX },
    { "fcmovnbe",ST, STi, XX, XX },
    { "fcmovnu",ST, STi, XX, XX },
    { FGRPdb_4 },
    { "fucomi",	ST, STi, XX, XX },
    { "fcomi",	ST, STi, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
  },
  /* dc */
  {
    { "fadd",	STi, ST, XX, XX },
    { "fmul",	STi, ST, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
#if UNIXWARE_COMPAT
    { "fsub",	STi, ST, XX, XX },
    { "fsubr",	STi, ST, XX, XX },
    { "fdiv",	STi, ST, XX, XX },
    { "fdivr",	STi, ST, XX, XX },
#else
    { "fsubr",	STi, ST, XX, XX },
    { "fsub",	STi, ST, XX, XX },
    { "fdivr",	STi, ST, XX, XX },
    { "fdiv",	STi, ST, XX, XX },
#endif
  },
  /* dd */
  {
    { "ffree",	STi, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "fst",	STi, XX, XX, XX },
    { "fstp",	STi, XX, XX, XX },
    { "fucom",	STi, XX, XX, XX },
    { "fucomp",	STi, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
  },
  /* de */
  {
    { "faddp",	STi, ST, XX, XX },
    { "fmulp",	STi, ST, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { FGRPde_3 },
#if UNIXWARE_COMPAT
    { "fsubp",	STi, ST, XX, XX },
    { "fsubrp",	STi, ST, XX, XX },
    { "fdivp",	STi, ST, XX, XX },
    { "fdivrp",	STi, ST, XX, XX },
#else
    { "fsubrp",	STi, ST, XX, XX },
    { "fsubp",	STi, ST, XX, XX },
    { "fdivrp",	STi, ST, XX, XX },
    { "fdivp",	STi, ST, XX, XX },
#endif
  },
  /* df */
  {
    { "ffreep",	STi, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
    { FGRPdf_4 },
    { "fucomip",ST, STi, XX, XX },
    { "fcomip", ST, STi, XX, XX },
    { "(bad)",	XX, XX, XX, XX },
  },
};

static char *fgrps[][8] = {
  /* d9_2  0 */
  {
    "fnop","(bad)","(bad)","(bad)","(bad)","(bad)","(bad)","(bad)",
  },

  /* d9_4  1 */
  {
    "fchs","fabs","(bad)","(bad)","ftst","fxam","(bad)","(bad)",
  },

  /* d9_5  2 */
  {
    "fld1","fldl2t","fldl2e","fldpi","fldlg2","fldln2","fldz","(bad)",
  },

  /* d9_6  3 */
  {
    "f2xm1","fyl2x","fptan","fpatan","fxtract","fprem1","fdecstp","fincstp",
  },

  /* d9_7  4 */
  {
    "fprem","fyl2xp1","fsqrt","fsincos","frndint","fscale","fsin","fcos",
  },

  /* da_5  5 */
  {
    "(bad)","fucompp","(bad)","(bad)","(bad)","(bad)","(bad)","(bad)",
  },

  /* db_4  6 */
  {
    "feni(287 only)","fdisi(287 only)","fNclex","fNinit",
    "fNsetpm(287 only)","(bad)","(bad)","(bad)",
  },

  /* de_3  7 */
  {
    "(bad)","fcompp","(bad)","(bad)","(bad)","(bad)","(bad)","(bad)",
  },

  /* df_4  8 */
  {
    "fNstsw","(bad)","(bad)","(bad)","(bad)","(bad)","(bad)","(bad)",
  },
};

static void
dofloat (int sizeflag)
{
  const struct dis386 *dp;
  unsigned char floatop;

  floatop = codep[-1];

  if (mod != 3)
    {
      int fp_indx = (floatop - 0xd8) * 8 + reg;

      putop (float_mem[fp_indx], sizeflag);
      obufp = op1out;
      op_ad = 2;
      OP_E (float_mem_mode[fp_indx], sizeflag);
      return;
    }
  /* Skip mod/rm byte.  */
  MODRM_CHECK;
  codep++;

  dp = &float_reg[floatop - 0xd8][reg];
  if (dp->name == NULL)
    {
      putop (fgrps[dp->bytemode1][rm], sizeflag);

      /* Instruction fnstsw is only one with strange arg.  */
      if (floatop == 0xdf && codep[-1] == 0xe0)
	strcpy (op1out, names16[0]);
    }
  else
    {
      putop (dp->name, sizeflag);

      obufp = op1out;
      op_ad = 2;
      if (dp->op1)
	(*dp->op1) (dp->bytemode1, sizeflag);

      obufp = op2out;
      op_ad = 1;
      if (dp->op2)
	(*dp->op2) (dp->bytemode2, sizeflag);
    }
}

static void
OP_ST (int bytemode ATTRIBUTE_UNUSED, int sizeflag ATTRIBUTE_UNUSED)
{
  oappend ("%st" + intel_syntax);
}

static void
OP_STi (int bytemode ATTRIBUTE_UNUSED, int sizeflag ATTRIBUTE_UNUSED)
{
  sprintf (scratchbuf, "%%st(%d)", rm);
  oappend (scratchbuf + intel_syntax);
}

/* Capital letters in template are macros.  */
static int
putop (const char *template, int sizeflag)
{
  const char *p;
  int alt = 0;

  for (p = template; *p; p++)
    {
      switch (*p)
	{
	default:
	  *obufp++ = *p;
	  break;
	case '{':
	  alt = 0;
	  if (intel_syntax)
	    alt += 1;
	  if (address_mode == mode_64bit)
	    alt += 2;
	  while (alt != 0)
	    {
	      while (*++p != '|')
		{
		  if (*p == '}')
		    {
		      /* Alternative not valid.  */
		      strcpy (obuf, "(bad)");
		      obufp = obuf + 5;
		      return 1;
		    }
		  else if (*p == '\0')
		    abort ();
		}
	      alt--;
	    }
	  /* Fall through.  */
	case 'I':
	  alt = 1;
	  continue;
	case '|':
	  while (*++p != '}')
	    {
	      if (*p == '\0')
		abort ();
	    }
	  break;
	case '}':
	  break;
	case 'A':
	  if (intel_syntax)
	    break;
	  if (mod != 3 || (sizeflag & SUFFIX_ALWAYS))
	    *obufp++ = 'b';
	  break;
	case 'B':
	  if (intel_syntax)
	    break;
	  if (sizeflag & SUFFIX_ALWAYS)
	    *obufp++ = 'b';
	  break;
	case 'C':
	  if (intel_syntax && !alt)
	    break;
	  if ((prefixes & PREFIX_DATA) || (sizeflag & SUFFIX_ALWAYS))
	    {
	      if (sizeflag & DFLAG)
		*obufp++ = intel_syntax ? 'd' : 'l';
	      else
		*obufp++ = intel_syntax ? 'w' : 's';
	      used_prefixes |= (prefixes & PREFIX_DATA);
	    }
	  break;
	case 'D':
	  if (intel_syntax || !(sizeflag & SUFFIX_ALWAYS))
	    break;
	  USED_REX (REX_MODE64);
	  if (mod == 3)
	    {
	      if (rex & REX_MODE64)
		*obufp++ = 'q';
	      else if (sizeflag & DFLAG)
		*obufp++ = intel_syntax ? 'd' : 'l';
	      else
		*obufp++ = 'w';
	      used_prefixes |= (prefixes & PREFIX_DATA);
	    }
	  else
	    *obufp++ = 'w';
	  break;
	case 'E':		/* For jcxz/jecxz */
	  if (address_mode == mode_64bit)
	    {
	      if (sizeflag & AFLAG)
		*obufp++ = 'r';
	      else
		*obufp++ = 'e';
	    }
	  else
	    if (sizeflag & AFLAG)
	      *obufp++ = 'e';
	  used_prefixes |= (prefixes & PREFIX_ADDR);
	  break;
	case 'F':
	  if (intel_syntax)
	    break;
	  if ((prefixes & PREFIX_ADDR) || (sizeflag & SUFFIX_ALWAYS))
	    {
	      if (sizeflag & AFLAG)
		*obufp++ = address_mode == mode_64bit ? 'q' : 'l';
	      else
		*obufp++ = address_mode == mode_64bit ? 'l' : 'w';
	      used_prefixes |= (prefixes & PREFIX_ADDR);
	    }
	  break;
	case 'G':
	  if (intel_syntax || (obufp[-1] != 's' && !(sizeflag & SUFFIX_ALWAYS)))
	    break;
	  if ((rex & REX_MODE64) || (sizeflag & DFLAG))
	    *obufp++ = 'l';
	  else
	    *obufp++ = 'w';
	  if (!(rex & REX_MODE64))
	    used_prefixes |= (prefixes & PREFIX_DATA);
	  break;
	case 'H':
	  if (intel_syntax)
	    break;
	  if ((prefixes & (PREFIX_CS | PREFIX_DS)) == PREFIX_CS
	      || (prefixes & (PREFIX_CS | PREFIX_DS)) == PREFIX_DS)
	    {
	      used_prefixes |= prefixes & (PREFIX_CS | PREFIX_DS);
	      *obufp++ = ',';
	      *obufp++ = 'p';
	      if (prefixes & PREFIX_DS)
		*obufp++ = 't';
	      else
		*obufp++ = 'n';
	    }
	  break;
	case 'J':
	  if (intel_syntax)
	    break;
	  *obufp++ = 'l';
	  break;
	case 'Z':
	  if (intel_syntax)
	    break;
	  if (address_mode == mode_64bit && (sizeflag & SUFFIX_ALWAYS))
	    {
	      *obufp++ = 'q';
	      break;
	    }
	  /* Fall through.  */
	case 'L':
	  if (intel_syntax)
	    break;
	  if (sizeflag & SUFFIX_ALWAYS)
	    *obufp++ = 'l';
	  break;
	case 'N':
	  if ((prefixes & PREFIX_FWAIT) == 0)
	    *obufp++ = 'n';
	  else
	    used_prefixes |= PREFIX_FWAIT;
	  break;
	case 'O':
	  USED_REX (REX_MODE64);
	  if (rex & REX_MODE64)
	    *obufp++ = 'o';
	  else if (intel_syntax && (sizeflag & DFLAG))
	    *obufp++ = 'q';
	  else
	    *obufp++ = 'd';
	  if (!(rex & REX_MODE64))
	    used_prefixes |= (prefixes & PREFIX_DATA);
	  break;
	case 'T':
	  if (intel_syntax)
	    break;
	  if (address_mode == mode_64bit && (sizeflag & DFLAG))
	    {
	      *obufp++ = 'q';
	      break;
	    }
	  /* Fall through.  */
	case 'P':
	  if (intel_syntax)
	    break;
	  if ((prefixes & PREFIX_DATA)
	      || (rex & REX_MODE64)
	      || (sizeflag & SUFFIX_ALWAYS))
	    {
	      USED_REX (REX_MODE64);
	      if (rex & REX_MODE64)
		*obufp++ = 'q';
	      else
		{
		   if (sizeflag & DFLAG)
		      *obufp++ = 'l';
		   else
		     *obufp++ = 'w';
		}
	      used_prefixes |= (prefixes & PREFIX_DATA);
	    }
	  break;
	case 'U':
	  if (intel_syntax)
	    break;
	  if (address_mode == mode_64bit && (sizeflag & DFLAG))
	    {
	      if (mod != 3 || (sizeflag & SUFFIX_ALWAYS))
		*obufp++ = 'q';
	      break;
	    }
	  /* Fall through.  */
	case 'Q':
	  if (intel_syntax && !alt)
	    break;
	  USED_REX (REX_MODE64);
	  if (mod != 3 || (sizeflag & SUFFIX_ALWAYS))
	    {
	      if (rex & REX_MODE64)
		*obufp++ = 'q';
	      else
		{
		  if (sizeflag & DFLAG)
		    *obufp++ = intel_syntax ? 'd' : 'l';
		  else
		    *obufp++ = 'w';
		}
	      used_prefixes |= (prefixes & PREFIX_DATA);
	    }
	  break;
	case 'R':
	  USED_REX (REX_MODE64);
	  if (rex & REX_MODE64)
	    *obufp++ = 'q';
	  else if (sizeflag & DFLAG)
	    {
	      if (intel_syntax)
		  *obufp++ = 'd';
	      else
		  *obufp++ = 'l';
	    }
	  else
	    *obufp++ = 'w';
	  if (intel_syntax && !p[1]
	      && ((rex & REX_MODE64) || (sizeflag & DFLAG)))
	    *obufp++ = 'e';
	  if (!(rex & REX_MODE64))
	    used_prefixes |= (prefixes & PREFIX_DATA);
	  break;
	case 'V':
	  if (intel_syntax)
	    break;
	  if (address_mode == mode_64bit && (sizeflag & DFLAG))
	    {
	      if (sizeflag & SUFFIX_ALWAYS)
		*obufp++ = 'q';
	      break;
	    }
	  /* Fall through.  */
	case 'S':
	  if (intel_syntax)
	    break;
	  if (sizeflag & SUFFIX_ALWAYS)
	    {
	      if (rex & REX_MODE64)
		*obufp++ = 'q';
	      else
		{
		  if (sizeflag & DFLAG)
		    *obufp++ = 'l';
		  else
		    *obufp++ = 'w';
		  used_prefixes |= (prefixes & PREFIX_DATA);
		}
	    }
	  break;
	case 'X':
	  if (prefixes & PREFIX_DATA)
	    *obufp++ = 'd';
	  else
	    *obufp++ = 's';
	  used_prefixes |= (prefixes & PREFIX_DATA);
	  break;
	case 'Y':
	  if (intel_syntax)
	    break;
	  if (rex & REX_MODE64)
	    {
	      USED_REX (REX_MODE64);
	      *obufp++ = 'q';
	    }
	  break;
	  /* implicit operand size 'l' for i386 or 'q' for x86-64 */
	case 'W':
	  /* operand size flag for cwtl, cbtw */
	  USED_REX (REX_MODE64);
	  if (rex & REX_MODE64)
	    {
	      if (intel_syntax)
		*obufp++ = 'd';
	      else
		*obufp++ = 'l';
	    }
	  else if (sizeflag & DFLAG)
	    *obufp++ = 'w';
	  else
	    *obufp++ = 'b';
	  if (!(rex & REX_MODE64))
	    used_prefixes |= (prefixes & PREFIX_DATA);
	  break;
	}
      alt = 0;
    }
  *obufp = 0;
  return 0;
}

static void
oappend (const char *s)
{
  strcpy (obufp, s);
  obufp += strlen (s);
}

static void
append_seg (void)
{
  if (prefixes & PREFIX_CS)
    {
      used_prefixes |= PREFIX_CS;
      oappend ("%cs:" + intel_syntax);
    }
  if (prefixes & PREFIX_DS)
    {
      used_prefixes |= PREFIX_DS;
      oappend ("%ds:" + intel_syntax);
    }
  if (prefixes & PREFIX_SS)
    {
      used_prefixes |= PREFIX_SS;
      oappend ("%ss:" + intel_syntax);
    }
  if (prefixes & PREFIX_ES)
    {
      used_prefixes |= PREFIX_ES;
      oappend ("%es:" + intel_syntax);
    }
  if (prefixes & PREFIX_FS)
    {
      used_prefixes |= PREFIX_FS;
      oappend ("%fs:" + intel_syntax);
    }
  if (prefixes & PREFIX_GS)
    {
      used_prefixes |= PREFIX_GS;
      oappend ("%gs:" + intel_syntax);
    }
}

static void
OP_indirE (int bytemode, int sizeflag)
{
  if (!intel_syntax)
    oappend ("*");
  OP_E (bytemode, sizeflag);
}

static void
print_operand_value (char *buf, int hex, bfd_vma disp)
{
  if (address_mode == mode_64bit)
    {
      if (hex)
	{
	  char tmp[30];
	  int i;
	  buf[0] = '0';
	  buf[1] = 'x';
	  sprintf_vma (tmp, disp);
	  for (i = 0; tmp[i] == '0' && tmp[i + 1]; i++);
	  strcpy (buf + 2, tmp + i);
	}
      else
	{
	  bfd_signed_vma v = disp;
	  char tmp[30];
	  int i;
	  if (v < 0)
	    {
	      *(buf++) = '-';
	      v = -disp;
	      /* Check for possible overflow on 0x8000000000000000.  */
	      if (v < 0)
		{
		  strcpy (buf, "9223372036854775808");
		  return;
		}
	    }
	  if (!v)
	    {
	      strcpy (buf, "0");
	      return;
	    }

	  i = 0;
	  tmp[29] = 0;
	  while (v)
	    {
	      tmp[28 - i] = (v % 10) + '0';
	      v /= 10;
	      i++;
	    }
	  strcpy (buf, tmp + 29 - i);
	}
    }
  else
    {
      if (hex)
	sprintf (buf, "0x%x", (unsigned int) disp);
      else
	sprintf (buf, "%d", (int) disp);
    }
}

static void
intel_operand_size (int bytemode, int sizeflag)
{
  switch (bytemode)
    {
    case b_mode:
      oappend ("BYTE PTR ");
      break;
    case w_mode:
    case dqw_mode:
      oappend ("WORD PTR ");
      break;
    case stack_v_mode:
      if (address_mode == mode_64bit && (sizeflag & DFLAG))
	{
	  oappend ("QWORD PTR ");
	  used_prefixes |= (prefixes & PREFIX_DATA);
	  break;
	}
      /* FALLTHRU */
    case v_mode:
    case dq_mode:
      USED_REX (REX_MODE64);
      if (rex & REX_MODE64)
	oappend ("QWORD PTR ");
      else if ((sizeflag & DFLAG) || bytemode == dq_mode)
	oappend ("DWORD PTR ");
      else
	oappend ("WORD PTR ");
      used_prefixes |= (prefixes & PREFIX_DATA);
      break;
    case z_mode:
      if ((rex & REX_MODE64) || (sizeflag & DFLAG))
	*obufp++ = 'D';
      oappend ("WORD PTR ");
      if (!(rex & REX_MODE64))
	used_prefixes |= (prefixes & PREFIX_DATA);
      break;
    case d_mode:
      oappend ("DWORD PTR ");
      break;
    case q_mode:
      oappend ("QWORD PTR ");
      break;
    case m_mode:
      if (address_mode == mode_64bit)
	oappend ("QWORD PTR ");
      else
	oappend ("DWORD PTR ");
      break;
    case f_mode:
      if (sizeflag & DFLAG)
	oappend ("FWORD PTR ");
      else
	oappend ("DWORD PTR ");
      used_prefixes |= (prefixes & PREFIX_DATA);
      break;
    case t_mode:
      oappend ("TBYTE PTR ");
      break;
    case x_mode:
      oappend ("XMMWORD PTR ");
      break;
    case o_mode:
      oappend ("OWORD PTR ");
      break;
    default:
      break;
    }
}

static void
OP_E (int bytemode, int sizeflag)
{
  bfd_vma disp;
  int add = 0;
  int riprel = 0;
  USED_REX (REX_EXTZ);
  if (rex & REX_EXTZ)
    add += 8;

  /* Skip mod/rm byte.  */
  MODRM_CHECK;
  codep++;

  if (mod == 3)
    {
      switch (bytemode)
	{
	case b_mode:
	  USED_REX (0);
	  if (rex)
	    oappend (names8rex[rm + add]);
	  else
	    oappend (names8[rm + add]);
	  break;
	case w_mode:
	  oappend (names16[rm + add]);
	  break;
	case d_mode:
	  oappend (names32[rm + add]);
	  break;
	case q_mode:
	  oappend (names64[rm + add]);
	  break;
	case m_mode:
	  if (address_mode == mode_64bit)
	    oappend (names64[rm + add]);
	  else
	    oappend (names32[rm + add]);
	  break;
	case stack_v_mode:
	  if (address_mode == mode_64bit && (sizeflag & DFLAG))
	    {
	      oappend (names64[rm + add]);
	      used_prefixes |= (prefixes & PREFIX_DATA);
	      break;
	    }
	  bytemode = v_mode;
	  /* FALLTHRU */
	case v_mode:
	case dq_mode:
	case dqw_mode:
	  USED_REX (REX_MODE64);
	  if (rex & REX_MODE64)
	    oappend (names64[rm + add]);
	  else if ((sizeflag & DFLAG) || bytemode != v_mode)
	    oappend (names32[rm + add]);
	  else
	    oappend (names16[rm + add]);
	  used_prefixes |= (prefixes & PREFIX_DATA);
	  break;
	case 0:
	  break;
	default:
	  oappend (INTERNAL_DISASSEMBLER_ERROR);
	  break;
	}
      return;
    }

  disp = 0;
  if (intel_syntax)
    intel_operand_size (bytemode, sizeflag);
  append_seg ();

  if ((sizeflag & AFLAG) || address_mode == mode_64bit) /* 32 bit address mode */
    {
      int havesib;
      int havebase;
      int base;
      int index = 0;
      int scale = 0;

      havesib = 0;
      havebase = 1;
      base = rm;

      if (base == 4)
	{
	  havesib = 1;
	  FETCH_DATA (the_info, codep + 1);
	  index = (*codep >> 3) & 7;
	  if (address_mode == mode_64bit || index != 0x4)
	    /* When INDEX == 0x4 in 32 bit mode, SCALE is ignored.  */
	    scale = (*codep >> 6) & 3;
	  base = *codep & 7;
	  USED_REX (REX_EXTY);
	  if (rex & REX_EXTY)
	    index += 8;
	  codep++;
	}
      base += add;

      switch (mod)
	{
	case 0:
	  if ((base & 7) == 5)
	    {
	      havebase = 0;
	      if (address_mode == mode_64bit && !havesib)
		riprel = 1;
	      disp = get32s ();
	    }
	  break;
	case 1:
	  FETCH_DATA (the_info, codep + 1);
	  disp = *codep++;
	  if ((disp & 0x80) != 0)
	    disp -= 0x100;
	  break;
	case 2:
	  disp = get32s ();
	  break;
	}

      if (!intel_syntax)
	if (mod != 0 || (base & 7) == 5)
	  {
	    print_operand_value (scratchbuf, !riprel, disp);
	    oappend (scratchbuf);
	    if (riprel)
	      {
		set_op (disp, 1);
		oappend ("(%rip)");
	      }
	  }

      if (havebase || (havesib && (index != 4 || scale != 0)))
	{
	  *obufp++ = open_char;
	  if (intel_syntax && riprel)
	    oappend ("rip + ");
	  *obufp = '\0';
	  if (havebase)
	    oappend (address_mode == mode_64bit && (sizeflag & AFLAG)
		     ? names64[base] : names32[base]);
	  if (havesib)
	    {
	      if (index != 4)
		{
		  if (!intel_syntax || havebase)
		    {
		      *obufp++ = separator_char;
		      *obufp = '\0';
		    }
		  oappend (address_mode == mode_64bit && (sizeflag & AFLAG)
			   ? names64[index] : names32[index]);
		}
	      if (scale != 0 || (!intel_syntax && index != 4))
		{
		  *obufp++ = scale_char;
		  *obufp = '\0';
		  sprintf (scratchbuf, "%d", 1 << scale);
		  oappend (scratchbuf);
		}
	    }
	  if (intel_syntax && disp)
	    {
	      if ((bfd_signed_vma) disp > 0)
		{
		  *obufp++ = '+';
		  *obufp = '\0';
		}
	      else if (mod != 1)
		{
		  *obufp++ = '-';
		  *obufp = '\0';
		  disp = - (bfd_signed_vma) disp;
		}

	      print_operand_value (scratchbuf, mod != 1, disp);
	      oappend (scratchbuf);
	    }

	  *obufp++ = close_char;
	  *obufp = '\0';
	}
      else if (intel_syntax)
	{
	  if (mod != 0 || (base & 7) == 5)
	    {
	      if (prefixes & (PREFIX_CS | PREFIX_SS | PREFIX_DS
			      | PREFIX_ES | PREFIX_FS | PREFIX_GS))
		;
	      else
		{
		  oappend (names_seg[ds_reg - es_reg]);
		  oappend (":");
		}
	      print_operand_value (scratchbuf, 1, disp);
	      oappend (scratchbuf);
	    }
	}
    }
  else
    { /* 16 bit address mode */
      switch (mod)
	{
	case 0:
	  if (rm == 6)
	    {
	      disp = get16 ();
	      if ((disp & 0x8000) != 0)
		disp -= 0x10000;
	    }
	  break;
	case 1:
	  FETCH_DATA (the_info, codep + 1);
	  disp = *codep++;
	  if ((disp & 0x80) != 0)
	    disp -= 0x100;
	  break;
	case 2:
	  disp = get16 ();
	  if ((disp & 0x8000) != 0)
	    disp -= 0x10000;
	  break;
	}

      if (!intel_syntax)
	if (mod != 0 || rm == 6)
	  {
	    print_operand_value (scratchbuf, 0, disp);
	    oappend (scratchbuf);
	  }

      if (mod != 0 || rm != 6)
	{
	  *obufp++ = open_char;
	  *obufp = '\0';
	  oappend (index16[rm]);
	  if (intel_syntax && disp)
	    {
	      if ((bfd_signed_vma) disp > 0)
		{
		  *obufp++ = '+';
		  *obufp = '\0';
		}
	      else if (mod != 1)
		{
		  *obufp++ = '-';
		  *obufp = '\0';
		  disp = - (bfd_signed_vma) disp;
		}

	      print_operand_value (scratchbuf, mod != 1, disp);
	      oappend (scratchbuf);
	    }

	  *obufp++ = close_char;
	  *obufp = '\0';
	}
      else if (intel_syntax)
	{
	  if (prefixes & (PREFIX_CS | PREFIX_SS | PREFIX_DS
			  | PREFIX_ES | PREFIX_FS | PREFIX_GS))
	    ;
	  else
	    {
	      oappend (names_seg[ds_reg - es_reg]);
	      oappend (":");
	    }
	  print_operand_value (scratchbuf, 1, disp & 0xffff);
	  oappend (scratchbuf);
	}
    }
}

static void
OP_G (int bytemode, int sizeflag)
{
  int add = 0;
  USED_REX (REX_EXTX);
  if (rex & REX_EXTX)
    add += 8;
  switch (bytemode)
    {
    case b_mode:
      USED_REX (0);
      if (rex)
	oappend (names8rex[reg + add]);
      else
	oappend (names8[reg + add]);
      break;
    case w_mode:
      oappend (names16[reg + add]);
      break;
    case d_mode:
      oappend (names32[reg + add]);
      break;
    case q_mode:
      oappend (names64[reg + add]);
      break;
    case v_mode:
    case dq_mode:
    case dqw_mode:
      USED_REX (REX_MODE64);
      if (rex & REX_MODE64)
	oappend (names64[reg + add]);
      else if ((sizeflag & DFLAG) || bytemode != v_mode)
	oappend (names32[reg + add]);
      else
	oappend (names16[reg + add]);
      used_prefixes |= (prefixes & PREFIX_DATA);
      break;
    case m_mode:
      if (address_mode == mode_64bit)
	oappend (names64[reg + add]);
      else
	oappend (names32[reg + add]);
      break;
    default:
      oappend (INTERNAL_DISASSEMBLER_ERROR);
      break;
    }
}

static bfd_vma
get64 (void)
{
  bfd_vma x;
#ifdef BFD64
  unsigned int a;
  unsigned int b;

  FETCH_DATA (the_info, codep + 8);
  a = *codep++ & 0xff;
  a |= (*codep++ & 0xff) << 8;
  a |= (*codep++ & 0xff) << 16;
  a |= (*codep++ & 0xff) << 24;
  b = *codep++ & 0xff;
  b |= (*codep++ & 0xff) << 8;
  b |= (*codep++ & 0xff) << 16;
  b |= (*codep++ & 0xff) << 24;
  x = a + ((bfd_vma) b << 32);
#else
  abort ();
  x = 0;
#endif
  return x;
}

static bfd_signed_vma
get32 (void)
{
  bfd_signed_vma x = 0;

  FETCH_DATA (the_info, codep + 4);
  x = *codep++ & (bfd_signed_vma) 0xff;
  x |= (*codep++ & (bfd_signed_vma) 0xff) << 8;
  x |= (*codep++ & (bfd_signed_vma) 0xff) << 16;
  x |= (*codep++ & (bfd_signed_vma) 0xff) << 24;
  return x;
}

static bfd_signed_vma
get32s (void)
{
  bfd_signed_vma x = 0;

  FETCH_DATA (the_info, codep + 4);
  x = *codep++ & (bfd_signed_vma) 0xff;
  x |= (*codep++ & (bfd_signed_vma) 0xff) << 8;
  x |= (*codep++ & (bfd_signed_vma) 0xff) << 16;
  x |= (*codep++ & (bfd_signed_vma) 0xff) << 24;

  x = (x ^ ((bfd_signed_vma) 1 << 31)) - ((bfd_signed_vma) 1 << 31);

  return x;
}

static int
get16 (void)
{
  int x = 0;

  FETCH_DATA (the_info, codep + 2);
  x = *codep++ & 0xff;
  x |= (*codep++ & 0xff) << 8;
  return x;
}

static void
set_op (bfd_vma op, int riprel)
{
  op_index[op_ad] = op_ad;
  if (address_mode == mode_64bit)
    {
      op_address[op_ad] = op;
      op_riprel[op_ad] = riprel;
    }
  else
    {
      /* Mask to get a 32-bit address.  */
      op_address[op_ad] = op & 0xffffffff;
      op_riprel[op_ad] = riprel & 0xffffffff;
    }
}

static void
OP_REG (int code, int sizeflag)
{
  const char *s;
  int add = 0;
  USED_REX (REX_EXTZ);
  if (rex & REX_EXTZ)
    add = 8;

  switch (code)
    {
    case ax_reg: case cx_reg: case dx_reg: case bx_reg:
    case sp_reg: case bp_reg: case si_reg: case di_reg:
      s = names16[code - ax_reg + add];
      break;
    case es_reg: case ss_reg: case cs_reg:
    case ds_reg: case fs_reg: case gs_reg:
      s = names_seg[code - es_reg + add];
      break;
    case al_reg: case ah_reg: case cl_reg: case ch_reg:
    case dl_reg: case dh_reg: case bl_reg: case bh_reg:
      USED_REX (0);
      if (rex)
	s = names8rex[code - al_reg + add];
      else
	s = names8[code - al_reg];
      break;
    case rAX_reg: case rCX_reg: case rDX_reg: case rBX_reg:
    case rSP_reg: case rBP_reg: case rSI_reg: case rDI_reg:
      if (address_mode == mode_64bit && (sizeflag & DFLAG))
	{
	  s = names64[code - rAX_reg + add];
	  break;
	}
      code += eAX_reg - rAX_reg;
      /* Fall through.  */
    case eAX_reg: case eCX_reg: case eDX_reg: case eBX_reg:
    case eSP_reg: case eBP_reg: case eSI_reg: case eDI_reg:
      USED_REX (REX_MODE64);
      if (rex & REX_MODE64)
	s = names64[code - eAX_reg + add];
      else if (sizeflag & DFLAG)
	s = names32[code - eAX_reg + add];
      else
	s = names16[code - eAX_reg + add];
      used_prefixes |= (prefixes & PREFIX_DATA);
      break;
    default:
      s = INTERNAL_DISASSEMBLER_ERROR;
      break;
    }
  oappend (s);
}

static void
OP_IMREG (int code, int sizeflag)
{
  const char *s;

  switch (code)
    {
    case indir_dx_reg:
      if (intel_syntax)
	s = "dx";
      else
	s = "(%dx)";
      break;
    case ax_reg: case cx_reg: case dx_reg: case bx_reg:
    case sp_reg: case bp_reg: case si_reg: case di_reg:
      s = names16[code - ax_reg];
      break;
    case es_reg: case ss_reg: case cs_reg:
    case ds_reg: case fs_reg: case gs_reg:
      s = names_seg[code - es_reg];
      break;
    case al_reg: case ah_reg: case cl_reg: case ch_reg:
    case dl_reg: case dh_reg: case bl_reg: case bh_reg:
      USED_REX (0);
      if (rex)
	s = names8rex[code - al_reg];
      else
	s = names8[code - al_reg];
      break;
    case eAX_reg: case eCX_reg: case eDX_reg: case eBX_reg:
    case eSP_reg: case eBP_reg: case eSI_reg: case eDI_reg:
      USED_REX (REX_MODE64);
      if (rex & REX_MODE64)
	s = names64[code - eAX_reg];
      else if (sizeflag & DFLAG)
	s = names32[code - eAX_reg];
      else
	s = names16[code - eAX_reg];
      used_prefixes |= (prefixes & PREFIX_DATA);
      break;
    case z_mode_ax_reg:
      if ((rex & REX_MODE64) || (sizeflag & DFLAG))
	s = *names32;
      else
	s = *names16;
      if (!(rex & REX_MODE64))
	used_prefixes |= (prefixes & PREFIX_DATA);
      break;
    default:
      s = INTERNAL_DISASSEMBLER_ERROR;
      break;
    }
  oappend (s);
}

static void
OP_I (int bytemode, int sizeflag)
{
  bfd_signed_vma op;
  bfd_signed_vma mask = -1;

  switch (bytemode)
    {
    case b_mode:
      FETCH_DATA (the_info, codep + 1);
      op = *codep++;
      mask = 0xff;
      break;
    case q_mode:
      if (address_mode == mode_64bit)
	{
	  op = get32s ();
	  break;
	}
      /* Fall through.  */
    case v_mode:
      USED_REX (REX_MODE64);
      if (rex & REX_MODE64)
	op = get32s ();
      else if (sizeflag & DFLAG)
	{
	  op = get32 ();
	  mask = 0xffffffff;
	}
      else
	{
	  op = get16 ();
	  mask = 0xfffff;
	}
      used_prefixes |= (prefixes & PREFIX_DATA);
      break;
    case w_mode:
      mask = 0xfffff;
      op = get16 ();
      break;
    case const_1_mode:
      if (intel_syntax)
        oappend ("1");
      return;
    default:
      oappend (INTERNAL_DISASSEMBLER_ERROR);
      return;
    }

  op &= mask;
  scratchbuf[0] = '$';
  print_operand_value (scratchbuf + 1, 1, op);
  oappend (scratchbuf + intel_syntax);
  scratchbuf[0] = '\0';
}

static void
OP_I64 (int bytemode, int sizeflag)
{
  bfd_signed_vma op;
  bfd_signed_vma mask = -1;

  if (address_mode != mode_64bit)
    {
      OP_I (bytemode, sizeflag);
      return;
    }

  switch (bytemode)
    {
    case b_mode:
      FETCH_DATA (the_info, codep + 1);
      op = *codep++;
      mask = 0xff;
      break;
    case v_mode:
      USED_REX (REX_MODE64);
      if (rex & REX_MODE64)
	op = get64 ();
      else if (sizeflag & DFLAG)
	{
	  op = get32 ();
	  mask = 0xffffffff;
	}
      else
	{
	  op = get16 ();
	  mask = 0xfffff;
	}
      used_prefixes |= (prefixes & PREFIX_DATA);
      break;
    case w_mode:
      mask = 0xfffff;
      op = get16 ();
      break;
    default:
      oappend (INTERNAL_DISASSEMBLER_ERROR);
      return;
    }

  op &= mask;
  scratchbuf[0] = '$';
  print_operand_value (scratchbuf + 1, 1, op);
  oappend (scratchbuf + intel_syntax);
  scratchbuf[0] = '\0';
}

static void
OP_sI (int bytemode, int sizeflag)
{
  bfd_signed_vma op;
  bfd_signed_vma mask = -1;

  switch (bytemode)
    {
    case b_mode:
      FETCH_DATA (the_info, codep + 1);
      op = *codep++;
      if ((op & 0x80) != 0)
	op -= 0x100;
      mask = 0xffffffff;
      break;
    case v_mode:
      USED_REX (REX_MODE64);
      if (rex & REX_MODE64)
	op = get32s ();
      else if (sizeflag & DFLAG)
	{
	  op = get32s ();
	  mask = 0xffffffff;
	}
      else
	{
	  mask = 0xffffffff;
	  op = get16 ();
	  if ((op & 0x8000) != 0)
	    op -= 0x10000;
	}
      used_prefixes |= (prefixes & PREFIX_DATA);
      break;
    case w_mode:
      op = get16 ();
      mask = 0xffffffff;
      if ((op & 0x8000) != 0)
	op -= 0x10000;
      break;
    default:
      oappend (INTERNAL_DISASSEMBLER_ERROR);
      return;
    }

  scratchbuf[0] = '$';
  print_operand_value (scratchbuf + 1, 1, op);
  oappend (scratchbuf + intel_syntax);
}

static void
OP_J (int bytemode, int sizeflag)
{
  bfd_vma disp;
  bfd_vma mask = -1;

  switch (bytemode)
    {
    case b_mode:
      FETCH_DATA (the_info, codep + 1);
      disp = *codep++;
      if ((disp & 0x80) != 0)
	disp -= 0x100;
      break;
    case v_mode:
      if ((sizeflag & DFLAG) || (rex & REX_MODE64))
	disp = get32s ();
      else
	{
	  disp = get16 ();
	  /* For some reason, a data16 prefix on a jump instruction
	     means that the pc is masked to 16 bits after the
	     displacement is added!  */
	  mask = 0xffff;
	}
      used_prefixes |= (prefixes & PREFIX_DATA);
      break;
    default:
      oappend (INTERNAL_DISASSEMBLER_ERROR);
      return;
    }
  disp = (start_pc + codep - start_codep + disp) & mask;
  set_op (disp, 0);
  print_operand_value (scratchbuf, 1, disp);
  oappend (scratchbuf);
}

static void
OP_SEG (int bytemode, int sizeflag)
{
  if (bytemode == w_mode)
    oappend (names_seg[reg]);
  else
    OP_E (mod == 3 ? bytemode : w_mode, sizeflag);
}

static void
OP_DIR (int dummy ATTRIBUTE_UNUSED, int sizeflag)
{
  int seg, offset;

  if (sizeflag & DFLAG)
    {
      offset = get32 ();
      seg = get16 ();
    }
  else
    {
      offset = get16 ();
      seg = get16 ();
    }
  used_prefixes |= (prefixes & PREFIX_DATA);
  if (intel_syntax)
    sprintf (scratchbuf, "0x%x:0x%x", seg, offset);
  else
    sprintf (scratchbuf, "$0x%x,$0x%x", seg, offset);
  oappend (scratchbuf);
}

static void
OP_OFF (int bytemode, int sizeflag)
{
  bfd_vma off;

  if (intel_syntax && (sizeflag & SUFFIX_ALWAYS))
    intel_operand_size (bytemode, sizeflag);
  append_seg ();

  if ((sizeflag & AFLAG) || address_mode == mode_64bit)
    off = get32 ();
  else
    off = get16 ();

  if (intel_syntax)
    {
      if (!(prefixes & (PREFIX_CS | PREFIX_SS | PREFIX_DS
			| PREFIX_ES | PREFIX_FS | PREFIX_GS)))
	{
	  oappend (names_seg[ds_reg - es_reg]);
	  oappend (":");
	}
    }
  print_operand_value (scratchbuf, 1, off);
  oappend (scratchbuf);
}

static void
OP_OFF64 (int bytemode, int sizeflag)
{
  bfd_vma off;

  if (address_mode != mode_64bit
      || (prefixes & PREFIX_ADDR))
    {
      OP_OFF (bytemode, sizeflag);
      return;
    }

  if (intel_syntax && (sizeflag & SUFFIX_ALWAYS))
    intel_operand_size (bytemode, sizeflag);
  append_seg ();

  off = get64 ();

  if (intel_syntax)
    {
      if (!(prefixes & (PREFIX_CS | PREFIX_SS | PREFIX_DS
			| PREFIX_ES | PREFIX_FS | PREFIX_GS)))
	{
	  oappend (names_seg[ds_reg - es_reg]);
	  oappend (":");
	}
    }
  print_operand_value (scratchbuf, 1, off);
  oappend (scratchbuf);
}

static void
ptr_reg (int code, int sizeflag)
{
  const char *s;

  *obufp++ = open_char;
  used_prefixes |= (prefixes & PREFIX_ADDR);
  if (address_mode == mode_64bit)
    {
      if (!(sizeflag & AFLAG))
	s = names32[code - eAX_reg];
      else
	s = names64[code - eAX_reg];
    }
  else if (sizeflag & AFLAG)
    s = names32[code - eAX_reg];
  else
    s = names16[code - eAX_reg];
  oappend (s);
  *obufp++ = close_char;
  *obufp = 0;
}

static void
OP_ESreg (int code, int sizeflag)
{
  if (intel_syntax)
    {
      switch (codep[-1])
	{
	case 0x6d:	/* insw/insl */
	  intel_operand_size (z_mode, sizeflag);
	  break;
	case 0xa5:	/* movsw/movsl/movsq */
	case 0xa7:	/* cmpsw/cmpsl/cmpsq */
	case 0xab:	/* stosw/stosl */
	case 0xaf:	/* scasw/scasl */
	  intel_operand_size (v_mode, sizeflag);
	  break;
	default:
	  intel_operand_size (b_mode, sizeflag);
	}
    }
  oappend ("%es:" + intel_syntax);
  ptr_reg (code, sizeflag);
}

static void
OP_DSreg (int code, int sizeflag)
{
  if (intel_syntax)
    {
      switch (codep[-1])
	{
	case 0x6f:	/* outsw/outsl */
	  intel_operand_size (z_mode, sizeflag);
	  break;
	case 0xa5:	/* movsw/movsl/movsq */
	case 0xa7:	/* cmpsw/cmpsl/cmpsq */
	case 0xad:	/* lodsw/lodsl/lodsq */
	  intel_operand_size (v_mode, sizeflag);
	  break;
	default:
	  intel_operand_size (b_mode, sizeflag);
	}
    }
  if ((prefixes
       & (PREFIX_CS
	  | PREFIX_DS
	  | PREFIX_SS
	  | PREFIX_ES
	  | PREFIX_FS
	  | PREFIX_GS)) == 0)
    prefixes |= PREFIX_DS;
  append_seg ();
  ptr_reg (code, sizeflag);
}

static void
OP_C (int dummy ATTRIBUTE_UNUSED, int sizeflag ATTRIBUTE_UNUSED)
{
  int add = 0;
  if (rex & REX_EXTX)
    {
      USED_REX (REX_EXTX);
      add = 8;
    }
  else if (address_mode != mode_64bit && (prefixes & PREFIX_LOCK))
    {
      used_prefixes |= PREFIX_LOCK;
      add = 8;
    }
  sprintf (scratchbuf, "%%cr%d", reg + add);
  oappend (scratchbuf + intel_syntax);
}

static void
OP_D (int dummy ATTRIBUTE_UNUSED, int sizeflag ATTRIBUTE_UNUSED)
{
  int add = 0;
  USED_REX (REX_EXTX);
  if (rex & REX_EXTX)
    add = 8;
  if (intel_syntax)
    sprintf (scratchbuf, "db%d", reg + add);
  else
    sprintf (scratchbuf, "%%db%d", reg + add);
  oappend (scratchbuf);
}

static void
OP_T (int dummy ATTRIBUTE_UNUSED, int sizeflag ATTRIBUTE_UNUSED)
{
  sprintf (scratchbuf, "%%tr%d", reg);
  oappend (scratchbuf + intel_syntax);
}

static void
OP_Rd (int bytemode, int sizeflag)
{
  if (mod == 3)
    OP_E (bytemode, sizeflag);
  else
    BadOp ();
}

static void
OP_MMX (int bytemode ATTRIBUTE_UNUSED, int sizeflag ATTRIBUTE_UNUSED)
{
  used_prefixes |= (prefixes & PREFIX_DATA);
  if (prefixes & PREFIX_DATA)
    {
      int add = 0;
      USED_REX (REX_EXTX);
      if (rex & REX_EXTX)
	add = 8;
      sprintf (scratchbuf, "%%xmm%d", reg + add);
    }
  else
    sprintf (scratchbuf, "%%mm%d", reg);
  oappend (scratchbuf + intel_syntax);
}

static void
OP_XMM (int bytemode ATTRIBUTE_UNUSED, int sizeflag ATTRIBUTE_UNUSED)
{
  int add = 0;
  USED_REX (REX_EXTX);
  if (rex & REX_EXTX)
    add = 8;
  sprintf (scratchbuf, "%%xmm%d", reg + add);
  oappend (scratchbuf + intel_syntax);
}

static void
OP_EM (int bytemode, int sizeflag)
{
  if (mod != 3)
    {
      if (intel_syntax && bytemode == v_mode)
	{
	  bytemode = (prefixes & PREFIX_DATA) ? x_mode : q_mode;
	  used_prefixes |= (prefixes & PREFIX_DATA);
 	}
      OP_E (bytemode, sizeflag);
      return;
    }

  /* Skip mod/rm byte.  */
  MODRM_CHECK;
  codep++;
  used_prefixes |= (prefixes & PREFIX_DATA);
  if (prefixes & PREFIX_DATA)
    {
      int add = 0;

      USED_REX (REX_EXTZ);
      if (rex & REX_EXTZ)
	add = 8;
      sprintf (scratchbuf, "%%xmm%d", rm + add);
    }
  else
    sprintf (scratchbuf, "%%mm%d", rm);
  oappend (scratchbuf + intel_syntax);
}

/* cvt* are the only instructions in sse2 which have 
   both SSE and MMX operands and also have 0x66 prefix 
   in their opcode. 0x66 was originally used to differentiate 
   between SSE and MMX instruction(operands). So we have to handle the 
   cvt* separately using OP_EMC and OP_MXC */
static void
OP_EMC (int bytemode, int sizeflag)
{
  if (mod != 3)
    {
      if (intel_syntax && bytemode == v_mode)
	{
	  bytemode = (prefixes & PREFIX_DATA) ? x_mode : q_mode;
	  used_prefixes |= (prefixes & PREFIX_DATA);
 	}
      OP_E (bytemode, sizeflag);
      return;
    }
  
  /* Skip mod/rm byte.  */
  MODRM_CHECK;
  codep++;
  used_prefixes |= (prefixes & PREFIX_DATA);
  sprintf (scratchbuf, "%%mm%d", rm);
  oappend (scratchbuf + intel_syntax);
}

static void
OP_MXC (int bytemode ATTRIBUTE_UNUSED, int sizeflag ATTRIBUTE_UNUSED)
{
  used_prefixes |= (prefixes & PREFIX_DATA);
  sprintf (scratchbuf, "%%mm%d", reg);
  oappend (scratchbuf + intel_syntax);
}

static void
OP_EX (int bytemode, int sizeflag)
{
  int add = 0;
  if (mod != 3)
    {
      if (intel_syntax && bytemode == v_mode)
	{
	  switch (prefixes & (PREFIX_DATA|PREFIX_REPZ|PREFIX_REPNZ))
	    {
	    case 0:            bytemode = x_mode; break;
	    case PREFIX_REPZ:  bytemode = d_mode; used_prefixes |= PREFIX_REPZ;  break;
	    case PREFIX_DATA:  bytemode = x_mode; used_prefixes |= PREFIX_DATA;  break;
	    case PREFIX_REPNZ: bytemode = q_mode; used_prefixes |= PREFIX_REPNZ; break;
	    default:           bytemode = 0; break;
	    }
	}
      OP_E (bytemode, sizeflag);
      return;
    }
  USED_REX (REX_EXTZ);
  if (rex & REX_EXTZ)
    add = 8;

  /* Skip mod/rm byte.  */
  MODRM_CHECK;
  codep++;
  sprintf (scratchbuf, "%%xmm%d", rm + add);
  oappend (scratchbuf + intel_syntax);
}

static void
OP_MS (int bytemode, int sizeflag)
{
  if (mod == 3)
    OP_EM (bytemode, sizeflag);
  else
    BadOp ();
}

static void
OP_XS (int bytemode, int sizeflag)
{
  if (mod == 3)
    OP_EX (bytemode, sizeflag);
  else
    BadOp ();
}

static void
OP_M (int bytemode, int sizeflag)
{
  if (mod == 3)
    /* bad bound,lea,lds,les,lfs,lgs,lss,cmpxchg8b,vmptrst modrm */
    BadOp ();
  else
    OP_E (bytemode, sizeflag);
}

static void
OP_0f07 (int bytemode, int sizeflag)
{
  if (mod != 3 || rm != 0)
    BadOp ();
  else
    OP_E (bytemode, sizeflag);
}

static void
OP_0fae (int bytemode, int sizeflag)
{
  if (mod == 3)
    {
      if (reg == 7)
	strcpy (obuf + strlen (obuf) - sizeof ("clflush") + 1, "sfence");

      if (reg < 5 || rm != 0)
	{
	  BadOp ();	/* bad sfence, mfence, or lfence */
	  return;
	}
    }
  else if (reg != 7)
    {
      BadOp ();		/* bad clflush */
      return;
    }

  OP_E (bytemode, sizeflag);
}

/* NOP is an alias of "xchg %ax,%ax" in 16bit mode, "xchg %eax,%eax" in
   32bit mode and "xchg %rax,%rax" in 64bit mode.  NOP with REPZ prefix
   is called PAUSE.  We display "xchg %ax,%ax" instead of "data16 nop".
 */

static void
NOP_Fixup1 (int bytemode, int sizeflag)
{
  if (prefixes == PREFIX_REPZ)
    strcpy (obuf, "pause");
  else if (prefixes == PREFIX_DATA
	   || ((rex & REX_MODE64) && rex != 0x48))
    OP_REG (bytemode, sizeflag);
  else
    strcpy (obuf, "nop");
}

static void
NOP_Fixup2 (int bytemode, int sizeflag)
{
  if (prefixes == PREFIX_DATA
      || ((rex & REX_MODE64) && rex != 0x48))
    OP_IMREG (bytemode, sizeflag);
}

static const char *const Suffix3DNow[] = {
/* 00 */	NULL,		NULL,		NULL,		NULL,
/* 04 */	NULL,		NULL,		NULL,		NULL,
/* 08 */	NULL,		NULL,		NULL,		NULL,
/* 0C */	"pi2fw",	"pi2fd",	NULL,		NULL,
/* 10 */	NULL,		NULL,		NULL,		NULL,
/* 14 */	NULL,		NULL,		NULL,		NULL,
/* 18 */	NULL,		NULL,		NULL,		NULL,
/* 1C */	"pf2iw",	"pf2id",	NULL,		NULL,
/* 20 */	NULL,		NULL,		NULL,		NULL,
/* 24 */	NULL,		NULL,		NULL,		NULL,
/* 28 */	NULL,		NULL,		NULL,		NULL,
/* 2C */	NULL,		NULL,		NULL,		NULL,
/* 30 */	NULL,		NULL,		NULL,		NULL,
/* 34 */	NULL,		NULL,		NULL,		NULL,
/* 38 */	NULL,		NULL,		NULL,		NULL,
/* 3C */	NULL,		NULL,		NULL,		NULL,
/* 40 */	NULL,		NULL,		NULL,		NULL,
/* 44 */	NULL,		NULL,		NULL,		NULL,
/* 48 */	NULL,		NULL,		NULL,		NULL,
/* 4C */	NULL,		NULL,		NULL,		NULL,
/* 50 */	NULL,		NULL,		NULL,		NULL,
/* 54 */	NULL,		NULL,		NULL,		NULL,
/* 58 */	NULL,		NULL,		NULL,		NULL,
/* 5C */	NULL,		NULL,		NULL,		NULL,
/* 60 */	NULL,		NULL,		NULL,		NULL,
/* 64 */	NULL,		NULL,		NULL,		NULL,
/* 68 */	NULL,		NULL,		NULL,		NULL,
/* 6C */	NULL,		NULL,		NULL,		NULL,
/* 70 */	NULL,		NULL,		NULL,		NULL,
/* 74 */	NULL,		NULL,		NULL,		NULL,
/* 78 */	NULL,		NULL,		NULL,		NULL,
/* 7C */	NULL,		NULL,		NULL,		NULL,
/* 80 */	NULL,		NULL,		NULL,		NULL,
/* 84 */	NULL,		NULL,		NULL,		NULL,
/* 88 */	NULL,		NULL,		"pfnacc",	NULL,
/* 8C */	NULL,		NULL,		"pfpnacc",	NULL,
/* 90 */	"pfcmpge",	NULL,		NULL,		NULL,
/* 94 */	"pfmin",	NULL,		"pfrcp",	"pfrsqrt",
/* 98 */	NULL,		NULL,		"pfsub",	NULL,
/* 9C */	NULL,		NULL,		"pfadd",	NULL,
/* A0 */	"pfcmpgt",	NULL,		NULL,		NULL,
/* A4 */	"pfmax",	NULL,		"pfrcpit1",	"pfrsqit1",
/* A8 */	NULL,		NULL,		"pfsubr",	NULL,
/* AC */	NULL,		NULL,		"pfacc",	NULL,
/* B0 */	"pfcmpeq",	NULL,		NULL,		NULL,
/* B4 */	"pfmul",	NULL,		"pfrcpit2",	"pfmulhrw",
/* B8 */	NULL,		NULL,		NULL,		"pswapd",
/* BC */	NULL,		NULL,		NULL,		"pavgusb",
/* C0 */	NULL,		NULL,		NULL,		NULL,
/* C4 */	NULL,		NULL,		NULL,		NULL,
/* C8 */	NULL,		NULL,		NULL,		NULL,
/* CC */	NULL,		NULL,		NULL,		NULL,
/* D0 */	NULL,		NULL,		NULL,		NULL,
/* D4 */	NULL,		NULL,		NULL,		NULL,
/* D8 */	NULL,		NULL,		NULL,		NULL,
/* DC */	NULL,		NULL,		NULL,		NULL,
/* E0 */	NULL,		NULL,		NULL,		NULL,
/* E4 */	NULL,		NULL,		NULL,		NULL,
/* E8 */	NULL,		NULL,		NULL,		NULL,
/* EC */	NULL,		NULL,		NULL,		NULL,
/* F0 */	NULL,		NULL,		NULL,		NULL,
/* F4 */	NULL,		NULL,		NULL,		NULL,
/* F8 */	NULL,		NULL,		NULL,		NULL,
/* FC */	NULL,		NULL,		NULL,		NULL,
};

static void
OP_3DNowSuffix (int bytemode ATTRIBUTE_UNUSED, int sizeflag ATTRIBUTE_UNUSED)
{
  const char *mnemonic;

  FETCH_DATA (the_info, codep + 1);
  /* AMD 3DNow! instructions are specified by an opcode suffix in the
     place where an 8-bit immediate would normally go.  ie. the last
     byte of the instruction.  */
  obufp = obuf + strlen (obuf);
  mnemonic = Suffix3DNow[*codep++ & 0xff];
  if (mnemonic)
    oappend (mnemonic);
  else
    {
      /* Since a variable sized modrm/sib chunk is between the start
	 of the opcode (0x0f0f) and the opcode suffix, we need to do
	 all the modrm processing first, and don't know until now that
	 we have a bad opcode.  This necessitates some cleaning up.  */
      op1out[0] = '\0';
      op2out[0] = '\0';
      BadOp ();
    }
}

static const char *simd_cmp_op[] = {
  "eq",
  "lt",
  "le",
  "unord",
  "neq",
  "nlt",
  "nle",
  "ord"
};

static void
OP_SIMD_Suffix (int bytemode ATTRIBUTE_UNUSED, int sizeflag ATTRIBUTE_UNUSED)
{
  unsigned int cmp_type;

  FETCH_DATA (the_info, codep + 1);
  obufp = obuf + strlen (obuf);
  cmp_type = *codep++ & 0xff;
  if (cmp_type < 8)
    {
      char suffix1 = 'p', suffix2 = 's';
      used_prefixes |= (prefixes & PREFIX_REPZ);
      if (prefixes & PREFIX_REPZ)
	suffix1 = 's';
      else
	{
	  used_prefixes |= (prefixes & PREFIX_DATA);
	  if (prefixes & PREFIX_DATA)
	    suffix2 = 'd';
	  else
	    {
	      used_prefixes |= (prefixes & PREFIX_REPNZ);
	      if (prefixes & PREFIX_REPNZ)
		suffix1 = 's', suffix2 = 'd';
	    }
	}
      sprintf (scratchbuf, "cmp%s%c%c",
	       simd_cmp_op[cmp_type], suffix1, suffix2);
      used_prefixes |= (prefixes & PREFIX_REPZ);
      oappend (scratchbuf);
    }
  else
    {
      /* We have a bad extension byte.  Clean up.  */
      op1out[0] = '\0';
      op2out[0] = '\0';
      BadOp ();
    }
}

static void
SIMD_Fixup (int extrachar, int sizeflag ATTRIBUTE_UNUSED)
{
  /* Change movlps/movhps to movhlps/movlhps for 2 register operand
     forms of these instructions.  */
  if (mod == 3)
    {
      char *p = obuf + strlen (obuf);
      *(p + 1) = '\0';
      *p       = *(p - 1);
      *(p - 1) = *(p - 2);
      *(p - 2) = *(p - 3);
      *(p - 3) = extrachar;
    }
}

static void
PNI_Fixup (int extrachar ATTRIBUTE_UNUSED, int sizeflag)
{
  if (mod == 3 && reg == 1 && rm <= 1)
    {
      /* Override "sidt".  */
      size_t olen = strlen (obuf);
      char *p = obuf + olen - 4;
      const char **names = (address_mode == mode_64bit
			    ? names64 : names32);

      /* We might have a suffix when disassembling with -Msuffix.  */
      if (*p == 'i')
	--p;

      /* Remove "addr16/addr32" if we aren't in Intel mode.  */
      if (!intel_syntax
	  && (prefixes & PREFIX_ADDR)
	  && olen >= (4 + 7)
	  && *(p - 1) == ' '
	  && CONST_STRNEQ (p - 7, "addr")
	  && (CONST_STRNEQ (p - 3, "16")
	      || CONST_STRNEQ (p - 3, "32")))
	p -= 7;

      if (rm)
	{
	  /* mwait %eax,%ecx  */
	  strcpy (p, "mwait");
	  if (!intel_syntax)
	    strcpy (op1out, names[0]);
	}
      else
	{
	  /* monitor %eax,%ecx,%edx"  */
	  strcpy (p, "monitor");
	  if (!intel_syntax)
	    {
	      const char **op1_names;
	      if (!(prefixes & PREFIX_ADDR))
		op1_names = (address_mode == mode_16bit
			     ? names16 : names);
	      else
		{
		  op1_names = (address_mode != mode_32bit
			       ? names32 : names16);
		  used_prefixes |= PREFIX_ADDR;
		}
	      strcpy (op1out, op1_names[0]);
	      strcpy (op3out, names[2]);
	    }
	}
      if (!intel_syntax)
	{
	  strcpy (op2out, names[1]);
	  two_source_ops = 1;
	}

      codep++;
    }
  else
    OP_M (0, sizeflag);
}

static void
SVME_Fixup (int bytemode, int sizeflag)
{
  const char *alt;
  char *p;

  switch (*codep)
    {
    case 0xd8:
      alt = "vmrun";
      break;
    case 0xd9:
      alt = "vmmcall";
      break;
    case 0xda:
      alt = "vmload";
      break;
    case 0xdb:
      alt = "vmsave";
      break;
    case 0xdc:
      alt = "stgi";
      break;
    case 0xdd:
      alt = "clgi";
      break;
    case 0xde:
      alt = "skinit";
      break;
    case 0xdf:
      alt = "invlpga";
      break;
    default:
      OP_M (bytemode, sizeflag);
      return;
    }
  /* Override "lidt".  */
  p = obuf + strlen (obuf) - 4;
  /* We might have a suffix.  */
  if (*p == 'i')
    --p;
  strcpy (p, alt);
  if (!(prefixes & PREFIX_ADDR))
    {
      ++codep;
      return;
    }
  used_prefixes |= PREFIX_ADDR;
  switch (*codep++)
    {
    case 0xdf:
      strcpy (op2out, names32[1]);
      two_source_ops = 1;
	  /* Fall through.  */
    case 0xd8:
    case 0xda:
    case 0xdb:
      *obufp++ = open_char;
      if (address_mode == mode_64bit || (sizeflag & AFLAG))
        alt = names32[0];
      else
        alt = names16[0];
      strcpy (obufp, alt);
      obufp += strlen (alt);
      *obufp++ = close_char;
      *obufp = '\0';
      break;
    }
}

static void
INVLPG_Fixup (int bytemode, int sizeflag)
{
  const char *alt;

  switch (*codep)
    {
    case 0xf8:
      alt = "swapgs";
      break;
    case 0xf9:
      alt = "rdtscp";
      break;
    default:
      OP_M (bytemode, sizeflag);
      return;
    }
  /* Override "invlpg".  */
  strcpy (obuf + strlen (obuf) - 6, alt);
  codep++;
}

static void
BadOp (void)
{
  /* Throw away prefixes and 1st. opcode byte.  */
  codep = insn_codep + 1;
  oappend ("(bad)");
}

static void
VMX_Fixup (int extrachar ATTRIBUTE_UNUSED, int sizeflag)
{
  if (mod == 3 && reg == 0 && rm >=1 && rm <= 4)
    {
      /* Override "sgdt".  */
      char *p = obuf + strlen (obuf) - 4;

      /* We might have a suffix when disassembling with -Msuffix.  */
      if (*p == 'g')
	--p;

      switch (rm)
	{
	case 1:
	  strcpy (p, "vmcall");
	  break;
	case 2:
	  strcpy (p, "vmlaunch");
	  break;
	case 3:
	  strcpy (p, "vmresume");
	  break;
	case 4:
	  strcpy (p, "vmxoff");
	  break;
	}

      codep++;
    }
  else
    OP_E (0, sizeflag);
}

static void
OP_VMX (int bytemode, int sizeflag)
{
  used_prefixes |= (prefixes & (PREFIX_DATA | PREFIX_REPZ));
  if (prefixes & PREFIX_DATA)
    strcpy (obuf, "vmclear");
  else if (prefixes & PREFIX_REPZ)
    strcpy (obuf, "vmxon");
  else
    strcpy (obuf, "vmptrld");
  OP_E (bytemode, sizeflag);
}

static void
REP_Fixup (int bytemode, int sizeflag)
{
  /* The 0xf3 prefix should be displayed as "rep" for ins, outs, movs,
     lods and stos.  */
  size_t ilen = 0;

  if (prefixes & PREFIX_REPZ)
    switch (*insn_codep) 
      {
      case 0x6e:	/* outsb */
      case 0x6f:	/* outsw/outsl */
      case 0xa4:	/* movsb */
      case 0xa5:	/* movsw/movsl/movsq */
	if (!intel_syntax)
	  ilen = 5;
	else
	  ilen = 4;
	break;
      case 0xaa:	/* stosb */
      case 0xab:	/* stosw/stosl/stosq */
      case 0xac:	/* lodsb */
      case 0xad:	/* lodsw/lodsl/lodsq */
	if (!intel_syntax && (sizeflag & SUFFIX_ALWAYS))
	  ilen = 5;
	else
	  ilen = 4;
	break;
      case 0x6c:	/* insb */
      case 0x6d:	/* insl/insw */
	if (!intel_syntax)
	  ilen = 4;
	else
	  ilen = 3;
	break;
      default:
	abort ();
	break;
      }

  if (ilen != 0)
    {
      size_t olen;
      char *p;

      olen = strlen (obuf);
      p = obuf + olen - ilen - 1 - 4;
      /* Handle "repz [addr16|addr32]".  */
      if ((prefixes & PREFIX_ADDR))
	p -= 1 + 6;

      memmove (p + 3, p + 4, olen - (p + 3 - obuf));
    }

  switch (bytemode)
    {
    case al_reg:
    case eAX_reg:
    case indir_dx_reg:
      OP_IMREG (bytemode, sizeflag);
      break;
    case eDI_reg:
      OP_ESreg (bytemode, sizeflag);
      break;
    case eSI_reg:
      OP_DSreg (bytemode, sizeflag);
      break;
    default:
      abort ();
      break;
    }
}

static void
CMPXCHG8B_Fixup (int bytemode, int sizeflag)
{
  USED_REX (REX_MODE64);
  if (rex & REX_MODE64)
    {
      /* Change cmpxchg8b to cmpxchg16b.  */
      char *p = obuf + strlen (obuf) - 2;
      strcpy (p, "16b");
      bytemode = o_mode;
    }
  OP_M (bytemode, sizeflag);
}
