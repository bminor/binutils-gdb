/* s390-opc.c -- S390 opcode list
   Copyright (C) 2000, 2001 Free Software Foundation, Inc.
   Contributed by Martin Schwidefsky (schwidefsky@de.ibm.com).

   This file is part of GDB, GAS, and the GNU binutils.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#include <stdio.h>
#include "ansidecl.h"
#include "opcode/s390.h"

/* This file holds the S390 opcode table.  The opcode table
   includes almost all of the extended instruction mnemonics.  This
   permits the disassembler to use them, and simplifies the assembler
   logic, at the cost of increasing the table size.  The table is
   strictly constant data, so the compiler should be able to put it in
   the .text section.

   This file also holds the operand table.  All knowledge about
   inserting operands into instructions and vice-versa is kept in this
   file.  */

/* The operands table.
   The fields are bits, shift, insert, extract, flags.  */

const struct s390_operand s390_operands[] =
{
#define UNUSED 0
  { 0, 0, 0 },                    /* Indicates the end of the operand list */

#define R_8    1                  /* GPR starting at position 8 */
  { 4, 8, S390_OPERAND_GPR },
#define R_12   2                  /* GPR starting at position 12 */
  { 4, 12, S390_OPERAND_GPR },    
#define R_16   3                  /* GPR starting at position 16 */
  { 4, 16, S390_OPERAND_GPR },    
#define R_20   4                  /* GPR starting at position 20 */
  { 4, 20, S390_OPERAND_GPR },    
#define R_24   5                  /* GPR starting at position 24 */
  { 4, 24, S390_OPERAND_GPR },    
#define R_28   6                  /* GPR starting at position 28 */
  { 4, 28, S390_OPERAND_GPR },    
#define R_32   7                  /* GPR starting at position 32 */
  { 4, 32, S390_OPERAND_GPR },

#define F_8    8                  /* FPR starting at position 8 */
  { 4, 8, S390_OPERAND_FPR },
#define F_12   9                  /* FPR starting at position 12 */
  { 4, 12, S390_OPERAND_FPR },
#define F_16   10                 /* FPR starting at position 16 */
  { 4, 16, S390_OPERAND_FPR },
#define F_20   11                 /* FPR starting at position 16 */
  { 4, 16, S390_OPERAND_FPR },
#define F_24   12                 /* FPR starting at position 24 */
  { 4, 24, S390_OPERAND_FPR },
#define F_28   13                 /* FPR starting at position 28 */
  { 4, 28, S390_OPERAND_FPR },
#define F_32   14                 /* FPR starting at position 32 */
  { 4, 32, S390_OPERAND_FPR },

#define A_8    15                 /* Access reg. starting at position 8 */
  { 4, 8, S390_OPERAND_AR },
#define A_12   16                 /* Access reg. starting at position 12 */
  { 4, 12, S390_OPERAND_AR },
#define A_24   17                 /* Access reg. starting at position 24 */
  { 4, 24, S390_OPERAND_AR },
#define A_28   18                 /* Access reg. starting at position 28 */
  { 4, 28, S390_OPERAND_AR },

#define C_8    19                 /* Control reg. starting at position 8 */
  { 4, 8, S390_OPERAND_CR },
#define C_12   20                 /* Control reg. starting at position 12 */
  { 4, 12, S390_OPERAND_CR },

#define B_16   21                 /* Base register starting at position 16 */
  { 4, 16, S390_OPERAND_BASE|S390_OPERAND_GPR },   
#define B_32   22                 /* Base register starting at position 32 */
  { 4, 32, S390_OPERAND_BASE|S390_OPERAND_GPR },   

#define X_12   23                 /* Index register starting at position 12 */
  { 4, 12, S390_OPERAND_INDEX|S390_OPERAND_GPR },                   

#define D_20   24                 /* Displacement starting at position 20 */
  { 12, 20, S390_OPERAND_DISP },  
#define D_36   25                 /* Displacement starting at position 36 */
  { 12, 36, S390_OPERAND_DISP },  

#define L4_8   26                 /* 4 bit length starting at position 8 */
  { 4, 8, S390_OPERAND_LENGTH },                         
#define L4_12  27                 /* 4 bit length starting at position 12 */
  { 4, 12, S390_OPERAND_LENGTH },
#define L8_8   28                 /* 8 bit length starting at position 8 */
  { 8, 8, S390_OPERAND_LENGTH },                    

#define U4_8   29                 /* 4 bit unsigned value starting at 8 */
  { 4, 8, 0 },
#define U4_12  30                 /* 4 bit unsigned value starting at 12 */
  { 4, 12, 0 },
#define U4_16  31                 /* 4 bit unsigned value starting at 16 */
  { 4, 16, 0 },
#define U4_20  32                 /* 4 bit unsigned value starting at 20 */
  { 4, 20, 0 },
#define U8_8   33                 /* 8 bit unsigned value starting at 8 */
  { 8, 8, 0 },                    
#define U8_16  34                 /* 8 bit unsigned value starting at 16 */
  { 8, 16, 0 },
#define I16_16 35                 /* 16 bit signed value starting at 16 */
  { 16, 16, S390_OPERAND_SIGNED },
#define U16_16 36                 /* 16 bit unsigned value starting at 16 */
  { 16, 16, 0 },                  
#define J16_16 37                 /* PC relative jump offset at 16 */
  { 16, 16, S390_OPERAND_PCREL },  
#define J32_16 38                 /* PC relative long offset at 16 */
  { 32, 16, S390_OPERAND_PCREL }
};


/* Macros used to form opcodes.  */

/* 8/16/48 bit opcodes */
#define OP8(x) { x, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define OP16(x) { x>>8, x&255, 0x00, 0x00, 0x00, 0x00 }
#define OP48(x) { x>>40, (x>>32)&255, (x>>24)&255, \
                  (x>>16)&255, (x>>8)&255, x&255}

