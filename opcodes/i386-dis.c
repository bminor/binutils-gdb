/* Print i386 instructions for GDB, the GNU debugger.
   Copyright (C) 1988, 89, 91, 93, 94, 95, 96, 97, 98, 1999
   Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/*
 * 80386 instruction printer by Pace Willisson (pace@prep.ai.mit.edu)
 * July 1988
 *  modified by John Hassey (hassey@dg-rtp.dg.com)
 */

/*
 * The main tables describing the instructions is essentially a copy
 * of the "Opcode Map" chapter (Appendix A) of the Intel 80386
 * Programmers Manual.  Usually, there is a capital letter, followed
 * by a small letter.  The capital letter tell the addressing mode,
 * and the small letter tells about the operand size.  Refer to
 * the Intel manual for details.
 */

#include "dis-asm.h"
#include "sysdep.h"
#include "opintl.h"

#define MAXLEN 20

#include <setjmp.h>

#ifndef UNIXWARE_COMPAT
/* Set non-zero for broken, compatible instructions.  Set to zero for
   non-broken opcodes.  */
#define UNIXWARE_COMPAT 1
#endif


static int fetch_data PARAMS ((struct disassemble_info *, bfd_byte *));

struct dis_private
{
  /* Points to first byte not fetched.  */
  bfd_byte *max_fetched;
  bfd_byte the_buffer[MAXLEN];
  bfd_vma insn_start;
  jmp_buf bailout;
};

/* Make sure that bytes from INFO->PRIVATE_DATA->BUFFER (inclusive)
   to ADDR (exclusive) are valid.  Returns 1 for success, longjmps
   on error.  */
#define FETCH_DATA(info, addr) \
  ((addr) <= ((struct dis_private *)(info->private_data))->max_fetched \
   ? 1 : fetch_data ((info), (addr)))

static int
fetch_data (info, addr)
     struct disassemble_info *info;
     bfd_byte *addr;
{
  int status;
  struct dis_private *priv = (struct dis_private *)info->private_data;
  bfd_vma start = priv->insn_start + (priv->max_fetched - priv->the_buffer);

  status = (*info->read_memory_func) (start,
				      priv->max_fetched,
				      addr - priv->max_fetched,
				      info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, start, info);
      longjmp (priv->bailout, 1);
    }
  else
    priv->max_fetched = addr;
  return 1;
}

#define Eb OP_E, b_mode
#define indirEb OP_indirE, b_mode
#define Gb OP_G, b_mode
#define Ev OP_E, v_mode
#define indirEv OP_indirE, v_mode
#define Ew OP_E, w_mode
#define Ma OP_E, v_mode
#define M OP_E, 0
#define Mp OP_E, 0		/* ? */
#define Gv OP_G, v_mode
#define Gw OP_G, w_mode
#define Rw OP_rm, w_mode
#define Rd OP_rm, d_mode
#define Ib OP_I, b_mode
#define sIb OP_sI, b_mode	/* sign extened byte */
#define Iv OP_I, v_mode
#define Iw OP_I, w_mode
#define Jb OP_J, b_mode
#define Jv OP_J, v_mode
#if 0
#define ONE OP_ONE, 0
#endif
#define Cd OP_C, d_mode
#define Dd OP_D, d_mode
#define Td OP_T, d_mode

#define eAX OP_REG, eAX_reg
#define eBX OP_REG, eBX_reg
#define eCX OP_REG, eCX_reg
#define eDX OP_REG, eDX_reg
#define eSP OP_REG, eSP_reg
#define eBP OP_REG, eBP_reg
#define eSI OP_REG, eSI_reg
#define eDI OP_REG, eDI_reg
#define AL OP_REG, al_reg
#define CL OP_REG, cl_reg
#define DL OP_REG, dl_reg
#define BL OP_REG, bl_reg
#define AH OP_REG, ah_reg
#define CH OP_REG, ch_reg
#define DH OP_REG, dh_reg
#define BH OP_REG, bh_reg
#define AX OP_REG, ax_reg
#define DX OP_REG, dx_reg
#define indirDX OP_REG, indir_dx_reg

#define Sw OP_SEG, w_mode
#define Ap OP_DIR, 0
#define Ob OP_OFF, b_mode
#define Ov OP_OFF, v_mode
#define Xb OP_DSreg, eSI_reg
#define Xv OP_DSreg, eSI_reg
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
#define MS OP_MS, b_mode
#define None OP_E, 0
#define OPSUF OP_3DNowSuffix, 0
#define OPSIMD OP_SIMD_Suffix, 0

/* bits in sizeflag */
#if 0 /* leave undefined until someone adds the extra flag to objdump */
#define SUFFIX_ALWAYS 4
#endif
#define AFLAG 2
#define DFLAG 1

typedef void (*op_rtn) PARAMS ((int bytemode, int sizeflag ));

static void OP_E PARAMS ((int, int));
static void OP_G PARAMS ((int, int));
static void OP_I PARAMS ((int, int));
static void OP_indirE PARAMS ((int, int));
static void OP_sI PARAMS ((int, int));
static void OP_REG PARAMS ((int, int));
static void OP_J PARAMS ((int, int));
static void OP_DIR PARAMS ((int, int));
static void OP_OFF PARAMS ((int, int));
static void OP_ESreg PARAMS ((int, int));
static void OP_DSreg PARAMS ((int, int));
static void OP_SEG PARAMS ((int, int));
static void OP_C PARAMS ((int, int));
static void OP_D PARAMS ((int, int));
static void OP_T PARAMS ((int, int));
static void OP_rm PARAMS ((int, int));
static void OP_ST PARAMS ((int, int));
static void OP_STi  PARAMS ((int, int));
#if 0
static void OP_ONE PARAMS ((int, int));
#endif
static void OP_MMX PARAMS ((int, int));
static void OP_XMM PARAMS ((int, int));
static void OP_EM PARAMS ((int, int));
static void OP_EX PARAMS ((int, int));
static void OP_MS PARAMS ((int, int));
static void OP_3DNowSuffix PARAMS ((int, int));
static void OP_SIMD_Suffix PARAMS ((int, int));
static void SIMD_Fixup PARAMS ((int, int));

static void append_seg PARAMS ((void));
static void set_op PARAMS ((unsigned int op));
static void putop PARAMS ((char *template, int sizeflag));
static void dofloat PARAMS ((int sizeflag));
static int get16 PARAMS ((void));
static int get32 PARAMS ((void));
static void ckprefix PARAMS ((void));
static void ptr_reg PARAMS ((int, int));

#define b_mode 1
#define v_mode 2
#define w_mode 3
#define d_mode 4
#define x_mode 5

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

#define indir_dx_reg 150

#define USE_GROUPS 1
#define USE_PREFIX_USER_TABLE 2

#define GRP1b NULL, NULL, 0, NULL, USE_GROUPS
#define GRP1S NULL, NULL, 1, NULL, USE_GROUPS
#define GRP1Ss NULL, NULL, 2, NULL, USE_GROUPS
#define GRP2b NULL, NULL, 3, NULL, USE_GROUPS
#define GRP2S NULL, NULL, 4, NULL, USE_GROUPS
#define GRP2b_one NULL, NULL, 5, NULL, USE_GROUPS
#define GRP2S_one NULL, NULL, 6, NULL, USE_GROUPS
#define GRP2b_cl NULL, NULL, 7, NULL, USE_GROUPS
#define GRP2S_cl NULL, NULL, 8, NULL, USE_GROUPS
#define GRP3b NULL, NULL, 9, NULL, USE_GROUPS
#define GRP3S NULL, NULL, 10, NULL, USE_GROUPS
#define GRP4  NULL, NULL, 11, NULL, USE_GROUPS
#define GRP5  NULL, NULL, 12, NULL, USE_GROUPS
#define GRP6  NULL, NULL, 13, NULL, USE_GROUPS
#define GRP7 NULL, NULL, 14, NULL, USE_GROUPS
#define GRP8 NULL, NULL, 15, NULL, USE_GROUPS
#define GRP9 NULL, NULL, 16, NULL, USE_GROUPS
#define GRP10 NULL, NULL, 17, NULL, USE_GROUPS
#define GRP11 NULL, NULL, 18, NULL, USE_GROUPS
#define GRP12 NULL, NULL, 19, NULL, USE_GROUPS
#define GRP13 NULL, NULL, 20, NULL, USE_GROUPS
#define GRP14 NULL, NULL, 21, NULL, USE_GROUPS
#define GRPAMD NULL, NULL, 22, NULL, USE_GROUPS

#define PREGRP0 NULL, NULL, 0, NULL, USE_PREFIX_USER_TABLE
#define PREGRP1 NULL, NULL, 1, NULL, USE_PREFIX_USER_TABLE
#define PREGRP2 NULL, NULL, 2, NULL, USE_PREFIX_USER_TABLE
#define PREGRP3 NULL, NULL, 3, NULL, USE_PREFIX_USER_TABLE
#define PREGRP4 NULL, NULL, 4, NULL, USE_PREFIX_USER_TABLE
#define PREGRP5 NULL, NULL, 5, NULL, USE_PREFIX_USER_TABLE
#define PREGRP6 NULL, NULL, 6, NULL, USE_PREFIX_USER_TABLE
#define PREGRP7 NULL, NULL, 7, NULL, USE_PREFIX_USER_TABLE
#define PREGRP8 NULL, NULL, 8, NULL, USE_PREFIX_USER_TABLE
#define PREGRP9 NULL, NULL, 9, NULL, USE_PREFIX_USER_TABLE
#define PREGRP10 NULL, NULL, 10, NULL, USE_PREFIX_USER_TABLE
#define PREGRP11 NULL, NULL, 11, NULL, USE_PREFIX_USER_TABLE
#define PREGRP12 NULL, NULL, 12, NULL, USE_PREFIX_USER_TABLE
#define PREGRP13 NULL, NULL, 13, NULL, USE_PREFIX_USER_TABLE
#define PREGRP14 NULL, NULL, 14, NULL, USE_PREFIX_USER_TABLE

#define FLOATCODE 50
#define FLOAT NULL, NULL, FLOATCODE

struct dis386 {
  char *name;
  op_rtn op1;
  int bytemode1;
  op_rtn op2;
  int bytemode2;
  op_rtn op3;
  int bytemode3;
};

/* Upper case letters in the instruction names here are macros.
   'A' => print 'b' if no register operands or suffix_always is true
   'B' => print 'b' if suffix_always is true
   'E' => print 'e' if 32-bit form of jcxz
   'L' => print 'l' if suffix_always is true
   'N' => print 'n' if instruction has no wait "prefix"
   'P' => print 'w' or 'l' if instruction has an operand size prefix,
                              or suffix_always is true
   'Q' => print 'w' or 'l' if no register operands or suffix_always is true
   'R' => print 'w' or 'l'
   'S' => print 'w' or 'l' if suffix_always is true
   'W' => print 'b' or 'w'
*/

