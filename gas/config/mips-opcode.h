/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log$
 * Revision 1.1  1993/02/08 19:20:26  raeburn
 * from Ralph Campbell's work
 *
 * Revision 1.10.1.2  1992/02/05  21:37:06  kmk
 *	Defined the M_LA_AB enumeral in support of the "la t,A(b)"
 *	instruction.
 *
 *	Eliminated the abbreviated patterns employing the v, w, r,
 *	V, and W specifiers.  The meaning of these specifiers has been
 *	changed to represent an optional register in a single pattern.
 *
 * Revision 1.10  1991/05/22  15:50:46  devrcs
 *	Remove unnecessary macros.  No longer need a special macro to do 32-bit
 *	operands for instructions that specify 16-bit immediates.
 *	Modify the way delays are done.  Missing some coprocessor instruction delay slots.
 *	[91/05/07  09:34:07  duthie]
 *
 *	Bob Setzer fixes + mtcN opcode table fixes
 *	[91/03/06  14:37:06  boot]
 *
 *	Merged changes from Bob Setzer.
 *	[90/11/14  16:02:22  boot]
 *
 *	Fixed several problems including bad code generated for the slt,sltu,
 *	bal, bgezal and bltzal instructions. Also the delay problem with the
 *	mfhi and mflo instructions.
 *	[90/10/11  16:42:42  setzer]
 *
 * Revision 1.7  90/10/07  21:01:04  devrcs
 *	fixed bug 1231 l.d Illegal operands.
 *	Also implemented similar missing macros.
 *	[90/10/02  12:38:56  setzer]
 * 
 *	 Fixed several bugs including 855 (plumhall ansi test)
 *	 and 708 (the alignment problem). Also implemented many
 *	 previously unimplemented macro instructions.
 *	[90/09/27  13:41:34  setzer]
 * 
 * Revision 1.6  90/09/23  16:46:24  devrcs
 *	Updated flags mainly so a nop would be inserted after the ctc1 instruction.
 *	[90/09/12  17:08:41  setzer]
 * 
 *	Fixed register ordering of xori instruction.  Extended all cpu
 *	instruction masks to include MBZ fields.
 *	[90/09/09  07:40:01  gm]
 * 
 *	Fixed floating point bugs.
 *	[90/09/04  15:13:55  setzer]
 * 
 * Revision 1.5  90/08/09  14:31:26  devrcs
 *	Works Well.
 *	[90/08/03  13:03:04  setzer]
 * 
 * Revision 1.4  90/07/06  00:31:56  devrcs
 *	Empty submission: clean up any loose ends
 *	[90/06/29  20:45:43  kim]
 * 
 * $EndLog$
 */
/*
 * mips-opcode.h
 *
 */


#if !defined(__STDC__) && !defined(const)
#define const
#endif

#define INSN_WRITE_GPR_D            0x00000001
#define INSN_WRITE_GPR_S            0x00000002
#define INSN_WRITE_GPR_T            0x00000004
#define INSN_WRITE_GPR_31           0x00000008
#define INSN_WRITE_FPR_D            0x00000010
#define INSN_WRITE_FPR_S            0x00000020
#define INSN_WRITE_FPR_T            0x00000040
#define INSN_READ_GPR_D             0x00000080
#define INSN_READ_GPR_S             0x00000100
#define INSN_READ_GPR_T             0x00000200
#define INSN_READ_GPR_31            0x00000400
#define INSN_READ_FPR_D             0x00000800
#define INSN_READ_FPR_S             0x00001000
#define INSN_READ_FPR_T             0x00002000
#define INSN_TRAP                   0x00004000
#define INSN_COND_CODE              0x00008000
#define INSN_TLB                    0x00010000
#define INSN_RFE                    0x00020000
#define INSN_COP                    0x00040000
#define INSN_LOAD_DELAY             0x00080000
#define INSN_UNCOND_BRANCH_DELAY    0x00100000
#define INSN_COND_BRANCH_DELAY      0x00200000
#define INSN_COPROC_DELAY           0x00400000
#define INSN_STORE_DELAY            0x00800000
#define INSN_EXTRA_DELAY            0x01000000
#define INSN_R4000	            0x80000000

#define INSN_MACRO                  0xffffffff

/* 
 * This is a list of macro expanded instructions. 
 *
 * _I appended means immeadiate
 * _A appended means address
 */