#define INSTR_E      2, { 0,0,0,0,0,0 }                  /* e.g. pr   */
#define INSTR_RR     2, { R_8,R_12,0,0,0,0 }             /* e.g. lr   */
#define INSTR_RR_M   2, { U4_8,R_12,0,0,0,0 }            /* e.g. bcr  */
#define INSTR_RR_B   2, { R_12, 0,0,0,0,0 }              /* e.g. br   */
#define INSTR_RR_I   2, { U8_8, 0,0,0,0,0 }              /* e.g. svc  */
#define INSTR_RR_R   2, { R_8, 0,0,0,0,0 }               /* e.g. spm  */
#define INSTR_RR_E   2, { R_8,R_12,0,0,0,0 }             /* e.g. aer  */
#define INSTR_RR_D   2, { F_8,F_12,0,0,0,0 }             /* e.g. adr  */
#define INSTR_RR_X   2, { R_8,R_12,0,0,0,0 }             /* e.g. mxr  */
#define INSTR_RR_ED  2, { F_8,F_12,0,0,0,0 }             /* e.g. mer  */
#define INSTR_RR_DE  2, { F_8,F_12,0,0,0,0 }             /* e.g. lrer */
#define INSTR_RR_DX  2, { F_8,F_12,0,0,0,0 }             /* e.g. mxdr */
#define INSTR_RR_XD  2, { F_8,F_12,0,0,0,0 }             /* e.g. lrdr */
#define INSTR_RRE    4, { R_24,R_28,0,0,0,0 }            /* e.g. lura */
#define INSTR_RRE_A  4, { A_24,A_28,0,0,0,0 }            /* e.g. cpya */
#define INSTR_RRE_F  4, { F_24,F_28,0,0,0,0 }            /* e.g. debr */
#define INSTR_RRE_O  4, { 0,0,0,0,0,0 }                  /* e.g. palb */
#define INSTR_RRE_R  4, { R_24,0,0,0,0,0 }               /* e.g. ipm  */
#define INSTR_RRE_R2 4, { R_28,0,0,0,0,0 }               /* e.g. tb   */
#define INSTR_RRE_E  4, { F_24,0,0,0,0,0 }               /* e.g. sqer */
#define INSTR_RRE_D  4, { F_24,0,0,0,0,0 }               /* e.g. sqdr */
#define INSTR_RRE_X  4, { F_24,0,0,0,0,0 }               /* e.g. dxr  */
#define INSTR_RRE_AR 4, { A_24,R_28,0,0,0,0 }            /* e.g. sar  */
#define INSTR_RRE_RA 4, { R_24,A_28,0,0,0,0 }            /* e.g. ear  */
#define INSTR_RRF_M  4, { R_24,U4_16,R_28,0,0,0 }        /* e.g. cfxbr*/
#define INSTR_RRF_RM 4, { R_24,R_16,R_28,U4_20,0,0 }     /* e.g. didbr*/
#define INSTR_RRF_R  4, { R_16,R_24,R_28,0,0,0 }         /* e.g. madbr*/
#define INSTR_RRF_F  4, { F_16,F_24,F_28,0,0,0 }         /* e.g. madbr*/
#define INSTR_RS     4, { R_8,R_12,D_20,B_16,0,0 }       /* e.g. cs   */
#define INSTR_RS_A   4, { A_8,A_12,D_20,B_16,0,0 }       /* e.g. lam  */
#define INSTR_RS_C   4, { C_8,C_12,D_20,B_16,0,0 }       /* e.g. lctl */
#define INSTR_RS_M   4, { R_8,U4_12,D_20,B_16,0,0 }      /* e.g. icm  */
#define INSTR_RS_S   4, { R_8,D_20,B_16,0,0,0 }          /* e.g. sll  */
#define INSTR_RS_D   4, { R_8,D_20,B_16,0,0,0 }          /* e.g. sldl */
#define INSTR_RX     4, { R_8,D_20,X_12,B_16,0,0 }       /* e.g. l    */
#define INSTR_RX_M   4, { U4_8,D_20,X_12,B_16,0,0 }      /* e.g. bc   */
#define INSTR_RX_B   4, { D_20,X_12,B_16,0,0,0 }         /* e.g. b    */
#define INSTR_RX_E   4, { F_8,D_20,X_12,B_16,0,0 }       /* e.g. ae   */
#define INSTR_RX_D   4, { F_8,D_20,X_12,B_16,0,0 }       /* e.g. ad   */
#define INSTR_RX_ED  4, { F_8,D_20,X_12,B_16,0,0 }       /* e.g. me   */
#define INSTR_RX_DX  4, { F_8,D_20,X_12,B_16,0,0 }       /* e.g. mxd  */
#define INSTR_RXE    6, { R_8,D_20,X_12,B_16,0,0 }       /* e.g. agr  */
#define INSTR_RXE_F  6, { F_8,D_20,X_12,B_16,0,0 }       /* e.g. axbr */
#define INSTR_RXF    6, { F_32,D_20,X_12,B_16,F_8,0 }    /* e.g. madb */
#define INSTR_S      4, { D_20,B_16,0,0,0,0 }            /* e.g. lpsw */
#define INSTR_S_O    4, { 0,0,0,0,0,0 }                  /* e.g. hsch */
#define INSTR_SI     4, { D_20,B_16,U8_8,0,0,0 }         /* e.g. cli  */
#define INSTR_SS_RR  6, { D_20,R_8,B_16,D_36,B_32,R_12 } /* e.g. mvck */
#define INSTR_SS_LL  6, { D_20,L4_8,B_16,D_36,L4_12,B_32 } /* e.g. pack */
#define INSTR_SS_LI  6, { D_20,L4_8,B_16,D_36,B_32,U4_12 } /* e.g. srp  */
#define INSTR_SS_L   6, { D_20,L8_8,B_16,D_36,B_32,0 }   /* e.g. mvc  */
#define INSTR_SS_LMD 6, { R_8,R_12,D_20,B_16,D_36,B_32 } /* e.g. lmd  */
#define INSTR_SS_PLO 6, { R_8,D_20,B_16,R_12,D_36,B_32 } /* e.g. plo  */
#define INSTR_SSE    6, { D_20,B_16,D_36,B_32,0,0 }      /* e.g. mvsdk */
#define INSTR_RI     4, { R_8,I16_16,0,0,0,0 }           /* e.g. ahi  */
#define INSTR_RI_U   4, { R_8,U16_16,0,0,0,0 }           /* e.g. tml  */
#define INSTR_RI_A   4, { R_8,J16_16,0,0,0,0 }           /* e.g. brct */
#define INSTR_RI_MA  4, { U4_8,J16_16,0,0,0 }            /* e.g. brc  */
#define INSTR_RI_B   4, { J16_16,0,0,0,0 }               /* e.g. j    */
#define INSTR_RSI_A  4, { R_8,R_12,J16_16,0,0,0 }        /* e.g. brxh */
#define INSTR_RSE    6, { R_8,D_20,B_16,R_12,0,0 }       /* e.g. lmg  */
#define INSTR_RSE_M  6, { R_8,U4_12,D_20,B_16,0,0 }      /* e.g. icmh */
#define INSTR_RSE_R  6, { R_8,R_12,D_20,B_16,0,0 }       /* e.g. lmh  */
#define INSTR_RIE_A  6, { R_8,J16_16,R_12,0,0,0 }        /* e.g. brxhg */
#define INSTR_RIL_A  6, { R_8,J32_16,0,0,0,0 }           /* e.g. brasl */
#define INSTR_RIL_B  6, { J32_16,0,0,0,0,0 }             /* e.g. jg   */
#define INSTR_RIL_MA 6, { R_8,J32_16,0,0,0,0 }           /* e.g. brcl */
 
#define MASK_E      { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RR     { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RR_M   { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RR_B   { 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RR_I   { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RR_R   { 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RR_E   { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RR_D   { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RR_X   { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RR_ED  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RR_DE  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RR_DX  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RR_XD  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RRE    { 0xff, 0xff, 0xff, 0x00, 0x00, 0x00 }
#define MASK_RRE_A  { 0xff, 0xff, 0xff, 0x00, 0x00, 0x00 }
#define MASK_RRE_F  { 0xff, 0xff, 0xff, 0x00, 0x00, 0x00 }
#define MASK_RRE_O  { 0xff, 0xff, 0xff, 0xff, 0x00, 0x00 }
#define MASK_RRE_R  { 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00 }
#define MASK_RRE_R2 { 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00 }
#define MASK_RRE_E  { 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00 }
#define MASK_RRE_D  { 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00 }
#define MASK_RRE_X  { 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00 }
#define MASK_RRE_AR { 0xff, 0xff, 0xff, 0x00, 0x00, 0x00 }
#define MASK_RRE_RA { 0xff, 0xff, 0xff, 0x00, 0x00, 0x00 }
#define MASK_RRF_M  { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RRF_RM { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RRF_R  { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RRF_F  { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RS     { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RS_A   { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RS_C   { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RS_M   { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RS_S   { 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RS_D   { 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RX     { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RX_M   { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RX_B   { 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RX_E   { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RX_D   { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RX_ED  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RX_DX  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RXE    { 0xff, 0x00, 0x00, 0x00, 0x00, 0xff }
#define MASK_RXE_F  { 0xff, 0x00, 0x00, 0x00, 0x00, 0xff }
#define MASK_RXF    { 0xff, 0x00, 0x00, 0x00, 0x00, 0xff }
#define MASK_S      { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 }
#define MASK_S_O    { 0xff, 0xff, 0xff, 0xff, 0x00, 0x00 }
#define MASK_SI     { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_SS_RR  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_SS_LL  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_SS_LI  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_SS_L   { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_SS_LMD { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_SS_PLO { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_SSE    { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RI     { 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RI_U   { 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RI_A   { 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RI_MA  { 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RI_B   { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RSI_A  { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RSE    { 0xff, 0x00, 0x00, 0x00, 0x00, 0xff }
#define MASK_RSE_M  { 0xff, 0x00, 0x00, 0x00, 0x00, 0xff }
#define MASK_RSE_R  { 0xff, 0x00, 0x00, 0x00, 0x00, 0xff }
#define MASK_RIE_A  { 0xff, 0x00, 0x00, 0x00, 0x00, 0xff }
#define MASK_RIL_A  { 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RIL_B  { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RIL_M  { 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00 }
#define MASK_RIL_MA { 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00 }
 
/* The opcode formats table (blueprints for .insn pseudo mnemonic). */

