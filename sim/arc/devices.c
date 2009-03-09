/* arc device support
   Copyright (C) 1997, 1998 Free Software Foundation, Inc.

This file is part of GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "sim-main.h"

int
device_io_read_buffer (device *me, void *source, int space,
                       address_word addr, unsigned nr_bytes,
                       SIM_DESC sd, SIM_CPU *cpu, sim_cia cia)
{
  if (STATE_ENVIRONMENT (sd) != OPERATING_ENVIRONMENT)
    return nr_bytes;
  abort ();
}

int
device_io_write_buffer (device *me, const void *source, int space,
                        address_word addr, unsigned nr_bytes,
                        SIM_DESC sd, SIM_CPU *cpu, sim_cia cia)
{
  if (STATE_ENVIRONMENT (sd) != OPERATING_ENVIRONMENT)
    return nr_bytes;
  abort ();
}

void
device_error (device *me, char *message, ...)
{
}
