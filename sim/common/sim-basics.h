/*  This file is part of the program psim.

    Copyright (C) 1994-1997, Andrew Cagney <cagney@highland.com.au>
    Copyright (C) 1997, Free Software Foundation, Inc.

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


#ifndef _SIM_BASICS_H_
#define _SIM_BASICS_H_


/* Basic configuration */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* Basic host dependant mess - hopefully <stdio.h> + <stdarg.h> will
   bring potential conflicts out in the open */

#include <stdarg.h>
#include <stdio.h>
#include <setjmp.h>

#ifndef NULL
#define NULL 0
#endif



	
/* Some versions of GCC include an attribute operator, define it */

#if !defined (__attribute__)
#if (!defined(__GNUC__) \
     || (__GNUC__ < 2) \
     || (__GNUC__ == 2 && __GNUC_MINOR__ < 6))
#define __attribute__(arg)
#endif
#endif



/* Memory management with an allocator that clears memory before use. */

void *zalloc (unsigned long size);

#define ZALLOC(TYPE) (TYPE*)zalloc(sizeof (TYPE))

void zfree(void*);

/* Turn VALUE into a string with commas.  */
char *sim_add_commas (char *, int, unsigned long);


/* Utilities for elapsed time reporting.  */
/* Opaque type, known only inside sim_elapsed_time_foo fns.  */
typedef unsigned long SIM_ELAPSED_TIME;
/* Get reference point for future call to sim_time_elapsed.  */
SIM_ELAPSED_TIME sim_elapsed_time_get (void);
/* Elapsed time in milliseconds since START.  */
unsigned long sim_elapsed_time_since (SIM_ELAPSED_TIME start);


/* Global types that code manipulates */

typedef struct _device device;


/* Address access attributes */
typedef enum _access_type {
  access_invalid = 0,
  access_read = 1,
  access_write = 2,
  access_read_write = 3,
  access_exec = 4,
  access_read_exec = 5,
  access_write_exec = 6,
  access_read_write_exec = 7,
} access_type;


/* Address attachement types */
typedef enum _attach_type {
  attach_invalid,
  attach_raw_memory,
  attach_callback,
  /* ... */
} attach_type;



/* Basic definitions - ordered so that nothing calls what comes after
   it */

#ifdef HAVE_CONFIG_H
#include "tconfig.h"
#endif
#include "ansidecl.h"
#include "callback.h"
#include "remote-sim.h"

#include "sim-config.h"

#include "sim-module.h"
#include "sim-trace.h"
#include "sim-profile.h"
#include "sim-model.h"
#include "sim-base.h"

#include "sim-inline.h"

#include "sim-types.h"
#include "sim-bits.h"
#include "sim-endian.h"

#endif /* _SIM_BASICS_H_ */
