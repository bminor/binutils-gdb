/* run front end support for arm
   Copyright (C) 1995 Free Software Foundation, Inc.

This file is part of ARM SIM

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111, USA.  */

#include <stdio.h>
#include <stdarg.h>
#include <armdefs.h>
#include <bfd.h>
#include <signal.h>
#include "callback.h"
#include "remote-sim.h"
static struct ARMul_State *state;

static void 
init ()
{
  static int done;
  if (!done)
    {
      ARMul_EmulateInit();
      state = ARMul_NewState ();
      ARMul_MemoryInit(state, 1<<21);
      ARMul_OSInit(state);
      ARMul_CoProInit(state); 
      done = 1;
    }

}
void 
ARMul_ConsolePrint (ARMul_State * state, const char *format,...)
{
  va_list ap;
  va_start (ap, format);
  vprintf (format, ap);
  va_end (ap);
}

ARMword 
ARMul_Debug (ARMul_State * state, ARMword pc, ARMword instr)
{

}

void 
sim_size (size)
     int size;
{
  init ();
  ARMul_MemoryInit (state, 1 << size);
}


void 
sim_set_profile ()
{
}
void 
sim_set_profile_size ()
{
}

int
sim_write (addr, buffer, size)
     SIM_ADDR addr;
     unsigned char *buffer;
     int size;
{
  int i;
  init ();
  for (i = 0; i < size; i++)
    {
      ARMul_WriteByte (state, addr+i, buffer[i]);
    }
  return size;
}

int
sim_read (addr, buffer, size)
     SIM_ADDR addr;
     unsigned char *buffer;
     int size;
{
  int i;
  init ();
  for (i = 0; i < size; i++)
    {
      buffer[i] = ARMul_ReadByte (state, addr + i);
    }
  return size;
}

void 
sim_trace ()
{
}

static int rc;
void
sim_resume (step, siggnal)
     int step, siggnal;
{
  if (step)
    {
      rc = SIGTRAP;
      state->Reg[15] =       ARMul_DoInstr (state);
    }
  else
    {
      state->Reg[15] =       ARMul_DoProg (state);
    }
}

void
sim_create_inferior (start_address, argv, env)
     SIM_ADDR start_address;
     char **argv;
     char **env;
{
  ARMul_SetPC(state, start_address);
}

void
sim_info (verbose)
     int verbose;
{
}


int 
frommem (state, memory)
     struct ARMul_State *state;
     unsigned char *memory;
{
  if (state->bigendSig == HIGH)
    {
      return (memory[0] << 24)
	| (memory[1] << 16)
	| (memory[2] << 8)
	| (memory[3] << 0);
    }
  else
    {
      return (memory[3] << 24)
	| (memory[2] << 16)
	| (memory[1] << 8)
	| (memory[0] << 0);
    }
}


void 
tomem (state, memory,  val)
     struct ARMul_State *state;
     unsigned char *memory;
     int val;
{
  if (state->bigendSig == HIGH)
    {
      memory[0] = val >> 24;
      memory[1] = val >> 16;
      memory[2] = val >> 8;
      memory[3] = val >> 0;
    }
  else
    {
      memory[3] = val >> 24;
      memory[2] = val >> 16;
      memory[1] = val >> 8;
      memory[0] = val >> 0;
    }
}

void
sim_store_register (rn, memory)
     int rn;
     unsigned char *memory;
{
  init ();
  ARMul_SetReg(state, state->Mode, rn, frommem (state, memory));
}

void
sim_fetch_register (rn, memory)
     int rn;
     unsigned char *memory;
{
  init ();
  tomem (state, memory, ARMul_GetReg(state, state->Mode, rn));
}




void
sim_open (name)
     char *name;
{
  /* nothing to do */
}

void
sim_close (quitting)
     int quitting;
{
  /* nothing to do */
}

int
sim_load (prog, from_tty)
     char *prog;
     int from_tty;
{
  /* Return nonzero so GDB will handle it.  */
  return 1;
}

void
sim_stop_reason (reason, sigrc)
     enum sim_stop *reason;
     int *sigrc;
{
  *reason = sim_stopped;
  *sigrc = rc;
}
void
sim_kill ()
{
  /* nothing to do */
}

void
sim_do_command (cmd)
     char *cmd;
{
  printf_filtered ("This simulator does not accept any commands.\n");
}


void
sim_set_callbacks (ptr)
struct host_callback_struct *ptr;
{

}
