/*  Copyright (C) 1998, Cygnus Solutions

    */

#ifndef VU_H_
#define VU_H_

#include "sim-main.h"
#include <sys/types.h>

typedef u_long MEM_Entry_Type[4];
typedef u_long uMEM_Entry_Type[2];

typedef enum { VU_READY, VU_RUN, VU_BREAK } RunState;

/* See VU Specifications (Ver. 2.10), p7-11 */
typedef struct {
    u_long vbs : 1;		/* busy		0: Idle, 	  1: Busy  */
    u_long vds : 1;		/* D bit stop 	0: No D bit stop, 1: D bit stop */
    u_long vts : 1;		/* T bit stop 	0: No T bit stop, 1: T bit stop*/
    u_long vfs : 1;		/* ForceBreak stop 0: no ForceBreak stop, 1: ForceBreak stop*/
    u_long vgw : 1;		/* XGKICK wait 	0: Do not wait, 1: Wait */
    u_long div : 1;		/* DIV busy 	0: Idle, 1: Busy*/
    u_long efu : 1;		/* EF busy	0: Idle, 1: Busy */
    u_long reserved_2:25;
} VpeStat;

typedef struct {
	float VF[32][4];
	short VI[16];
	u_long MST;
	u_long MMC;
	u_long MCP;
	u_long MR;
	u_long MI;
	u_long MQ;
	u_long MP;
	u_long MTPC;
	VpeStat VPE_STAT;
} VectorUnitRegs;

typedef struct {
	MEM_Entry_Type *MEM; /* VU (data) memory */
	uMEM_Entry_Type *uMEM; /* Micro (instruction) memory */
	VectorUnitRegs regs;
	RunState runState;
} VectorUnitState;

#endif