const struct s390_opcode s390_opformats[] = {
  { "e",	OP8(0x00LL),	MASK_E,		INSTR_E,	3 },
  { "ri",	OP8(0x00LL),	MASK_RI,	INSTR_RI,	3 },
  { "ri_a",	OP8(0x00LL),	MASK_RI_A,	INSTR_RI_A,	3 },
  { "ri_b",	OP8(0x00LL),	MASK_RI_B,	INSTR_RI_B,	3 },
  { "ri_ma",	OP8(0x00LL),	MASK_RI_MA,	INSTR_RI_MA,	3 },
  { "ri_u",	OP8(0x00LL),	MASK_RI_U,	INSTR_RI_U,	3 },
  { "rie_a",	OP8(0x00LL),	MASK_RIE_A,	INSTR_RIE_A,	3 },
  { "ril_a",	OP8(0x00LL),	MASK_RIL_A,	INSTR_RIL_A,	3 },
  { "ril_b",	OP8(0x00LL),	MASK_RIL_B,	INSTR_RIL_B,	3 },
  { "ril_ma",	OP8(0x00LL),	MASK_RIL_MA,	INSTR_RIL_MA,	3 },
  { "rr",	OP8(0x00LL),	MASK_RR,	INSTR_RR,	3 },
  { "rr_b",	OP8(0x00LL),	MASK_RR_B,	INSTR_RR_B,	3 },
  { "rr_d",	OP8(0x00LL),	MASK_RR_D,	INSTR_RR_D,	3 },
  { "rr_de",	OP8(0x00LL),	MASK_RR_DE,	INSTR_RR_DE,	3 },
  { "rr_dx",	OP8(0x00LL),	MASK_RR_DX,	INSTR_RR_DX,	3 },
  { "rr_e",	OP8(0x00LL),	MASK_RR_E,	INSTR_RR_E,	3 },
  { "rr_ed",	OP8(0x00LL),	MASK_RR_ED,	INSTR_RR_ED,	3 },
  { "rr_i",	OP8(0x00LL),	MASK_RR_I,	INSTR_RR_I,	3 },
  { "rr_m",	OP8(0x00LL),	MASK_RR_M,	INSTR_RR_M,	3 },
  { "rr_r",	OP8(0x00LL),	MASK_RR_R,	INSTR_RR_R,	3 },
  { "rr_x",	OP8(0x00LL),	MASK_RR_X,	INSTR_RR_X,	3 },
  { "rr_xd",	OP8(0x00LL),	MASK_RR_XD,	INSTR_RR_XD,	3 },
  { "rre",	OP8(0x00LL),	MASK_RRE,	INSTR_RRE,	3 },
  { "rre_a",	OP8(0x00LL),	MASK_RRE_A,	INSTR_RRE_A,	3 },
  { "rre_ar",	OP8(0x00LL),	MASK_RRE_AR,	INSTR_RRE_AR,	3 },
  { "rre_d",	OP8(0x00LL),	MASK_RRE_D,	INSTR_RRE_D,	3 },
  { "rre_e",	OP8(0x00LL),	MASK_RRE_E,	INSTR_RRE_E,	3 },
  { "rre_f",	OP8(0x00LL),	MASK_RRE_F,	INSTR_RRE_F,	3 },
  { "rre_o",	OP8(0x00LL),	MASK_RRE_O,	INSTR_RRE_O,	3 },
  { "rre_r",	OP8(0x00LL),	MASK_RRE_R,	INSTR_RRE_R,	3 },
  { "rre_r2",	OP8(0x00LL),	MASK_RRE_R2,	INSTR_RRE_R2,	3 },
  { "rre_ra",	OP8(0x00LL),	MASK_RRE_RA,	INSTR_RRE_RA,	3 },
  { "rre_x",	OP8(0x00LL),	MASK_RRE_X,	INSTR_RRE_X,	3 },
  { "rrf_f",	OP8(0x00LL),	MASK_RRF_F,	INSTR_RRF_F,	3 },
  { "rrf_m",	OP8(0x00LL),	MASK_RRF_M,	INSTR_RRF_M,	3 },
  { "rrf_r",	OP8(0x00LL),	MASK_RRF_R,	INSTR_RRF_R,	3 },
  { "rrf_rm",	OP8(0x00LL),	MASK_RRF_RM,	INSTR_RRF_RM,	3 },
  { "rs",	OP8(0x00LL),	MASK_RS,	INSTR_RS,	3 },
  { "rs_a",	OP8(0x00LL),	MASK_RS_A,	INSTR_RS_A,	3 },
  { "rs_c",	OP8(0x00LL),	MASK_RS_C,	INSTR_RS_C,	3 },
  { "rs_d",	OP8(0x00LL),	MASK_RS_D,	INSTR_RS_D,	3 },
  { "rs_m",	OP8(0x00LL),	MASK_RS_M,	INSTR_RS_M,	3 },
  { "rs_s",	OP8(0x00LL),	MASK_RS_S,	INSTR_RS_S,	3 },
  { "rse",	OP8(0x00LL),	MASK_RSE,	INSTR_RSE,	3 },
  { "rse_m",	OP8(0x00LL),	MASK_RSE_M,	INSTR_RSE_M,	3 },
  { "rse_r",	OP8(0x00LL),	MASK_RSE_R,	INSTR_RSE_R,	3 },
  { "rsi_a",	OP8(0x00LL),	MASK_RSI_A,	INSTR_RSI_A,	3 },
  { "rx",	OP8(0x00LL),	MASK_RX,	INSTR_RX,	3 },
  { "rx_b",	OP8(0x00LL),	MASK_RX_B,	INSTR_RX_B,	3 },
  { "rx_d",	OP8(0x00LL),	MASK_RX_D,	INSTR_RX_D,	3 },
  { "rx_dx",	OP8(0x00LL),	MASK_RX_DX,	INSTR_RX_DX,	3 },
  { "rx_e",	OP8(0x00LL),	MASK_RX_E,	INSTR_RX_E,	3 },
  { "rx_ed",	OP8(0x00LL),	MASK_RX_ED,	INSTR_RX_ED,	3 },
  { "rx_m",	OP8(0x00LL),	MASK_RX_M,	INSTR_RX_M,	3 },
  { "rxe",	OP8(0x00LL),	MASK_RXE,	INSTR_RXE,	3 },
  { "rxe_f",	OP8(0x00LL),	MASK_RXE_F,	INSTR_RXE_F,	3 },
  { "rxf",	OP8(0x00LL),	MASK_RXF,	INSTR_RXF,	3 },
  { "s",	OP8(0x00LL),	MASK_S,		INSTR_S,	3 },
  { "si",	OP8(0x00LL),	MASK_SI,	INSTR_SI,	3 },
  { "ss_l",	OP8(0x00LL),	MASK_SS_L,	INSTR_SS_L,	3 },
  { "ss_li",	OP8(0x00LL),	MASK_SS_LI,	INSTR_SS_LI,	3 },
  { "ss_ll",	OP8(0x00LL),	MASK_SS_LL,	INSTR_SS_LL,	3 },
  { "ss_lmd",	OP8(0x00LL),	MASK_SS_LMD,	INSTR_SS_LMD,	3 },
  { "ss_plo",	OP8(0x00LL),	MASK_SS_PLO,	INSTR_SS_PLO,	3 },
  { "ss_rr",	OP8(0x00LL),	MASK_SS_RR,	INSTR_SS_RR,	3 },
  { "sse",	OP8(0x00LL),	MASK_SSE,	INSTR_SSE,	3 },
};

const int s390_num_opformats =
  sizeof (s390_opformats) / sizeof (s390_opformats[0]);

/* The opcode table.

   The format of the opcode table is:

   NAME	     OPCODE	MASK	OPERANDS

   NAME is the name of the instruction.
   OPCODE is the instruction opcode.
   MASK is the opcode mask; this is used to tell the disassembler
     which bits in the actual opcode must match OPCODE.
   OPERANDS is the list of operands.

   The disassembler reads the table in order and prints the first
   instruction which matches. */

