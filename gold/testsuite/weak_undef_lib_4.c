/* weak_undef_lib_4.c -- test non-default weak undefined symbol in DSO.

   Copyright (C) 2024 Free Software Foundation, Inc.

   This file is part of gold.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* Non-default weak undefined symbol in DSO should be resolved to 0 at
   runtime.  */

#include <stdlib.h>

extern void undefined (void) __attribute__((visibility("hidden"))) __attribute__((weak));
extern void protected (void) __attribute__((visibility("protected")))  __attribute__((weak));

extern void foo (void);

void
foo (void)
{
  if (&undefined != NULL)
    abort ();

  if (&protected != NULL)
    abort ();
}
