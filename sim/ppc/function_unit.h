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


#ifndef _FUNCTION_UNIT_H_
#define _FUNCTION_UNIT_H_

#include "itable.h"

#ifndef INLINE_FUNCTION_UNIT
#define INLINE_FUNCTION_UNIT
#endif

/* basic types */

typedef struct _function_unit function_unit;

typedef struct _function_unit_print function_unit_print;
struct _function_unit_print {
  function_unit_print *next;
  const char *name;
  const char *suffix_singular;
  const char *suffix_plural;
  unsigned count;
};

/* constructor */

INLINE_FUNCTION_UNIT function_unit *function_unit_create
(void);

INLINE_FUNCTION_UNIT void function_unit_init
(function_unit *func_unit);

INLINE_FUNCTION_UNIT void function_unit_halt
(cpu *processor,
 function_unit *func_unit);

INLINE_FUNCTION_UNIT function_unit_print *function_unit_mon_info
(function_unit *func_unit);

INLINE_FUNCTION_UNIT void function_unit_mon_free
(function_unit *func_unit,
 function_unit_print *ptr);

INLINE_FUNCTION_UNIT void function_unit_issue
(itable_index index,
 function_unit *func_unit,
 unsigned_word cia);

INLINE_FUNCTION_UNIT void function_unit_model
(const char *model);

#endif /* _FUNCTION_UNIT_H_ */
