/****************************************************************************/
/*                                                                          */
/*             Sony Computer Entertainment CONFIDENTIAL                     */
/*      (C) 1997 Sony Computer Entertainment Inc. All Rights Reserved       */
/*                                                                          */
/*       VU simulator global definitions                                    */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <strings.h>
#include <math.h>

typedef struct {
	int	no; /* destination register number VF:0-31 VI:32-47*/
	int mask; /* specify which units calculate */
	float vf[4]; /* calculated value */
	int flag; /*0: empty in this pipeline stage,
				1: write value to reg,
				2: store value from reg,
				3: write value to I-reg ( not used ),
				4: write only status to statusflag,
				5: write only clip value to clipflag,
				6: move value from EFU reg */
	u_long status; /* calculation unit status */
	u_long addr; /* store address ( store operation )*/
	char code[32]; /* instruction */
}	PIPELINE; /* pipeline stage specification for FMAC, Ld/St, RANDU, FDIV */

typedef struct {
	int no; /* destination register number VI:0-15 */
	short vi; /* calculated value */
	int flag; /* 0: empty in this pipeline stage,
				 1: write value to reg */
	char code[32]; /* instruction */
}	IPIPELINE; /* pipeline stage specification for IALU */

typedef struct {
	int flag; /* 0: empty in this pipeline stage,
				 1: write value to I-reg */
	float val; /* calculated value */
}	LOIPIPELINE; /* pipeline stage specification for I bit */

typedef struct {
	int mask; /* specify which units calculate */
	float acc[4]; /* calculated value */
	int flag; /* 0: empty in this pipeline stage,
				 1: write value to ACC */
} APIPELINE; /* accumulator pipeline stage specification */

typedef struct {
	int no; /* left stage count of pipeline */
	float vn; /* calculated value */
	int flag; /* 0: move to VN[4]
				 1: move to VN[0] (not used)*/
} SUPIPELINE;

u_long instbuf[2]; /* instruction buffer. instbuf[0]:Upper, instbuf[1]:Lower*/
u_long pc, opc; /* pc is next PC, opc is executed PC. see fetch_inst() */
u_long jaddr; /* branch address */

float VF[32][4]; /* VF registers */
	/* VF[?][0] = x, VF[?][1] = y, VF[?][2] = z, VF[?][3] = w */
short VI[16]; /* VI registers */
float ACC[4]; /* accumulator registers */
float Q; /* FDIV register */
float I; /* I register */
u_long R; /* RANDU register */
float VN[32]; /* EFU registers */
u_long MACflag;
u_long statusflag;
u_long clipflag;

u_long MEM[4096][4]; /* VU (data) memory */
u_long uMEM[1024][2]; /* Micro (instruction) memory */

PIPELINE	pipe[4][2]; /* FMAC, Ld/St, RANDU piepline */
PIPELINE	qpipe[7]; /* FDIV(DIV,DQRT) pipeline */
PIPELINE	rqpipe[13]; /*FDIV(RSQRT) pipeline */
IPIPELINE	ipipe[2]; /* IALU pipeline */
APIPELINE	apipe; /* ACC pipeline stage */
SUPIPELINE	spipe; /* EFU pipeline stage */
LOIPIPELINE Ipipe; /* I-bit pipeline stage */

int	eflag, jflag, peflag, sflag;
	/*	eflag: E-bit detect flag
				0: not detect, 1: detect, -1: fetch stage terminate
		jflag: branch detect flag
				0: not detect, 1: detect
		peflag: end delay slot execute flag
				0: not execute, 1: execute, -1: finished ececution
		sflag: data hazard stall flag
				0: not stall, 1: stall */
u_long bp; /* break point address */
u_long ecount; /* end counter */
int intr_mode; /* interactive mode enable */
int verb_mode; /* verbose mode enable */
int	dpr_mode; /* PRT (debug print) instruction enable */
u_long all_count; /* amount of executed cycles */
u_long hc_count; /* amount of hazard stall cycles */

extern int _GIF_SIM_OFF; /* internal GPUIF simulator disable */
extern int _GIF_BUSY; /* external GPUIF simulator is BUSY flag,
							this flag uses when _GIF_SIM_OFF = 1 */
extern int _GIF_VUCALL; /* XGKICK instruction detect flag,
							this flag uses when _GIF_SIM_OFF = 1 */
extern int _GIF_VUADDR; /* appointed address by XGKICK instruction,
							this flag uses when _GIF_SIM_OFF = 1 */

#define DEST_X	0x8
#define DEST_Y	0x4
#define DEST_Z	0x2
#define DEST_W	0x1