static struct dis386 dis386_att[] = {
  /* 00 */
  { "addB",	Eb, Gb },
  { "addS",	Ev, Gv },
  { "addB",	Gb, Eb },
  { "addS",	Gv, Ev },
  { "addB",	AL, Ib },
  { "addS",	eAX, Iv },
  { "pushP",	es },
  { "popP",	es },
  /* 08 */
  { "orB",	Eb, Gb },
  { "orS",	Ev, Gv },
  { "orB",	Gb, Eb },
  { "orS",	Gv, Ev },
  { "orB",	AL, Ib },
  { "orS",	eAX, Iv },
  { "pushP",	cs },
  { "(bad)" },	/* 0x0f extended opcode escape */
  /* 10 */
  { "adcB",	Eb, Gb },
  { "adcS",	Ev, Gv },
  { "adcB",	Gb, Eb },
  { "adcS",	Gv, Ev },
  { "adcB",	AL, Ib },
  { "adcS",	eAX, Iv },
  { "pushP",	ss },
  { "popP",	ss },
  /* 18 */
  { "sbbB",	Eb, Gb },
  { "sbbS",	Ev, Gv },
  { "sbbB",	Gb, Eb },
  { "sbbS",	Gv, Ev },
  { "sbbB",	AL, Ib },
  { "sbbS",	eAX, Iv },
  { "pushP",	ds },
  { "popP",	ds },
  /* 20 */
  { "andB",	Eb, Gb },
  { "andS",	Ev, Gv },
  { "andB",	Gb, Eb },
  { "andS",	Gv, Ev },
  { "andB",	AL, Ib },
  { "andS",	eAX, Iv },
  { "(bad)" },			/* SEG ES prefix */
  { "daa" },
  /* 28 */
  { "subB",	Eb, Gb },
  { "subS",	Ev, Gv },
  { "subB",	Gb, Eb },
  { "subS",	Gv, Ev },
  { "subB",	AL, Ib },
  { "subS",	eAX, Iv },
  { "(bad)" },			/* SEG CS prefix */
  { "das" },
  /* 30 */
  { "xorB",	Eb, Gb },
  { "xorS",	Ev, Gv },
  { "xorB",	Gb, Eb },
  { "xorS",	Gv, Ev },
  { "xorB",	AL, Ib },
  { "xorS",	eAX, Iv },
  { "(bad)" },			/* SEG SS prefix */
  { "aaa" },
  /* 38 */
  { "cmpB",	Eb, Gb },
  { "cmpS",	Ev, Gv },
  { "cmpB",	Gb, Eb },
  { "cmpS",	Gv, Ev },
  { "cmpB",	AL, Ib },
  { "cmpS",	eAX, Iv },
  { "(bad)" },			/* SEG DS prefix */
  { "aas" },
  /* 40 */
  { "incS",	eAX },
  { "incS",	eCX },
  { "incS",	eDX },
  { "incS",	eBX },
  { "incS",	eSP },
  { "incS",	eBP },
  { "incS",	eSI },
  { "incS",	eDI },
  /* 48 */
  { "decS",	eAX },
  { "decS",	eCX },
  { "decS",	eDX },
  { "decS",	eBX },
  { "decS",	eSP },
  { "decS",	eBP },
  { "decS",	eSI },
  { "decS",	eDI },
  /* 50 */
  { "pushS",	eAX },
  { "pushS",	eCX },
  { "pushS",	eDX },
  { "pushS",	eBX },
  { "pushS",	eSP },
  { "pushS",	eBP },
  { "pushS",	eSI },
  { "pushS",	eDI },
  /* 58 */
  { "popS",	eAX },
  { "popS",	eCX },
  { "popS",	eDX },
  { "popS",	eBX },
  { "popS",	eSP },
  { "popS",	eBP },
  { "popS",	eSI },
  { "popS",	eDI },
  /* 60 */
  { "pushaP" },
  { "popaP" },
  { "boundS",	Gv, Ma },
  { "arpl",	Ew, Gw },
  { "(bad)" },			/* seg fs */
  { "(bad)" },			/* seg gs */
  { "(bad)" },			/* op size prefix */
  { "(bad)" },			/* adr size prefix */
  /* 68 */
  { "pushP",	Iv },		/* 386 book wrong */
  { "imulS",	Gv, Ev, Iv },
  { "pushP",	sIb },		/* push of byte really pushes 2 or 4 bytes */
  { "imulS",	Gv, Ev, sIb },
  { "insb",	Yb, indirDX },
  { "insR",	Yv, indirDX },
  { "outsb",	indirDX, Xb },
  { "outsR",	indirDX, Xv },
  /* 70 */
  { "jo",	Jb },
  { "jno",	Jb },
  { "jb",	Jb },
  { "jae",	Jb },
  { "je",	Jb },
  { "jne",	Jb },
  { "jbe",	Jb },
  { "ja",	Jb },
  /* 78 */
  { "js",	Jb },
  { "jns",	Jb },
  { "jp",	Jb },
  { "jnp",	Jb },
  { "jl",	Jb },
  { "jge",	Jb },
  { "jle",	Jb },
  { "jg",	Jb },
  /* 80 */
  { GRP1b },
  { GRP1S },
  { "(bad)" },
  { GRP1Ss },
  { "testB",	Eb, Gb },
  { "testS",	Ev, Gv },
  { "xchgB",	Eb, Gb },
  { "xchgS",	Ev, Gv },
  /* 88 */
  { "movB",	Eb, Gb },
  { "movS",	Ev, Gv },
  { "movB",	Gb, Eb },
  { "movS",	Gv, Ev },
  { "movQ",	Ev, Sw },
  { "leaS",	Gv, M },
  { "movQ",	Sw, Ev },
  { "popQ",	Ev },
  /* 90 */
  { "nop" },
  { "xchgS",	eCX, eAX },
  { "xchgS",	eDX, eAX },
  { "xchgS",	eBX, eAX },
  { "xchgS",	eSP, eAX },
  { "xchgS",	eBP, eAX },
  { "xchgS",	eSI, eAX },
  { "xchgS",	eDI, eAX },
  /* 98 */
  { "cWtR" },
  { "cRtd" },
  { "lcallP",	Ap },
  { "(bad)" },		/* fwait */
  { "pushfP" },
  { "popfP" },
  { "sahf" },
  { "lahf" },
  /* a0 */
  { "movB",	AL, Ob },
  { "movS",	eAX, Ov },
  { "movB",	Ob, AL },
  { "movS",	Ov, eAX },
  { "movsb",	Yb, Xb },
  { "movsR",	Yv, Xv },
  { "cmpsb",	Xb, Yb },
  { "cmpsR",	Xv, Yv },
  /* a8 */
  { "testB",	AL, Ib },
  { "testS",	eAX, Iv },
  { "stosB",	Yb, AL },
  { "stosS",	Yv, eAX },
  { "lodsB",	AL, Xb },
  { "lodsS",	eAX, Xv },
  { "scasB",	AL, Yb },
  { "scasS",	eAX, Yv },
  /* b0 */
  { "movB",	AL, Ib },
  { "movB",	CL, Ib },
  { "movB",	DL, Ib },
  { "movB",	BL, Ib },
  { "movB",	AH, Ib },
  { "movB",	CH, Ib },
  { "movB",	DH, Ib },
  { "movB",	BH, Ib },
  /* b8 */
  { "movS",	eAX, Iv },
  { "movS",	eCX, Iv },
  { "movS",	eDX, Iv },
  { "movS",	eBX, Iv },
  { "movS",	eSP, Iv },
  { "movS",	eBP, Iv },
  { "movS",	eSI, Iv },
  { "movS",	eDI, Iv },
  /* c0 */
  { GRP2b },
  { GRP2S },
  { "retP",	Iw },
  { "retP" },
  { "lesS",	Gv, Mp },
  { "ldsS",	Gv, Mp },
  { "movA",	Eb, Ib },
  { "movQ",	Ev, Iv },
  /* c8 */
  { "enterP",	Iw, Ib },
  { "leaveP" },
  { "lretP",	Iw },
  { "lretP" },
  { "int3" },
  { "int",	Ib },
  { "into" },
  { "iretP" },
  /* d0 */
  { GRP2b_one },
  { GRP2S_one },
  { GRP2b_cl },
  { GRP2S_cl },
  { "aam",	sIb },
  { "aad",	sIb },
  { "(bad)" },
  { "xlat",	DSBX },
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
  { "loopne",	Jb },
  { "loope",	Jb },
  { "loop",	Jb },
  { "jEcxz",	Jb },
  { "inB",	AL, Ib },
  { "inS",	eAX, Ib },
  { "outB",	Ib, AL },
  { "outS",	Ib, eAX },
  /* e8 */
  { "callP",	Jv },
  { "jmpP",	Jv },
  { "ljmpP",	Ap },
  { "jmp",	Jb },
  { "inB",	AL, indirDX },
  { "inS",	eAX, indirDX },
  { "outB",	indirDX, AL },
  { "outS",	indirDX, eAX },
  /* f0 */
  { "(bad)" },			/* lock prefix */
  { "(bad)" },
  { "(bad)" },			/* repne */
  { "(bad)" },			/* repz */
  { "hlt" },
  { "cmc" },
  { GRP3b },
  { GRP3S },
  /* f8 */
  { "clc" },
  { "stc" },
  { "cli" },
  { "sti" },
  { "cld" },
  { "std" },
  { GRP4 },
  { GRP5 },
};

