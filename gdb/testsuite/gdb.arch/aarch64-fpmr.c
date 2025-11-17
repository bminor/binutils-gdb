/* This file is part of GDB, the GNU debugger.

   Copyright 2008-2025 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <stdint.h>

enum FPM_FORMAT
{
  E5M2,
  E4M3,
};

enum FPM_OVERFLOW
{
  INFNAN,
  SATURATE,
};

void
set_fpmr (uint64_t value)
{
  register uint64_t x0_val asm ("x0") = value;
  __asm__ volatile (".inst	0xd51b4440" : : );
  /* msr	fpmr, x0 */
}

uint64_t
modify_src1_fmt (uint64_t fpmr, uint64_t fmt)
{
  return (fpmr & ~(0x7)) | (fmt & 0x7);
}

uint64_t
modify_src2_fmt (uint64_t fpmr, uint64_t fmt)
{
  return (fpmr & ~((0x7) << 3)) | ((fmt & 0x7) << 3);
}

uint64_t
modify_dst_fmt (uint64_t fpmr, uint64_t fmt)
{
  return (fpmr & ~((0x7) << 6)) | ((fmt & 0x7) << 6);
}

uint64_t
modify_osm (uint64_t fpmr, uint64_t overflow)
{
  return (fpmr & ~((0x1) << 14)) | ((overflow & 0x1) << 14);
}

uint64_t
modify_osc (uint64_t fpmr, uint64_t overflow)
{
  return (fpmr & ~((0x1) << 15)) | ((overflow & 0x1) << 15);
}

uint64_t
modify_lscale (uint64_t fpmr, uint64_t scale)
{
  return (fpmr & ~((0x7f) << 16)) | ((scale & 0x7f) << 16);
}

uint64_t
modify_nscale (uint64_t fpmr, uint64_t scale)
{
  return (fpmr & ~((0xff) << 24)) | ((scale & 0xff) << 24);
}

uint64_t
modify_lscale2 (uint64_t fpmr, uint64_t scale)
{
  return (fpmr & ~((uint64_t)(0x3f) << 32)) | ((uint64_t)(scale & 0x3f) << 32);
}

int
main (void)
{
  uint64_t fpmr = 0;

  fpmr = modify_src1_fmt (fpmr, E4M3);
  set_fpmr (fpmr); /* SRC1 */

  fpmr = modify_src2_fmt (fpmr, E4M3);
  set_fpmr (fpmr); /* SRC2 */

  fpmr = modify_dst_fmt (fpmr, E4M3);
  set_fpmr (fpmr); /* DST */

  fpmr = modify_osm (fpmr, SATURATE);
  set_fpmr (fpmr); /* OSM */

  fpmr = modify_osc (fpmr, SATURATE);
  set_fpmr (fpmr); /* OSC */

  fpmr = modify_lscale (fpmr, -1);
  set_fpmr (fpmr); /* LSCALE */

  fpmr = modify_nscale (fpmr, -1);
  set_fpmr (fpmr); /* NSCALE */

  fpmr = modify_lscale2 (fpmr, -1);
  set_fpmr (fpmr); /* LSCALE2 */

  return 1;
}
