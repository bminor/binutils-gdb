/*  Copyright (C) 1998, Cygnus Solutions

    */

#ifndef GPUIF_H_
#define GPUIF_H_

#include "sim-main.h"

void gpuif_attach(SIM_DESC sd);

#define GPUIF_REGISTER_WINDOW_START 0x10000700

#define GPUIF_CTRL_ADDR 0x10000700

#define GPUIF_REGISTER_WINDOW_END   0x10000790
#define GPUIF_REGISTER_WINDOW_SIZE (GPUIF_REGISTER_WINDOW_END - GPUIF_REGISTER_WINDOW_START)

#endif