static struct dis386 dis386_intel[] = {
  /* 00 */
  { "add",	Eb, Gb },
  { "add",	Ev, Gv },
  { "add",	Gb, Eb },
  { "add",	Gv, Ev },
  { "add",	AL, Ib },
  { "add",	eAX, Iv },
  { "push",	es },
  { "pop",	es },
  /* 08 */
  { "or",	Eb, Gb },
  { "or",	Ev, Gv },
  { "or",	Gb, Eb },
  { "or",	Gv, Ev },
  { "or",	AL, Ib },
  { "or",	eAX, Iv },
  { "push",	cs },
  { "(bad)" },	/* 0x0f extended opcode escape */
  /* 10 */
  { "adc",	Eb, Gb },
  { "adc",	Ev, Gv },
  { "adc",	Gb, Eb },
  { "adc",	Gv, Ev },
  { "adc",	AL, Ib },
  { "adc",	eAX, Iv },
  { "push",	ss },
  { "pop",	ss },
  /* 18 */
  { "sbb",	Eb, Gb },
  { "sbb",	Ev, Gv },
  { "sbb",	Gb, Eb },
  { "sbb",	Gv, Ev },
  { "sbb",	AL, Ib },
  { "sbb",	eAX, Iv },
  { "push",	ds },
  { "pop",	ds },
  /* 20 */
  { "and",	Eb, Gb },
  { "and",	Ev, Gv },
  { "and",	Gb, Eb },
  { "and",	Gv, Ev },
  { "and",	AL, Ib },
  { "and",	eAX, Iv },
  { "(bad)" },			/* SEG ES prefix */
  { "daa" },
  /* 28 */
  { "sub",	Eb, Gb },
  { "sub",	Ev, Gv },
  { "sub",	Gb, Eb },
  { "sub",	Gv, Ev },
  { "sub",	AL, Ib },
  { "sub",	eAX, Iv },
  { "(bad)" },			/* SEG CS prefix */
  { "das" },
  /* 30 */
  { "xor",	Eb, Gb },
  { "xor",	Ev, Gv },
  { "xor",	Gb, Eb },
  { "xor",	Gv, Ev },
  { "xor",	AL, Ib },
  { "xor",	eAX, Iv },
  { "(bad)" },			/* SEG SS prefix */
  { "aaa" },
  /* 38 */
  { "cmp",	Eb, Gb },
  { "cmp",	Ev, Gv },
  { "cmp",	Gb, Eb },
  { "cmp",	Gv, Ev },
  { "cmp",	AL, Ib },
  { "cmp",	eAX, Iv },
  { "(bad)" },			/* SEG DS prefix */
  { "aas" },
  /* 40 */
  { "inc",	eAX },
  { "inc",	eCX },
  { "inc",	eDX },
  { "inc",	eBX },
  { "inc",	eSP },
  { "inc",	eBP },
  { "inc",	eSI },
  { "inc",	eDI },
  /* 48 */
  { "dec",	eAX },
  { "dec",	eCX },
  { "dec",	eDX },
  { "dec",	eBX },
  { "dec",	eSP },
  { "dec",	eBP },
  { "dec",	eSI },
  { "dec",	eDI },
  /* 50 */
  { "push",	eAX },
  { "push",	eCX },
  { "push",	eDX },
  { "push",	eBX },
  { "push",	eSP },
  { "push",	eBP },
  { "push",	eSI },
  { "push",	eDI },
  /* 58 */
  { "pop",	eAX },
  { "pop",	eCX },
  { "pop",	eDX },
  { "pop",	eBX },
  { "pop",	eSP },
  { "pop",	eBP },
  { "pop",	eSI },
  { "pop",	eDI },
  /* 60 */
  { "pusha" },
  { "popa" },
  { "bound",	Gv, Ma },
  { "arpl",	Ew, Gw },
  { "(bad)" },			/* seg fs */
  { "(bad)" },			/* seg gs */
  { "(bad)" },			/* op size prefix */
  { "(bad)" },			/* adr size prefix */
  /* 68 */
  { "push",	Iv },		/* 386 book wrong */
  { "imul",	Gv, Ev, Iv },
  { "push",	sIb },		/* push of byte really pushes 2 or 4 bytes */
  { "imul",	Gv, Ev, sIb },
  { "ins",	Yb, indirDX },
  { "ins",	Yv, indirDX },
  { "outs",	indirDX, Xb },
  { "outs",	indirDX, Xv },
  /* 70 */
  { "jo",	Jb },
  { "jno",	Jb },
  { "jb",	Jb },
  { "jae",	Jb },
  { "je",	Jb },
  { "jne",	Jb },
  { "jbe",	Jb },
  { "ja",	Jb },
  /* 78 */
  { "js",	Jb },
  { "jns",	Jb },
  { "jp",	Jb },
  { "jnp",	Jb },
  { "jl",	Jb },
  { "jge",	Jb },
  { "jle",	Jb },
  { "jg",	Jb },
  /* 80 */
  { GRP1b },
  { GRP1S },
  { "(bad)" },
  { GRP1Ss },
  { "test",	Eb, Gb },
  { "test",	Ev, Gv },
  { "xchg",	Eb, Gb },
  { "xchg",	Ev, Gv },
  /* 88 */
  { "mov",	Eb, Gb },
  { "mov",	Ev, Gv },
  { "mov",	Gb, Eb },
  { "mov",	Gv, Ev },
  { "mov",	Ev, Sw },
  { "lea",	Gv, M },
  { "mov",	Sw, Ev },
  { "pop",	Ev },
  /* 90 */
  { "nop" },
  { "xchg",	eCX, eAX },
  { "xchg",	eDX, eAX },
  { "xchg",	eBX, eAX },
  { "xchg",	eSP, eAX },
  { "xchg",	eBP, eAX },
  { "xchg",	eSI, eAX },
  { "xchg",	eDI, eAX },
  /* 98 */
  { "cW" },		/* cwde and cbw */
  { "cR" },		/* cdq and cwd */
  { "lcall",	Ap },
  { "(bad)" },		/* fwait */
  { "pushf" },
  { "popf" },
  { "sahf" },
  { "lahf" },
  /* a0 */
  { "mov",	AL, Ob },
  { "mov",	eAX, Ov },
  { "mov",	Ob, AL },
  { "mov",	Ov, eAX },
  { "movs",	Yb, Xb },
  { "movs",	Yv, Xv },
  { "cmps",	Xb, Yb },
  { "cmps",	Xv, Yv },
  /* a8 */
  { "test",	AL, Ib },
  { "test",	eAX, Iv },
  { "stos",	Yb, AL },
  { "stos",	Yv, eAX },
  { "lods",	AL, Xb },
  { "lods",	eAX, Xv },
  { "scas",	AL, Yb },
  { "scas",	eAX, Yv },
  /* b0 */
  { "mov",	AL, Ib },
  { "mov",	CL, Ib },
  { "mov",	DL, Ib },
  { "mov",	BL, Ib },
  { "mov",	AH, Ib },
  { "mov",	CH, Ib },
  { "mov",	DH, Ib },
  { "mov",	BH, Ib },
  /* b8 */
  { "mov",	eAX, Iv },
  { "mov",	eCX, Iv },
  { "mov",	eDX, Iv },
  { "mov",	eBX, Iv },
  { "mov",	eSP, Iv },
  { "mov",	eBP, Iv },
  { "mov",	eSI, Iv },
  { "mov",	eDI, Iv },
  /* c0 */
  { GRP2b },
  { GRP2S },
  { "ret",	Iw },
  { "ret" },
  { "les",	Gv, Mp },
  { "lds",	Gv, Mp },
  { "mov",	Eb, Ib },
  { "mov",	Ev, Iv },
  /* c8 */
  { "enter",	Iw, Ib },
  { "leave" },
  { "lret",	Iw },
  { "lret" },
  { "int3" },
  { "int",	Ib },
  { "into" },
  { "iret" },
  /* d0 */
  { GRP2b_one },
  { GRP2S_one },
  { GRP2b_cl },
  { GRP2S_cl },
  { "aam",	sIb },
  { "aad",	sIb },
  { "(bad)" },
  { "xlat",	DSBX },
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
  { "loopne",	Jb },
  { "loope",	Jb },
  { "loop",	Jb },
  { "jEcxz",	Jb },
  { "in",	AL, Ib },
  { "in",	eAX, Ib },
  { "out",	Ib, AL },
  { "out",	Ib, eAX },
  /* e8 */
  { "call",	Jv },
  { "jmp",	Jv },
  { "ljmp",	Ap },
  { "jmp",	Jb },
  { "in",	AL, indirDX },
  { "in",	eAX, indirDX },
  { "out",	indirDX, AL },
  { "out",	indirDX, eAX },
  /* f0 */
  { "(bad)" },			/* lock prefix */
  { "(bad)" },
  { "(bad)" },			/* repne */
  { "(bad)" },			/* repz */
  { "hlt" },
  { "cmc" },
  { GRP3b },
  { GRP3S },
  /* f8 */
  { "clc" },
  { "stc" },
  { "cli" },
  { "sti" },
  { "cld" },
  { "std" },
  { GRP4 },
  { GRP5 },
};

static struct dis386 dis386_twobyte_att[] = {
  /* 00 */
  { GRP6 },
  { GRP7 },
  { "larS", Gv, Ew },
  { "lslS", Gv, Ew },
  { "(bad)" },
  { "(bad)" },
  { "clts" },
  { "(bad)" },
  /* 08 */
  { "invd" },
  { "wbinvd" },
  { "(bad)" },
  { "ud2a" },
  { "(bad)" },
  { GRPAMD },
  { "femms" },
  { "", MX, EM, OPSUF }, /* See OP_3DNowSuffix */
  /* 10 */
  { PREGRP8 },
  { PREGRP9 },
  { "movlps", XM, EX, SIMD_Fixup, 'h' },  /* really only 2 operands */
  { "movlps", EX, XM, SIMD_Fixup, 'h' },
  { "unpcklps", XM, EX },
  { "unpckhps", XM, EX },
  { "movhps", XM, EX, SIMD_Fixup, 'l' },
  { "movhps", EX, XM, SIMD_Fixup, 'l' },
  /* 18 */
  { GRP14 },
  { "(bad)" },  { "(bad)" },  { "(bad)" },  
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },
  /* 20 */
  /* these are all backward in appendix A of the intel book */
  { "movL", Rd, Cd },
  { "movL", Rd, Dd },
  { "movL", Cd, Rd },
  { "movL", Dd, Rd },
  { "movL", Rd, Td },
  { "(bad)" },
  { "movL", Td, Rd },
  { "(bad)" },
  /* 28 */
  { "movaps", XM, EX },
  { "movaps", EX, XM },
  { PREGRP2 },
  { "movntps", Ev, XM },
  { PREGRP4 },  
  { PREGRP3 },
  { "ucomiss", XM, EX },
  { "comiss", XM, EX },  
  /* 30 */
  { "wrmsr" },  { "rdtsc" },  { "rdmsr" },  { "rdpmc" },
  { "sysenter" },  { "sysexit" },  { "(bad)" },  { "(bad)" },
  /* 38 */
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },
  /* 40 */
  { "cmovo", Gv,Ev }, { "cmovno", Gv,Ev }, { "cmovb", Gv,Ev }, { "cmovae", Gv,Ev },
  { "cmove", Gv,Ev }, { "cmovne", Gv,Ev }, { "cmovbe", Gv,Ev }, { "cmova", Gv,Ev },
  /* 48 */
  { "cmovs", Gv,Ev }, { "cmovns", Gv,Ev }, { "cmovp", Gv,Ev }, { "cmovnp", Gv,Ev },
  { "cmovl", Gv,Ev }, { "cmovge", Gv,Ev }, { "cmovle", Gv,Ev }, { "cmovg", Gv,Ev },
  /* 50 */
  { "movmskps", Gv, EX },
  { PREGRP13 },
  { PREGRP12 },
  { PREGRP11 },
  { "andps", XM, EX },
  { "andnps", XM, EX },
  { "orps", XM, EX },
  { "xorps", XM, EX },  
  /* 58 */
  { PREGRP0 },
  { PREGRP10 },
  { "(bad)" },
  { "(bad)" },  
  { PREGRP14 },
  { PREGRP7 },
  { PREGRP5 },
  { PREGRP6 },  
  /* 60 */
  { "punpcklbw", MX, EM },
  { "punpcklwd", MX, EM },
  { "punpckldq", MX, EM },
  { "packsswb", MX, EM },
  { "pcmpgtb", MX, EM },
  { "pcmpgtw", MX, EM },
  { "pcmpgtd", MX, EM },
  { "packuswb", MX, EM },
  /* 68 */
  { "punpckhbw", MX, EM },
  { "punpckhwd", MX, EM },
  { "punpckhdq", MX, EM },
  { "packssdw", MX, EM },
  { "(bad)" },  { "(bad)" },
  { "movd", MX, Ev },
  { "movq", MX, EM },
  /* 70 */
  { "pshufw", MX, EM, Ib },
  { GRP10 },
  { GRP11 },
  { GRP12 },
  { "pcmpeqb", MX, EM },
  { "pcmpeqw", MX, EM },
  { "pcmpeqd", MX, EM },
  { "emms" },
  /* 78 */
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },
  { "(bad)" },  { "(bad)" },
  { "movd", Ev, MX },
  { "movq", EM, MX },
  /* 80 */
  { "jo", Jv },
  { "jno", Jv },
  { "jb", Jv },
  { "jae", Jv },
  { "je", Jv },
  { "jne", Jv },
  { "jbe", Jv },
  { "ja", Jv },
  /* 88 */
  { "js", Jv },
  { "jns", Jv },
  { "jp", Jv },
  { "jnp", Jv },
  { "jl", Jv },
  { "jge", Jv },
  { "jle", Jv },
  { "jg", Jv },
  /* 90 */
  { "seto", Eb },
  { "setno", Eb },
  { "setb", Eb },
  { "setae", Eb },
  { "sete", Eb },
  { "setne", Eb },
  { "setbe", Eb },
  { "seta", Eb },
  /* 98 */
  { "sets", Eb },
  { "setns", Eb },
  { "setp", Eb },
  { "setnp", Eb },
  { "setl", Eb },
  { "setge", Eb },
  { "setle", Eb },
  { "setg", Eb },
  /* a0 */
  { "pushP", fs },
  { "popP", fs },
  { "cpuid" },
  { "btS", Ev, Gv },
  { "shldS", Ev, Gv, Ib },
  { "shldS", Ev, Gv, CL },
  { "(bad)" },
  { "(bad)" },
  /* a8 */
  { "pushP", gs },
  { "popP", gs },
  { "rsm" },
  { "btsS", Ev, Gv },
  { "shrdS", Ev, Gv, Ib },
  { "shrdS", Ev, Gv, CL },
  { GRP13 },
  { "imulS", Gv, Ev },
  /* b0 */
  { "cmpxchgB", Eb, Gb },
  { "cmpxchgS", Ev, Gv },
  { "lssS", Gv, Mp },	/* 386 lists only Mp */
  { "btrS", Ev, Gv },
  { "lfsS", Gv, Mp },	/* 386 lists only Mp */
  { "lgsS", Gv, Mp },	/* 386 lists only Mp */
  { "movzbR", Gv, Eb },
  { "movzwR", Gv, Ew }, /* yes, there really is movzww ! */
  /* b8 */
  { "(bad)" },
  { "ud2b" },
  { GRP8 },
  { "btcS", Ev, Gv },
  { "bsfS", Gv, Ev },
  { "bsrS", Gv, Ev },
  { "movsbR", Gv, Eb },
  { "movswR", Gv, Ew }, /* yes, there really is movsww ! */
  /* c0 */
  { "xaddB", Eb, Gb },
  { "xaddS", Ev, Gv },
  { PREGRP1 },
  { "(bad)" },
  { "pinsrw", MX, Ev, Ib },
  { "pextrw", Ev, MX, Ib },
  { "shufps", XM, EX, Ib },
  { GRP9 },
  /* c8 */
  { "bswap", eAX },	/* bswap doesn't support 16 bit regs */
  { "bswap", eCX },
  { "bswap", eDX },
  { "bswap", eBX },
  { "bswap", eSP },
  { "bswap", eBP },
  { "bswap", eSI },
  { "bswap", eDI },
  /* d0 */
  { "(bad)" },
  { "psrlw", MX, EM },
  { "psrld", MX, EM },
  { "psrlq", MX, EM },
  { "(bad)" },
  { "pmullw", MX, EM },
  { "(bad)" },
  { "pmovmskb", Ev, MX },  
  /* d8 */
  { "psubusb", MX, EM },
  { "psubusw", MX, EM },
  { "pminub", MX, EM },
  { "pand", MX, EM },
  { "paddusb", MX, EM },
  { "paddusw", MX, EM },
  { "pmaxub", MX, EM },
  { "pandn", MX, EM },
  /* e0 */
  { "pavgb", MX, EM },
  { "psraw", MX, EM },
  { "psrad", MX, EM },
  { "pavgw", MX, EM },
  { "pmulhuw", MX, EM },
  { "pmulhw", MX, EM },
  { "(bad)" },
  { "movntq", Ev, MX },  
  /* e8 */
  { "psubsb", MX, EM },
  { "psubsw", MX, EM },
  { "pminsw", MX, EM },
  { "por", MX, EM },
  { "paddsb", MX, EM },
  { "paddsw", MX, EM },
  { "pmaxsw", MX, EM },
  { "pxor", MX, EM },
  /* f0 */
  { "(bad)" },
  { "psllw", MX, EM },
  { "pslld", MX, EM },
  { "psllq", MX, EM },
  { "(bad)" },
  { "pmaddwd", MX, EM },
  { "psadbw", MX, EM },
  { "maskmovq", MX, EM },  
  /* f8 */
  { "psubb", MX, EM },
  { "psubw", MX, EM },
  { "psubd", MX, EM },
  { "(bad)" },
  { "paddb", MX, EM },
  { "paddw", MX, EM },
  { "paddd", MX, EM },
  { "(bad)" }
};