const struct s390_opcode s390_opcodes[] = {
  { "dp", OP8(0xfdLL), MASK_SS_LL, INSTR_SS_LL, 3},
  { "mp", OP8(0xfcLL), MASK_SS_LL, INSTR_SS_LL, 3},
  { "sp", OP8(0xfbLL), MASK_SS_LL, INSTR_SS_LL, 3},
  { "ap", OP8(0xfaLL), MASK_SS_LL, INSTR_SS_LL, 3},
  { "cp", OP8(0xf9LL), MASK_SS_LL, INSTR_SS_LL, 3},
  { "zap", OP8(0xf8LL), MASK_SS_LL, INSTR_SS_LL, 3},
  { "unpk", OP8(0xf3LL), MASK_SS_LL, INSTR_SS_LL, 3},
  { "pack", OP8(0xf2LL), MASK_SS_LL, INSTR_SS_LL, 3},
  { "mvo", OP8(0xf1LL), MASK_SS_LL, INSTR_SS_LL, 3},
  { "srp", OP8(0xf0LL), MASK_SS_LI, INSTR_SS_LI, 3},
  { "lmd", OP8(0xefLL), MASK_SS_LMD, INSTR_SS_LMD, 2},
  { "plo", OP8(0xeeLL), MASK_SS_PLO, INSTR_SS_PLO, 3},
  { "msdb", OP48(0xed000000001fLL), MASK_RXF, INSTR_RXF, 3},
  { "madb", OP48(0xed000000001eLL), MASK_RXF, INSTR_RXF, 3},
  { "ddb", OP48(0xed000000001dLL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "mdb", OP48(0xed000000001cLL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "sdb", OP48(0xed000000001bLL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "adb", OP48(0xed000000001aLL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "cdb", OP48(0xed0000000019LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "kdb", OP48(0xed0000000018LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "meeb", OP48(0xed0000000017LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "sqdb", OP48(0xed0000000015LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "sqeb", OP48(0xed0000000014LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "tcxb", OP48(0xed0000000012LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "tcdb", OP48(0xed0000000011LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "tceb", OP48(0xed0000000010LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "mseb", OP48(0xed000000000fLL), MASK_RXF, INSTR_RXF, 3},
  { "maeb", OP48(0xed000000000eLL), MASK_RXF, INSTR_RXF, 3},
  { "deb", OP48(0xed000000000dLL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "mdeb", OP48(0xed000000000cLL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "seb", OP48(0xed000000000bLL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "aeb", OP48(0xed000000000aLL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "ceb", OP48(0xed0000000009LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "keb", OP48(0xed0000000008LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "mxdb", OP48(0xed0000000007LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "lxeb", OP48(0xed0000000006LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "lxdb", OP48(0xed0000000005LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "ldeb", OP48(0xed0000000004LL), MASK_RXE_F, INSTR_RXE_F, 3},
  { "brxlg", OP48(0xec0000000045LL), MASK_RIE_A, INSTR_RIE_A, 2},
  { "brxhg", OP48(0xec0000000044LL), MASK_RIE_A, INSTR_RIE_A, 2},
  { "lmh", OP48(0xeb0000000096LL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "icmh", OP48(0xeb0000000080LL), MASK_RSE_M, INSTR_RSE_M, 2},
  { "bxleg", OP48(0xeb0000000045LL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "bxhg", OP48(0xeb0000000044LL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "cdsg", OP48(0xeb000000003eLL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "csg", OP48(0xeb0000000030LL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "lctlg", OP48(0xeb000000002fLL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "stcmh", OP48(0xeb000000002cLL), MASK_RSE_M, INSTR_RSE_M, 2},
  { "stmh", OP48(0xeb0000000026LL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "stctg", OP48(0xeb0000000025LL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "stmg", OP48(0xeb0000000024LL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "clmh", OP48(0xeb0000000020LL), MASK_RSE_M, INSTR_RSE_M, 2},
  { "rll", OP48(0xeb000000001dLL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "rllg", OP48(0xeb000000001cLL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "tracg", OP48(0xeb000000000fLL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "sllg", OP48(0xeb000000000dLL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "srlg", OP48(0xeb000000000cLL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "slag", OP48(0xeb000000000bLL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "srag", OP48(0xeb000000000aLL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "lmg", OP48(0xeb0000000004LL), MASK_RSE_R, INSTR_RSE_R, 2},
  { "mvcin", OP8(0xe8LL), MASK_SS_L, INSTR_SS_L, 3},
  { "mvcdk", OP16(0xe50fLL), MASK_SSE, INSTR_SSE, 3},
  { "mvcsk", OP16(0xe50eLL), MASK_SSE, INSTR_SSE, 3},
  { "tprot", OP16(0xe501LL), MASK_SSE, INSTR_SSE, 3},
  { "strag", OP48(0xe50000000002LL), MASK_SSE, INSTR_SSE, 2},
  { "lasp", OP16(0xe500LL), MASK_SSE, INSTR_SSE, 3},
  { "slb", OP48(0xe30000000099LL), MASK_RXE, INSTR_RXE, 2},
  { "alc", OP48(0xe30000000098LL), MASK_RXE, INSTR_RXE, 2},
  { "dl", OP48(0xe30000000097LL), MASK_RXE, INSTR_RXE, 2},
  { "ml", OP48(0xe30000000096LL), MASK_RXE, INSTR_RXE, 2},
  { "llgh", OP48(0xe30000000091LL), MASK_RXE, INSTR_RXE, 2},
  { "llgc", OP48(0xe30000000090LL), MASK_RXE, INSTR_RXE, 2},
  { "lpq", OP48(0xe3000000008fLL), MASK_RXE, INSTR_RXE, 2},
  { "stpq", OP48(0xe3000000008eLL), MASK_RXE, INSTR_RXE, 2},
  { "slbg", OP48(0xe30000000089LL), MASK_RXE, INSTR_RXE, 2},
  { "alcg", OP48(0xe30000000088LL), MASK_RXE, INSTR_RXE, 2},
  { "dlg", OP48(0xe30000000087LL), MASK_RXE, INSTR_RXE, 2},
  { "mlg", OP48(0xe30000000086LL), MASK_RXE, INSTR_RXE, 2},
  { "xg", OP48(0xe30000000082LL), MASK_RXE, INSTR_RXE, 2},
  { "og", OP48(0xe30000000081LL), MASK_RXE, INSTR_RXE, 2},
  { "ng", OP48(0xe30000000080LL), MASK_RXE, INSTR_RXE, 2},
  { "bctg", OP48(0xe30000000046LL), MASK_RXE, INSTR_RXE, 2},
  { "strvh", OP48(0xe3000000003fLL), MASK_RXE, INSTR_RXE, 2},
  { "strv", OP48(0xe3000000003eLL), MASK_RXE, INSTR_RXE, 2},
  { "clgf", OP48(0xe30000000031LL), MASK_RXE, INSTR_RXE, 2},
  { "cgf", OP48(0xe30000000030LL), MASK_RXE, INSTR_RXE, 2},
  { "strvg", OP48(0xe3000000002fLL), MASK_RXE, INSTR_RXE, 2},
  { "cvdg", OP48(0xe3000000002eLL), MASK_RXE, INSTR_RXE, 2},
  { "stg", OP48(0xe30000000024LL), MASK_RXE, INSTR_RXE, 2},
  { "clg", OP48(0xe30000000021LL), MASK_RXE, INSTR_RXE, 2},
  { "cg", OP48(0xe30000000020LL), MASK_RXE, INSTR_RXE, 2},
  { "lrvh", OP48(0xe3000000001fLL), MASK_RXE, INSTR_RXE, 2},
  { "lrv", OP48(0xe3000000001eLL), MASK_RXE, INSTR_RXE, 2},
  { "dsgf", OP48(0xe3000000001dLL), MASK_RXE, INSTR_RXE, 2},
  { "msgf", OP48(0xe3000000001cLL), MASK_RXE, INSTR_RXE, 2},
  { "slgf", OP48(0xe3000000001bLL), MASK_RXE, INSTR_RXE, 2},
  { "algf", OP48(0xe3000000001aLL), MASK_RXE, INSTR_RXE, 2},
  { "sgf", OP48(0xe30000000019LL), MASK_RXE, INSTR_RXE, 2},
  { "agf", OP48(0xe30000000018LL), MASK_RXE, INSTR_RXE, 2},
  { "llgt", OP48(0xe30000000017LL), MASK_RXE, INSTR_RXE, 2},
  { "llgf", OP48(0xe30000000016LL), MASK_RXE, INSTR_RXE, 2},
  { "lgf", OP48(0xe30000000014LL), MASK_RXE, INSTR_RXE, 2},
  { "lrvg", OP48(0xe3000000000fLL), MASK_RXE, INSTR_RXE, 2},
  { "cvbg", OP48(0xe3000000000eLL), MASK_RXE, INSTR_RXE, 2},
  { "dsg", OP48(0xe3000000000dLL), MASK_RXE, INSTR_RXE, 2},
  { "msg", OP48(0xe3000000000cLL), MASK_RXE, INSTR_RXE, 2},
  { "slg", OP48(0xe3000000000bLL), MASK_RXE, INSTR_RXE, 2},
  { "alg", OP48(0xe3000000000aLL), MASK_RXE, INSTR_RXE, 2},
  { "sg", OP48(0xe30000000009LL), MASK_RXE, INSTR_RXE, 2},
  { "ag", OP48(0xe30000000008LL), MASK_RXE, INSTR_RXE, 2},
  { "lg", OP48(0xe30000000004LL), MASK_RXE, INSTR_RXE, 2},
  { "lrag", OP48(0xe30000000003LL), MASK_RXE, INSTR_RXE, 2},
  { "edmk", OP8(0xdfLL), MASK_SS_L, INSTR_SS_L, 3},
  { "ed", OP8(0xdeLL), MASK_SS_L, INSTR_SS_L, 3},
  { "trt", OP8(0xddLL), MASK_SS_L, INSTR_SS_L, 3},
  { "tr", OP8(0xdcLL), MASK_SS_L, INSTR_SS_L, 3},
  { "mvcs", OP8(0xdbLL), MASK_SS_RR, INSTR_SS_RR, 3},
  { "mvcp", OP8(0xdaLL), MASK_SS_RR, INSTR_SS_RR, 3},
  { "mvck", OP8(0xd9LL), MASK_SS_RR, INSTR_SS_RR, 3},
  { "xc", OP8(0xd7LL), MASK_SS_L, INSTR_SS_L, 3},
  { "oc", OP8(0xd6LL), MASK_SS_L, INSTR_SS_L, 3},
  { "clc", OP8(0xd5LL), MASK_SS_L, INSTR_SS_L, 3},
  { "nc", OP8(0xd4LL), MASK_SS_L, INSTR_SS_L, 3},
  { "mvz", OP8(0xd3LL), MASK_SS_L, INSTR_SS_L, 3},
  { "mvc", OP8(0xd2LL), MASK_SS_L, INSTR_SS_L, 3},
  { "mvn", OP8(0xd1LL), MASK_SS_L, INSTR_SS_L, 3},
  { "jg", OP16(0xc0f4LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgno", OP16(0xc0e4LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgnh", OP16(0xc0d4LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgnp", OP16(0xc0d4LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgle", OP16(0xc0c4LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgnl", OP16(0xc0b4LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgnm", OP16(0xc0b4LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jghe", OP16(0xc0a4LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jge", OP16(0xc084LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgz", OP16(0xc084LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgne", OP16(0xc074LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgnz", OP16(0xc074LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgnhe", OP16(0xc054LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgl", OP16(0xc044LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgm", OP16(0xc044LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgnle", OP16(0xc034LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgh", OP16(0xc024LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgp", OP16(0xc024LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "jgo", OP16(0xc014LL), MASK_RIL_B, INSTR_RIL_B, 2},
  { "brasl", OP16(0xc005LL), MASK_RIL_A, INSTR_RIL_A, 2},
  { "brcl", OP16(0xc004LL), MASK_RIL_MA, INSTR_RIL_MA, 2},
  { "larl", OP16(0xc000LL), MASK_RIL_A, INSTR_RIL_A, 2},
  { "icm", OP8(0xbfLL), MASK_RS_M, INSTR_RS_M, 3},
  { "stcm", OP8(0xbeLL), MASK_RS_M, INSTR_RS_M, 3},
  { "clm", OP8(0xbdLL), MASK_RS_M, INSTR_RS_M, 3},
  { "cds", OP8(0xbbLL), MASK_RS, INSTR_RS, 3},
  { "cs", OP8(0xbaLL), MASK_RS, INSTR_RS, 3},
  { "esea", OP16(0xb99dLL), MASK_RRE_R, INSTR_RRE_R, 2},
  { "slbr", OP16(0xb999LL), MASK_RRE, INSTR_RRE, 2},
  { "alcr", OP16(0xb998LL), MASK_RRE, INSTR_RRE, 2},
  { "dlr", OP16(0xb997LL), MASK_RRE, INSTR_RRE, 2},
  { "mlr", OP16(0xb996LL), MASK_RRE, INSTR_RRE, 2},
  { "epsw", OP16(0xb98dLL), MASK_RRE, INSTR_RRE, 2},
  { "slbgr", OP16(0xb989LL), MASK_RRE, INSTR_RRE, 2},
  { "alcgr", OP16(0xb988LL), MASK_RRE, INSTR_RRE, 2},
  { "dlgr", OP16(0xb987LL), MASK_RRE, INSTR_RRE, 2},
  { "mlgr", OP16(0xb986LL), MASK_RRE, INSTR_RRE, 2},
  { "xgr", OP16(0xb982LL), MASK_RRE, INSTR_RRE, 2},
  { "ogr", OP16(0xb981LL), MASK_RRE, INSTR_RRE, 2},
  { "ngr", OP16(0xb980LL), MASK_RRE, INSTR_RRE, 2},
  { "bctgr", OP16(0xb946LL), MASK_RRE, INSTR_RRE, 2},
  { "clgfr", OP16(0xb931LL), MASK_RRE, INSTR_RRE, 2},
  { "cgfr", OP16(0xb930LL), MASK_RRE, INSTR_RRE, 2},
  { "sturg", OP16(0xb925LL), MASK_RRE, INSTR_RRE, 2},
  { "clgr", OP16(0xb921LL), MASK_RRE, INSTR_RRE, 2},
  { "cgr", OP16(0xb920LL), MASK_RRE, INSTR_RRE, 2},
  { "lrvr", OP16(0xb91fLL), MASK_RRE, INSTR_RRE, 2},
  { "dsgfr", OP16(0xb91dLL), MASK_RRE, INSTR_RRE, 2},
  { "msgfr", OP16(0xb91cLL), MASK_RRE, INSTR_RRE, 2},
  { "slgfr", OP16(0xb91bLL), MASK_RRE, INSTR_RRE, 2},
  { "algfr", OP16(0xb91aLL), MASK_RRE, INSTR_RRE, 2},
  { "sgfr", OP16(0xb919LL), MASK_RRE, INSTR_RRE, 2},
  { "agfr", OP16(0xb918LL), MASK_RRE, INSTR_RRE, 2},
  { "llgtr", OP16(0xb917LL), MASK_RRE, INSTR_RRE, 2},
  { "llgfr", OP16(0xb916LL), MASK_RRE, INSTR_RRE, 2},
  { "lgfr", OP16(0xb914LL), MASK_RRE, INSTR_RRE, 2},
  { "lcgfr", OP16(0xb913LL), MASK_RRE, INSTR_RRE, 2},
  { "ltgfr", OP16(0xb912LL), MASK_RRE, INSTR_RRE, 2},
  { "lngfr", OP16(0xb911LL), MASK_RRE, INSTR_RRE, 2},
  { "lpgfr", OP16(0xb910LL), MASK_RRE, INSTR_RRE, 2},
  { "lrvgr", OP16(0xb90fLL), MASK_RRE, INSTR_RRE, 2},
  { "eregg", OP16(0xb90eLL), MASK_RRE, INSTR_RRE, 2},
  { "dsgr", OP16(0xb90dLL), MASK_RRE, INSTR_RRE, 2},
  { "msgr", OP16(0xb90cLL), MASK_RRE, INSTR_RRE, 2},
  { "slgr", OP16(0xb90bLL), MASK_RRE, INSTR_RRE, 2},
  { "algr", OP16(0xb90aLL), MASK_RRE, INSTR_RRE, 2},
  { "sgr", OP16(0xb909LL), MASK_RRE, INSTR_RRE, 2},
  { "agr", OP16(0xb908LL), MASK_RRE, INSTR_RRE, 2},
  { "lurag", OP16(0xb905LL), MASK_RRE, INSTR_RRE, 2},
  { "lgr", OP16(0xb904LL), MASK_RRE, INSTR_RRE, 2},
  { "lcgr", OP16(0xb903LL), MASK_RRE, INSTR_RRE, 2},
  { "ltgr", OP16(0xb902LL), MASK_RRE, INSTR_RRE, 2},
  { "lngr", OP16(0xb901LL), MASK_RRE, INSTR_RRE, 2},
  { "lpgr", OP16(0xb900LL), MASK_RRE, INSTR_RRE, 2},
  { "lctl", OP8(0xb7LL), MASK_RS_C, INSTR_RS_C, 3},
  { "stctl", OP8(0xb6LL), MASK_RS_C, INSTR_RS_C, 3},
  { "cgxr", OP16(0xb3caLL), MASK_RRF_F, INSTR_RRF_F, 2},
  { "cgdr", OP16(0xb3c9LL), MASK_RRF_F, INSTR_RRF_F, 2},
  { "cger", OP16(0xb3c8LL), MASK_RRF_F, INSTR_RRF_F, 2},
  { "cxgr", OP16(0xb3c6LL), MASK_RRE, INSTR_RRE, 2},
  { "cdgr", OP16(0xb3c5LL), MASK_RRE, INSTR_RRE, 2},
  { "cegr", OP16(0xb3c4LL), MASK_RRE, INSTR_RRE, 2},
  { "cgxbr", OP16(0xb3aaLL), MASK_RRF_M, INSTR_RRF_M, 2},
  { "cgdbr", OP16(0xb3a9LL), MASK_RRF_M, INSTR_RRF_M, 2},
  { "cgebr", OP16(0xb3a8LL), MASK_RRF_M, INSTR_RRF_M, 2},
  { "cxgbr", OP16(0xb3a6LL), MASK_RRE, INSTR_RRE, 2},
  { "cdgbr", OP16(0xb3a5LL), MASK_RRE, INSTR_RRE, 2},
  { "cegbr", OP16(0xb3a4LL), MASK_RRE, INSTR_RRE, 2},
  { "cfxbr", OP16(0xb39aLL), MASK_RRF_M, INSTR_RRF_M, 3},
  { "cfdbr", OP16(0xb399LL), MASK_RRF_M, INSTR_RRF_M, 3},
  { "cfebr", OP16(0xb398LL), MASK_RRF_M, INSTR_RRF_M, 3},
  { "cxfbr", OP16(0xb396LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "cdfbr", OP16(0xb395LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "cefbr", OP16(0xb394LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "efpc", OP16(0xb38cLL), MASK_RRE, INSTR_RRE, 3},
  { "sfpc", OP16(0xb384LL), MASK_RRE, INSTR_RRE, 3},
  { "fidbr", OP16(0xb35fLL), MASK_RRF_M, INSTR_RRF_M, 3},
  { "didbr", OP16(0xb35bLL), MASK_RRF_RM, INSTR_RRF_RM, 3},
  { "fiebr", OP16(0xb357LL), MASK_RRF_M, INSTR_RRF_M, 3},
  { "diebr", OP16(0xb353LL), MASK_RRF_RM, INSTR_RRF_RM, 3},
  { "dxbr", OP16(0xb34dLL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "mxbr", OP16(0xb34cLL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "sxbr", OP16(0xb34bLL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "axbr", OP16(0xb34aLL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "cxbr", OP16(0xb349LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "kxbr", OP16(0xb348LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "fixbr", OP16(0xb347LL), MASK_RRF_M, INSTR_RRF_M, 3},
  { "lexbr", OP16(0xb346LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "ldxbr", OP16(0xb345LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "ledbr", OP16(0xb344LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "lcxbr", OP16(0xb343LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "ltxbr", OP16(0xb342LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "lnxbr", OP16(0xb341LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "lpxbr", OP16(0xb340LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "msdbr", OP16(0xb31fLL), MASK_RRF_R, INSTR_RRF_R, 3},
  { "madbr", OP16(0xb31eLL), MASK_RRF_R, INSTR_RRF_R, 3},
  { "ddbr", OP16(0xb31dLL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "mdbr", OP16(0xb31cLL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "sdbr", OP16(0xb31bLL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "adbr", OP16(0xb31aLL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "cdbr", OP16(0xb319LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "kdbr", OP16(0xb318LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "meebr", OP16(0xb317LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "sqxbr", OP16(0xb316LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "sqdbr", OP16(0xb315LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "sqebr", OP16(0xb314LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "lcdbr", OP16(0xb313LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "ltdbr", OP16(0xb312LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "lndbr", OP16(0xb311LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "lpdbr", OP16(0xb310LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "msebr", OP16(0xb30fLL), MASK_RRF_R, INSTR_RRF_R, 3},
  { "maebr", OP16(0xb30eLL), MASK_RRF_R, INSTR_RRF_R, 3},
  { "debr", OP16(0xb30dLL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "mdebr", OP16(0xb30cLL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "sebr", OP16(0xb30bLL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "aebr", OP16(0xb30aLL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "cebr", OP16(0xb309LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "kebr", OP16(0xb308LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "mxdbr", OP16(0xb307LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "lxebr", OP16(0xb306LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "lxdbr", OP16(0xb305LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "ldebr", OP16(0xb304LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "lcebr", OP16(0xb303LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "ltebr", OP16(0xb302LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "lnebr", OP16(0xb301LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "lpebr", OP16(0xb300LL), MASK_RRE_F, INSTR_RRE_F, 3},
  { "trap4", OP16(0xb2ffLL), MASK_S, INSTR_S, 3},
  { "lpswe", OP16(0xb2b2LL), MASK_S, INSTR_S, 2},
  { "stfl", OP16(0xb2b1LL), MASK_S, INSTR_S, 2},
  { "cutfu", OP16(0xb2a7LL), MASK_RRE, INSTR_RRE, 3},
  { "cuutf", OP16(0xb2a6LL), MASK_RRE, INSTR_RRE, 3},
  { "tre", OP16(0xb2a5LL), MASK_RRE, INSTR_RRE, 2},
  { "lfpc", OP16(0xb29dLL), MASK_S, INSTR_S, 3},
  { "stfpc", OP16(0xb29cLL), MASK_S, INSTR_S, 3},
  { "srnm", OP16(0xb299LL), MASK_S, INSTR_S, 3},
  { "stsi", OP16(0xb27dLL), MASK_S, INSTR_S, 3},
  { "sacf", OP16(0xb279LL), MASK_S, INSTR_S, 3},
  { "stcke", OP16(0xb278LL), MASK_S, INSTR_S, 2},
  { "rp", OP16(0xb277LL), MASK_S, INSTR_S, 3},
  { "siga", OP16(0xb274LL), MASK_S, INSTR_S, 3},
  { "cmpsc", OP16(0xb263LL), MASK_RRE, INSTR_RRE, 3},
  { "srst", OP16(0xb25eLL), MASK_RRE, INSTR_RRE, 3},
  { "clst", OP16(0xb25dLL), MASK_RRE, INSTR_RRE, 3},
  { "bsa", OP16(0xb25aLL), MASK_RRE, INSTR_RRE, 3},
  { "bsg", OP16(0xb258LL), MASK_RRE, INSTR_RRE, 3},
  { "cuse", OP16(0xb257LL), MASK_RRE, INSTR_RRE, 3},
  { "mvst", OP16(0xb255LL), MASK_RRE, INSTR_RRE, 3},
  { "mvpg", OP16(0xb254LL), MASK_RRE, INSTR_RRE, 3},
  { "msr", OP16(0xb252LL), MASK_RRE, INSTR_RRE, 3},
  { "csp", OP16(0xb250LL), MASK_RRE, INSTR_RRE, 3},
  { "ear", OP16(0xb24fLL), MASK_RRE_RA, INSTR_RRE_RA, 3},
  { "sar", OP16(0xb24eLL), MASK_RRE_AR, INSTR_RRE_AR, 3},
  { "cpya", OP16(0xb24dLL), MASK_RRE_A, INSTR_RRE_A, 3},
  { "tar", OP16(0xb24cLL), MASK_RRE_AR, INSTR_RRE_AR, 3},
  { "lura", OP16(0xb24bLL), MASK_RRE, INSTR_RRE, 3},
  { "esta", OP16(0xb24aLL), MASK_RRE, INSTR_RRE, 3},
  { "ereg", OP16(0xb249LL), MASK_RRE, INSTR_RRE, 3},
  { "palb", OP16(0xb248LL), MASK_RRE_O, INSTR_RRE_O, 3},
  { "msta", OP16(0xb247LL), MASK_RRE_R, INSTR_RRE_R, 3},
  { "stura", OP16(0xb246LL), MASK_RRE, INSTR_RRE, 3},
  { "sqer", OP16(0xb245LL), MASK_RRE_E, INSTR_RRE_E, 3},
  { "sqdr", OP16(0xb244LL), MASK_RRE_D, INSTR_RRE_D, 3},
  { "cksm", OP16(0xb241LL), MASK_RRE, INSTR_RRE, 3},
  { "bakr", OP16(0xb240LL), MASK_RRE, INSTR_RRE, 3},
  { "schm", OP16(0xb23cLL), MASK_S_O, INSTR_S_O, 3},
  { "rchp", OP16(0xb23bLL), MASK_S_O, INSTR_S_O, 3},
  { "stcps", OP16(0xb23aLL), MASK_S, INSTR_S, 3},
  { "stcrw", OP16(0xb239LL), MASK_S, INSTR_S, 3},
  { "rsch", OP16(0xb238LL), MASK_S_O, INSTR_S_O, 3},
  { "sal", OP16(0xb237LL), MASK_S_O, INSTR_S_O, 3},
  { "tpi", OP16(0xb236LL), MASK_S, INSTR_S, 3},
  { "tsch", OP16(0xb235LL), MASK_S, INSTR_S, 3},
  { "stsch", OP16(0xb234LL), MASK_S, INSTR_S, 3},
  { "ssch", OP16(0xb233LL), MASK_S, INSTR_S, 3},
  { "msch", OP16(0xb232LL), MASK_S, INSTR_S, 3},
  { "hsch", OP16(0xb231LL), MASK_S_O, INSTR_S_O, 3},
  { "csch", OP16(0xb230LL), MASK_S_O, INSTR_S_O, 3},
  { "dxr", OP16(0xb22dLL), MASK_RRE_X, INSTR_RRE_X, 3},
  { "tb", OP16(0xb22cLL), MASK_RRE_R2, INSTR_RRE_R2, 3},
  { "sske", OP16(0xb22bLL), MASK_RRE, INSTR_RRE, 3},
  { "rrbe", OP16(0xb22aLL), MASK_RRE, INSTR_RRE, 3},
  { "iske", OP16(0xb229LL), MASK_RRE, INSTR_RRE, 3},
  { "pt", OP16(0xb228LL), MASK_RRE, INSTR_RRE, 3},
  { "esar", OP16(0xb227LL), MASK_RRE_R, INSTR_RRE_R, 3},
  { "epar", OP16(0xb226LL), MASK_RRE_R, INSTR_RRE_R, 3},
  { "ssar", OP16(0xb225LL), MASK_RRE_R, INSTR_RRE_R, 3},
  { "iac", OP16(0xb224LL), MASK_RRE_R, INSTR_RRE_R, 3},
  { "ivsk", OP16(0xb223LL), MASK_RRE, INSTR_RRE, 3},
  { "ipm", OP16(0xb222LL), MASK_RRE_R, INSTR_RRE_R, 3},
  { "ipte", OP16(0xb221LL), MASK_RRE, INSTR_RRE, 3},
  { "cfc", OP16(0xb21aLL), MASK_S, INSTR_S, 3},
  { "sac", OP16(0xb219LL), MASK_S, INSTR_S, 3},
  { "pc", OP16(0xb218LL), MASK_S, INSTR_S, 3},
  { "sie", OP16(0xb214LL), MASK_S, INSTR_S, 3},
  { "stap", OP16(0xb212LL), MASK_S, INSTR_S, 3},
  { "stpx", OP16(0xb211LL), MASK_S, INSTR_S, 3},
  { "spx", OP16(0xb210LL), MASK_S, INSTR_S, 3},
  { "ptlb", OP16(0xb20dLL), MASK_S_O, INSTR_S_O, 3},
  { "ipk", OP16(0xb20bLL), MASK_S_O, INSTR_S_O, 3},
  { "spka", OP16(0xb20aLL), MASK_S, INSTR_S, 3},
  { "stpt", OP16(0xb209LL), MASK_S, INSTR_S, 3},
  { "spt", OP16(0xb208LL), MASK_S, INSTR_S, 3},
  { "stckc", OP16(0xb207LL), MASK_S, INSTR_S, 3},
  { "sckc", OP16(0xb206LL), MASK_S, INSTR_S, 3},
  { "stck", OP16(0xb205LL), MASK_S, INSTR_S, 3},
  { "sck", OP16(0xb204LL), MASK_S, INSTR_S, 3},
  { "stidp", OP16(0xb202LL), MASK_S, INSTR_S, 3},
  { "lra", OP8(0xb1LL), MASK_RX, INSTR_RX, 3},
  { "mc", OP8(0xafLL), MASK_SI, INSTR_SI, 3},
  { "sigp", OP8(0xaeLL), MASK_RS, INSTR_RS, 3},
  { "stosm", OP8(0xadLL), MASK_SI, INSTR_SI, 3},
  { "stnsm", OP8(0xacLL), MASK_SI, INSTR_SI, 3},
  { "clcle", OP8(0xa9LL), MASK_RS, INSTR_RS, 3},
  { "mvcle", OP8(0xa8LL), MASK_RS, INSTR_RS, 3},
  { "j", OP16(0xa7f4LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jno", OP16(0xa7e4LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jnh", OP16(0xa7d4LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jnp", OP16(0xa7d4LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jle", OP16(0xa7c4LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jnl", OP16(0xa7b4LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jnm", OP16(0xa7b4LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jhe", OP16(0xa7a4LL), MASK_RI_B, INSTR_RI_B, 3},
  { "je", OP16(0xa784LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jz", OP16(0xa784LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jne", OP16(0xa774LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jnz", OP16(0xa774LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jnhe", OP16(0xa754LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jl", OP16(0xa744LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jm", OP16(0xa744LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jnle", OP16(0xa734LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jh", OP16(0xa724LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jp", OP16(0xa724LL), MASK_RI_B, INSTR_RI_B, 3},
  { "jo", OP16(0xa714LL), MASK_RI_B, INSTR_RI_B, 3},
  { "cghi", OP16(0xa70fLL), MASK_RI, INSTR_RI, 2},
  { "chi", OP16(0xa70eLL), MASK_RI, INSTR_RI, 3},
  { "mghi", OP16(0xa70dLL), MASK_RI, INSTR_RI, 2},
  { "mhi", OP16(0xa70cLL), MASK_RI, INSTR_RI, 3},
  { "aghi", OP16(0xa70bLL), MASK_RI, INSTR_RI, 2},
  { "ahi", OP16(0xa70aLL), MASK_RI, INSTR_RI, 3},
  { "lghi", OP16(0xa709LL), MASK_RI, INSTR_RI, 2},
  { "lhi", OP16(0xa708LL), MASK_RI, INSTR_RI, 3},
  { "brctg", OP16(0xa707LL), MASK_RI_A, INSTR_RI_A, 2},
  { "brct", OP16(0xa706LL), MASK_RI_A, INSTR_RI_A, 3},
  { "bras", OP16(0xa705LL), MASK_RI_A, INSTR_RI_A, 3},
  { "brc", OP16(0xa704LL), MASK_RI_MA, INSTR_RI_MA, 3},
  { "tmhl", OP16(0xa703LL), MASK_RI_U, INSTR_RI_U, 2},
  { "tmhh", OP16(0xa702LL), MASK_RI_U, INSTR_RI_U, 2},
  { "tml", OP16(0xa701LL), MASK_RI_U, INSTR_RI_U, 3},
  { "tmll", OP16(0xa701LL), MASK_RI_U, INSTR_RI_U, 2},
  { "tmh", OP16(0xa700LL), MASK_RI_U, INSTR_RI_U, 3},
  { "tmlh", OP16(0xa700LL), MASK_RI_U, INSTR_RI_U, 2},
  { "llill", OP16(0xa50fLL), MASK_RI_U, INSTR_RI_U, 2},
  { "llilh", OP16(0xa50eLL), MASK_RI_U, INSTR_RI_U, 2},
  { "llihl", OP16(0xa50dLL), MASK_RI_U, INSTR_RI_U, 2},
  { "llihh", OP16(0xa50cLL), MASK_RI_U, INSTR_RI_U, 2},
  { "oill", OP16(0xa50bLL), MASK_RI_U, INSTR_RI_U, 2},
  { "oilh", OP16(0xa50aLL), MASK_RI_U, INSTR_RI_U, 2},
  { "oihl", OP16(0xa509LL), MASK_RI_U, INSTR_RI_U, 2},
  { "oihh", OP16(0xa508LL), MASK_RI_U, INSTR_RI_U, 2},
  { "nill", OP16(0xa507LL), MASK_RI_U, INSTR_RI_U, 2},
  { "nilh", OP16(0xa506LL), MASK_RI_U, INSTR_RI_U, 2},
  { "nihl", OP16(0xa505LL), MASK_RI_U, INSTR_RI_U, 2},
  { "nihh", OP16(0xa504LL), MASK_RI_U, INSTR_RI_U, 2},
  { "iill", OP16(0xa503LL), MASK_RI_U, INSTR_RI_U, 2},
  { "iilh", OP16(0xa502LL), MASK_RI_U, INSTR_RI_U, 2},
  { "iihl", OP16(0xa501LL), MASK_RI_U, INSTR_RI_U, 2},
  { "iihh", OP16(0xa500LL), MASK_RI_U, INSTR_RI_U, 2},
  { "stam", OP8(0x9bLL), MASK_RS_A, INSTR_RS_A, 3},
  { "lam", OP8(0x9aLL), MASK_RS_A, INSTR_RS_A, 3},
  { "trace", OP8(0x99LL), MASK_RS, INSTR_RS, 3},
  { "lm", OP8(0x98LL), MASK_RS, INSTR_RS, 3},
  { "xi", OP8(0x97LL), MASK_SI, INSTR_SI, 3},
  { "oi", OP8(0x96LL), MASK_SI, INSTR_SI, 3},
  { "cli", OP8(0x95LL), MASK_SI, INSTR_SI, 3},
  { "ni", OP8(0x94LL), MASK_SI, INSTR_SI, 3},
  { "ts", OP8(0x93LL), MASK_S, INSTR_S, 3},
  { "mvi", OP8(0x92LL), MASK_SI, INSTR_SI, 3},
  { "tm", OP8(0x91LL), MASK_SI, INSTR_SI, 3},
  { "stm", OP8(0x90LL), MASK_RS, INSTR_RS, 3},
  { "slda", OP8(0x8fLL), MASK_RS_D, INSTR_RS_D, 3},
  { "srda", OP8(0x8eLL), MASK_RS_D, INSTR_RS_D, 3},
  { "sldl", OP8(0x8dLL), MASK_RS_D, INSTR_RS_D, 3},
  { "srdl", OP8(0x8cLL), MASK_RS_D, INSTR_RS_D, 3},
  { "sla", OP8(0x8bLL), MASK_RS_S, INSTR_RS_S, 3},
  { "sra", OP8(0x8aLL), MASK_RS_S, INSTR_RS_S, 3},
  { "sll", OP8(0x89LL), MASK_RS_S, INSTR_RS_S, 3},
  { "srl", OP8(0x88LL), MASK_RS_S, INSTR_RS_S, 3},
  { "bxle", OP8(0x87LL), MASK_RS, INSTR_RS, 3},
  { "bxh", OP8(0x86LL), MASK_RS, INSTR_RS, 3},
  { "brxle", OP8(0x85LL), MASK_RSI_A, INSTR_RSI_A, 3},
  { "brxh", OP8(0x84LL), MASK_RSI_A, INSTR_RSI_A, 3},
  { "diag", OP8(0x83LL), MASK_RS, INSTR_RS, 3},
  { "lpsw", OP8(0x82LL), MASK_S, INSTR_S, 3},
  { "ssm", OP8(0x80LL), MASK_S, INSTR_S, 3},
  { "su", OP8(0x7fLL), MASK_RX_E, INSTR_RX_E, 3},
  { "au", OP8(0x7eLL), MASK_RX_E, INSTR_RX_E, 3},
  { "de", OP8(0x7dLL), MASK_RX_E, INSTR_RX_E, 3},
  { "me", OP8(0x7cLL), MASK_RX_ED, INSTR_RX_ED, 3},
  { "se", OP8(0x7bLL), MASK_RX_E, INSTR_RX_E, 3},
  { "ae", OP8(0x7aLL), MASK_RX_E, INSTR_RX_E, 3},
  { "ce", OP8(0x79LL), MASK_RX_E, INSTR_RX_E, 3},
  { "le", OP8(0x78LL), MASK_RX_E, INSTR_RX_E, 3},
  { "ms", OP8(0x71LL), MASK_RX, INSTR_RX, 3},
  { "ste", OP8(0x70LL), MASK_RX_E, INSTR_RX_E, 3},
  { "sw", OP8(0x6fLL), MASK_RX_D, INSTR_RX_D, 3},
  { "aw", OP8(0x6eLL), MASK_RX_D, INSTR_RX_D, 3},
  { "dd", OP8(0x6dLL), MASK_RX_D, INSTR_RX_D, 3},
  { "md", OP8(0x6cLL), MASK_RX_D, INSTR_RX_D, 3},
  { "sd", OP8(0x6bLL), MASK_RX_D, INSTR_RX_D, 3},
  { "ad", OP8(0x6aLL), MASK_RX_D, INSTR_RX_D, 3},
  { "cd", OP8(0x69LL), MASK_RX_D, INSTR_RX_D, 3},
  { "ld", OP8(0x68LL), MASK_RX_D, INSTR_RX_D, 3},
  { "mxd", OP8(0x67LL), MASK_RX_DX, INSTR_RX_DX, 3},
  { "std", OP8(0x60LL), MASK_RX_D, INSTR_RX_D, 3},
  { "sl", OP8(0x5fLL), MASK_RX, INSTR_RX, 3},
  { "al", OP8(0x5eLL), MASK_RX, INSTR_RX, 3},
  { "d", OP8(0x5dLL), MASK_RX, INSTR_RX, 3},
  { "m", OP8(0x5cLL), MASK_RX, INSTR_RX, 3},
  { "s", OP8(0x5bLL), MASK_RX, INSTR_RX, 3},
  { "a", OP8(0x5aLL), MASK_RX, INSTR_RX, 3},
  { "c", OP8(0x59LL), MASK_RX, INSTR_RX, 3},
  { "l", OP8(0x58LL), MASK_RX, INSTR_RX, 3},
  { "x", OP8(0x57LL), MASK_RX, INSTR_RX, 3},
  { "o", OP8(0x56LL), MASK_RX, INSTR_RX, 3},
  { "cl", OP8(0x55LL), MASK_RX, INSTR_RX, 3},
  { "n", OP8(0x54LL), MASK_RX, INSTR_RX, 3},
  { "lae", OP8(0x51LL), MASK_RX, INSTR_RX, 3},
  { "st", OP8(0x50LL), MASK_RX, INSTR_RX, 3},
  { "cvb", OP8(0x4fLL), MASK_RX, INSTR_RX, 3},
  { "cvd", OP8(0x4eLL), MASK_RX, INSTR_RX, 3},
  { "bas", OP8(0x4dLL), MASK_RX, INSTR_RX, 3},
  { "mh", OP8(0x4cLL), MASK_RX, INSTR_RX, 3},
  { "sh", OP8(0x4bLL), MASK_RX, INSTR_RX, 3},
  { "ah", OP8(0x4aLL), MASK_RX, INSTR_RX, 3},
  { "ch", OP8(0x49LL), MASK_RX, INSTR_RX, 3},
  { "lh", OP8(0x48LL), MASK_RX, INSTR_RX, 3},
  { "b", OP16(0x47f0LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bno", OP16(0x47e0LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bnh", OP16(0x47d0LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bnp", OP16(0x47d0LL), MASK_RX_B, INSTR_RX_B, 3},
  { "ble", OP16(0x47c0LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bnl", OP16(0x47b0LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bnm", OP16(0x47b0LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bhe", OP16(0x47a0LL), MASK_RX_B, INSTR_RX_B, 3},
  { "be", OP16(0x4780LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bz", OP16(0x4780LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bne", OP16(0x4770LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bnz", OP16(0x4770LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bnhe", OP16(0x4750LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bl", OP16(0x4740LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bm", OP16(0x4740LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bnle", OP16(0x4730LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bh", OP16(0x4720LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bp", OP16(0x4720LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bo", OP16(0x4710LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bc", OP8(0x47LL), MASK_RX_M, INSTR_RX_M, 3},
  { "nop", OP16(0x4700LL), MASK_RX_B, INSTR_RX_B, 3},
  { "bct", OP8(0x46LL), MASK_RX, INSTR_RX, 3},
  { "bal", OP8(0x45LL), MASK_RX, INSTR_RX, 3},
  { "ex", OP8(0x44LL), MASK_RX, INSTR_RX, 3},
  { "ic", OP8(0x43LL), MASK_RX, INSTR_RX, 3},
  { "stc", OP8(0x42LL), MASK_RX, INSTR_RX, 3},
  { "la", OP8(0x41LL), MASK_RX, INSTR_RX, 3},
  { "sth", OP8(0x40LL), MASK_RX, INSTR_RX, 3},
  { "sur", OP8(0x3fLL), MASK_RR_E, INSTR_RR_E, 3},
  { "aur", OP8(0x3eLL), MASK_RR_E, INSTR_RR_E, 3},
  { "der", OP8(0x3dLL), MASK_RR_E, INSTR_RR_E, 3},
  { "mer", OP8(0x3cLL), MASK_RR_ED, INSTR_RR_ED, 3},
  { "ser", OP8(0x3bLL), MASK_RR_E, INSTR_RR_E, 3},
  { "aer", OP8(0x3aLL), MASK_RR_E, INSTR_RR_E, 3},
  { "cer", OP8(0x39LL), MASK_RR_E, INSTR_RR_E, 3},
  { "ler", OP8(0x38LL), MASK_RR_E, INSTR_RR_E, 3},
  { "sxr", OP8(0x37LL), MASK_RR_X, INSTR_RR_X, 3},
  { "axr", OP8(0x36LL), MASK_RR, INSTR_RR, 3},
  { "lrer", OP8(0x35LL), MASK_RR_DE, INSTR_RR_DE, 3},
  { "her", OP8(0x34LL), MASK_RR_E, INSTR_RR_E, 3},
  { "lcer", OP8(0x33LL), MASK_RR_E, INSTR_RR_E, 3},
  { "lter", OP8(0x32LL), MASK_RR_E, INSTR_RR_E, 3},
  { "lner", OP8(0x31LL), MASK_RR_E, INSTR_RR_E, 3},
  { "lper", OP8(0x30LL), MASK_RR_E, INSTR_RR_E, 3},
  { "swr", OP8(0x2fLL), MASK_RR_D, INSTR_RR_D, 3},
  { "awr", OP8(0x2eLL), MASK_RR_D, INSTR_RR_D, 3},
  { "ddr", OP8(0x2dLL), MASK_RR_D, INSTR_RR_D, 3},
  { "mdr", OP8(0x2cLL), MASK_RR_D, INSTR_RR_D, 3},
  { "sdr", OP8(0x2bLL), MASK_RR_D, INSTR_RR_D, 3},
  { "adr", OP8(0x2aLL), MASK_RR_D, INSTR_RR_D, 3},
  { "cdr", OP8(0x29LL), MASK_RR_D, INSTR_RR_D, 3},
  { "ldr", OP8(0x28LL), MASK_RR_D, INSTR_RR_D, 3},
  { "mxdr", OP8(0x27LL), MASK_RR_DX, INSTR_RR_DX, 3},
  { "mxr", OP8(0x26LL), MASK_RR_X, INSTR_RR_X, 3},
  { "lrdr", OP8(0x25LL), MASK_RR_XD, INSTR_RR_XD, 3},
  { "hdr", OP8(0x24LL), MASK_RR_D, INSTR_RR_D, 3},
  { "lcdr", OP8(0x23LL), MASK_RR_D, INSTR_RR_D, 3},
  { "ltdr", OP8(0x22LL), MASK_RR_D, INSTR_RR_D, 3},
  { "lndr", OP8(0x21LL), MASK_RR_D, INSTR_RR_D, 3},
  { "lpdr", OP8(0x20LL), MASK_RR_D, INSTR_RR_D, 3},
  { "slr", OP8(0x1fLL), MASK_RR, INSTR_RR, 3},
  { "alr", OP8(0x1eLL), MASK_RR, INSTR_RR, 3},
  { "dr", OP8(0x1dLL), MASK_RR, INSTR_RR, 3},
  { "mr", OP8(0x1cLL), MASK_RR, INSTR_RR, 3},
  { "sr", OP8(0x1bLL), MASK_RR, INSTR_RR, 3},
  { "ar", OP8(0x1aLL), MASK_RR, INSTR_RR, 3},
  { "cr", OP8(0x19LL), MASK_RR, INSTR_RR, 3},
  { "lr", OP8(0x18LL), MASK_RR, INSTR_RR, 3},
  { "xr", OP8(0x17LL), MASK_RR, INSTR_RR, 3},
  { "or", OP8(0x16LL), MASK_RR, INSTR_RR, 3},
  { "clr", OP8(0x15LL), MASK_RR, INSTR_RR, 3},
  { "nr", OP8(0x14LL), MASK_RR, INSTR_RR, 3},
  { "lcr", OP8(0x13LL), MASK_RR, INSTR_RR, 3},
  { "ltr", OP8(0x12LL), MASK_RR, INSTR_RR, 3},
  { "lnr", OP8(0x11LL), MASK_RR, INSTR_RR, 3},
  { "lpr", OP8(0x10LL), MASK_RR, INSTR_RR, 3},
  { "clcl", OP8(0x0fLL), MASK_RR, INSTR_RR, 3},
  { "mvcl", OP8(0x0eLL), MASK_RR, INSTR_RR, 3},
  { "basr", OP8(0x0dLL), MASK_RR, INSTR_RR, 3},
  { "bassm", OP8(0x0cLL), MASK_RR, INSTR_RR, 3},
  { "bsm", OP8(0x0bLL), MASK_RR, INSTR_RR, 3},
  { "svc", OP8(0x0aLL), MASK_RR_I, INSTR_RR_I, 3},
  { "br", OP16(0x07f0LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bnor", OP16(0x07e0LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bnhr", OP16(0x07d0LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bnpr", OP16(0x07d0LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bler", OP16(0x07c0LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bnlr", OP16(0x07b0LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bnmr", OP16(0x07b0LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bher", OP16(0x07a0LL), MASK_RR_B, INSTR_RR_B, 3},
  { "ber", OP16(0x0780LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bzr", OP16(0x0780LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bner", OP16(0x0770LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bnzr", OP16(0x0770LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bnher", OP16(0x0750LL), MASK_RR_B, INSTR_RR_B, 3},
  { "blr", OP16(0x0740LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bmr", OP16(0x0740LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bnler", OP16(0x0730LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bhr", OP16(0x0720LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bpr", OP16(0x0720LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bor", OP16(0x0710LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bcr", OP8(0x07LL), MASK_RR_M, INSTR_RR_M, 3},
  { "nopr", OP16(0x0700LL), MASK_RR_B, INSTR_RR_B, 3},
  { "bctr", OP8(0x06LL), MASK_RR, INSTR_RR, 3},
  { "balr", OP8(0x05LL), MASK_RR, INSTR_RR, 3},
  { "spm", OP8(0x04LL), MASK_RR_R, INSTR_RR_R, 3},
  { "trap2", OP16(0x01ffLL), MASK_E, INSTR_E, 3},
  { "sam64", OP16(0x010eLL), MASK_E, INSTR_E, 2},
  { "sam31", OP16(0x010dLL), MASK_E, INSTR_E, 2},
  { "sam24", OP16(0x010cLL), MASK_E, INSTR_E, 2},
  { "tam", OP16(0x010bLL), MASK_E, INSTR_E, 2},
  { "sckpf", OP16(0x0107LL), MASK_E, INSTR_E, 3},
  { "upt", OP16(0x0102LL), MASK_E, INSTR_E, 3},
  { "pr", OP16(0x0101LL), MASK_E, INSTR_E, 3}
};

const int s390_num_opcodes =
  sizeof (s390_opcodes) / sizeof (s390_opcodes[0]);



