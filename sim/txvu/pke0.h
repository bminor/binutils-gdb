/*  Copyright (C) 1998, Cygnus Solutions

    */

#ifndef PKE0_H_
#define PKE0_H_

#include "sim-main.h"

void pke0_attach(SIM_DESC sd);

#define PKE0_REGISTER_WINDOW_START 0x10000800
#define PKE0_REGISTER_WINDOW_END   0x10000980

#define PKE0_REGISTER_WINDOW_SIZE (PKE0_REGISTER_WINDOW_END - PKE0_REGISTER_WINDOW_START)

#endif