static struct dis386 dis386_twobyte_intel[] = {
  /* 00 */
  { GRP6 },
  { GRP7 },
  { "lar", Gv, Ew },
  { "lsl", Gv, Ew },
  { "(bad)" },
  { "(bad)" },
  { "clts" },
  { "(bad)" },
  /* 08 */
  { "invd" },
  { "wbinvd" },
  { "(bad)" },
  { "ud2a" },
  { "(bad)" },
  { GRPAMD },
  { "femms" },
  { "", MX, EM, OPSUF }, /* See OP_3DNowSuffix */
  /* 10 */
  { PREGRP8 },
  { PREGRP9 },
  { "movlps", XM, EX, SIMD_Fixup, 'h' },  /* really only 2 operands */
  { "movlps", EX, XM, SIMD_Fixup, 'h' },
  { "unpcklps", XM, EX },
  { "unpckhps", XM, EX },
  { "movhps", XM, EX, SIMD_Fixup, 'l' },
  { "movhps", EX, XM, SIMD_Fixup, 'l' },
  /* 18 */
  { GRP14 },
  { "(bad)" },  { "(bad)" },  { "(bad)" },  
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },
  /* 20 */
  /* these are all backward in appendix A of the intel book */
  { "mov", Rd, Cd },
  { "mov", Rd, Dd },
  { "mov", Cd, Rd },
  { "mov", Dd, Rd },
  { "mov", Rd, Td },
  { "(bad)" },
  { "mov", Td, Rd },
  { "(bad)" },
  /* 28 */
  { "movaps", XM, EX },
  { "movaps", EX, XM },
  { PREGRP2 },
  { "movntps", Ev, XM },
  { PREGRP4 },  
  { PREGRP3 },
  { "ucomiss", XM, EX },
  { "comiss", XM, EX },  
  /* 30 */
  { "wrmsr" },  { "rdtsc" },  { "rdmsr" },  { "rdpmc" },
  { "sysenter" },  { "sysexit" },  { "(bad)" },  { "(bad)" },
  /* 38 */
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },
  /* 40 */
  { "cmovo", Gv,Ev }, { "cmovno", Gv,Ev }, { "cmovb", Gv,Ev }, { "cmovae", Gv,Ev },
  { "cmove", Gv,Ev }, { "cmovne", Gv,Ev }, { "cmovbe", Gv,Ev }, { "cmova", Gv,Ev },
  /* 48 */
  { "cmovs", Gv,Ev }, { "cmovns", Gv,Ev }, { "cmovp", Gv,Ev }, { "cmovnp", Gv,Ev },
  { "cmovl", Gv,Ev }, { "cmovge", Gv,Ev }, { "cmovle", Gv,Ev }, { "cmovg", Gv,Ev },
  /* 50 */
  { "movmskps", Gv, EX },
  { PREGRP13 },
  { PREGRP12 },
  { PREGRP11 },
  { "andps", XM, EX },
  { "andnps", XM, EX },
  { "orps", XM, EX },
  { "xorps", XM, EX },  
  /* 58 */
  { PREGRP0 },
  { PREGRP10 },
  { "(bad)" },
  { "(bad)" },  
  { PREGRP14 },
  { PREGRP7 },
  { PREGRP5 },
  { PREGRP6 },  
  /* 60 */
  { "punpcklbw", MX, EM },
  { "punpcklwd", MX, EM },
  { "punpckldq", MX, EM },
  { "packsswb", MX, EM },
  { "pcmpgtb", MX, EM },
  { "pcmpgtw", MX, EM },
  { "pcmpgtd", MX, EM },
  { "packuswb", MX, EM },
  /* 68 */
  { "punpckhbw", MX, EM },
  { "punpckhwd", MX, EM },
  { "punpckhdq", MX, EM },
  { "packssdw", MX, EM },
  { "(bad)" },  { "(bad)" },
  { "movd", MX, Ev },
  { "movq", MX, EM },
  /* 70 */
  { "pshufw", MX, EM, Ib },
  { GRP10 },
  { GRP11 },
  { GRP12 },
  { "pcmpeqb", MX, EM },
  { "pcmpeqw", MX, EM },
  { "pcmpeqd", MX, EM },
  { "emms" },
  /* 78 */
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },
  { "(bad)" },  { "(bad)" },
  { "movd", Ev, MX },
  { "movq", EM, MX },
  /* 80 */
  { "jo", Jv },
  { "jno", Jv },
  { "jb", Jv },
  { "jae", Jv },
  { "je", Jv },
  { "jne", Jv },
  { "jbe", Jv },
  { "ja", Jv },
  /* 88 */
  { "js", Jv },
  { "jns", Jv },
  { "jp", Jv },
  { "jnp", Jv },
  { "jl", Jv },
  { "jge", Jv },
  { "jle", Jv },
  { "jg", Jv },
  /* 90 */
  { "seto", Eb },
  { "setno", Eb },
  { "setb", Eb },
  { "setae", Eb },
  { "sete", Eb },
  { "setne", Eb },
  { "setbe", Eb },
  { "seta", Eb },
  /* 98 */
  { "sets", Eb },
  { "setns", Eb },
  { "setp", Eb },
  { "setnp", Eb },
  { "setl", Eb },
  { "setge", Eb },
  { "setle", Eb },
  { "setg", Eb },
  /* a0 */
  { "push", fs },
  { "pop", fs },
  { "cpuid" },
  { "bt", Ev, Gv },
  { "shld", Ev, Gv, Ib },
  { "shld", Ev, Gv, CL },
  { "(bad)" },
  { "(bad)" },
  /* a8 */
  { "push", gs },
  { "pop", gs },
  { "rsm" },
  { "bts", Ev, Gv },
  { "shrd", Ev, Gv, Ib },
  { "shrd", Ev, Gv, CL },
  { GRP13 },
  { "imul", Gv, Ev },
  /* b0 */
  { "cmpxchg", Eb, Gb },
  { "cmpxchg", Ev, Gv },
  { "lss", Gv, Mp },	/* 386 lists only Mp */
  { "btr", Ev, Gv },
  { "lfs", Gv, Mp },	/* 386 lists only Mp */
  { "lgs", Gv, Mp },	/* 386 lists only Mp */
  { "movzx", Gv, Eb },
  { "movzx", Gv, Ew },
  /* b8 */
  { "(bad)" },
  { "ud2b" },
  { GRP8 },
  { "btc", Ev, Gv },
  { "bsf", Gv, Ev },
  { "bsr", Gv, Ev },
  { "movsx", Gv, Eb },
  { "movsx", Gv, Ew },
  /* c0 */
  { "xadd", Eb, Gb },
  { "xadd", Ev, Gv },
  { PREGRP1 },
  { "(bad)" },
  { "pinsrw", MX, Ev, Ib },
  { "pextrw", Ev, MX, Ib },
  { "shufps", XM, EX, Ib },
  { GRP9 },
  /* c8 */
  { "bswap", eAX },	/* bswap doesn't support 16 bit regs */
  { "bswap", eCX },
  { "bswap", eDX },
  { "bswap", eBX },
  { "bswap", eSP },
  { "bswap", eBP },
  { "bswap", eSI },
  { "bswap", eDI },
  /* d0 */
  { "(bad)" },
  { "psrlw", MX, EM },
  { "psrld", MX, EM },
  { "psrlq", MX, EM },
  { "(bad)" },
  { "pmullw", MX, EM },
  { "(bad)" },
  { "pmovmskb", Ev, MX },  
  /* d8 */
  { "psubusb", MX, EM },
  { "psubusw", MX, EM },
  { "pminub", MX, EM },
  { "pand", MX, EM },
  { "paddusb", MX, EM },
  { "paddusw", MX, EM },
  { "pmaxub", MX, EM },
  { "pandn", MX, EM },
  /* e0 */
  { "pavgb", MX, EM },
  { "psraw", MX, EM },
  { "psrad", MX, EM },
  { "pavgw", MX, EM },
  { "pmulhuw", MX, EM },
  { "pmulhw", MX, EM },
  { "(bad)" },
  { "movntq", Ev, MX },  
  /* e8 */
  { "psubsb", MX, EM },
  { "psubsw", MX, EM },
  { "pminsw", MX, EM },
  { "por", MX, EM },
  { "paddsb", MX, EM },
  { "paddsw", MX, EM },
  { "pmaxsw", MX, EM },
  { "pxor", MX, EM },
  /* f0 */
  { "(bad)" },
  { "psllw", MX, EM },
  { "pslld", MX, EM },
  { "psllq", MX, EM },
  { "(bad)" },
  { "pmaddwd", MX, EM },
  { "psadbw", MX, EM },
  { "maskmovq", MX, EM },  
  /* f8 */
  { "psubb", MX, EM },
  { "psubw", MX, EM },
  { "psubd", MX, EM },
  { "(bad)" },
  { "paddb", MX, EM },
  { "paddw", MX, EM },
  { "paddd", MX, EM },
  { "(bad)" }
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
  /* 10 */ 1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0, /* 1f */
  /* 20 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 2f */
  /* 30 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 3f */
  /* 40 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 4f */
  /* 50 */ 1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1, /* 5f */
  /* 60 */ 1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1, /* 6f */
  /* 70 */ 1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1, /* 7f */
  /* 80 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8f */
  /* 90 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 9f */
  /* a0 */ 0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1, /* af */
  /* b0 */ 1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1, /* bf */
  /* c0 */ 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0, /* cf */
  /* d0 */ 0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1, /* df */
  /* e0 */ 1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1, /* ef */
  /* f0 */ 0,1,1,1,0,1,1,1,1,1,1,0,1,1,1,0  /* ff */
  /*       -------------------------------        */
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
};

