/*      @(#)reg.h 1.1 86/07/07 SMI      */

/*
 * Copyright (c) 1986 by Sun Microsystems, Inc.
 */

/*
modification history
--------------------
01a,05jun90,llk  borrowed.
*/

#ifndef _REG_
#define _REG_

#ifdef I80960

/* Intel 960 register values passed over the wire by RPC:  */

struct regs
{
  int r_lreg[16];		/* local registers              */
  int r_greg[16];		/* global registers             */
  int r_pcw;			/* process control word         */
  int r_acw;			/* arithmetic control word      */
  int r_tcw;			/* trace control word           */
};

#define FP_REG_SIZE	12

struct fp_status {
	char	fps_regs[4][FP_REG_SIZE];	/* floating point regs */
};

#else  /* For now, just 68000 */

/*
 * Location of the users' stored
 * registers relative to R0.
 * Usage is u.u_ar0[XX].
 */
#define	R0	(0)
#define	R1	(1)
#define	R2	(2)
#define	R3	(3)
#define	R4	(4)
#define	R5	(5)
#define	R6	(6)
#define	R7	(7)
#define	AR0	(8)
#define	AR1	(9)
#define	AR2	(10)
#define	AR3	(11)
#define	AR4	(12)
#define	AR5	(13)
#define	AR6	(14)
#define	AR7	(15)
#define	SP	(15)
#define	PS	(16)
#define	PC	(17)

/*
 * And now for something completely the same...
 */
#ifndef LOCORE
struct regs {	
	int	r_dreg[8];	/* data registers */
#define r_r0	r_dreg[0]	/* r0 for portability */
	int	r_areg[8];	/* address registers */
#define r_sp	r_areg[7]	/* user stack pointer */
	int	r_sr;		/* status register (actually a short) */
#define	r_ps	r_sr
	int	r_pc;		/* program counter */
};

struct stkfmt {
	int	f_stkfmt : 4;	/* stack format */
	int		 : 2;
	int	f_vector : 10;	/* vector offset */
	short	f_beibase;	/* start of bus error info (if any) */
};


/*
 * Struct for floating point registers and general state
 * for the MC68881 (the sky fpp has no user visible state).
 * If fps_flags == FPS_UNUSED, the other 68881 fields have no meaning.
 * fps_code and fps_flags are software implemented fields.
 * fps_flags is not used when set by user level programs,
 * but changing fps_code has the side effect of changing u.u_code.
 */

typedef	struct ext_fp {
	int	fp[3];
} ext_fp;		/* extended 96-bit 68881 fp registers */

struct fp_status {
	ext_fp	fps_regs[8];		/* 68881 floating point regs */
	int	fps_control;		/* 68881 control reg */
	int	fps_status;		/* 68881 status reg */
	int	fps_iaddr;		/* 68881 instruction address reg */
	int	fps_code;		/* additional word for signals */
	int	fps_flags;		/* r/o - unused, idle or busy */
};
#endif !LOCORE

/*
 * Values defined for `fps_flags'.
 */
#define	FPS_UNUSED	0		/* 68881 never used yet */
#define	FPS_IDLE	1		/* 68881 instruction completed */
#define	FPS_BUSY	2		/* 68881 instruction interrupted */

/*
 * The EXT_FPS_FLAGS() macro is used to convert a pointer to an
 * fp_istate into a value to be used for the user visible state
 * found in fps_flags.  As a speed optimization, this convertion
 * is only done is required (e.g.  the PTRACE_GETFPREGS ptrace
 * call or when dumping core) instead of on each context switch.
 * The tests that we base the state on are that a fpis_vers of
 * FPIS_VERSNULL means NULL state, else a fpis_bufsiz of FPIS_IDLESZ
 * means IDLE state, else we assume BUSY state.
 */
#define	FPIS_VERSNULL	0x0
#define	FPIS_IDLESIZE	0x18

#define EXT_FPS_FLAGS(istatep) \
	((istatep)->fpis_vers == FPIS_VERSNULL ? FPS_UNUSED : \
	    (istatep)->fpis_bufsiz == FPIS_IDLESIZE ? FPS_IDLE : FPS_BUSY)

#ifndef LOCORE
/*
 * Struct for the internal state of the MC68881
 * Although the MC68881 can have a smaller maximum for
 * internal state, we allow for more to allow for expansion.
 */
#define	FPIS_BUFSIZ	0xc0

struct fp_istate {
	unsigned char	fpis_vers;		/* version number */
	unsigned char	fpis_bufsiz;		/* size of info in fpis_buf */
	unsigned short	fpis_reserved;		/* reserved word */
	unsigned char	fpis_buf[FPIS_BUFSIZ];	/* fpp internal state buffer */
};

/* 
 * Structures for the status and data registers are defined here.
 * Struct fpa_status are included in the u area.
 * Struct fpa_regs is included in struct core.
 */

/* struct fpa_status is saved/restored during context switch */
struct fpa_status {
	unsigned int	fpas_state;	/* STATE, supervisor privileged reg */
	unsigned int	fpas_imask;	/* IMASK */
	unsigned int	fpas_load_ptr;	/* LOAD_PTR */
	unsigned int	fpas_ierr;	/* IERR */
	unsigned int	fpas_act_instr; /* pipe active instruction halves */
	unsigned int	fpas_nxt_instr; /* pipe next instruction halves */
	unsigned int	fpas_act_d1half;/* pipe active data first half */
	unsigned int	fpas_act_d2half;/* pipe active data second half */
	unsigned int	fpas_nxt_d1half;/* pipe next data first half */
	unsigned int	fpas_nxt_d2half;/* pipe next data second half */
	unsigned int	fpas_mode3_0;	/* FPA MODE3_0 register */
	unsigned int	fpas_wstatus;	/* FPA WSTATUS register */
};

/* 
 * Since there are 32 contexts supported by the FPA hardware,
 * when we do context switch on the FPA, we don't save/restore
 * the data registers between the FPA and the u area.
 * If there are already 32 processes using the fpa concurrently,
 * we give an error message to the 33rd process trying to use the fpa.
 * (Hopefully there will not be this many processes using FPA concurrently.)
 */

#define FPA_NCONTEXTS		32
#define FPA_NDATA_REGS		32

typedef struct fpa_long {
	int     fpl_data[2];
} fpa_long;		 /* 64 bit double precision registers */

/* Struct fpa_regs is included in struct core. */
struct fpa_regs {
	unsigned int	fpar_flags; /* if zero, other fields are meaningless */
        struct fpa_status	fpar_status;
        fpa_long	fpar_data[FPA_NDATA_REGS];
};

/*
 * The size of struct fpa_regs is changed from 141 ints in 3.0 to
 * 77 ints in 3.x.  A pad of this size difference is added to struct core.
 */
#define CORE_PADLEN     64

/*
 * If there is going to be external FPU state then we must define the FPU
 * variable
 */
struct fpu {
        struct  fp_status f_fpstatus;   /* External FPP state, if any */
        struct  fpa_regs f_fparegs;     /* FPA registers, if any */
        int     f_pad[CORE_PADLEN];     /* see comment above */
};

#endif !LOCORE
#endif /* !I80960 */
#endif !_REG_
