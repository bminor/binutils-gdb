/*  Copyright (C) 1998, Cygnus Solutions

    */

#ifndef VU1_H_
#define VU1_H_

#include "sim-main.h"

void vu1_issue(void);
void vu1_init(SIM_DESC sd);
int vu1_status(void);
int vu1_busy(void);

#define VU1_MEM0_WINDOW_START 	0x11008000
#define VU1_MEM0_SIZE		0x4000    /* 16K = 16384 */

#define VU1_MEM1_WINDOW_START 	0x1100c000
#define VU1_MEM1_SIZE  		0x4000    /* 16K = 16384 */

#define VU1_REGISTER_WINDOW_START 0x11007000

/* FIX ME: These should be derived from enum in sky-vu.h */
#define VPU_STAT		0x110073d0
#define VU1_CIA			0x110073e0

#endif
