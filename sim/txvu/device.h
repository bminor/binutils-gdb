/*  Copyright (C) 1998, Cygnus Solutions

    */

#ifndef DEVICE_H_
#define DEVICE_H_

#include "sim-main.h"

typedef int io_read_buffer_callback_type (device *, void *, int,
                        address_word, unsigned ,
                        sim_cpu *, sim_cia);

typedef int io_write_buffer_callback_type (device *, const void *, int,
                        address_word, unsigned ,
                        sim_cpu *, sim_cia);

struct _device {
    char *name;
    io_read_buffer_callback_type *io_read_buffer_callback;
    io_write_buffer_callback_type *io_write_buffer_callback;
};

#endif