static const unsigned char twobyte_uses_f3_prefix[256] = {
  /*       0 1 2 3 4 5 6 7 8 9 a b c d e f        */
  /*       -------------------------------        */
  /* 00 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0f */
  /* 10 */ 1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 1f */
  /* 20 */ 0,0,0,0,0,0,0,0,0,0,1,0,1,1,0,0, /* 2f */
  /* 30 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 3f */
  /* 40 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 4f */
  /* 50 */ 0,1,1,1,0,0,0,0,1,1,0,0,1,1,1,1, /* 5f */
  /* 60 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 6f */
  /* 70 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 7f */
  /* 80 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8f */
  /* 90 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 9f */
  /* a0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* af */
  /* b0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* bf */
  /* c0 */ 0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0, /* cf */
  /* d0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* df */
  /* e0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* ef */
  /* f0 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0  /* ff */
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
static void oappend PARAMS ((char *s));

static char *names32[]={
  "%eax","%ecx","%edx","%ebx", "%esp","%ebp","%esi","%edi",
};
static char *names16[] = {
  "%ax","%cx","%dx","%bx","%sp","%bp","%si","%di",
};
static char *names8[] = {
  "%al","%cl","%dl","%bl","%ah","%ch","%dh","%bh",
};
static char *names_seg[] = {
  "%es","%cs","%ss","%ds","%fs","%gs","%?","%?",
};
static char *index16[] = {
  "%bx,%si","%bx,%di","%bp,%si","%bp,%di","%si","%di","%bp","%bx"
};

static struct dis386 grps[][8] = {
  /* GRP1b */
  {
    { "addA",	Eb, Ib },
    { "orA",	Eb, Ib },
    { "adcA",	Eb, Ib },
    { "sbbA",	Eb, Ib },
    { "andA",	Eb, Ib },
    { "subA",	Eb, Ib },
    { "xorA",	Eb, Ib },
    { "cmpA",	Eb, Ib }
  },
  /* GRP1S */
  {
    { "addQ",	Ev, Iv },
    { "orQ",	Ev, Iv },
    { "adcQ",	Ev, Iv },
    { "sbbQ",	Ev, Iv },
    { "andQ",	Ev, Iv },
    { "subQ",	Ev, Iv },
    { "xorQ",	Ev, Iv },
    { "cmpQ",	Ev, Iv }
  },
  /* GRP1Ss */
  {
    { "addQ",	Ev, sIb },
    { "orQ",	Ev, sIb },
    { "adcQ",	Ev, sIb },
    { "sbbQ",	Ev, sIb },
    { "andQ",	Ev, sIb },
    { "subQ",	Ev, sIb },
    { "xorQ",	Ev, sIb },
    { "cmpQ",	Ev, sIb }
  },
  /* GRP2b */
  {
    { "rolA",	Eb, Ib },
    { "rorA",	Eb, Ib },
    { "rclA",	Eb, Ib },
    { "rcrA",	Eb, Ib },
    { "shlA",	Eb, Ib },
    { "shrA",	Eb, Ib },
    { "(bad)" },
    { "sarA",	Eb, Ib },
  },
  /* GRP2S */
  {
    { "rolQ",	Ev, Ib },
    { "rorQ",	Ev, Ib },
    { "rclQ",	Ev, Ib },
    { "rcrQ",	Ev, Ib },
    { "shlQ",	Ev, Ib },
    { "shrQ",	Ev, Ib },
    { "(bad)" },
    { "sarQ",	Ev, Ib },
  },
  /* GRP2b_one */
  {
    { "rolA",	Eb },
    { "rorA",	Eb },
    { "rclA",	Eb },
    { "rcrA",	Eb },
    { "shlA",	Eb },
    { "shrA",	Eb },
    { "(bad)" },
    { "sarA",	Eb },
  },
  /* GRP2S_one */
  {
    { "rolQ",	Ev },
    { "rorQ",	Ev },
    { "rclQ",	Ev },
    { "rcrQ",	Ev },
    { "shlQ",	Ev },
    { "shrQ",	Ev },
    { "(bad)" },
    { "sarQ",	Ev },
  },
  /* GRP2b_cl */
  {
    { "rolA",	Eb, CL },
    { "rorA",	Eb, CL },
    { "rclA",	Eb, CL },
    { "rcrA",	Eb, CL },
    { "shlA",	Eb, CL },
    { "shrA",	Eb, CL },
    { "(bad)" },
    { "sarA",	Eb, CL },
  },
  /* GRP2S_cl */
  {
    { "rolQ",	Ev, CL },
    { "rorQ",	Ev, CL },
    { "rclQ",	Ev, CL },
    { "rcrQ",	Ev, CL },
    { "shlQ",	Ev, CL },
    { "shrQ",	Ev, CL },
    { "(bad)" },
    { "sarQ",	Ev, CL }
  },
  /* GRP3b */
  {
    { "testA",	Eb, Ib },
    { "(bad)",	Eb },
    { "notA",	Eb },
    { "negA",	Eb },
    { "mulB",	AL, Eb },
    { "imulB",	AL, Eb },
    { "divB",	AL, Eb },
    { "idivB",	AL, Eb }
  },
  /* GRP3S */
  {
    { "testQ",	Ev, Iv },
    { "(bad)" },
    { "notQ",	Ev },
    { "negQ",	Ev },
    { "mulS",	eAX, Ev },
    { "imulS",	eAX, Ev },
    { "divS",	eAX, Ev },
    { "idivS",	eAX, Ev },
  },
  /* GRP4 */
  {
    { "incA", Eb },
    { "decA", Eb },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
  },
  /* GRP5 */
  {
    { "incQ",	Ev },
    { "decQ",	Ev },
    { "callP",	indirEv },
    { "callP",	indirEv },
    { "jmpP",	indirEv },
    { "ljmpP",	indirEv },
    { "pushQ",	Ev },
    { "(bad)" },
  },
  /* GRP6 */
  {
    { "sldt",	Ew },
    { "str",	Ew },
    { "lldt",	Ew },
    { "ltr",	Ew },
    { "verr",	Ew },
    { "verw",	Ew },
    { "(bad)" },
    { "(bad)" }
  },
  /* GRP7 */
  {
    { "sgdt", Ew },
    { "sidt", Ew },
    { "lgdt", Ew },
    { "lidt", Ew },
    { "smsw", Ew },
    { "(bad)" },
    { "lmsw", Ew },
    { "invlpg", Ew },
  },
  /* GRP8 */
  {
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "btQ",	Ev, Ib },
    { "btsQ",	Ev, Ib },
    { "btrQ",	Ev, Ib },
    { "btcQ",	Ev, Ib },
  },
  /* GRP9 */
  {
    { "(bad)" },
    { "cmpxchg8b", Ev },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
  },
  /* GRP10 */
  {
    { "(bad)" },
    { "(bad)" },
    { "psrlw", MS, Ib },
    { "(bad)" },
    { "psraw", MS, Ib },
    { "(bad)" },
    { "psllw", MS, Ib },
    { "(bad)" },
  },
  /* GRP11 */
  {
    { "(bad)" },
    { "(bad)" },
    { "psrld", MS, Ib },
    { "(bad)" },
    { "psrad", MS, Ib },
    { "(bad)" },
    { "pslld", MS, Ib },
    { "(bad)" },
  },
  /* GRP12 */
  {
    { "(bad)" },
    { "(bad)" },
    { "psrlq", MS, Ib },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "psllq", MS, Ib },
    { "(bad)" },
  },
  /* GRP13 */
  {
    { "fxsave", Ev },
    { "fxrstor", Ev },
    { "ldmxcsr", Ev },
    { "stmxcsr", Ev },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "sfence", None },
  },
  /* GRP14 */
  {
    { "prefetchnta", Ev },
    { "prefetcht0", Ev },
    { "prefetcht1", Ev },
    { "prefetcht2", Ev },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
  },
  /* GRPAMD */
  {
    { "prefetch", Eb },
    { "prefetchw", Eb },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
  }

};

static struct dis386 prefix_user_table[][2] = {
  /* PREGRP0 */
  {
    { "addps", XM, EX },
    { "addss", XM, EX },
  },
  /* PREGRP1 */
  {
    { "", XM, EX, OPSIMD },	/* See OP_SIMD_SUFFIX */
    { "", XM, EX, OPSIMD },
  },
  /* PREGRP2 */
  {
    { "cvtpi2ps", XM, EM },
    { "cvtsi2ss", XM, Ev },
  },
  /* PREGRP3 */
  {
    { "cvtps2pi", MX, EX },
    { "cvtss2si", Gv, EX },
  },
  /* PREGRP4 */
  {
    { "cvttps2pi", MX, EX },  
    { "cvttss2si", Gv, EX },  
  },
  /* PREGRP5 */
  {
    { "divps", XM, EX },
    { "divss", XM, EX },
  },
  /* PREGRP6 */
  {
    { "maxps", XM, EX },
    { "maxss", XM, EX },
  },
  /* PREGRP7 */
  {
    { "minps", XM, EX },
    { "minss", XM, EX },
  },
  /* PREGRP8 */
  {
    { "movups", XM, EX },
    { "movss", XM, EX },
  },
  /* PREGRP9 */
  {
    { "movups", EX, XM },
    { "movss", EX, XM },
  },
  /* PREGRP10 */
  {
    { "mulps", XM, EX },
    { "mulss", XM, EX },
  },
  /* PREGRP11 */
  {
    { "rcpps", XM, EX },
    { "rcpss", XM, EX },
  },
  /* PREGRP12 */
  {
    { "rsqrtps", XM, EX },
    { "rsqrtss", XM, EX },
  },
  /* PREGRP13 */
  {
    { "sqrtps", XM, EX },
    { "sqrtss", XM, EX },
  },
  /* PREGRP14 */
  {
    { "subps", XM, EX },
    { "subss", XM, EX },
  }
};

