/* Print i386 instructions for GDB, the GNU debugger.
   Copyright (C) 1988, 89, 91, 93, 94, 95, 96, 97, 1998
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
#define Ap OP_DIR, lptr
#define Av OP_DIR, v_mode
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
#define EM OP_EM, v_mode
#define MS OP_MS, b_mode

/* bits in sizeflag */
#if 0 /* leave undefined until someone adds the extra flag to objdump */
#define SUFFIX_ALWAYS 4
#endif
#define AFLAG 2
#define DFLAG 1

typedef void (*op_rtn) PARAMS ((int bytemode, int sizeflag));

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
static void OP_EM PARAMS ((int, int));
static void OP_MS PARAMS ((int, int));

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

#define es_reg 100
#define cs_reg 101
#define ss_reg 102
#define ds_reg 103
#define fs_reg 104
#define gs_reg 105
#define eAX_reg 107
#define eCX_reg 108
#define eDX_reg 109
#define eBX_reg 110
#define eSP_reg 111
#define eBP_reg 112
#define eSI_reg 113
#define eDI_reg 114

#define lptr 115

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

#define GRP1b NULL, NULL, 0
#define GRP1S NULL, NULL, 1
#define GRP1Ss NULL, NULL, 2
#define GRP2b NULL, NULL, 3
#define GRP2S NULL, NULL, 4
#define GRP2b_one NULL, NULL, 5
#define GRP2S_one NULL, NULL, 6
#define GRP2b_cl NULL, NULL, 7
#define GRP2S_cl NULL, NULL, 8
#define GRP3b NULL, NULL, 9
#define GRP3S NULL, NULL, 10
#define GRP4  NULL, NULL, 11
#define GRP5  NULL, NULL, 12
#define GRP6  NULL, NULL, 13
#define GRP7 NULL, NULL, 14
#define GRP8 NULL, NULL, 15
#define GRP9 NULL, NULL, 16
#define GRP10 NULL, NULL, 17
#define GRP11 NULL, NULL, 18
#define GRP12 NULL, NULL, 19

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

static struct dis386 dis386[] = {
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
  { "callP",	Av },
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

static struct dis386 dis386_twobyte[] = {
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
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },  
  /* 10 */
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },  
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },  
  /* 18 */
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },  
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
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },  
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },  
  /* 30 */
  { "wrmsr" },  { "rdtsc" },  { "rdmsr" },  { "rdpmc" },  
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },  
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
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },  
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },  
  /* 58 */
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },  
  { "(bad)" },  { "(bad)" },  { "(bad)" },  { "(bad)" },  
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
  { "(bad)" },
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
  { "(bad)" },
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
  { "(bad)" },
  { "(bad)" },  
  { "(bad)" },
  { "(bad)" },
  { "(bad)" },
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
  { "(bad)" },  { "(bad)" },  
  /* d8 */
  { "psubusb", MX, EM },
  { "psubusw", MX, EM },
  { "(bad)" },
  { "pand", MX, EM },
  { "paddusb", MX, EM },
  { "paddusw", MX, EM },
  { "(bad)" },
  { "pandn", MX, EM },
  /* e0 */
  { "(bad)" },
  { "psraw", MX, EM },
  { "psrad", MX, EM },
  { "(bad)" },
  { "(bad)" },
  { "pmulhw", MX, EM },
  { "(bad)" },  { "(bad)" },  
  /* e8 */
  { "psubsb", MX, EM },
  { "psubsw", MX, EM },
  { "(bad)" },
  { "por", MX, EM },
  { "paddsb", MX, EM },
  { "paddsw", MX, EM },
  { "(bad)" },
  { "pxor", MX, EM },
  /* f0 */
  { "(bad)" },
  { "psllw", MX, EM },
  { "pslld", MX, EM },
  { "psllq", MX, EM },
  { "(bad)" },
  { "pmaddwd", MX, EM },
  { "(bad)" },  { "(bad)" },  
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
  1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,
  1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,
  1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,
  1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,1,1,0,0,0,0,0,1,0,1,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,
  1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1
};

static const unsigned char twobyte_has_modrm[256] = {
  /* 00 */ 1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0, /* 0f */
  /* 10 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 1f */
  /* 20 */ 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0, /* 2f */
  /* 30 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 3f */
  /* 40 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 4f */
  /* 50 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 5f */
  /* 60 */ 1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1, /* 6f */
  /* 70 */ 0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1, /* 7f */
  /* 80 */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8f */
  /* 90 */ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 9f */
  /* a0 */ 0,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1, /* af */
  /* b0 */ 1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1, /* bf */
  /* c0 */ 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0, /* cf */
  /* d0 */ 0,1,1,1,0,1,0,0,1,1,0,1,1,1,0,1, /* df */
  /* e0 */ 0,1,1,0,0,1,0,0,1,1,0,1,1,1,0,1, /* ef */
  /* f0 */ 0,1,1,1,0,1,0,0,1,1,1,0,1,1,1,0  /* ff */
};

