/*  Copyright (C) 1998, Cygnus Solutions

    */

#ifndef VU0_H_
#define VU0_H_

#include "sim-main.h"

void vu0_attach(SIM_DESC sd);

#define VU0_MEM0_WINDOW_START 	0x11000000
#define VU0_MEM0_SIZE  		0x1000	/* 4K = 4096 */

#define VU0_MEM1_WINDOW_START 	0x11004000
#define VU0_MEM1_SIZE  		0x1000	/* 4K = 4096 */

#define VU0_REGISTER_WINDOW_START 0x10000c00

#define VU0_VF00		0x10000c00
/* ... */
#define VU0_VF31		0x10000df0

#define VU0_VI00		0x10000e00
/* ... */
#define VU0_VI15		0x10000ef0

/* ... */

#define VPE0_STAT		0x10000fd0

#define VU0_REGISTER_WINDOW_END   0x10000fe0

#define VU0_REGISTER_WINDOW_SIZE (VU0_REGISTER_WINDOW_END - VU0_REGISTER_WINDOW_START)

#endif
