/*  Copyright (C) 1998, Cygnus Solutions

    */

#ifndef PKE1_H_
#define PKE1_H_

#include "sim-main.h"

void pke1_attach(SIM_DESC sd);

#define PKE1_REGISTER_WINDOW_START 0x10000a00
#define PKE1_REGISTER_WINDOW_END   0x10000b80

#define PKE1_REGISTER_WINDOW_SIZE (PKE1_REGISTER_WINDOW_END - PKE1_REGISTER_WINDOW_START)

#endif
