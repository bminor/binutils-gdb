/*  Copyright (C) 1998, Cygnus Solutions

    */

#ifndef VU1_H_
#define VU1_H_

#include "sim-main.h"

void vu1_attach(SIM_DESC sd);
void vu1_issue(void);
void vu1_init(SIM_DESC sd);

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

#define VU1_MST			0x11007300
#define VU1_MMC			0x11007310
#define VU1_MCP			0x11007320
#define VU1_MR			0x11007330
#define VU1_MI			0x11007340
#define VU1_MQ			0x11007350
#define VU1_MP			0x11007360
#define VU1_MTPC		0x110073a0
#define VPE1_STAT		0x110073d0

#define VU1_REGISTER_WINDOW_END   0x110073e0

#define VU1_REGISTER_WINDOW_SIZE (VU1_REGISTER_WINDOW_END - VU1_REGISTER_WINDOW_START)

#endif
