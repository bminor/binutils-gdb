/*  This file is part of the program psim.

    Copyright (C) 1994-1995, Andrew Cagney <cagney@highland.com.au>

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


#ifndef _BASICS_H_
#define _BASICS_H_

#include "config.h"
#include "words.h"
#include "ppc-endian.h"
#include "debug.h"
#include "bits.h"
#include "sim_callbacks.h"

#ifndef NULL
#define NULL 0
#endif

/* many things pass around the cpu type without knowing what it is */

typedef struct _cpu cpu;

/* many things are moving data between the host and target */

typedef enum {
  cooked_transfer,
  raw_transfer,
} transfer_mode;

/* possible exit statuses */

typedef enum {
  was_continuing, was_trap, was_exited, was_signalled
} stop_reason;

#endif /* _BASICS_H_ */