#define INTERNAL_DISASSEMBLER_ERROR _("<internal disassembler error>")

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

static int prefixes;

static void
ckprefix ()
{
  prefixes = 0;
  while (1)
    {
      FETCH_DATA (the_info, codep + 1);
      switch (*codep)
	{
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
	case 0x9b:
	  /* fwait is really an instruction.  If there are prefixes
	     before the fwait, they belong to the fwait, *not* to the
	     following instruction.  */
	  if (prefixes)
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
      codep++;
    }
}

static char op1out[100], op2out[100], op3out[100];
static int op_ad, op_index[3];
static unsigned int op_address[3];
static unsigned int start_pc;


/*
 *   On the 386's of 1988, the maximum length of an instruction is 15 bytes.
 *   (see topic "Redundant prefixes" in the "Differences from 8086"
 *   section of the "Virtual 8086 Mode" chapter.)
 * 'pc' should be the address of this instruction, it will
 *   be used to print the target address if this is a relative jump or call
 * The function returns the length of this instruction in bytes.
 */

static int print_insn_x86
  PARAMS ((bfd_vma pc, disassemble_info *info, int sizeflag));
static int print_insn_i386
  PARAMS ((bfd_vma pc, disassemble_info *info));

static char intel_syntax;
static char open_char;
static char close_char;
static char separator_char;
static char scale_char;

int
print_insn_i386_att (pc, info)
     bfd_vma pc;
     disassemble_info *info;
{
  intel_syntax = 0;
  open_char = '(';
  close_char =  ')';
  separator_char = ',';
  scale_char = ',';

  return print_insn_i386 (pc, info);
}

int
print_insn_i386_intel (pc, info)
     bfd_vma pc;
     disassemble_info *info;
{
  intel_syntax = 1;
  open_char = '[';
  close_char = ']';
  separator_char = '+';
  scale_char = '*';

  return print_insn_i386 (pc, info);
}

static int
print_insn_i386 (pc, info)
     bfd_vma pc;
     disassemble_info *info;
{
  int flags;
  if (info->mach == bfd_mach_i386_i386
      || info->mach == bfd_mach_i386_i386_intel_syntax)
    flags = AFLAG|DFLAG;
  else if (info->mach == bfd_mach_i386_i8086)
    flags = 0;
  else
    abort ();
  return print_insn_x86 (pc, info, flags);
}

static int
print_insn_x86 (pc, info, sizeflag)
     bfd_vma pc;
     disassemble_info *info;
     int sizeflag;
{
  struct dis386 *dp;
  int i;
  int two_source_ops;
  char *first, *second, *third;
  int needcomma;
  unsigned char need_modrm;
  unsigned char uses_f3_prefix;

  struct dis_private priv;
  bfd_byte *inbuf = priv.the_buffer;

  /* The output looks better if we put 6 bytes on a line, since that
     puts most long word instructions on a single line.  */
  info->bytes_per_line = 6;

  info->private_data = (PTR) &priv;
  priv.max_fetched = priv.the_buffer;
  priv.insn_start = pc;
  if (setjmp (priv.bailout) != 0)
    /* Error return.  */
    return -1;

  obuf[0] = 0;
  op1out[0] = 0;
  op2out[0] = 0;
  op3out[0] = 0;

  op_index[0] = op_index[1] = op_index[2] = -1;

  the_info = info;
  start_pc = pc;
  start_codep = inbuf;
  codep = inbuf;

  ckprefix ();

  insn_codep = codep;

  FETCH_DATA (info, codep + 1);
  two_source_ops = (*codep == 0x62) || (*codep == 0xc8);

  obufp = obuf;

  if ((prefixes & PREFIX_FWAIT)
      && ((*codep < 0xd8) || (*codep > 0xdf)))
    {
      /* fwait not followed by floating point instruction.  */
      (*info->fprintf_func) (info->stream, "fwait");
      /* There may be other prefixes.  Skip any before the fwait.  */
      return codep - inbuf;
    }

  if (*codep == 0x0f)
    {
      FETCH_DATA (info, codep + 2);
      if (intel_syntax)
        dp = &dis386_twobyte_intel[*++codep];
      else
        dp = &dis386_twobyte_att[*++codep];
      need_modrm = twobyte_has_modrm[*codep];
      uses_f3_prefix = twobyte_uses_f3_prefix[*codep];
    }
  else
    {
      if (intel_syntax)
        dp = &dis386_intel[*codep];
      else
        dp = &dis386_att[*codep];
      need_modrm = onebyte_has_modrm[*codep];
      uses_f3_prefix = 0;
    }
  codep++;

  if (!uses_f3_prefix && (prefixes & PREFIX_REPZ))
    oappend ("repz ");
  if (prefixes & PREFIX_REPNZ)
    oappend ("repnz ");
  if (prefixes & PREFIX_LOCK)
    oappend ("lock ");

  if (prefixes & PREFIX_DATA)
    sizeflag ^= DFLAG;

  if (prefixes & PREFIX_ADDR)
    {
      sizeflag ^= AFLAG;
      if (sizeflag & AFLAG)
        oappend ("addr32 ");
      else
	oappend ("addr16 ");
    }

  if (need_modrm)
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
      if (dp->name == NULL)
	{
	  switch(dp->bytemode2)
	    {
	      case USE_GROUPS:
	        dp = &grps[dp->bytemode1][reg];
		break;
	      case USE_PREFIX_USER_TABLE:
		dp = &prefix_user_table[dp->bytemode1][prefixes & PREFIX_REPZ ? 1 : 0];
		break;
	      default:
		oappend (INTERNAL_DISASSEMBLER_ERROR);
		break;
	    }
	}

      putop (dp->name, sizeflag);

      obufp = op1out;
      op_ad = 2;
      if (dp->op1)
	(*dp->op1)(dp->bytemode1, sizeflag);

      obufp = op2out;
      op_ad = 1;
      if (dp->op2)
	(*dp->op2)(dp->bytemode2, sizeflag);

      obufp = op3out;
      op_ad = 0;
      if (dp->op3)
	(*dp->op3)(dp->bytemode3, sizeflag);
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
      op_ad = op_index[0];
      op_index[0] = op_index[2];
      op_index[2] = op_ad;
    }
  else
    {
      first = op3out;
      second = op2out;
      third = op1out;
    }
  needcomma = 0;
  if (*first)
    {
      if (op_index[0] != -1)
	(*info->print_address_func) ((bfd_vma) op_address[op_index[0]], info);
      else
	(*info->fprintf_func) (info->stream, "%s", first);
      needcomma = 1;
    }
  if (*second)
    {
      if (needcomma)
	(*info->fprintf_func) (info->stream, ",");
      if (op_index[1] != -1)
	(*info->print_address_func) ((bfd_vma) op_address[op_index[1]], info);
      else
	(*info->fprintf_func) (info->stream, "%s", second);
      needcomma = 1;
    }
  if (*third)
    {
      if (needcomma)
	(*info->fprintf_func) (info->stream, ",");
      if (op_index[2] != -1)
	(*info->print_address_func) ((bfd_vma) op_address[op_index[2]], info);
      else
	(*info->fprintf_func) (info->stream, "%s", third);
    }
  return codep - inbuf;
}

static char *float_mem_att[] = {
  /* d8 */
  "fadds",
  "fmuls",
  "fcoms",
  "fcomps",
  "fsubs",
  "fsubrs",
  "fdivs",
  "fdivrs",
  /*  d9 */
  "flds",
  "(bad)",
  "fsts",
  "fstps",
  "fldenv",
  "fldcw",
  "fNstenv",
  "fNstcw",
  /* da */
  "fiaddl",
  "fimull",
  "ficoml",
  "ficompl",
  "fisubl",
  "fisubrl",
  "fidivl",
  "fidivrl",
  /* db */
  "fildl",
  "(bad)",
  "fistl",
  "fistpl",
  "(bad)",
  "fldt",
  "(bad)",
  "fstpt",
  /* dc */
  "faddl",
  "fmull",
  "fcoml",
  "fcompl",
  "fsubl",
  "fsubrl",
  "fdivl",
  "fdivrl",
  /* dd */
  "fldl",
  "(bad)",
  "fstl",
  "fstpl",
  "frstor",
  "(bad)",
  "fNsave",
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
  "(bad)",
  "fist",
  "fistp",
  "fbld",
  "fildll",
  "fbstp",
  "fistpll",
};

static char *float_mem_intel[] = {
  /* d8 */
  "fadd",
  "fmul",
  "fcom",
  "fcomp",
  "fsub",
  "fsubr",
  "fdiv",
  "fdivr",
  /*  d9 */
  "fld",
  "(bad)",
  "fst",
  "fstp",
  "fldenv",
  "fldcw",
  "fNstenv",
  "fNstcw",
  /* da */
  "fiadd",
  "fimul",
  "ficom",
  "ficomp",
  "fisub",
  "fisubr",
  "fidiv",
  "fidivr",
  /* db */
  "fild",
  "(bad)",
  "fist",
  "fistp",
  "(bad)",
  "fld",
  "(bad)",
  "fstp",
  /* dc */
  "fadd",
  "fmul",
  "fcom",
  "fcomp",
  "fsub",
  "fsubr",
  "fdiv",
  "fdivr",
  /* dd */
  "fld",
  "(bad)",
  "fst",
  "fstp",
  "frstor",
  "(bad)",
  "fNsave",
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
  "(bad)",
  "fist",
  "fistp",
  "fbld",
  "fild",
  "fbstp",
  "fistpll",
};

#define ST OP_ST, 0
#define STi OP_STi, 0

#define FGRPd9_2 NULL, NULL, 0
#define FGRPd9_4 NULL, NULL, 1
#define FGRPd9_5 NULL, NULL, 2
#define FGRPd9_6 NULL, NULL, 3
#define FGRPd9_7 NULL, NULL, 4
#define FGRPda_5 NULL, NULL, 5
#define FGRPdb_4 NULL, NULL, 6
#define FGRPde_3 NULL, NULL, 7
#define FGRPdf_4 NULL, NULL, 8