static char obuf[100];
static char *obufp;
static char scratchbuf[100];
static unsigned char *start_codep;
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
  "bx+si","bx+di","bp+si","bp+di","si","di","bp","bx"
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
    { "lcallP",	indirEv },
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
  }
};

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
	  prefixes |= PREFIX_FWAIT;
	  codep++;		/* fwait is really an instruction */
	  return;		/* so stop accumulating prefixes */
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

int print_insn_x86 PARAMS ((bfd_vma pc, disassemble_info *info, int sizeflag));

int
print_insn_i386 (pc, info)
     bfd_vma pc;
     disassemble_info *info;
{
  int flags;
  if (info->mach == bfd_mach_i386_i386)
    flags = AFLAG|DFLAG;
  else if (info->mach == bfd_mach_i386_i8086)
    flags = 0;
  else
    abort ();
  return print_insn_x86 (pc, info, flags);
}

int
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

  struct dis_private priv;
  bfd_byte *inbuf = priv.the_buffer;

  /* The output looks better if we put 5 bytes on a line, since that
     puts long word instructions on a single line.  */
  info->bytes_per_line = 5;

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
  
  if (prefixes & PREFIX_REPZ)
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
  
  if (*codep == 0x0f)
    {
      FETCH_DATA (info, codep + 2);
      dp = &dis386_twobyte[*++codep];
      need_modrm = twobyte_has_modrm[*codep];
    }
  else
    {
      dp = &dis386[*codep];
      need_modrm = onebyte_has_modrm[*codep];
    }
  codep++;

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
	dp = &grps[dp->bytemode1][reg];
      
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
  if (two_source_ops)
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

static char *float_mem[] = {
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
      putop (float_mem[(floatop - 0xd8) * 8 + reg], sizeflag);
      obufp = op1out;
      OP_E (v_mode, sizeflag);
      return;
    }
  codep++;
  
  dp = &float_reg[floatop - 0xd8][reg];
  if (dp->name == NULL)
    {
      putop (fgrps[dp->bytemode1][rm], sizeflag);
      /* instruction fnstsw is only one with strange arg */
      if (floatop == 0xdf
	  && FETCH_DATA (the_info, codep + 1)
	  && *codep == 0xe0)
	strcpy (op1out, "%eax");
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
	  if (mod != 3
#ifdef SUFFIX_ALWAYS
	      || (sizeflag & SUFFIX_ALWAYS)
#endif
	      )
	    *obufp++ = 'b';
	  break;
	case 'B':
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
	  if (sizeflag & DFLAG)
	    *obufp++ = 'l';
	  else
	    *obufp++ = 'w';
	  break;
	case 'S':
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
	default:
	  oappend ("<bad dis table>");
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

      if (mod != 0 || base == 5)
	{
	  sprintf (scratchbuf, "0x%x", disp);
	  oappend (scratchbuf);
	}

      if (havebase || (havesib && (index != 4 || scale != 0)))
	{
	  oappend ("(");
	  if (havebase)
	    oappend (names32[base]);
	  if (havesib)
	    {
	      if (index != 4)
		{
		  sprintf (scratchbuf, ",%s", names32[index]);
		  oappend (scratchbuf);
		}
	      sprintf (scratchbuf, ",%d", 1 << scale);
	      oappend (scratchbuf);
	    }
	  oappend (")");
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

      if (mod != 0 || rm == 6)
	{
	  sprintf (scratchbuf, "0x%x", disp);
	  oappend (scratchbuf);
	}

      if (mod != 0 || rm != 6)
	{
	  oappend ("(");
	  oappend (index16[rm]);
	  oappend (")");
	}
    }
}

#define INTERNAL_DISASSEMBLER_ERROR _("<internal disassembler error>")

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
    case indir_dx_reg: s = "(%dx)"; break;
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
  sprintf (scratchbuf, "$0x%x", op);
  oappend (scratchbuf);
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
	  if ((disp & 0x8000) != 0)
	    disp -= 0x10000;
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

static void
OP_DIR (size, sizeflag)
     int size;
     int sizeflag;
{
  int seg, offset;
  
  switch (size) 
    {
    case lptr:
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
      break;
    case v_mode:
      if (sizeflag & DFLAG)
	offset = get32 ();
      else
	{
	  offset = get16 ();
	  if ((offset & 0x8000) != 0)
	    offset -= 0x10000;
	}
      
      offset = start_pc + codep - start_codep + offset;
      set_op (offset);
      sprintf (scratchbuf, "0x%x", offset);
      oappend (scratchbuf);
      break;
    default:
      oappend (INTERNAL_DISASSEMBLER_ERROR);
      break;
    }
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
OP_MS (ignore, sizeflag)
     int ignore;
     int sizeflag;
{
  ++codep;
  sprintf (scratchbuf, "%%mm%d", rm);
  oappend (scratchbuf);
}
