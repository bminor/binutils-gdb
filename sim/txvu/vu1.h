/*  Copyright (C) 1998, Cygnus Solutions

    */

#ifndef VU1_H_
#define VU1_H_

#include "sim-main.h"

void vu1_attach(SIM_DESC sd);
void vu1_issue();

#define VU1_MEM0_WINDOW_START 	0x11008000
#define VU1_MEM0_SIZE		0x4000    /* 16K = 16384 */

#define VU1_MEM1_WINDOW_START 	0x1100c000
#define VU1_MEM1_SIZE  		0x4000    /* 16K = 16384 */

#define VU1_REGISTER_WINDOW_START 0x11007000

#define VU1_VF00		0x11007000
/* ... */
#define VU1_VF31		0x110071f0

#define VU1_VI00		0x11007200
/* ... */
#define VU1_VI15		0x110072f0

/* ... */

#define VPE1_STAT		0x11007370

#define VU1_REGISTER_WINDOW_END   0x11007380

#define VU1_REGISTER_WINDOW_SIZE (VU1_REGISTER_WINDOW_END - VU1_REGISTER_WINDOW_START)

#endif