static struct dis386 float_reg[][8] = {
  /* d8 */
  {
    { "fadd",	ST, STi },
    { "fmul",	ST, STi },
    { "fcom",	STi },
    { "fcomp",	STi },
    { "fsub",	ST, STi },
    { "fsubr",	ST, STi },
    { "fdiv",	ST, STi },
    { "fdivr",	ST, STi },
  },
  /* d9 */
  {
    { "fld",	STi },
    { "fxch",	STi },
    { FGRPd9_2 },
    { "(bad)" },
    { FGRPd9_4 },
    { FGRPd9_5 },
    { FGRPd9_6 },
    { FGRPd9_7 },
  },
  /* da */
  {
    { "fcmovb",	ST, STi },
    { "fcmove",	ST, STi },
    { "fcmovbe",ST, STi },
    { "fcmovu",	ST, STi },
    { "(bad)" },
    { FGRPda_5 },
    { "(bad)" },
    { "(bad)" },
  },
  /* db */
  {
    { "fcmovnb",ST, STi },
    { "fcmovne",ST, STi },
    { "fcmovnbe",ST, STi },
    { "fcmovnu",ST, STi },
    { FGRPdb_4 },
    { "fucomi",	ST, STi },
    { "fcomi",	ST, STi },
    { "(bad)" },
  },
  /* dc */
  {
    { "fadd",	STi, ST },
    { "fmul",	STi, ST },
    { "(bad)" },
    { "(bad)" },
#if UNIXWARE_COMPAT
    { "fsub",	STi, ST },
    { "fsubr",	STi, ST },
    { "fdiv",	STi, ST },
    { "fdivr",	STi, ST },
#else
    { "fsubr",	STi, ST },
    { "fsub",	STi, ST },
    { "fdivr",	STi, ST },
    { "fdiv",	STi, ST },
#endif
  },
  /* dd */
  {
    { "ffree",	STi },
    { "(bad)" },
    { "fst",	STi },
    { "fstp",	STi },
    { "fucom",	STi },
    { "fucomp",	STi },
    { "(bad)" },
    { "(bad)" },
  },
  /* de */
  {
    { "faddp",	STi, ST },
    { "fmulp",	STi, ST },
    { "(bad)" },
    { FGRPde_3 },
#if UNIXWARE_COMPAT
    { "fsubp",	STi, ST },
    { "fsubrp",	STi, ST },
    { "fdivp",	STi, ST },
    { "fdivrp",	STi, ST },
#else
    { "fsubrp",	STi, ST },
    { "fsubp",	STi, ST },
    { "fdivrp",	STi, ST },
    { "fdivp",	STi, ST },
#endif
  },
  /* df */
  {
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { "(bad)" },
    { FGRPdf_4 },
    { "fucomip",ST, STi },
    { "fcomip", ST, STi },
    { "(bad)" },
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
dofloat (sizeflag)
     int sizeflag;
{
  struct dis386 *dp;
  unsigned char floatop;

  floatop = codep[-1];

  if (mod != 3)
    {
      if (intel_syntax)
        putop (float_mem_intel[(floatop - 0xd8 ) * 8 + reg], sizeflag);
      else
        putop (float_mem_att[(floatop - 0xd8 ) * 8 + reg], sizeflag);
      obufp = op1out;
      if (floatop == 0xdb)
        OP_E (x_mode, sizeflag);
      else if (floatop == 0xdd)
        OP_E (d_mode, sizeflag);
      else 
        OP_E (v_mode, sizeflag);
      return;
    }
  codep++;

  dp = &float_reg[floatop - 0xd8][reg];
  if (dp->name == NULL)
    {
      putop (fgrps[dp->bytemode1][rm], sizeflag);

      /* instruction fnstsw is only one with strange arg */
      if (floatop == 0xdf && codep[-1] == 0xe0)
	strcpy (op1out, names16[0]);
    }
  else
    {
      putop (dp->name, sizeflag);

      obufp = op1out;
      if (dp->op1)
	(*dp->op1)(dp->bytemode1, sizeflag);
      obufp = op2out;
      if (dp->op2)
	(*dp->op2)(dp->bytemode2, sizeflag);
    }
}

/* ARGSUSED */
static void
OP_ST (ignore, sizeflag)
     int ignore;
     int sizeflag;
{
  oappend ("%st");
}

/* ARGSUSED */
static void
OP_STi (ignore, sizeflag)
     int ignore;
     int sizeflag;
{
  sprintf (scratchbuf, "%%st(%d)", rm);
  oappend (scratchbuf);
}


/* capital letters in template are macros */
static void
putop (template, sizeflag)
     char *template;
     int sizeflag;
{
  char *p;

  for (p = template; *p; p++)
    {
      switch (*p)
	{
	default:
	  *obufp++ = *p;
	  break;
	case 'A':
          if (intel_syntax)
            break;
	  if (mod != 3
#ifdef SUFFIX_ALWAYS
	      || (sizeflag & SUFFIX_ALWAYS)
#endif
	      )
	    *obufp++ = 'b';
	  break;
	case 'B':
          if (intel_syntax)
            break;
#ifdef SUFFIX_ALWAYS
	  if (sizeflag & SUFFIX_ALWAYS)
	    *obufp++ = 'b';
#endif
	  break;
	case 'E':		/* For jcxz/jecxz */
	  if (sizeflag & AFLAG)
	    *obufp++ = 'e';
	  break;
	case 'L':
          if (intel_syntax)
            break;
#ifdef SUFFIX_ALWAYS
	  if (sizeflag & SUFFIX_ALWAYS)
	    *obufp++ = 'l';
#endif
	  break;
	case 'N':
	  if ((prefixes & PREFIX_FWAIT) == 0)
	    *obufp++ = 'n';
	  break;
	case 'P':
          if (intel_syntax)
            break;
	  if ((prefixes & PREFIX_DATA)
#ifdef SUFFIX_ALWAYS
	      || (sizeflag & SUFFIX_ALWAYS)
#endif
	      )
	    {
	      if (sizeflag & DFLAG)
		*obufp++ = 'l';
	      else
		*obufp++ = 'w';
	    }
	  break;
	case 'Q':
          if (intel_syntax)
            break;
	  if (mod != 3
#ifdef SUFFIX_ALWAYS
	      || (sizeflag & SUFFIX_ALWAYS)
#endif
	      )
	    {
	      if (sizeflag & DFLAG)
		*obufp++ = 'l';
	      else
		*obufp++ = 'w';
	    }
	  break;
	case 'R':
          if (intel_syntax)
	    {
	      if (sizeflag & DFLAG)
		{
		  *obufp++ = 'd';
		  *obufp++ = 'q';
		}
	      else
		{
		  *obufp++ = 'w';
		  *obufp++ = 'd';
		}
	    }
	  else
	    {
	      if (sizeflag & DFLAG)
		*obufp++ = 'l';
	      else
		*obufp++ = 'w';
	    }
	  break;
	case 'S':
          if (intel_syntax)
            break;
#ifdef SUFFIX_ALWAYS
	  if (sizeflag & SUFFIX_ALWAYS)
	    {
	      if (sizeflag & DFLAG)
		*obufp++ = 'l';
	      else
		*obufp++ = 'w';
	    }
#endif
	  break;
	case 'W':
	  /* operand size flag for cwtl, cbtw */
	  if (sizeflag & DFLAG)
	    *obufp++ = 'w';
	  else
	    *obufp++ = 'b';
          if (intel_syntax)
	    {
	      if (sizeflag & DFLAG)
		{
		  *obufp++ = 'd';
		  *obufp++ = 'e';
		}
	      else
		{
		  *obufp++ = 'w';
		}
	    }
	  break;
	}
    }
  *obufp = 0;
}

static void
oappend (s)
     char *s;
{
  strcpy (obufp, s);
  obufp += strlen (s);
}

static void
append_seg ()
{
  if (prefixes & PREFIX_CS)
    oappend ("%cs:");
  if (prefixes & PREFIX_DS)
    oappend ("%ds:");
  if (prefixes & PREFIX_SS)
    oappend ("%ss:");
  if (prefixes & PREFIX_ES)
    oappend ("%es:");
  if (prefixes & PREFIX_FS)
    oappend ("%fs:");
  if (prefixes & PREFIX_GS)
    oappend ("%gs:");
}

static void
OP_indirE (bytemode, sizeflag)
     int bytemode;
     int sizeflag;
{
  if (!intel_syntax)
    oappend ("*");
  OP_E (bytemode, sizeflag);
}

static void
OP_E (bytemode, sizeflag)
     int bytemode;
     int sizeflag;
{
  int disp;

  /* skip mod/rm byte */
  codep++;

  if (mod == 3)
    {
      switch (bytemode)
	{
	case b_mode:
	  oappend (names8[rm]);
	  break;
	case w_mode:
	  oappend (names16[rm]);
	  break;
	case v_mode:
	  if (sizeflag & DFLAG)
	    oappend (names32[rm]);
	  else
	    oappend (names16[rm]);
	  break;
	case 0:		/* sfence */
	  break;
	default:
	  oappend (INTERNAL_DISASSEMBLER_ERROR);
	  break;
	}
      return;
    }

  disp = 0;
  append_seg ();

  if (sizeflag & AFLAG) /* 32 bit address mode */
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
	  scale = (*codep >> 6) & 3;
	  index = (*codep >> 3) & 7;
	  base = *codep & 7;
	  codep++;
	}

      switch (mod)
	{
	case 0:
	  if (base == 5)
	    {
	      havebase = 0;
	      disp = get32 ();
	    }
	  break;
	case 1:
	  FETCH_DATA (the_info, codep + 1);
	  disp = *codep++;
	  if ((disp & 0x80) != 0)
	    disp -= 0x100;
	  break;
	case 2:
	  disp = get32 ();
	  break;
	}

      if (!intel_syntax)
        if (mod != 0 || base == 5)
          {
            sprintf (scratchbuf, "0x%x", disp);
            oappend (scratchbuf);
          }
      
      if (havebase || (havesib && (index != 4 || scale != 0)))
	{
          if (intel_syntax)
            {
              switch (bytemode)
                {
                case b_mode:
                  oappend("BYTE PTR ");
                  break;
                case w_mode:
                  oappend("WORD PTR ");
                  break;
                case v_mode:
                  oappend("DWORD PTR ");
                  break;
                case d_mode:
                  oappend("QWORD PTR ");
                  break;
                case x_mode:
                  oappend("XWORD PTR ");
                  break;
                default:
                  break;
                }
             }
	  *obufp++ = open_char;
          *obufp = '\0';
	  if (havebase)
	    oappend (names32[base]);
	  if (havesib)
	    {
	      if (index != 4)
		{
                  if (intel_syntax)
                    {
                      if (havebase)
                        {
                          *obufp++ = separator_char;
                          *obufp = '\0';
                        }
                      sprintf (scratchbuf, "%s", names32[index]);
                    }
                  else
		    sprintf (scratchbuf, ",%s", names32[index]);
		  oappend (scratchbuf);
		}
              if (!intel_syntax
                  || (intel_syntax 
                      && bytemode != b_mode
                      && bytemode != w_mode
                      && bytemode != v_mode))
                {
                  *obufp++ = scale_char;
                  *obufp = '\0';
	          sprintf (scratchbuf, "%d", 1 << scale);
	          oappend (scratchbuf);
                }
	    }
          if (intel_syntax)
            if (mod != 0 || base == 5)
              {
                /* Don't print zero displacements */
                if (disp > 0)
                  {
                    sprintf (scratchbuf, "+%d", disp);
                    oappend (scratchbuf);
                  }
                else if (disp < 0)
                  {
                    sprintf (scratchbuf, "%d", disp);
                    oappend (scratchbuf);
                  }
              }

	  *obufp++ = close_char;
          *obufp = '\0';
	}
      else if (intel_syntax)
        {
          if (mod != 0 || base == 5)
            {
	      if (prefixes & (PREFIX_CS | PREFIX_SS | PREFIX_DS
			      | PREFIX_ES | PREFIX_FS | PREFIX_GS))
		;
	      else
		{
		  oappend (names_seg[3]);
		  oappend (":");
		}
              sprintf (scratchbuf, "0x%x", disp);
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
            sprintf (scratchbuf, "%d", disp);
            oappend (scratchbuf);
          }

      if (mod != 0 || rm != 6)
	{
	  *obufp++ = open_char;
          *obufp = '\0';
	  oappend (index16[rm]);
          *obufp++ = close_char;
          *obufp = '\0';
	}
    }
}

static void
OP_G (bytemode, sizeflag)
     int bytemode;
     int sizeflag;
{
  switch (bytemode)
    {
    case b_mode:
      oappend (names8[reg]);
      break;
    case w_mode:
      oappend (names16[reg]);
      break;
    case d_mode:
      oappend (names32[reg]);
      break;
    case v_mode:
      if (sizeflag & DFLAG)
	oappend (names32[reg]);
      else
	oappend (names16[reg]);
      break;
    default:
      oappend (INTERNAL_DISASSEMBLER_ERROR);
      break;
    }
}

static int
get32 ()
{
  int x = 0;

  FETCH_DATA (the_info, codep + 4);
  x = *codep++ & 0xff;
  x |= (*codep++ & 0xff) << 8;
  x |= (*codep++ & 0xff) << 16;
  x |= (*codep++ & 0xff) << 24;
  return x;
}

static int
get16 ()
{
  int x = 0;

  FETCH_DATA (the_info, codep + 2);
  x = *codep++ & 0xff;
  x |= (*codep++ & 0xff) << 8;
  return x;
}

static void
set_op (op)
     unsigned int op;
{
  op_index[op_ad] = op_ad;
  op_address[op_ad] = op;
}

static void
OP_REG (code, sizeflag)
     int code;
     int sizeflag;
{
  char *s;

  switch (code)
    {
    case indir_dx_reg:
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
      s = names8[code - al_reg];
      break;
    case eAX_reg: case eCX_reg: case eDX_reg: case eBX_reg:
    case eSP_reg: case eBP_reg: case eSI_reg: case eDI_reg:
      if (sizeflag & DFLAG)
	s = names32[code - eAX_reg];
      else
	s = names16[code - eAX_reg];
      break;
    default:
      s = INTERNAL_DISASSEMBLER_ERROR;
      break;
    }
  oappend (s);
}

static void
OP_I (bytemode, sizeflag)
     int bytemode;
     int sizeflag;
{
  int op;

  switch (bytemode)
    {
    case b_mode:
      FETCH_DATA (the_info, codep + 1);
      op = *codep++ & 0xff;
      break;
    case v_mode:
      if (sizeflag & DFLAG)
	op = get32 ();
      else
	op = get16 ();
      break;
    case w_mode:
      op = get16 ();
      break;
    default:
      oappend (INTERNAL_DISASSEMBLER_ERROR);
      return;
    }

  if (intel_syntax)
    sprintf (scratchbuf, "0x%x", op);
  else
    sprintf (scratchbuf, "$0x%x", op);
  oappend (scratchbuf);
  scratchbuf[0] = '\0';
}

static void
OP_sI (bytemode, sizeflag)
     int bytemode;
     int sizeflag;
{
  int op;

  switch (bytemode)
    {
    case b_mode:
      FETCH_DATA (the_info, codep + 1);
      op = *codep++;
      if ((op & 0x80) != 0)
	op -= 0x100;
      break;
    case v_mode:
      if (sizeflag & DFLAG)
	op = get32 ();
      else
	{
	  op = get16();
	  if ((op & 0x8000) != 0)
	    op -= 0x10000;
	}
      break;
    case w_mode:
      op = get16 ();
      if ((op & 0x8000) != 0)
	op -= 0x10000;
      break;
    default:
      oappend (INTERNAL_DISASSEMBLER_ERROR);
      return;
    }
  if (intel_syntax)
    sprintf (scratchbuf, "%d", op);
  else
    sprintf (scratchbuf, "$0x%x", op);
  oappend (scratchbuf);
}

static void
OP_J (bytemode, sizeflag)
     int bytemode;
     int sizeflag;
{
  int disp;
  int mask = -1;

  switch (bytemode)
    {
    case b_mode:
      FETCH_DATA (the_info, codep + 1);
      disp = *codep++;
      if ((disp & 0x80) != 0)
	disp -= 0x100;
      break;
    case v_mode:
      if (sizeflag & DFLAG)
	disp = get32 ();
      else
	{
	  disp = get16 ();
	  /* for some reason, a data16 prefix on a jump instruction
	     means that the pc is masked to 16 bits after the
	     displacement is added!  */
	  mask = 0xffff;
	}
      break;
    default:
      oappend (INTERNAL_DISASSEMBLER_ERROR);
      return;
    }
  disp = (start_pc + codep - start_codep + disp) & mask;
  set_op (disp);
  sprintf (scratchbuf, "0x%x", disp);
  oappend (scratchbuf);
}

/* ARGSUSED */
static void
OP_SEG (dummy, sizeflag)
     int dummy;
     int sizeflag;
{
  static char *sreg[] = {
    "%es","%cs","%ss","%ds","%fs","%gs","%?","%?",
  };

  oappend (sreg[reg]);
}

/* ARGSUSED */
static void
OP_DIR (dummy, sizeflag)
     int dummy;
     int sizeflag;
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
  sprintf (scratchbuf, "$0x%x,$0x%x", seg, offset);
  oappend (scratchbuf);
}

