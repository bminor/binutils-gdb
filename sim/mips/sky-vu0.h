/*  Copyright (C) 1998, Cygnus Solutions

    */

#ifndef VU0_H_
#define VU0_H_

#include "sim-main.h"

void vu0_attach(SIM_DESC sd);
void vu0_issue(void);

#define VU0_MEM0_WINDOW_START 	0x11000000
#define VU0_MEM0_SIZE  		0x1000	/* 4K = 4096 */

#define VU0_MEM1_WINDOW_START 	0x11004000
#define VU0_MEM1_SIZE  		0x1000	/* 4K = 4096 */

#define VU0_REGISTER_WINDOW_START 0x10000c00

#define VPE0_STAT		0x10000fd0
#define VU0_CIA			0x10000fe0

#endif
