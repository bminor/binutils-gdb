/* Copyright (C) 2007 Free Software Foundation, Inc.

   This file is part of GDB.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "server.h"
#include "win32-low.h"

/* Fetch register(s) from gdbserver regcache data.  */
static void
do_fetch_inferior_registers (win32_thread_info *th, int r)
{
  char *context_offset = regptr (&th->context, r);
  supply_register (r, context_offset);
}

#define context_offset(x) ((int)&(((CONTEXT *)NULL)->x))
static const int mappings[] = {
  context_offset (R0),
  context_offset (R1),
  context_offset (R2),
  context_offset (R3),
  context_offset (R4),
  context_offset (R5),
  context_offset (R6),
  context_offset (R7),
  context_offset (R8),
  context_offset (R9),
  context_offset (R10),
  context_offset (R11),
  context_offset (R12),
  context_offset (Sp),
  context_offset (Lr),
  context_offset (Pc),
  -1, /* f0 */
  -1, /* f1 */
  -1, /* f2 */
  -1, /* f3 */
  -1, /* f4 */
  -1, /* f5 */
  -1, /* f6 */
  -1, /* f7 */
  -1, /* fps */
  context_offset (Psr),
};
#undef context_offset

static const unsigned char arm_wince_le_breakpoint[] =
  { 0x10, 0x00, 0x00, 0xe6 };

struct win32_target_ops the_low_target = {
  mappings,
  sizeof (mappings) / sizeof (mappings[0]),
  NULL, /* initial_stuff */
  NULL, /* store_debug_registers */
  NULL, /* load_debug_registers */
  do_fetch_inferior_registers,
  NULL, /* single_step */
  arm_wince_le_breakpoint,
  sizeof (arm_wince_le_breakpoint) / sizeof (arm_wince_le_breakpoint[0]),
  "arm" /* arch_string */
};