enum {
    M_ABS,
    M_ABSU,
    M_ADD_I,
    M_ADDU_I,
    M_AND_I,
    M_BEQ_I,
    M_BGE,
    M_BGE_I,
    M_BGEU,
    M_BGEU_I,
    M_BGT,
    M_BGT_I,
    M_BGTU,
    M_BGTU_I,
    M_BLE,
    M_BLE_I,
    M_BLEU,
    M_BLEU_I,
    M_BLT,
    M_BLT_I,
    M_BLTU,
    M_BLTU_I,
    M_BNE_I,
    M_DIV_3,
    M_DIV_3I,
    M_DIVU_3,
    M_DIVU_3I,
    M_L_D,
    M_L_DOB,
    M_L_DAB,
    M_LA,
    M_LA_AB,
    M_LB_A,
    M_LB_AB,
    M_LBU_A,
    M_LBU_AB,
    M_LD_A,
    M_LD_OB,
    M_LD_AB,
    M_LH_A,
    M_LH_AB,
    M_LHU_A,
    M_LHU_AB,
    M_LI,
    M_LI_D,
    M_LI_DD,
    M_LS_A,
    M_LW_A,
    M_LW_AB,
    M_LWC0_A,
    M_LWC0_AB,
    M_LWC1_A,
    M_LWC1_AB,
    M_LWC2_A,
    M_LWC2_AB,
    M_LWC3_A,
    M_LWC3_AB,
    M_LWL_A,
    M_LWL_AB,
    M_LWR_A,
    M_LWR_AB,
    M_MUL,
    M_MUL_I, 
    M_MULO,
    M_MULO_I, 
    M_MULOU,
    M_MULOU_I, 
    M_NOR_I,
    M_OR_I,
    M_REM_3,
    M_REM_3I,
    M_REMU_3,
    M_REMU_3I,
    M_ROL,
    M_ROL_I,
    M_ROR,
    M_ROR_I,
    M_S_DA,
    M_S_DOB,
    M_S_DAB,
    M_S_S,
    M_SD_A,
    M_SD_OB,
    M_SD_AB,
    M_SEQ,
    M_SEQ_I,
    M_SGE,
    M_SGE_I,
    M_SGEU,
    M_SGEU_I,
    M_SGT,
    M_SGT_I,
    M_SGTU,
    M_SGTU_I,
    M_SLE,
    M_SLE_I,
    M_SLEU,
    M_SLEU_I,
    M_SLT_I,
    M_SLTU_I,
    M_SNE,
    M_SNE_I,
    M_SB_A,
    M_SB_AB,
    M_SH_A,
    M_SH_AB,
    M_SW_A,
    M_SW_AB,
    M_SWC0_A,
    M_SWC0_AB,
    M_SWC1_A,
    M_SWC1_AB,
    M_SWC2_A,
    M_SWC2_AB,
    M_SWC3_A,
    M_SWC3_AB,
    M_SWL_A,
    M_SWL_AB,
    M_SWR_A,
    M_SWR_AB,
    M_SUB_I,
    M_SUBU_I,
    M_TRUNCWD,
    M_TRUNCWS,
    M_ULH,
    M_ULH_A,
    M_ULHU,
    M_ULHU_A,
    M_ULW,
    M_ULW_A,
    M_USH,
    M_USH_A,
    M_USW,
    M_USW_A,
    M_XOR_I
};

/*

Kinds of operands:

    a   -   26 bit address
    b   -   base register
    c   -   break code
    d   -   destination register
    i   -   16 bit unsigned immediate
    j   -   16 bit signed immediate
    o   -   16 bit offset
    p   -   pc relative offset
    s   -   source register
    t   -   target register
    r   -   both source and target register
    v   -   both source and destination register
    w   -   both target and destination register
    u   -   upper 16 bits
    x   -   temporary register (usually $at).

    D   -   floating point destination register
    S   -   floating point source register
    T   -   floating point target register
    V   -   both floating source and destination register
    W   -   both floating target and destination register
    I   -   32 bit immediate (built in macro instrs only)

    F   -   Floating point constant (li.d only).

    <   -   Shift constant

*/


/* Short hand so the lines aren't too long. */
/* ANY_DELAY is used in mips.c to determine if we need to add
   nop's in the delay slots */

/* load delays */
#define LDD     INSN_LOAD_DELAY

/* branch delays */
#define BD      INSN_UNCOND_BRANCH_DELAY|INSN_COND_BRANCH_DELAY

/* coprocessor delays */
#define COD     INSN_COPROC_DELAY

/* instructions that require and extra delay */
#define EXD     INSN_EXTRA_DELAY

#define ANY_DELAY (LDD|BD|COD|EXD)
                  

/* None of these are ever referenced in the code??? */
/* There is really no need to distinguish between ALL these 
   different types of load delays ...USE THE ABOVE MACROS INSTEAD*/