/* ARGSUSED */
static void
OP_OFF (ignore, sizeflag)
     int ignore;
     int sizeflag;
{
  int off;

  append_seg ();

  if (sizeflag & AFLAG)
    off = get32 ();
  else
    off = get16 ();

  if (intel_syntax)
    {
      if (!(prefixes & (PREFIX_CS | PREFIX_SS | PREFIX_DS
		        | PREFIX_ES | PREFIX_FS | PREFIX_GS)))
	{
	  oappend (names_seg[3]);
	  oappend (":");
	}
    }
  sprintf (scratchbuf, "0x%x", off);
  oappend (scratchbuf);
}

static void
ptr_reg (code, sizeflag)
     int code;
     int sizeflag;
{
  char *s;
  oappend ("(");
  if (sizeflag & AFLAG)
    s = names32[code - eAX_reg];
  else
    s = names16[code - eAX_reg];
  oappend (s);
  oappend (")");
}

static void
OP_ESreg (code, sizeflag)
     int code;
     int sizeflag;
{
  oappend ("%es:");
  ptr_reg (code, sizeflag);
}

static void
OP_DSreg (code, sizeflag)
     int code;
     int sizeflag;
{
  if ((prefixes
       & (PREFIX_CS
	  | PREFIX_DS
	  | PREFIX_SS
	  | PREFIX_ES
	  | PREFIX_FS
	  | PREFIX_GS)) == 0)
    prefixes |= PREFIX_DS;
  append_seg();
  ptr_reg (code, sizeflag);
}

#if 0
/* Not used.  */

/* ARGSUSED */
static void
OP_ONE (dummy, sizeflag)
     int dummy;
     int sizeflag;
{
  oappend ("1");
}

#endif

/* ARGSUSED */
static void
OP_C (dummy, sizeflag)
     int dummy;
     int sizeflag;
{
  codep++; /* skip mod/rm */
  sprintf (scratchbuf, "%%cr%d", reg);
  oappend (scratchbuf);
}

/* ARGSUSED */
static void
OP_D (dummy, sizeflag)
     int dummy;
     int sizeflag;
{
  codep++; /* skip mod/rm */
  sprintf (scratchbuf, "%%db%d", reg);
  oappend (scratchbuf);
}

/* ARGSUSED */
static void
OP_T (dummy, sizeflag)
     int dummy;
     int sizeflag;
{
  codep++; /* skip mod/rm */
  sprintf (scratchbuf, "%%tr%d", reg);
  oappend (scratchbuf);
}

static void
OP_rm (bytemode, sizeflag)
     int bytemode;
     int sizeflag;
{
  switch (bytemode)
    {
    case d_mode:
      oappend (names32[rm]);
      break;
    case w_mode:
      oappend (names16[rm]);
      break;
    }
}

static void
OP_MMX (ignore, sizeflag)
     int ignore;
     int sizeflag;
{
  sprintf (scratchbuf, "%%mm%d", reg);
  oappend (scratchbuf);
}

static void
OP_XMM (bytemode, sizeflag)
     int bytemode;
     int sizeflag;
{
  sprintf (scratchbuf, "%%xmm%d", reg);
  oappend (scratchbuf);
}

static void
OP_EM (bytemode, sizeflag)
     int bytemode;
     int sizeflag;
{
  if (mod != 3)
    {
      OP_E (bytemode, sizeflag);
      return;
    }

  codep++;
  sprintf (scratchbuf, "%%mm%d", rm);
  oappend (scratchbuf);
}

static void
OP_EX (bytemode, sizeflag)
     int bytemode;
     int sizeflag;
{
  if (mod != 3)
    {
      OP_E (bytemode, sizeflag);
      return;
    }

  codep++;
  sprintf (scratchbuf, "%%xmm%d", rm);
  oappend (scratchbuf);
}

static void
OP_MS (ignore, sizeflag)
     int ignore;
     int sizeflag;
{
  ++codep;
  sprintf (scratchbuf, "%%mm%d", rm);
  oappend (scratchbuf);
}

static const char *Suffix3DNow[] = {
/* 00 */	NULL,		NULL,		NULL,		NULL,
/* 04 */	NULL,		NULL,		NULL,		NULL,
/* 08 */	NULL,		NULL,		NULL,		NULL,
/* 0C */	NULL,		"pi2fd",	NULL,		NULL,
/* 10 */	NULL,		NULL,		NULL,		NULL,
/* 14 */	NULL,		NULL,		NULL,		NULL,
/* 18 */	NULL,		NULL,		NULL,		NULL,
/* 1C */	NULL,		"pf2id",	NULL,		NULL,
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
/* 88 */	NULL,		NULL,		NULL,		NULL,
/* 8C */	NULL,		NULL,		NULL,		NULL,
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
/* B8 */	NULL,		NULL,		NULL,		NULL,
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
OP_3DNowSuffix (bytemode, sizeflag)
     int bytemode;
     int sizeflag;
{
  const char *mnemonic;

  FETCH_DATA (the_info, codep + 1);
  /* AMD 3DNow! instructions are specified by an opcode suffix in the
     place where an 8-bit immediate would normally go.  ie. the last
     byte of the instruction.  */
  mnemonic = Suffix3DNow[*codep++ & 0xff];
  if (mnemonic)
    strcat (obuf, mnemonic);
  else
    {
      /* Since a variable sized modrm/sib chunk is between the start
	 of the opcode (0x0f0f) and the opcode suffix, we need to do
	 all the modrm processing first, and don't know until now that
	 we have a bad opcode.  This necessitates some cleaning up.  */
      op1out[0] = 0;
      op2out[0] = 0;
      codep = insn_codep + 1;
      strcat (obuf, "(bad)");
    }
}


static const char *simd_cmp_op [] = {
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
OP_SIMD_Suffix (bytemode, sizeflag)
     int bytemode;
     int sizeflag;
{
  unsigned int cmp_type;

  FETCH_DATA (the_info, codep + 1);
  cmp_type = *codep++ & 0xff;
  if (cmp_type < 8)
    {
      sprintf (scratchbuf, "cmp%s%cs",
	       simd_cmp_op[cmp_type],
	       prefixes & PREFIX_REPZ ? 's' : 'p');
      strcat (obuf, scratchbuf);
    }
  else
    {
      /* We have a bad extension byte.  Clean up.  */
      op1out[0] = 0;
      op2out[0] = 0;
      codep = insn_codep + 1;
      strcat (obuf, "(bad)");
    }
}

static void
SIMD_Fixup (extrachar, sizeflag)
     int extrachar;
     int sizeflag;
{
  /* Change movlps/movhps to movhlps/movlhps for 2 register operand
     forms of these instructions.  */
  if (mod == 3)
    {
      char *p = obuf + strlen(obuf);
      *(p+1) = '\0';
      *p     = *(p-1);
      *(p-1) = *(p-2);
      *(p-2) = *(p-3);
      *(p-3) = extrachar;
    }
}
