/*  This file is part of the program psim.

    Copyright (C) 1994-1997, Andrew Cagney <cagney@highland.com.au>
    Copyright (C) 1997, Free Software Foundation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


#ifndef _SIM_MAIN_H_
#define _SIM_MAIN_H_


#include "sim-basics.h"
#include "sim-signal.h"

#include <signal.h> /* For kill() in insns:do_trap */

#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/* These are generated files.  */
#include "itable.h"
#include "idecode.h"
#include "idecode.h"

typedef instruction_address sim_cia;
static const sim_cia null_cia = {0}; /* Dummy */
#define NULL_CIA null_cia
/* FIXME: Perhaps igen should generate access macros for
   `instruction_address' that we could use.  */
#define CIA_ADDR(cia) ((cia).ip)

#define WITH_WATCHPOINTS 1

#include "sim-base.h"

#include "alu.h"
#include "cpu.h"


struct sim_state {

  /* the processors proper */
  sim_cpu cpu;
#define STATE_CPU(sd, n) (&(sd)->cpu)

  /* The base class.  */
  sim_state_base base;

};

/* (re) initialize the simulator */

extern void engine_init
(SIM_DESC sd);


#ifndef HAVE_GETPID
#define getpid() 42
#endif

#ifndef HAVE_KILL
#define kill(sig, pid) (errno = EINVAL, -1)
#endif
#endif