#define WR_d    INSN_WRITE_GPR_D
#define WR_s    INSN_WRITE_GPR_S
#define WR_t    INSN_WRITE_GPR_T
#define WR_31   INSN_WRITE_GPR_31       
#define WR_D    INSN_WRITE_FPR_D        
#define WR_S    INSN_WRITE_FPR_S        
#define WR_T    INSN_WRITE_FPR_T        
#define RD_d    INSN_READ_GPR_D         
#define RD_s    INSN_READ_GPR_S         
#define RD_b    INSN_READ_GPR_S         
#define RD_t    INSN_READ_GPR_T         
#define ST_t    INSN_READ_GPR_T         
#define RD_31   INSN_READ_GPR_31        
#define RD_D    INSN_READ_FPR_D         
#define RD_S    INSN_READ_FPR_S         
#define RD_T    INSN_READ_FPR_T         
#define CC      (INSN_COND_CODE|INSN_LOAD_DELAY)
#define ST_T    INSN_READ_FPR_T         
#define ST_C0   INSN_COP
#define ST_C1   INSN_READ_FPR_T         
#define ST_C2   INSN_COP
#define ST_C3   INSN_COP
#define RD_C0   INSN_COP
#define RD_C1   INSN_READ_FPR_T         
#define RD_C2   INSN_COP
#define RD_C3   INSN_COP
#define WR_C0   INSN_COP
#define WR_C1   INSN_READ_FPR_T         
#define WR_C2   INSN_COP
#define WR_C3   INSN_COP


/*  
    The order of overloaded instructions matters.
  Label arguments and register arguments look
  the same. Instructions that can have either for arguments
  must apear in the correct order in this table for the assembler
  to pick the right one. In other words, entries with immediate
  operands must apear after the same instruction with registers.
    Many instructions are short hand for other instructions
  (i.e., The jal <register> instruction is short for jalr <register>).
*/

static const struct mips_opcode mips_opcodes[] = {
{"abs",     "d,v",	0,    (int) M_ABS,	INSN_MACRO	},
{"abs.s",   "D,V",	0x46000005, 0xffff003f,	WR_D|RD_S	},
{"abs.d",   "D,V",	0x46200005, 0xffff003f,	WR_D|RD_S	},
{"absu",    "d,s",	0,    (int) M_ABSU,	INSN_MACRO	},
{"add",     "d,v,t",	0x00000020, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"add",     "t,r,I",	0,    (int) M_ADD_I,	INSN_MACRO	},
{"add.s",   "D,V,T",	0x46000000, 0xfff0003f,	WR_D|RD_S|RD_T	},
{"add.d",   "D,V,T",	0x46200000, 0xfff0003f,	WR_D|RD_S|RD_T	},
{"addi",    "t,r,j",	0x20000000, 0xfe000000,	WR_t|RD_s	},
{"addiu",   "t,r,j",	0x24000000, 0xfc000000,	WR_t|RD_s	},
{"addu",    "d,v,t",	0x00000021, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"addu",    "t,r,I",	0,    (int) M_ADDU_I,	INSN_MACRO	},
{"and",     "d,v,t",	0x00000024, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"and",     "t,r,I",	0,    (int) M_AND_I,	INSN_MACRO	},
{"andi",    "t,r,i",	0x30000000, 0xfc000000,	WR_t|RD_s	},
{"b",       "p",	0x10000000, 0xfc000000,	BD|RD_s|RD_t	}, /* beq 0,0 */
{"bal",     "p",	0x04110000, 0xfc1f0000,	BD|RD_s		}, /* bgezal 0 */
{"bc0f",    "p",	0x41000000, 0xffff0000,	BD		},
{"bc1f",    "p",	0x45000000, 0xffff0000,	BD		},
{"bc2f",    "p",	0x49000000, 0xffff0000,	BD		},
{"bc3f",    "p",	0x4d000000, 0xffff0000,	BD		},
{"bc0t",    "p",	0x41010000, 0xffff0000,	BD		},
{"bc1t",    "p",	0x45010000, 0xffff0000,	BD		},
{"bc2t",    "p",	0x49010000, 0xffff0000,	BD		},
{"bc3t",    "p",	0x4d010000, 0xffff0000,	BD		},
{"beq",     "s,t,p",	0x10000000, 0xfc000000,	BD|RD_s|RD_t	},
{"beq",     "s,I,p",	0,    (int) M_BEQ_I,	INSN_MACRO	},
{"beqz",    "s,p",	0x10000000, 0xfc000000,	BD|RD_s|RD_t	},
{"bge",     "s,t,p",	0,    (int) M_BGE,	INSN_MACRO	},
{"bge",     "s,I,p",	0,    (int) M_BGE_I,	INSN_MACRO	},
{"bgeu",    "s,t,p",	0,    (int) M_BGEU,	INSN_MACRO	},
{"bgeu",    "s,I,p",	0,    (int) M_BGEU_I,	INSN_MACRO	},
{"bgez",    "s,p",	0x04010000, 0xfc0f0000,	BD|RD_s		},
{"bgezal",  "s,p",	0x04110000, 0xfc1f0000,	BD|RD_s		},
{"bgt",     "s,t,p",	0,    (int) M_BGT,	INSN_MACRO	},
{"bgt",     "s,I,p",	0,    (int) M_BGT_I,	INSN_MACRO	},
{"bgtu",    "s,t,p",	0,    (int) M_BGTU,	INSN_MACRO	},
{"bgtu",    "s,I,p",	0,    (int) M_BGTU_I,	INSN_MACRO	},
{"bgtz",    "s,p",	0x1c000000, 0xfc000000,	BD|RD_s		},
{"ble",     "s,t,p",	0,    (int) M_BLE,	INSN_MACRO	},
{"ble",     "s,I,p",	0,    (int) M_BLE_I,	INSN_MACRO	},
{"bleu",    "s,t,p",	0,    (int) M_BLEU,	INSN_MACRO	},
{"bleu",    "s,I,p",	0,    (int) M_BLEU_I,	INSN_MACRO	},
{"blez",    "s,p",	0x18000000, 0xfc000000,	BD|RD_s		},
{"blt",     "s,t,p",	0,    (int) M_BLT,	INSN_MACRO	},
{"blt",     "s,I,p",	0,    (int) M_BLT_I,	INSN_MACRO	},
{"bltu",    "s,t,p",	0,    (int) M_BLTU,	INSN_MACRO	},
{"bltu",    "s,I,p",	0,    (int) M_BLTU_I,	INSN_MACRO	},
{"bltz",    "s,p",	0x04000000, 0xfc0f0000,	BD|RD_s		},
{"bltzal",  "s,p",	0x04100000, 0xfc1f0000,	BD|RD_s		},
{"bne",     "s,t,p",	0x14000000, 0xfc000000,	BD|RD_s|RD_t	},
{"bne",     "s,I,p",	0,    (int) M_BNE_I,	INSN_MACRO	},
{"bnez",    "s,p",	0x14000000, 0xfc000000,	BD|RD_s|RD_t	},
{"break",   "",		0x0000000d, 0xffffffff,	INSN_TRAP	},
{"break",   "c",	0x0000000d, 0xffffffff,	INSN_TRAP	},
{"c.f.d",   "S,T",	0x46200030, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.f.s",   "S,T",	0x46000030, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.un.d",  "S,T",	0x46200031, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.un.s",  "S,T",	0x46000031, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.eq.d",  "S,T",	0x46200032, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.eq.s",  "S,T",	0x46000032, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ueq.d", "S,T",	0x46200033, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ueq.s", "S,T",	0x46000033, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.olt.d", "S,T",	0x46200034, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.olt.s", "S,T",	0x46000034, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ult.d", "S,T",	0x46200035, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ult.s", "S,T",	0x46000035, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ole.d", "S,T",	0x46200036, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ole.s", "S,T",	0x46000036, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ule.d", "S,T",	0x46200037, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ule.s", "S,T",	0x46000037, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.sf.d",  "S,T",	0x46200038, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.sf.s",  "S,T",	0x46000038, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ngle.d","S,T",	0x46200039, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ngle.s","S,T",	0x46000039, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.seq.d", "S,T",	0x4620003a, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.seq.s", "S,T",	0x4600003a, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ngl.d", "S,T",	0x4620003b, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ngl.s", "S,T",	0x4600003b, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.lt.d",  "S,T",	0x4620003c, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.lt.s",  "S,T",	0x4600003c, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.nge.d", "S,T",	0x4620003d, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.nge.s", "S,T",	0x4600003d, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.le.d",  "S,T",	0x4620003e, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.le.s",  "S,T",	0x4600003e, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ngt.d", "S,T",	0x4620003f, 0xfff003ff,	RD_S|RD_T|CC	},
{"c.ngt.s", "S,T",	0x4600003f, 0xfff003ff,	RD_S|RD_T|CC	},
#if 0
/* these are not very safe to use, no bounds checking. */
{"c0",      "I",	0x42000000, 0xfff00000,	0		},
{"c1",      "I",	0x46000000, 0xfff00000,	0		},
{"c2",      "I",	0x4a000000, 0xfff00000,	0		},
{"c3",      "I",	0x4e000000, 0xfff00000,	0		},
#endif
{"cfc0",    "t,d",	0x40400000, 0xfff00000,	COD|RD_d	},
{"cfc1",    "t,d",	0x44400000, 0xfff00000,	COD|RD_S	},
{"cfc1",    "t,S",	0x44400000, 0xfff00000,	COD|RD_S	},
{"cfc2",    "t,d",	0x48400000, 0xfff00000,	COD|RD_d	},
{"cfc3",    "t,d",	0x4c400000, 0xfff00000,	COD|RD_d	},
{"ctc0",    "t,d",	0x40c00000, 0xfff00000,	COD|RD_t|RD_d	},
{"ctc1",    "t,d",	0x44c00000, 0xfff00000,	COD|RD_t	},
{"ctc1",    "t,S",	0x44c00000, 0xfff00000,	COD|RD_t	},
{"ctc2",    "t,d",	0x48c00000, 0xfff00000,	COD|RD_t|RD_d	},
{"ctc3",    "t,d",	0x4cc00000, 0xfff00000,	COD|RD_t|RD_d	},
{"cvt.d.s", "D,S",	0x46000021, 0xfff0003f,	WR_D|RD_S	},
{"cvt.d.w", "D,S",	0x46800021, 0xfff0003f,	WR_D|RD_S	},
{"cvt.s.d", "D,S",	0x46200020, 0xfff0003f,	WR_D|RD_S	},
{"cvt.s.w", "D,S",	0x46800020, 0xfff0003f,	WR_D|RD_S	},
{"cvt.w.d", "D,S",	0x46200024, 0xfff0003f,	WR_D|RD_S	},
{"cvt.w.s", "D,S",	0x46000024, 0xfff0003f,	WR_D|RD_S	},
{"div",     "s,t",	0x0000001a, 0xfc00003f,	RD_s|RD_t|EXD	},
{"div",     "d,s,t",	0,    (int) M_DIV_3,	INSN_MACRO	},
{"div",     "d,v,I",	0,    (int) M_DIV_3I,	INSN_MACRO	},
{"div.d",   "D,V,T",	0x46200003, 0xfff0003f,	WR_D|RD_S|RD_T	},
{"div.s",   "D,V,T",	0x46000003, 0xfff0003f,	WR_D|RD_S|RD_T	},
{"divu",    "s,t",	0x0000001b, 0xfc00ffff,	RD_s|RD_t|EXD	},
{"divu",    "d,s,t",	0,    (int) M_DIVU_3,	INSN_MACRO	},
{"divu",    "d,v,I",	0,    (int) M_DIVU_3I,	INSN_MACRO	},
{"j",       "s",	0x00000008, 0xfc1fffff,	BD|RD_s		},
{"j",       "a",	0x08000000, 0xfc000000,	BD		},
{"jal",     "d,s",	0x00000009, 0xfc1f07ff,	BD|RD_s|WR_d	},/* jalr */
{"jal",     "s",	0x0000f809, 0xfc00ff3f,	BD|RD_s|WR_d	},/* jalr $ra */
{"jal",     "a",	0x0c000000, 0xfc000000,	BD|WR_31	},
{"jalr",    "s",	0x0000f809, 0xfc00f83f,	BD|RD_s|WR_31	},
{"jalr",    "d,s",	0x00000009, 0xfc1f07ff,	BD|RD_s|WR_d	},
{"jr",      "s",	0x00000008, 0xfc1fffff,	BD|RD_s		},
{"l.d",     "T,o(b)",	0,    (int) M_L_DOB,	INSN_MACRO	},
{"l.d",     "T,A(b)",	0,    (int) M_L_DAB,	INSN_MACRO	},
{"l.s",     "T,o(b)",	0xc4000000, 0xfc000000,	COD|RD_b	}, /* lwc1 */
{"l.s",     "T,A(b)",	0,    (int) M_LWC1_AB,	INSN_MACRO	},
{"la",      "t,A",	0,    (int) M_LA,	INSN_MACRO	},
{"la",      "t,A(b)",	0,    (int) M_LA_AB,	INSN_MACRO	},
{"lb",      "t,o(b)",	0x80000000, 0xfc000000,	LDD|RD_b	},
{"lb",      "t,A(b)",	0,    (int) M_LB_AB,	INSN_MACRO	},
{"lbu",     "t,o(b)",	0x90000000, 0xfc000000,	LDD|RD_b	},
{"lbu",     "t,A(b)",	0,    (int) M_LBU_AB,	INSN_MACRO	},
{"ld",      "t,o(b)",	0,    (int) M_LD_OB,	INSN_MACRO	},
{"ld",      "t,A(b)",	0,    (int) M_LD_AB,	INSN_MACRO	},
{"lh",      "t,o(b)",	0x84000000, 0xfc000000,	LDD|RD_b	},
{"lh",      "t,A(b)",	0,    (int) M_LH_AB,	INSN_MACRO	},
{"lhu",     "t,o(b)",	0x94000000, 0xfc000000,	LDD|RD_b	},
{"lhu",     "t,A(b)",	0,    (int) M_LHU_AB,	INSN_MACRO	},
{"li",      "t,I",	0,    (int) M_LI,	INSN_MACRO	},
{"li.d",    "t,F",	0,    (int) M_LI_D,	INSN_MACRO	},
{"li.d",    "S,F",	0,    (int) M_LI_DD,	INSN_MACRO	},
{"lui",     "t,u",	0x3c000000, 0xfff00000,	WR_t		},
{"lw",      "t,o(b)",	0x8c000000, 0xfc000000,	LDD|RD_b	},
{"lw",      "t,A(b)",	0,    (int) M_LW_AB,	INSN_MACRO	},
{"lwc0",    "t,o(b)",	0xc0000000, 0xfc000000,	COD|RD_b	},
{"lwc0",    "t,A(b)",	0,    (int) M_LWC0_AB,	INSN_MACRO	},
{"lwc1",    "T,o(b)",	0xc4000000, 0xfc000000,	COD|RD_b	},
{"lwc1",    "T,A(b)",	0,    (int) M_LWC1_AB,	INSN_MACRO	},
{"lwc2",    "t,o(b)",	0xc8000000, 0xfc000000,	COD|RD_b	},
{"lwc2",    "t,A(b)",	0,    (int) M_LWC2_AB,	INSN_MACRO	},
{"lwc3",    "t,o(b)",	0xcc000000, 0xfc000000,	COD|RD_b	},
{"lwc3",    "t,A(b)",	0,    (int) M_LWC3_AB,	INSN_MACRO	},
{"lwl",     "t,o(b)",	0x88000000, 0xfc000000,	LDD|RD_b	},
{"lwl",     "t,A(b)",	0,    (int) M_LWL_AB,	INSN_MACRO	},
{"lwr",     "t,o(b)",	0x98000000, 0xfc000000,	LDD|RD_b	},
{"lwr",     "t,A(b)",	0,    (int) M_LWR_AB,	INSN_MACRO	},
{"mfc0",    "t,d",	0x40000000, 0xfff007ff,	COD|WR_t|RD_C0	},
{"mfc1",    "t,S",	0x44000000, 0xfff007ff,	COD|RD_S	},
{"mfc1",    "t,d",	0x44000000, 0xfff007ff,	COD|RD_C1	},
{"mfc2",    "t,d",	0x48000000, 0xfff007ff,	COD|WR_t|RD_C2	},
{"mfc3",    "t,d",	0x4c000000, 0xfff007ff,	COD|WR_t|RD_C3	},
{"mfhi",    "d",	0x00000010, 0xffff07ff,	WR_d|EXD	},
{"mflo",    "d",	0x00000012, 0xffff07ff,	WR_d|EXD	},
{"mov.d",   "D,S",	0x46200006, 0xfff0003f,	WR_t|RD_S	},
{"mov.s",   "D,S",	0x46000006, 0xfff0003f,	WR_t|RD_S	},
{"move",    "d,s",	0x00000021, 0xfc1f07ff,	WR_d|RD_s	}, /* addu */
{"mtc0",    "t,d",	0x40800000, 0xfff007ff,	COD|RD_t|WR_C0	},
{"mtc1",    "t,d",	0x44800000, 0xfff007ff,	COD|RD_t|WR_C1	},
{"mtc1",    "t,S",	0x44800000, 0xfff007ff,	COD|RD_t	},
{"mtc2",    "t,d",	0x48800000, 0xfff007ff,	COD|RD_t|WR_C2	},
{"mtc3",    "t,d",	0x4c800000, 0xfff007ff,	COD|RD_t|WR_C3	},
{"mthi",    "s",	0x00000011, 0xfc1fffff,	RD_s|EXD	},
{"mtlo",    "s",	0x00000013, 0xfc1fffff,	RD_s|EXD	},
{"mul.d",   "D,V,T",	0x46200002, 0xfff0003f,	WR_D|RD_S|RD_T	},
{"mul.s",   "D,V,T",	0x46000002, 0xfff0003f,	WR_D|RD_S|RD_T	},
{"mul",     "d,v,t",	0,    (int) M_MUL,	INSN_MACRO	},
{"mul",     "d,v,I",	0,    (int) M_MUL_I,	INSN_MACRO	},
{"mulo",    "d,v,t",	0,    (int) M_MULO,	INSN_MACRO	},
{"mulo",    "d,v,I",	0,    (int) M_MULO_I,	INSN_MACRO	},
{"mulou",   "d,v,t",	0,    (int) M_MULOU,	INSN_MACRO	},
{"mulou",   "d,v,I",	0,    (int) M_MULOU_I,	INSN_MACRO	},
{"mult",    "s,t",	0x00000018, 0xfc00ffff,	RD_s|RD_t|EXD	},
{"multu",   "s,t",	0x00000019, 0xfc00ffff,	RD_s|RD_t|EXD	},
{"neg",     "d,w",	0x00000022, 0xfc0007ff,	WR_d|RD_s|RD_t	}, /* sub	0 */
{"negu",    "d,w",	0x00000023, 0xfc0007ff,	WR_d|RD_s|RD_t	}, /* subu	0 */
{"neg.d",   "D,V",	0x46200007, 0xfff0003f,	WR_D|RD_S	},
{"neg.s",   "D,V",	0x46000007, 0xfff0003f,	WR_D|RD_S	},
{"nop",     "",		0x00000000, 0xffffffff,	0		},
{"nor",     "d,v,t",	0x00000027, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"nor",     "d,v,I",	0,    (int) M_NOR_I,	INSN_MACRO	},
{"not",     "d,v",	0x00000027, 0xfc0007ff,	WR_d|RD_s|RD_t	}, /* nor d,s,zero */
{"or",      "d,v,t",	0x00000025, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"or",      "t,r,I",	0,    (int) M_OR_I,	INSN_MACRO	},
{"ori",     "t,r,i",	0x34000000, 0xfc000000,	WR_t|RD_s	},
{"rem",     "d,v,t",	0,    (int) M_REM_3,	INSN_MACRO	},
{"rem",     "d,v,I",	0,    (int) M_REM_3I,	INSN_MACRO	},
{"remu",    "d,v,t",	0,    (int) M_REMU_3,	INSN_MACRO	},
{"remu",    "d,v,I",	0,    (int) M_REMU_3I,	INSN_MACRO	},
{"rfe",     "",		0x42000010, 0xffffffff,	INSN_RFE	},
{"rol",     "d,v,t",	0,    (int) M_ROL,	INSN_MACRO	},
{"rol",     "d,v,I",	0,    (int) M_ROL_I,	INSN_MACRO	},
{"ror",     "d,v,t",	0,    (int) M_ROR,	INSN_MACRO	},
{"ror",     "d,v,I",	0,    (int) M_ROR_I,	INSN_MACRO	},
{"s.d",     "T,o(b)",	0,    (int) M_S_DOB,	INSN_MACRO	},
{"s.d",     "T,A(b)",	0,    (int) M_S_DAB,	INSN_MACRO	},
{"s.s",     "T,o(b)",	0xe4000000, 0xfc000000,	ST_T|RD_b	}, /* swc1 */
{"s.s",     "T,A(b)",	0,    (int) M_SWC1_AB,	INSN_MACRO	},
{"sb",      "t,o(b)",	0xa0000000, 0xfc000000,	ST_t|RD_b	},
{"sb",      "t,A(b)",	0,    (int) M_SB_AB,	INSN_MACRO	},
{"sd",      "t,o(b)",	0,    (int) M_SD_OB,	INSN_MACRO	},
{"sd",      "t,A(b)",	0,    (int) M_SD_AB,	INSN_MACRO	},
{"seq",     "d,v,t",	0,    (int) M_SEQ,	INSN_MACRO	},
{"seq",     "d,v,I",	0,    (int) M_SEQ_I,	INSN_MACRO	},
{"sge",     "d,v,t",	0,    (int) M_SGE,	INSN_MACRO	},
{"sge",     "d,v,I",	0,    (int) M_SGE_I,	INSN_MACRO	},
{"sgeu",    "d,v,t",	0,    (int) M_SGEU,	INSN_MACRO	},
{"sgeu",    "d,v,I",	0,    (int) M_SGEU_I,	INSN_MACRO	},
{"sgt",     "d,v,t",	0,    (int) M_SGT,	INSN_MACRO	},
{"sgt",     "d,v,I",	0,    (int) M_SGT_I,	INSN_MACRO	},
{"sgtu",    "d,v,t",	0,    (int) M_SGTU,	INSN_MACRO	},
{"sgtu",    "d,v,I",	0,    (int) M_SGTU_I,	INSN_MACRO	},
{"sh",      "t,o(b)",	0xa4000000, 0xfc000000,	ST_t|RD_b	},
{"sh",      "t,A(b)",	0,    (int) M_SH_AB,	INSN_MACRO	},
{"sle",     "d,v,t",	0,    (int) M_SLE,	INSN_MACRO	},
{"sle",     "d,v,I",	0,    (int) M_SLE_I,	INSN_MACRO	},
{"sleu",    "d,v,t",	0,    (int) M_SLEU,	INSN_MACRO	},
{"sleu",    "d,v,I",	0,    (int) M_SLEU_I,	INSN_MACRO	},
{"sll",     "d,w,s",	0x00000004, 0xfc0007ff,	WR_d|RD_t|RD_s	}, /* sllv */
{"sll",     "d,w,<",	0x00000000, 0xffe0003f,	WR_d|RD_t	},
{"sllv",    "d,t,s",	0x00000004, 0xfc0007ff,	WR_d|RD_t|RD_s	},
{"slt",     "d,v,t",	0x0000002a, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"slt",     "d,v,I",	0,    (int) M_SLT_I,	INSN_MACRO	},
{"slti",    "t,r,j",	0x28000000, 0xfc000000,	WR_t|RD_s	},
{"sltiu",   "t,r,j",	0x2c000000, 0xfc000000,	WR_t|RD_s	},
{"sltu",    "d,v,t",	0x0000002b, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"sltu",    "d,v,I",	0,    (int) M_SLTU_I,	INSN_MACRO	},
{"sne",     "d,v,t",	0,    (int) M_SNE,	INSN_MACRO	},
{"sne",     "d,v,I",	0,    (int) M_SNE_I,	INSN_MACRO	},
{"sra",     "d,w,s",	0x00000007, 0xfc0007ff,	WR_d|RD_t|RD_s	}, /* srav */
{"sra",     "d,w,<",	0x00000003, 0xffe0003f,	WR_t|RD_d	},
{"srav",    "d,t,s",	0x00000007, 0xfc0007ff,	WR_d|RD_t|RD_s	},
{"srl",     "d,w,s",	0x00000006, 0xfc0007ff,	WR_d|RD_t|RD_s	}, /* srlv */
{"srl",     "d,w,<",	0x00000002, 0xffe0003f,	WR_d|RD_t	},
{"srlv",    "d,t,s",	0x00000006, 0xfc0007ff,	WR_d|RD_t|RD_s	},
{"sub",     "d,v,t",	0x00000022, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"sub",     "d,v,I",	0,    (int) M_SUB_I,	INSN_MACRO	},
{"sub.d",   "D,V,T",	0x46200001, 0xfff0003f,	WR_D|RD_S|RD_T	},     
{"sub.s",   "D,V,T",	0x46000001, 0xfff0003f,	WR_D|RD_S|RD_T	},
{"subu",    "d,v,t",	0x00000023, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"subu",    "d,v,I",	0,    (int) M_SUBU_I,	INSN_MACRO	},
{"sw",      "t,o(b)",	0xac000000, 0xfc000000,	ST_t|RD_b	},
{"sw",      "t,A(b)",	0,    (int) M_SW_AB,	INSN_MACRO	},
{"swc0",    "t,o(b)",	0xe0000000, 0xfc000000,	ST_C0|RD_b	},
{"swc0",    "t,A(b)",	0,    (int) M_SWC0_AB,	INSN_MACRO	},
{"swc1",    "t,o(b)",	0xe4000000, 0xfc000000,	ST_C1|RD_b	},
{"swc1",    "T,o(b)",	0xe4000000, 0xfc000000,	ST_T|RD_b	},
{"swc1",    "T,A(b)",	0,    (int) M_SWC1_AB,	INSN_MACRO	},
{"swc2",    "t,o(b)",	0xe8000000, 0xfc000000,	ST_C2|RD_b	},
{"swc2",    "t,A(b)",	0,    (int) M_SWC2_AB,	INSN_MACRO	},
{"swc3",    "t,o(b)",	0xec000000, 0xfc000000,	ST_C3|RD_b	},
{"swc3",    "t,A(b)",	0,    (int) M_SWC3_AB,	INSN_MACRO	},
{"swl",     "t,o(b)",	0xa8000000, 0xfc000000,	ST_t|RD_b	},
{"swl",     "t,A(b)",	0,    (int) M_SWL_AB,	INSN_MACRO	},
{"swr",     "t,o(b)",	0xb8000000, 0xfc000000,	ST_t|RD_b	},
{"swr",     "t,A(b)",	0,    (int) M_SWR_AB,	INSN_MACRO	},
{"syscall", "",		0x0000000c, 0xffffffff,	INSN_TRAP	},
{"tlbp",    "",		0x42000008, 0xffffffff,	INSN_TLB	},
{"tlbr",    "",		0x42000001, 0xffffffff,	INSN_TLB	},
{"tlbwi",   "",		0x42000002, 0xffffffff,	INSN_TLB	},
{"tlbwr",   "",		0x42000006, 0xffffffff,	INSN_TLB	},
{"trunc.w.d", "D,S,t",	0,    (int) M_TRUNCWD,	INSN_MACRO	},
{"trunc.w.s", "D,S,t",	0,    (int) M_TRUNCWS,	INSN_MACRO	},
{"ulh",     "t,o(b)",	0,    (int) M_ULH,	INSN_MACRO	},
{"ulh",     "t,A",	0,    (int) M_ULH_A,	INSN_MACRO	},
{"ulhu",    "t,o(b)",	0,    (int) M_ULHU,	INSN_MACRO	},
{"ulhu",    "t,A",	0,    (int) M_ULHU_A,	INSN_MACRO	},
{"ulw",     "t,o(b)",	0,    (int) M_ULW,	INSN_MACRO	},
{"ulw",     "t,A",	0,    (int) M_ULW_A,	INSN_MACRO	},
{"ush",     "t,o(b)",	0,    (int) M_USH,	INSN_MACRO	},
{"ush",     "t,A",	0,    (int) M_USH_A,	INSN_MACRO	},
{"usw",     "t,o(b)",	0,    (int) M_USW,	INSN_MACRO	},
{"usw",     "t,A",	0,    (int) M_USW_A,	INSN_MACRO	},
{"xor",     "d,v,t",	0x00000026, 0xfc0007ff,	WR_d|RD_s|RD_t	},
{"xor",     "t,r,I",	0,    (int) M_XOR_I,	INSN_MACRO	},
{"xori",    "t,r,i",	0x38000000, 0xfc000000,	WR_t|RD_s	},
};

#define NUMOPCODES          (sizeof(mips_opcodes)/sizeof(*mips_opcodes))

#if 0
int
main(void)
{
    int i;

    for (i = 0; i < NUMOPCODES; ++i) {
	if ((mips_opcodes[i].mask & mips_opcodes[i].match) !=
	    mips_opcodes[i].match) {
		printf("error i = %d, opcode= %s\n", i, mips_opcodes[i].name);
	    }
    }
    return 0;
}
#endif
